class HardwareConfig {
  HardwareConfig({
    required this.numButtons,
    required this.numProfiles,
    required this.numSelectLeds,
    required this.ledBluetooth,
    required this.ledPower,
    required this.ledSelect,
    required this.buttonSelect,
    required this.buttonPins,
  });

  final int numButtons;
  final int numProfiles;
  final int numSelectLeds;
  final int ledBluetooth;
  final int ledPower;
  final List<int> ledSelect;
  final int buttonSelect;
  final List<int> buttonPins;

  factory HardwareConfig.fromJson(Map<String, dynamic> json) {
    return HardwareConfig(
      numButtons: (json['numButtons'] as num).toInt(),
      numProfiles: (json['numProfiles'] as num).toInt(),
      numSelectLeds: (json['numSelectLeds'] as num).toInt(),
      ledBluetooth: (json['ledBluetooth'] as num).toInt(),
      ledPower: (json['ledPower'] as num).toInt(),
      ledSelect: (json['ledSelect'] as List<dynamic>)
          .map((e) => (e as num).toInt())
          .toList(),
      buttonSelect: (json['buttonSelect'] as num).toInt(),
      buttonPins: (json['buttonPins'] as List<dynamic>)
          .map((e) => (e as num).toInt())
          .toList(),
    );
  }

  Map<String, dynamic> toJson() => {
        'numButtons': numButtons,
        'numProfiles': numProfiles,
        'numSelectLeds': numSelectLeds,
        'ledBluetooth': ledBluetooth,
        'ledPower': ledPower,
        'ledSelect': ledSelect,
        'buttonSelect': buttonSelect,
        'buttonPins': buttonPins,
      };

  List<String> get buttonSlots {
    return List.generate(numButtons, (i) {
      return String.fromCharCode('A'.codeUnitAt(0) + i);
    });
  }
}
