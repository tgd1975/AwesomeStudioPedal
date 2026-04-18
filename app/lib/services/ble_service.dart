import 'dart:async';
import 'dart:convert';
import 'dart:math';
import 'dart:typed_data';

import 'package:flutter/foundation.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

import '../constants/ble_constants.dart';
import '../models/upload_result.dart';

class BleService extends ChangeNotifier {
  BluetoothDevice? _device;
  BluetoothCharacteristic? _writeChar;
  BluetoothCharacteristic? _writeHwChar;
  BluetoothCharacteristic? _statusChar;
  StreamSubscription<BluetoothConnectionState>? _connSub;

  bool _connected = false;
  String? _error;

  bool get isConnected => _connected;
  String? get lastError => _error;

  // Streams status notify values as UTF-8 strings.
  Stream<String> get statusStream => FlutterBluePlus.isAvailable.asStream().asyncExpand((_) {
        // Exposed after connect() stores _statusChar.
        if (_statusChar == null) return const Stream.empty();
        return _statusChar!.onValueReceived.map((v) => utf8.decode(v));
      });

  Future<List<ScanResult>> scan({Duration timeout = const Duration(seconds: 10)}) async {
    _clearError();
    try {
      await FlutterBluePlus.startScan(
        withServices: [Guid(kServiceUuid)],
        timeout: timeout,
      );
      await Future.delayed(timeout);
      final results = FlutterBluePlus.lastScanResults;
      return results;
    } on FlutterBluePlusException catch (e) {
      _setError(e.description ?? e.toString());
      return [];
    }
  }

  Future<void> connect(BluetoothDevice device) async {
    _clearError();
    try {
      await device.connect(autoConnect: false);
      _device = device;

      _connSub = device.connectionState.listen((state) {
        if (state == BluetoothConnectionState.disconnected) {
          _connected = false;
          _writeChar = null;
          _writeHwChar = null;
          _statusChar = null;
          notifyListeners();
        }
      });

      final services = await device.discoverServices();
      final pedal = services.firstWhere(
        (s) => s.uuid.str128.toLowerCase() == kServiceUuid,
        orElse: () => throw Exception('Pedal GATT service not found'),
      );

      for (final c in pedal.characteristics) {
        final uuid = c.uuid.str128.toLowerCase();
        if (uuid == kConfigWriteUuid) _writeChar = c;
        if (uuid == kConfigWriteHwUuid) _writeHwChar = c;
        if (uuid == kConfigStatusUuid) _statusChar = c;
      }

      if (_statusChar != null) {
        await _statusChar!.setNotifyValue(true);
      }

      _connected = true;
      notifyListeners();
    } on FlutterBluePlusException catch (e) {
      _setError(e.description ?? e.toString());
    } catch (e) {
      _setError(e.toString());
    }
  }

  void disconnect() {
    _connSub?.cancel();
    _device?.disconnect();
    _device = null;
    _writeChar = null;
    _writeHwChar = null;
    _statusChar = null;
    _connected = false;
    notifyListeners();
  }

  Future<UploadResult> uploadProfiles(String json) =>
      _upload(json, _writeChar, 'CONFIG_WRITE');

  Future<UploadResult> uploadConfig(String json) =>
      _upload(json, _writeHwChar, 'CONFIG_WRITE_HW');

  Future<UploadResult> _upload(
    String json,
    BluetoothCharacteristic? characteristic,
    String label,
  ) async {
    if (characteristic == null) {
      return const UploadResult.failure('Not connected or characteristic unavailable');
    }
    if (_statusChar == null) {
      return const UploadResult.failure('Status characteristic unavailable');
    }

    final completer = Completer<UploadResult>();
    StreamSubscription<List<int>>? sub;
    sub = _statusChar!.onValueReceived.listen((value) {
      final msg = utf8.decode(value);
      sub?.cancel();
      if (msg == 'OK') {
        completer.complete(const UploadResult.success());
      } else {
        final reason = msg.startsWith('ERROR:') ? msg.substring(6) : msg;
        completer.complete(UploadResult.failure(reason));
      }
    });

    try {
      final bytes = utf8.encode(json);
      const chunkSize = 510;
      final totalChunks = (bytes.length / chunkSize).ceil();

      for (int i = 0; i < totalChunks; i++) {
        final payload = bytes.sublist(
          i * chunkSize,
          min((i + 1) * chunkSize, bytes.length),
        );
        final seq = Uint8List(2)
          ..buffer.asByteData().setUint16(0, i, Endian.big);
        await characteristic.write([...seq, ...payload], withoutResponse: true);
      }

      // End-of-transfer packet.
      final eof = Uint8List(2)
        ..buffer.asByteData().setUint16(0, 0xFFFF, Endian.big);
      await characteristic.write([...eof], withoutResponse: true);
    } on FlutterBluePlusException catch (e) {
      sub.cancel();
      return UploadResult.failure(e.description ?? e.toString());
    }

    return completer.future.timeout(
      const Duration(seconds: 15),
      onTimeout: () {
        sub?.cancel();
        return const UploadResult.failure('Upload timed out waiting for status');
      },
    );
  }

  void _setError(String message) {
    _error = message;
    notifyListeners();
  }

  void _clearError() {
    _error = null;
  }
}
