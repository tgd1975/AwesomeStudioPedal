import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:provider/provider.dart';
import '../services/ble_service.dart';

class ScannerScreen extends StatefulWidget {
  const ScannerScreen({super.key});

  @override
  State<ScannerScreen> createState() => _ScannerScreenState();
}

class _ScannerScreenState extends State<ScannerScreen> {
  List<ScanResult> _results = [];
  bool _scanning = false;
  String? _error;
  String? _connecting;
  StreamSubscription<BluetoothAdapterState>? _adapterSub;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _scan());

    // Re-scan automatically when the user toggles Bluetooth back on (e.g.
    // from the notification shade) without leaving the scanner. Without
    // this, the "Bluetooth is off" card stays put even after the radio
    // is enabled, forcing a manual back-and-re-enter. See TASK-262.
    _adapterSub = context.read<BleService>().adapterState.listen((state) {
      if (!mounted) return;
      if (state == BluetoothAdapterState.on &&
          _error != null &&
          _error!.toLowerCase().contains('bluetooth')) {
        _scan();
      }
    });
  }

  @override
  void dispose() {
    _adapterSub?.cancel();
    super.dispose();
  }

  Future<void> _scan() async {
    setState(() {
      _scanning = true;
      _error = null;
    });
    final ble = context.read<BleService>();
    final results = await ble.scan();
    if (!mounted) return;
    setState(() {
      _scanning = false;
      _results = results;
      if (results.isEmpty && ble.lastError != null) {
        _error = ble.lastError;
      }
    });
  }

  Future<void> _connect(BluetoothDevice device) async {
    setState(() => _connecting = device.remoteId.str);
    final ble = context.read<BleService>();
    await ble.connect(device);
    if (!mounted) return;
    if (ble.isConnected) {
      Navigator.of(context).pop();
    } else {
      setState(() {
        _connecting = null;
        _error = ble.lastError ?? 'Connection failed';
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Connect to Pedal'),
        actions: [
          if (!_scanning)
            IconButton(
              icon: const Icon(Icons.refresh),
              onPressed: _scan,
              tooltip: 'Scan again',
            ),
        ],
      ),
      body: _buildBody(),
    );
  }

  Widget _buildBody() {
    if (_error != null) {
      final isBtOff = _error!.toLowerCase().contains('bluetooth') &&
          (_error!.toLowerCase().contains('off') ||
              _error!.toLowerCase().contains('disabled'));
      final isPermission = _error!.toLowerCase().contains('permission');

      if (isBtOff) {
        return _ErrorCard(
          icon: Icons.bluetooth_disabled,
          title: 'Bluetooth is off',
          message:
              'Enable Bluetooth in your device settings to scan for the pedal.',
          actionLabel: 'Open Bluetooth Settings',
          onAction: () => FlutterBluePlus.turnOn(),
        );
      }
      if (isPermission) {
        return _ErrorCard(
          icon: Icons.lock_outline,
          title: 'Bluetooth permission denied',
          message:
              'AwesomeStudioPedal needs Bluetooth permission to scan for the pedal. '
              'If you previously chose "Don\'t allow", reopen the app settings to grant it.',
          actionLabel: 'Open App Settings',
          onAction: () async {
            final ble = context.read<BleService>();
            // Re-request first; if the user permanently denied, the system
            // skips the prompt and we fall back to opening Settings.
            final granted = await ble.requestBlePermissions();
            if (!granted && context.mounted) {
              await openAppSettings();
            }
            if (context.mounted) await _scan();
          },
        );
      }

      return _ErrorCard(
        icon: Icons.error_outline,
        title: 'Scan error',
        message: _error!,
        actionLabel: 'Retry',
        onAction: _scan,
      );
    }

    if (_scanning) {
      return const Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            CircularProgressIndicator(),
            SizedBox(height: 16),
            Text('Scanning for pedal…'),
          ],
        ),
      );
    }

    if (_results.isEmpty) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            const Text('No pedal found nearby.'),
            const SizedBox(height: 12),
            FilledButton.icon(
              icon: const Icon(Icons.refresh),
              label: const Text('Scan again'),
              onPressed: _scan,
            ),
          ],
        ),
      );
    }

    return ListView.builder(
      itemCount: _results.length,
      itemBuilder: (ctx, i) {
        final r = _results[i];
        final isConnecting = _connecting == r.device.remoteId.str;
        final name = r.device.platformName.isNotEmpty
            ? r.device.platformName
            : r.device.remoteId.str;
        return ListTile(
          leading: const Icon(Icons.bluetooth),
          title: Tooltip(
            message: name,
            child: Text(
              name,
              maxLines: 1,
              overflow: TextOverflow.ellipsis,
              softWrap: false,
            ),
          ),
          subtitle: _RssiBar(rssi: r.rssi),
          trailing: isConnecting
              ? const SizedBox(
                  width: 24,
                  height: 24,
                  child: CircularProgressIndicator(strokeWidth: 2),
                )
              : FilledButton(
                  onPressed: () => _connect(r.device),
                  child: const Text('Connect'),
                ),
        );
      },
    );
  }
}

class _RssiBar extends StatelessWidget {
  const _RssiBar({required this.rssi});
  final int rssi;

  @override
  Widget build(BuildContext context) {
    final bars = rssi > -60
        ? 4
        : rssi > -70
            ? 3
            : rssi > -80
                ? 2
                : 1;
    return Row(
      mainAxisSize: MainAxisSize.min,
      children: List.generate(
        4,
        (i) => Padding(
          padding: const EdgeInsets.only(right: 2),
          child: Container(
            width: 6,
            height: 6 + i * 3.0,
            color: i < bars
                ? Theme.of(context).colorScheme.primary
                : Theme.of(context).dividerColor,
          ),
        ),
      ),
    );
  }
}

class _ErrorCard extends StatelessWidget {
  const _ErrorCard({
    required this.icon,
    required this.title,
    required this.message,
    required this.actionLabel,
    required this.onAction,
  });

  final IconData icon;
  final String title;
  final String message;
  final String actionLabel;
  final VoidCallback onAction;

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Card(
        margin: const EdgeInsets.all(24),
        child: Padding(
          padding: const EdgeInsets.all(24),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(icon, size: 48, color: Theme.of(context).colorScheme.error),
              const SizedBox(height: 12),
              Text(title,
                  style: Theme.of(context).textTheme.titleMedium,
                  textAlign: TextAlign.center),
              const SizedBox(height: 8),
              Text(message, textAlign: TextAlign.center),
              const SizedBox(height: 16),
              FilledButton(onPressed: onAction, child: Text(actionLabel)),
            ],
          ),
        ),
      ),
    );
  }
}
