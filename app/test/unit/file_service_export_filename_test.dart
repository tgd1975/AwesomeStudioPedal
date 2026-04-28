import 'package:awesome_studio_pedal/services/file_service.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('FileService.exportFilenameForDate', () {
    test('produces a friendly date-stamped name', () {
      final name = FileService.exportFilenameForDate(DateTime(2026, 4, 26));
      expect(name, 'profiles-2026-04-26.json');
    });

    test('zero-pads single-digit months and days', () {
      final name = FileService.exportFilenameForDate(DateTime(2026, 1, 9));
      expect(name, 'profiles-2026-01-09.json');
    });

    test('does not contain UUID-like fragments', () {
      final name = FileService.exportFilenameForDate(DateTime(2026, 4, 26));
      expect(name.contains('-'), isTrue);
      expect(RegExp(r'[a-f0-9]{8}').hasMatch(name), isFalse);
    });
  });
}
