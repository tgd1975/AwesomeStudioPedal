# Adding Hardware

## Current hardware targets

See [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) for the hardware maturity table.

## Adding a new microcontroller target

After EPIC-020, each platform target lives under `src/<target>/{include,src}/` with
the same shape: `include/` for headers, `src/` for implementations, plus a thin
`main.cpp` shim at the module root. The build system selects the active target via
`build_src_filter` (PIO) and, for host tests, via explicit file lists in
`test/CMakeLists.txt`.

Steps:

1. **Add a `[env:new-board]` section to `platformio.ini`.** It should set
   `build_src_filter = -<*> +<<target>/> +<pedal_app.cpp> +<ble_pedal_app.cpp>`
   and `build_flags` containing `-Iinclude -Isrc/<target>/include` (plus any
   `-D` defines the framework needs).

2. **Create `src/<target>/{include,src}/`** with these files:

   | File | Purpose |
   |---|---|
   | `include/builder_config.h` | Builder-facing pin and count configuration with `constexpr` values and `static_assert` validation. The only file a builder needs to edit for custom wiring. |
   | `src/config.cpp` | Constructs the global `hardwareConfig` from `builder_config.h`. Builders never open this file. |
   | `include/<target>_pedal_app.h`, `src/<target>_pedal_app.cpp` | Concrete `PedalApp` subclass — typically `class XxxPedalApp : public BlePedalApp`. Implements the four pure-virtual hooks (`platformSetup`, `platformLoop`, `saveProfile`, `loadProfile`). |
   | `src/<target>_file_system.cpp` | `IFileSystem` implementation using the platform's filesystem (LittleFS variant). Provides `createFileSystem()`. |
   | `include/ble_keyboard_adapter.h`, `src/ble_keyboard_adapter.cpp` | `IBleKeyboard` implementation wrapping the platform's BLE keyboard library. Provides `createBleKeyboardAdapter()`. |
   | `include/button.h`, `src/button.cpp` | `IButton` implementation (debounce, ISR). |
   | `include/button_controller.h`, `src/button_controller.cpp` | `IButtonController` implementation. |
   | `include/led_controller.h`, `src/led_controller.cpp` | `ILEDController` implementation. |
   | `main.cpp` (at module root, not in `src/`) | The 8-line entry-point shim that PIO compiles as the program's `setup()` / `loop()`. |

   The `main.cpp` shim looks like this:

   ```cpp
   #include <Arduino.h>

   #include "<target>_pedal_app.h"

   static <Target>PedalApp app;

   void setup() { app.setup(); }
   void loop() { app.loop(); }
   ```

3. **Frameworks with bundled libraries that aren't in the PIO registry**
   need a direct `#include <framework_header.h>` at the top of one of
   your platform `.cpp` files so the LDF activates them. Without this,
   PIO's `chain`-mode dependency finder won't see headers that are
   only included transitively through your project headers.

   Example from `src/nrf52840/src/ble_keyboard_adapter.cpp`:

   ```cpp
   #include <Adafruit_nRFCrypto.h>
   #include <bluefruit.h>

   #include "ble_keyboard_adapter.h"
   ```

   ESP32 doesn't need this trick because NimBLE is a registry library
   (added via `lib_deps`). A platform whose BLE / persistence libs are
   shipped *inside* its framework package needs the direct includes.

4. **Add a platform-specific guard in `include/platform.h`** if the new
   target needs a portability shim (e.g. an attribute that has no
   equivalent on other platforms — see the existing `IRAM_ATTR` shim).

5. **Add `make` targets** (build, upload, monitor, test) following the
   existing ESP32 pattern in the Makefile.

6. **Add an on-device test environment to `platformio.ini`** modelled on
   `feather-nrf52840-test`.

7. **Update the hardware maturity table** in
   [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) and
   [BUILD_GUIDE.md](../builders/BUILD_GUIDE.md).

## Adding a new Action type

1. Create a subclass of `Action` — header in `lib/PedalLogic/include/`,
   implementation in `lib/PedalLogic/src/`.
2. Register it in `config_loader.cpp::createActionFromJson()` so the JSON
   loader can instantiate it.
3. Write at least one host test in `test/unit/`.
4. Add an entry to the action type table in
   [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md).
