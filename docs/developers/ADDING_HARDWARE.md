# Adding Hardware

## Current hardware targets

See [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) for the hardware maturity table.

## Adding a new microcontroller target

1. Add a `[env:new-board]` section to `platformio.ini`.
2. Create `lib/hardware/<target>/` with these source files:
   - `config.cpp` — pin assignments
   - `led_controller.cpp` — implements `ILEDController`
   - `button_controller.cpp` — implements `IButtonController`
   - `ble_keyboard_adapter.cpp` — implements `IBleKeyboard`
3. Add `lib_ignore` to the other platform environments to exclude the new hardware package.
4. Add a platform-specific guard in `include/platform.h` if needed.
5. Add `make` targets (build, upload, monitor, test) following the existing ESP32 pattern in the
   Makefile.
6. Add an on-device test environment to `platformio.ini`.
7. Update the hardware maturity table in
   [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md) and
   [BUILD_GUIDE.md](../builders/BUILD_GUIDE.md).

## Adding a new Action type

1. Create a subclass of `Action` — header in `lib/PedalLogic/include/`, implementation in
   `lib/PedalLogic/src/`.
2. Register it in `config_loader.cpp::createActionFromJson()` so the JSON loader can instantiate it.
3. Write at least one host test in `test/unit/`.
4. Add an entry to the action type table in
   [HARDWARE_CONFIG.md](../builders/HARDWARE_CONFIG.md).
