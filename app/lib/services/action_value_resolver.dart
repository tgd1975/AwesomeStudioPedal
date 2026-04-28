import '../constants/action_types.dart';

/// Resolves whether an action's `value` field will be accepted by the
/// firmware at runtime. The schema permits any string, but the firmware's
/// `lookupKey()` (and equivalents) returns 0 for inputs that don't match
/// either:
///
/// - a named key in [kKeyNames] (for `SendKeyAction` / legacy
///   `SendCharAction`),
/// - a named media key in [kMediaKeyValues] (for `SendMediaKeyAction`),
/// - a hex literal (`0x..`) or decimal in `[1, 255]` (for `SendKeyAction`
///   raw-HID variant).
///
/// `SendCharAction` with a single-character literal (e.g. `"a"`, `" "`) is
/// also accepted by the firmware as a typeable character.
///
/// The named-key tables here mirror the firmware-internal codes; keep them
/// in sync with `lib/PedalLogic/src/key_lookup.cpp`. See TASK-264.
class ActionValueResolution {
  const ActionValueResolution.ok()
      : isResolvable = true,
        reason = null;
  const ActionValueResolution.fail(this.reason) : isResolvable = false;

  final bool isResolvable;
  final String? reason;
}

class ActionValueResolver {
  /// Returns whether the given [value] is resolvable for an action of the
  /// given [actionType]. Action types that don't have a value field (pin
  /// actions, macros, delayed actions) always resolve.
  static ActionValueResolution resolve({
    required String actionType,
    required String? value,
  }) {
    // Action types whose "payload" lives in fields other than `value`.
    if (actionType == 'PinHighAction' ||
        actionType == 'PinLowAction' ||
        actionType == 'PinToggleAction' ||
        actionType == 'PinHighWhilePressedAction' ||
        actionType == 'PinLowWhilePressedAction' ||
        actionType == 'DelayedAction' ||
        actionType == 'MacroAction') {
      return const ActionValueResolution.ok();
    }

    if (value == null || value.isEmpty) {
      return const ActionValueResolution.fail(
          'value is empty — provide a key name, hex code, or string');
    }

    switch (actionType) {
      case 'SendKeyAction':
        return _resolveKey(value);
      case 'SendMediaKeyAction':
        return _resolveMediaKey(value);
      case 'SendCharAction':
        return _resolveCharOrKey(value);
      case 'SendStringAction':
      case 'SerialOutputAction':
        // Free-form text — anything non-empty resolves.
        return const ActionValueResolution.ok();
      default:
        // Unknown action type — schema validation already flagged this.
        return ActionValueResolution.fail('unknown action type: $actionType');
    }
  }

  static ActionValueResolution _resolveKey(String value) {
    if (kKeyNames.contains(value)) return const ActionValueResolution.ok();
    final asInt = _parseHexOrDecimal(value);
    if (asInt != null && asInt >= 1 && asInt <= 255) {
      return const ActionValueResolution.ok();
    }
    return ActionValueResolution.fail(
        'cannot resolve "$value" as a named key, hex code, or decimal code in [1, 255]');
  }

  static ActionValueResolution _resolveMediaKey(String value) {
    if (kMediaKeyValues.contains(value)) {
      return const ActionValueResolution.ok();
    }
    return ActionValueResolution.fail(
        'cannot resolve "$value" as a media key (see kMediaKeyValues)');
  }

  static ActionValueResolution _resolveCharOrKey(String value) {
    // Legacy SendCharAction: either a single typeable character, or a
    // KEY_* / MEDIA_* name carried over from older schema versions.
    if (value.length == 1) return const ActionValueResolution.ok();
    if (kKeyNames.contains(value)) return const ActionValueResolution.ok();
    if (kMediaKeyValues.contains(value)) {
      return const ActionValueResolution.ok();
    }
    return ActionValueResolution.fail(
        'cannot resolve "$value" as a single character or named key');
  }

  static int? _parseHexOrDecimal(String s) {
    final lower = s.toLowerCase();
    if (lower.startsWith('0x')) {
      return int.tryParse(lower.substring(2), radix: 16);
    }
    return int.tryParse(s);
  }

  /// Walks a profiles JSON tree (matching `data/profiles.json`) and returns
  /// a list of human-readable error strings, one per unresolvable action.
  /// Each error contains the JSON-pointer path so the user can locate the
  /// offending button. See TASK-264.
  static List<String> findUnresolvable(Map<String, dynamic> profilesJson) {
    final errors = <String>[];
    final profiles = profilesJson['profiles'];
    if (profiles is! List) return errors;

    for (var i = 0; i < profiles.length; i++) {
      final profile = profiles[i];
      if (profile is! Map) continue;
      final buttons = profile['buttons'];
      if (buttons is! Map) continue;

      for (final entry in buttons.entries) {
        _walkAction(entry.value, '/profiles/$i/buttons/${entry.key}', errors);
      }
    }
    return errors;
  }

  static void _walkAction(dynamic action, String pointer, List<String> errors) {
    if (action is! Map) return;
    final type = action['type'];
    final value = action['value'];

    if (type is String) {
      final result = resolve(
        actionType: type,
        value: value is String ? value : null,
      );
      if (!result.isResolvable) {
        errors.add('$pointer/value: ${result.reason}');
      }
    }

    // Recurse into nested actions: longPress, DelayedAction.action,
    // MacroAction.steps[][].
    final longPress = action['longPress'];
    if (longPress != null) {
      _walkAction(longPress, '$pointer/longPress', errors);
    }
    final inner = action['action'];
    if (inner != null) {
      _walkAction(inner, '$pointer/action', errors);
    }
    final steps = action['steps'];
    if (steps is List) {
      for (var i = 0; i < steps.length; i++) {
        final step = steps[i];
        if (step is List) {
          for (var j = 0; j < step.length; j++) {
            _walkAction(step[j], '$pointer/steps/$i/$j', errors);
          }
        }
      }
    }
  }
}
