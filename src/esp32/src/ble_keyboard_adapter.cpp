#include "ble_keyboard_adapter.h"

// Best practice: keep the advertised name short. The BLE 4.x advertising
// PDU is 31 B total, and once Flags + TX Power + Service UUID AD entries
// are included, the GAP Local Name field is hard-capped to ~14 chars.
// Anything longer is truncated by the host stack (e.g. Android shows
// "AwesomeStudioPe" for "AwesomeStudioPedal"). The full name is still
// reachable post-connect via the GAP Device Name characteristic, but the
// pre-connect scan list will only see the first ~14 chars.
static HookableBleKeyboard bleKeyboard("AwesomeStudioPedal", "tgd1975");

BleKeyboardAdapter* createBleKeyboardAdapter() { return new BleKeyboardAdapter(bleKeyboard); }
