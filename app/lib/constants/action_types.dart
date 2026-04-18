// Ported from docs/tools/config-builder/builder.js

class ActionTypeOption {
  const ActionTypeOption({required this.label, required this.value});
  final String label;
  final String value;
}

const List<ActionTypeOption> kActionTypes = [
  ActionTypeOption(label: '(none)', value: ''),
  ActionTypeOption(label: 'Key (named)', value: 'SendCharAction'),
  ActionTypeOption(label: 'Key (raw HID)', value: 'SendKeyAction'),
  ActionTypeOption(label: 'Type String', value: 'SendStringAction'),
  ActionTypeOption(label: 'Media Key', value: 'SendMediaKeyAction'),
  ActionTypeOption(label: 'Serial Output', value: 'SerialOutputAction'),
  ActionTypeOption(label: 'Delayed Action', value: 'DelayedAction'),
  ActionTypeOption(label: 'Macro', value: 'MacroAction'),
  ActionTypeOption(label: 'Pin High', value: 'PinHighAction'),
  ActionTypeOption(label: 'Pin Low', value: 'PinLowAction'),
  ActionTypeOption(label: 'Pin Toggle', value: 'PinToggleAction'),
  ActionTypeOption(label: 'Pin High While Pressed', value: 'PinHighWhilePressedAction'),
  ActionTypeOption(label: 'Pin Low While Pressed', value: 'PinLowWhilePressedAction'),
];

// Advanced types enabled only after TASK-143 widgets are in place.
// Listed here so ActionTypeDropdown can mark them "coming soon".
const Set<String> kAdvancedActionTypes = {
  'DelayedAction',
  'MacroAction',
};

const List<String> kPinTypes = [
  'PinHighAction',
  'PinLowAction',
  'PinToggleAction',
  'PinHighWhilePressedAction',
  'PinLowWhilePressedAction',
];

const List<String> kMediaKeyValues = [
  'MEDIA_NEXT_TRACK',
  'KEY_MEDIA_NEXT_TRACK',
  'MEDIA_PREVIOUS_TRACK',
  'KEY_MEDIA_PREVIOUS_TRACK',
  'MEDIA_STOP',
  'KEY_MEDIA_STOP',
  'MEDIA_PLAY_PAUSE',
  'KEY_MEDIA_PLAY_PAUSE',
  'MEDIA_MUTE',
  'KEY_MEDIA_MUTE',
  'MEDIA_VOLUME_UP',
  'KEY_MEDIA_VOLUME_UP',
  'KEY_VOLUME_UP',
  'MEDIA_VOLUME_DOWN',
  'KEY_MEDIA_VOLUME_DOWN',
  'KEY_VOLUME_DOWN',
  'KEY_MEDIA_WWW_HOME',
  'KEY_MEDIA_WWW_BACK',
  'KEY_MEDIA_WWW_STOP',
  'KEY_MEDIA_WWW_SEARCH',
  'KEY_MEDIA_WWW_BOOKMARKS',
  'KEY_MEDIA_CALCULATOR',
  'KEY_MEDIA_EMAIL_READER',
  'KEY_MEDIA_LOCAL_MACHINE_BROWSER',
  'KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION',
];

const List<String> kKeyNames = [
  'KEY_LEFT_ARROW', 'LEFT_ARROW', 'KEY_RIGHT_ARROW', 'RIGHT_ARROW',
  'KEY_UP_ARROW', 'UP_ARROW', 'KEY_DOWN_ARROW', 'DOWN_ARROW',
  'KEY_PAGE_UP', 'KEY_PAGE_DOWN', 'KEY_HOME', 'KEY_END',
  'KEY_INSERT', 'KEY_DELETE',
  'KEY_BACKSPACE', 'KEY_TAB', 'KEY_RETURN', 'KEY_ENTER',
  'KEY_ESC', 'KEY_CAPS_LOCK', 'KEY_PRINTSCREEN',
  'KEY_LEFT_CTRL', 'KEY_LEFT_SHIFT', 'KEY_LEFT_ALT', 'KEY_LEFT_GUI',
  'KEY_RIGHT_CTRL', 'KEY_RIGHT_SHIFT', 'KEY_RIGHT_ALT', 'KEY_RIGHT_GUI',
  'KEY_F1', 'KEY_F2', 'KEY_F3', 'KEY_F4', 'KEY_F5', 'KEY_F6',
  'KEY_F7', 'KEY_F8', 'KEY_F9', 'KEY_F10', 'KEY_F11', 'KEY_F12',
  'KEY_F13', 'KEY_F14', 'KEY_F15', 'KEY_F16', 'KEY_F17', 'KEY_F18',
  'KEY_F19', 'KEY_F20', 'KEY_F21', 'KEY_F22', 'KEY_F23', 'KEY_F24',
  'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10',
  'F11', 'F12', 'F13', 'F14', 'F15', 'F16', 'F17', 'F18', 'F19',
  'F20', 'F21', 'F22', 'F23', 'F24',
  'KEY_NUM_0', 'KEY_NUM_1', 'KEY_NUM_2', 'KEY_NUM_3', 'KEY_NUM_4',
  'KEY_NUM_5', 'KEY_NUM_6', 'KEY_NUM_7', 'KEY_NUM_8', 'KEY_NUM_9',
  'KEY_NUM_SLASH', 'KEY_NUM_ASTERISK', 'KEY_NUM_MINUS',
  'KEY_NUM_PLUS', 'KEY_NUM_ENTER', 'KEY_NUM_PERIOD',
];
