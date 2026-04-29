---
id: IDEA-025
title: Configurable BLE Device Name
description: Allow users to configure the BLE device name via config.json
---

## Details

Currently, the BLE device name is hardcoded or not configurable. This idea proposes to make the BLE device name configurable through the `config.json` file. The vendor prefix should be hardcoded as "AwesomeStudioPedal" to maintain brand consistency, while the device name should be user-configurable.

### Proposed Implementation

1. **Vendor Prefix**: Hardcode the vendor prefix as "AwesomeStudioPedal" (or "ASP" for short).

2. **Device Name**: Add a new field in `config.json` to allow users to set a custom device name. For example:

   ```json
   {
     "ble": {
       "deviceName": "MyCustomPedal"
     }
   }
   ```

3. **Auto-Generation**: If the `deviceName` field is left empty or not provided, auto-generate the device name using the rule `ASP_{numberOfPedals}`. The `numberOfPedals` should be a unique identifier or counter to distinguish multiple pedals in the same environment.

4. **Full Device Name**: The full BLE device name should be constructed as follows:
   - If `deviceName` is provided: `AwesomeStudioPedal_{deviceName}`
   - If `deviceName` is not provided: `ASP_{numberOfPedals}`

### Example

- **Custom Name**: If the user sets `deviceName` to "MyCustomPedal", the BLE device name will be "AwesomeStudioPedal_MyCustomPedal".
- **Auto-Generated Name**: If the user does not provide a `deviceName`, the BLE device name will be "ASP_1", "ASP_2", etc., depending on the number of pedals.

### Benefits

- **Brand Consistency**: The hardcoded vendor prefix ensures that all pedals are easily identifiable as part of the AwesomeStudioPedal ecosystem.
- **User Customization**: Users can personalize their pedals with custom names, making it easier to identify their devices.
- **Auto-Generation**: The auto-generation feature ensures that even if users do not provide a custom name, their pedals will still have unique and identifiable names.

### Implementation Steps

1. Update the `config.schema.json` to include the new `ble.deviceName` field.
2. Modify the BLE initialization code to read the `deviceName` from `config.json`.
3. Implement the logic to construct the full BLE device name based on the presence or absence of the `deviceName` field.
4. Ensure that the auto-generated `numberOfPedals` is unique and persistent across reboots.
5. Update the configuration configurator (web and mobile apps) to include a field for setting the BLE device name.
6. Test the implementation to ensure that the BLE device name is correctly set and displayed in BLE scanners.
