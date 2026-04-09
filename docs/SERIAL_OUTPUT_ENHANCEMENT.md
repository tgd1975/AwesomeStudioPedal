# Enhanced Serial Output for Action Execution

## 🎯 Overview

The pedal's serial output has been **enhanced** to provide much more informative logging when buttons are pressed and actions are executed. This makes debugging and monitoring much easier.

## 📋 What Changed

### Before

```
Button A pressed (profile 1)
```

### After

```
Button A pressed (Profile: Navigation)
  -> Executing SendString action
```

## 🔧 Implementation Details

### New Helper Function

Created `executeActionWithLogging()` function that:

1. Gets the current profile name
2. Identifies the action type
3. Logs detailed information before execution
4. Executes the action

### Action Type Detection

The system now detects and logs the specific action type:

- `SendString` - Sends a text string
- `SendChar` - Sends a single character key
- `SendKey` - Sends a USB HID key code
- `SendMediaKey` - Sends a media key
- `SerialOutput` - Outputs to serial console
- `Delayed` - Delayed action execution
- `Unknown` - Fallback for unknown types

### Code Structure

```cpp
void executeActionWithLogging(ProfileManager& profileManager, const char* buttonName, uint8_t buttonIndex) {
    // Get profile name and log button press
    uint8_t profileIndex = profileManager.getCurrentProfile();
    const char* profileName = profileManager.getProfileName(profileIndex).c_str();
    Serial.printf("Button %s pressed (Profile: %s)\n", buttonName, profileName);
    
    // Get action and log type
    if (auto action = profileManager.getAction(profileIndex, buttonIndex))
    {
        const char* actionType = getActionTypeName(action->getType());
        Serial.printf("  -> Executing %s action\n", actionType);
        action->execute();
    }
    else
    {
        Serial.println("  -> no action configured");
    }
}
```

## 📱 Example Output

### Navigation Profile

```
Button A pressed (Profile: Navigation)
  -> Executing SendString action

Button B pressed (Profile: Navigation)
  -> Executing SendMediaKey action

Button C pressed (Profile: Navigation)
  -> Executing SendChar action

Button D pressed (Profile: Navigation)
  -> Executing SendChar action
```

### Messaging Profile

```
Button A pressed (Profile: Messaging)
  -> Executing SendString action

Button B pressed (Profile: Messaging)
  -> Executing SendString action

Button C pressed (Profile: Messaging)
  -> Executing SendKey action

Button D pressed (Profile: Messaging)
  -> Executing SendKey action
```

### Custom Profile (with mixed actions)

```
Button A pressed (Profile: Custom)
  -> Executing SendString action

Button B pressed (Profile: Custom)
  -> Executing SerialOutput action

Button C pressed (Profile: Custom)
  -> Executing Delayed action

Button D pressed (Profile: Custom)
  -> Executing SendString action
```

## 🎯 Benefits

### 1. Better Debugging

- **Profile Context**: Shows which profile is active
- **Action Type**: Clearly identifies what type of action is being executed
- **Consistent Format**: Easy to parse and understand

### 2. User Feedback

- Users can see exactly what each button does
- Helps verify configuration is working correctly
- Useful for troubleshooting

### 3. Monitoring

- Easy to monitor pedal activity
- Logs can be parsed by other tools
- Clear separation between button press and action execution

## 🔧 Technical Implementation

### Action Type Detection

Uses the virtual `getType()` method from the `Action` base class:

```cpp
switch (action->getType()) {
    case Action::Type::SendString: return "SendString";
    case Action::Type::SendChar: return "SendChar";
    case Action::Type::SendKey: return "SendKey";
    case Action::Type::SendMediaKey: return "SendMediaKey";
    case Action::Type::SerialOutput: return "SerialOutput";
    case Action::Type::Delayed: return "Delayed";
    default: return "Unknown";
}
```

### Profile Name Access

Uses the `ProfileManager::getProfileName()` method to get the current profile name:

```cpp
const char* profileName = profileManager.getProfileName(profileIndex).c_str();
```

### Clean Code Structure

- **DRY Principle**: Helper function eliminates code duplication
- **Single Responsibility**: Each function does one thing well
- **Readable**: Clear, self-documenting code

## 📚 Usage

### Monitoring Pedal Activity

Simply connect to the pedal's serial port and monitor the output:

```bash
# Using screen
screen /dev/ttyUSB0 115200

# Using PlatformIO monitor
pio device monitor
```

### Example Monitoring Session

```
started
Button A pressed (Profile: Navigation)
  -> Executing SendString action
Button B pressed (Profile: Navigation)
  -> Executing SendMediaKey action
Switched to Profile 2
Button C pressed (Profile: Messaging)
  -> Executing SendKey action
```

## 🔮 Future Enhancements

### 1. Action-Specific Details

- Show the actual string being sent for `SendStringAction`
- Show the key code for `SendKeyAction` and `SendCharAction`
- Show the delay time for `DelayedAction`

### 2. Timing Information

- Add timestamps to logs
- Show execution duration for actions

### 3. Error Handling

- Log error details if action execution fails
- Show Bluetooth connection status changes

### 4. JSON Configuration

- Log when configuration is loaded/saved
- Show configuration validation results

## 📋 Summary

The enhanced serial output provides:

- ✅ **Profile context** - Shows which profile is active
- ✅ **Action type** - Clearly identifies action being executed
- ✅ **Consistent format** - Easy to read and parse
- ✅ **Better debugging** - Helps identify issues quickly
- ✅ **User feedback** - Shows what each button does
- ✅ **Clean code** - Uses helper function to avoid duplication

This enhancement makes the pedal much more **user-friendly** and **developer-friendly** by providing clear, informative feedback about what's happening when buttons are pressed.
