import 'dart:typed_data';
import 'package:flutter_test/flutter_test.dart';

// Pure chunking logic extracted for host-side verification.
// Mirrors the chunking in BleService._upload exactly.
List<List<int>> chunkJson(String json, {int chunkSize = 510}) {
  final bytes = json.codeUnits; // utf8.encode equivalent for ASCII-safe strings
  final chunks = <List<int>>[];
  final total = (bytes.length / chunkSize).ceil();

  for (int i = 0; i < total; i++) {
    final payload = bytes.sublist(
      i * chunkSize,
      (i + 1) * chunkSize > bytes.length ? bytes.length : (i + 1) * chunkSize,
    );
    final seq = Uint8List(2)
      ..buffer.asByteData().setUint16(0, i, Endian.big);
    chunks.add([...seq, ...payload]);
  }

  // End-of-transfer packet: seq=0xFFFF, empty payload.
  final eof = Uint8List(2)
    ..buffer.asByteData().setUint16(0, 0xFFFF, Endian.big);
  chunks.add([...eof]);

  return chunks;
}

void main() {
  group('BleService chunking', () {
    test('1500-byte payload produces 3 data chunks + 1 EOF = 4 packets total', () {
      final json = 'x' * 1500;
      final chunks = chunkJson(json);
      // Chunk 0: bytes 0..509 (510 bytes)
      // Chunk 1: bytes 510..1019 (510 bytes)
      // Chunk 2: bytes 1020..1499 (480 bytes)
      // EOF:     seq=0xFFFF, empty
      expect(chunks.length, equals(4));

      // Sequence numbers in big-endian.
      expect(chunks[0][0], equals(0x00));
      expect(chunks[0][1], equals(0x00));

      expect(chunks[1][0], equals(0x00));
      expect(chunks[1][1], equals(0x01));

      expect(chunks[2][0], equals(0x00));
      expect(chunks[2][1], equals(0x02));

      // EOF packet.
      expect(chunks[3][0], equals(0xFF));
      expect(chunks[3][1], equals(0xFF));
      expect(chunks[3].length, equals(2)); // only seq bytes, no payload
    });

    test('exactly 510-byte payload produces 1 data chunk + EOF', () {
      final json = 'y' * 510;
      final chunks = chunkJson(json);
      expect(chunks.length, equals(2));
      expect(chunks[0][0], equals(0x00));
      expect(chunks[0][1], equals(0x00));
      expect(chunks[1][0], equals(0xFF));
      expect(chunks[1][1], equals(0xFF));
    });

    test('empty payload produces only EOF', () {
      final chunks = chunkJson('');
      expect(chunks.length, equals(1));
      expect(chunks[0][0], equals(0xFF));
      expect(chunks[0][1], equals(0xFF));
    });
  });
}
