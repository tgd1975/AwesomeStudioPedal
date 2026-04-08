# Architecture Review — AwesomeGuitarPedal

**Date:** 2026-04-08
**Reviewer:** Architecture Review (Claude Code)
**Scope:** Full codebase review — architecture, implementation correctness, code quality

---

## What Is Good

### Layer Separation

The codebase has a clear and correct three-layer architecture:

```text
lib/PedalLogic/      — pure business logic, no hardware dependency
lib/hardware/esp32/  — concrete hardware drivers (GPIO, BLE)
src/                 — firmware entry point, wiring only
```

This separation makes the logic testable on a host machine without any ESP32 hardware.

### Interface-Based Seams

`IBleKeyboard`, `ILEDController`, and `IButtonController` are well-defined pure virtual
interfaces. This is the right pattern for embedded systems — it allows hardware to be
swapped or mocked without touching business logic.

### BankManager

Clean ownership model using `std::unique_ptr<Send>`. The strategy pattern for `Send`
subclasses (`SendChar`, `SendString`, `SendKey`, `SendMediaKey`) is the right design.
Bounds checking in `addAction` and `getAction` prevents out-of-range access silently.

### EventDispatcher

Decouples button events from their handlers using `std::function` callbacks. Correct
use of the observer pattern.

### Test Infrastructure

Host-based unit tests with GoogleTest and GMock run without hardware. The
`HOST_TEST_BUILD` preprocessor flag separates firmware headers from host-compatible
code. The pre-commit hook and CI workflow enforce tests before every commit.

---

## Findings

### F-01 — Critical: ISR Data Race on `Button::pressed`

**File:** [include/button.h](include/button.h)

`Button::pressed` is written in an ISR (`isr()`) and read in the main loop (`event()`).
Without `volatile`, the compiler is free to cache the value in a register and the main
loop may never see the update written by the ISR.

```cpp
// Current — wrong
boolean pressed = false;

// Fix
volatile bool pressed = false;
```

Same applies to `numberKeyPresses` and `lastDebounceTime` if they are ever read outside
the ISR.

---

### F-02 — Critical: Double `millis()` Call in Debounce — Race Condition

**File:** [src/button.cpp:9-10](src/button.cpp#L9)

```cpp
bool Button::isDebounced() {
    if ((millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();   // <-- second call, different timestamp
        return true;
    }
    return false;
}
```

`lastDebounceTime` is set to a different (later) timestamp than the one used in the
comparison. Under high interrupt frequency, this introduces a small but real timing
inconsistency. Capture `millis()` once:

```cpp
bool Button::isDebounced() {
    unsigned long now = millis();
    if ((now - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = now;
        return true;
    }
    return false;
}
```

---

### F-03 — High: `Serial.printf` Inside ISR

**File:** [src/button.cpp:23](src/button.cpp#L23)

```cpp
void Button::isr() {
    if (isDebounced()) {
        incKeyPresses();
        Serial.printf("PIN %d pressed %d-times\n", PIN, numberKeyPresses);  // unsafe
        pressed = true;
    }
}
```

`Serial.printf` is not ISR-safe. It uses mutexes and buffers that are not safe to call
from interrupt context. This can cause a crash or watchdog reset under load.

Remove the `Serial.printf` from `isr()`. Debug logging in ISRs must use ISR-safe
mechanisms (e.g., set a flag and log in the main loop).

---

### F-04 — High: Missing `#pragma once` in `button.h`

**File:** [include/button.h](include/button.h)

Every other header in the project uses `#pragma once`. `button.h` does not, making it
vulnerable to double-inclusion.

Add `#pragma once` as the first line.

---

### F-05 — High: ODR Violation — `MediaKeyReport` Constant in Header

**File:** [lib/PedalLogic/include/i_ble_keyboard.h:12](lib/PedalLogic/include/i_ble_keyboard.h#L12)

```cpp
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
```

`MediaKeyReport` is `uint8_t[2]` — an array type. A `const` array in a header is
**not** implicitly `inline` (unlike scalar constants), so including this header in
multiple translation units creates multiple definitions, violating the One Definition
Rule. This is undefined behaviour and may cause linker errors on some compilers.

Fix: declare it `extern` in the header and define it once in a `.cpp` file, or make it
`inline constexpr` (C++17):

```cpp
// i_ble_keyboard.h
inline constexpr uint8_t KEY_MEDIA_STOP_DATA[2] = {4, 0};
```

Or use a struct wrapper to get value semantics.

---

### F-06 — Medium: `HOST_TEST_BUILD` Guards in `lib/PedalLogic`

**File:** [lib/PedalLogic/src/bank_manager.cpp:2-4, 25-27](lib/PedalLogic/src/bank_manager.cpp#L2)

```cpp
#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#endif
// ...
#ifndef HOST_TEST_BUILD
    Serial.printf("Switched to Bank %d\n", currentBank + 1);
#endif
```

`lib/PedalLogic` is intended to be hardware-independent. These guards prove it is not:
the library knows it is running on Arduino. This is a leaky abstraction.

The correct fix is dependency injection for logging — pass a log callback or use a
no-op logger interface. A pragmatic short-term fix is to move the `Serial.printf` into
`src/main.cpp` by having `switchBank()` return the new bank index and letting the
caller log it.

---

### F-07 — Medium: `Send::bleKeyboard` Is `public`

**File:** [lib/PedalLogic/include/send.h:9](lib/PedalLogic/include/send.h#L9)

```cpp
class Send {
  public:
    IBleKeyboard* bleKeyboard;   // should be protected or private
```

The keyboard pointer is an implementation detail used only by subclasses. Exposing it
as `public` allows external code to replace it arbitrarily. Change to `protected`.

---

### F-08 — Medium: Unused `ButtonController` Instances in `main.cpp`

**File:** [src/main.cpp:33-37](src/main.cpp#L33)

```cpp
ButtonController buttonSelect(hardwareConfig.buttonSelect);
ButtonController buttonA(hardwareConfig.buttonA);
ButtonController buttonB(hardwareConfig.buttonB);
ButtonController buttonC(hardwareConfig.buttonC);
ButtonController buttonD(hardwareConfig.buttonD);
```

These five instances are declared but never used. Button reading is done via the `Button`
class through interrupts, not through `ButtonController`. These declarations are dead
code and should be removed.

---

### F-09 — Medium: Unused `#define SHIFT 0x80`

**File:** [src/main.cpp:52](src/main.cpp#L52)

```cpp
#define SHIFT 0x80
```

This macro is never referenced. Remove it.

---

### F-10 — Medium: Redundant `#include <BleKeyboard.h>` in `main.cpp`

**File:** [src/main.cpp:9](src/main.cpp#L9)

```cpp
#include <BleKeyboard.h>
#include "ble_keyboard_adapter.h"
```

`ble_keyboard_adapter.h` already includes `<BleKeyboard.h>`. The direct include in
`main.cpp` is redundant.

---

### F-11 — Medium: `boolean` Instead of `bool`

**File:** [include/button.h](include/button.h), [src/button.cpp](src/button.cpp)

`boolean` is an Arduino-specific typedef for `bool`. `button.h` is compiled in
non-Arduino contexts (tests). The file currently avoids compilation errors only because
the test build doesn't compile `button.cpp`. Use standard `bool` throughout.

Same applies to `boolean connected` in [src/main.cpp:24](src/main.cpp#L24) (lower
priority since `main.cpp` is firmware-only).

---

### F-12 — Low: Duplicate Targets in `Makefile`

**File:** [Makefile:26,44,47,55,58](Makefile#L26)

The `Makefile` defines `test` three times and `test-coverage` twice. GNU Make uses the
last definition, silently discarding the earlier ones. The effective targets are the
`nodemcu-32s-test` environment variants, but the intent is unclear and the file is
misleading.

Consolidate to a single `test` and `test-coverage` target. Add a separate
`test-host` target for the CMake/GoogleTest suite.

---

### F-13 — Low: Hardcoded Magic Numbers in `BankManager`

**File:** [lib/PedalLogic/src/bank_manager.cpp:10](lib/PedalLogic/src/bank_manager.cpp#L10),
[lib/PedalLogic/include/bank_manager.h:18](lib/PedalLogic/include/bank_manager.h#L18)

```cpp
std::array<std::array<std::unique_ptr<Send>, 4>, 3> banks;
```

The values `3` (banks) and `4` (buttons per bank) appear in both the header and the
implementation without named constants. Define them as `static constexpr`:

```cpp
static constexpr uint8_t NUM_BANKS   = 3;
static constexpr uint8_t NUM_BUTTONS = 4;
```

---

### F-14 — Low: `Button::PIN` Should Be Private

**File:** [include/button.h:9](include/button.h#L9)

`PIN` is public but is only used internally. In `isr()` it is accessed via `Serial.printf`
(which should be removed — see F-03). After that fix, `PIN` has no external callers and
can be private.

---

### F-15 — Low: `numberKeyPresses` Has No External Use

**File:** [include/button.h:10](include/button.h#L10)

`numberKeyPresses` is incremented in `incKeyPresses()` and logged in the ISR (which
should be removed — see F-03). After that removal it is written but never read.
Remove it, or keep it only if it will be used for diagnostics.

---

### F-16 — Low: `BankManager::updateLEDs()` Is Public But Is an Implementation Detail

**File:** [lib/PedalLogic/include/bank_manager.h:14](lib/PedalLogic/include/bank_manager.h#L14)

`updateLEDs()` is called externally once — from `pedal_config.cpp` after all banks are
configured. Consider whether this should remain public (it exposes internal LED sync
logic) or whether `addAction` should trigger a final LED update automatically. At
minimum, document why it is public.

---

## Summary Table

| ID   | Severity | Category        | File                                      | Summary                                          |
|------|----------|-----------------|-------------------------------------------|--------------------------------------------------|
| F-01 | Critical | ISR Safety      | `include/button.h`                        | `pressed` not `volatile`                         |
| F-02 | Critical | ISR Safety      | `src/button.cpp`                          | Double `millis()` race in debounce               |
| F-03 | High     | ISR Safety      | `src/button.cpp`                          | `Serial.printf` inside ISR                       |
| F-04 | High     | Correctness     | `include/button.h`                        | Missing `#pragma once`                           |
| F-05 | High     | Correctness     | `lib/PedalLogic/include/i_ble_keyboard.h` | ODR violation on array constant                  |
| F-06 | Medium   | Architecture    | `lib/PedalLogic/src/bank_manager.cpp`     | `HOST_TEST_BUILD` guard leaks Arduino dependency |
| F-07 | Medium   | Encapsulation   | `lib/PedalLogic/include/send.h`           | `bleKeyboard` should be `protected`              |
| F-08 | Medium   | Dead Code       | `src/main.cpp`                            | 5 unused `ButtonController` instances            |
| F-09 | Medium   | Dead Code       | `src/main.cpp`                            | Unused `#define SHIFT 0x80`                      |
| F-10 | Medium   | Dead Code       | `src/main.cpp`                            | Redundant `#include <BleKeyboard.h>`             |
| F-11 | Medium   | Portability     | `include/button.h`                        | `boolean` instead of `bool`                      |
| F-12 | Low      | Build           | `Makefile`                                | Duplicate `test` and `test-coverage` targets     |
| F-13 | Low      | Maintainability | `lib/PedalLogic/`                         | Magic numbers 3 and 4 in BankManager             |
| F-14 | Low      | Encapsulation   | `include/button.h`                        | `PIN` should be private                          |
| F-15 | Low      | Dead Code       | `include/button.h`                        | `numberKeyPresses` written but never read        |
| F-16 | Low      | API Design      | `lib/PedalLogic/include/bank_manager.h`   | `updateLEDs()` public without clear reason       |
