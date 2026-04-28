import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../models/profiles_state.dart';
import '../services/ble_service.dart';
import '../services/schema_service.dart';
import '../theme/asp_theme.dart';

class UploadScreen extends StatefulWidget {
  const UploadScreen({super.key});

  @override
  State<UploadScreen> createState() => _UploadScreenState();
}

class _UploadScreenState extends State<UploadScreen> {
  bool _uploading = false;
  bool _uploadingConfig = false;
  double _progress = 0;
  List<String> _validationErrors = [];
  bool _valid = false;

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _validate());
  }

  Future<void> _validate() async {
    final state = context.read<ProfilesState>();
    final schema = context.read<SchemaService>();
    final result = await schema.validateProfiles(state.toProfilesJson());
    if (!mounted) return;
    setState(() {
      _valid = result.isValid;
      _validationErrors = result.errors;
    });
  }

  Future<void> _uploadProfiles() async {
    final state = context.read<ProfilesState>();
    final ble = context.read<BleService>();
    final json = state.toProfilesJsonString();

    setState(() {
      _uploading = true;
      _progress = 0;
    });

    try {
      // Simulate chunk progress for UX — actual chunking is inside BleService.
      final chunkCount = _getChunkCount(json);
      for (int i = 0; i <= chunkCount; i++) {
        if (!mounted) return;
        setState(() => _progress = i / chunkCount);
        await Future.delayed(const Duration(milliseconds: 30));
      }

      final result = await ble.uploadProfiles(json);
      if (!mounted) return;

      if (result.success) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Upload successful!'),
            backgroundColor: AspTokens.success,
          ),
        );
      } else {
        _showError(result.errorMessage ?? 'Unknown error');
      }
    } finally {
      // Always clear the progress UI, even if the upload threw. Without this
      // the screen stays stuck at "Uploading… chunk N/N" forever. See TASK-266.
      if (mounted) {
        setState(() {
          _uploading = false;
          _progress = 0;
        });
      }
    }
  }

  Future<void> _uploadConfig() async {
    final state = context.read<ProfilesState>();
    final ble = context.read<BleService>();
    if (state.hardwareConfig == null) {
      _showError('No hardware config loaded.');
      return;
    }

    setState(() => _uploadingConfig = true);

    try {
      // Hardware identity check: abort if config targets a different board.
      final configHw = state.hardwareConfig!.hardware;
      final deviceHw = await ble.readDeviceHardware();
      if (!mounted) return;

      if (deviceHw != null &&
          deviceHw.toLowerCase() != configHw.toLowerCase()) {
        _showError(
          'Hardware mismatch: config targets "$configHw" '
          'but connected device is "$deviceHw". Upload aborted.',
        );
        return;
      }

      final json = state.toConfigJsonString();
      final result = await ble.uploadConfig(json);
      if (!mounted) return;

      if (result.success) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text('Hardware config uploaded!')),
        );
      } else {
        _showError(result.errorMessage ?? 'Unknown error');
      }
    } finally {
      if (mounted) {
        setState(() => _uploadingConfig = false);
      }
    }
  }

  void _showError(String message) {
    showDialog<void>(
      context: context,
      builder: (_) => AlertDialog(
        title: const Text('Upload Failed'),
        content: Text(message),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: const Text('OK'),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final state = context.watch<ProfilesState>();
    final ble = context.watch<BleService>();

    return Scaffold(
      appBar: AppBar(title: const Text('Upload to Pedal')),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          Card(
            child: Padding(
              padding: const EdgeInsets.all(16),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text('Profiles summary',
                      style: Theme.of(context).textTheme.titleSmall),
                  const SizedBox(height: 8),
                  Text('${state.profiles.length} profile(s)'),
                  if (state.lastModified != null)
                    Text('Last modified: ${state.lastModified!.toLocal()}',
                        style: Theme.of(context).textTheme.bodySmall),
                ],
              ),
            ),
          ),
          const SizedBox(height: 8),
          if (_validationErrors.isNotEmpty) ...[
            Card(
              color: AspTokens.error.withValues(alpha: 0.1),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Validation errors — fix before uploading:',
                        style: TextStyle(
                            fontWeight: FontWeight.w600,
                            color: AspTokens.error)),
                    ...(_validationErrors.map((e) => Text('• $e',
                        style: const TextStyle(
                            fontSize: 12, color: AspTokens.error)))),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),
          ],
          if (_uploading) ...[
            LinearProgressIndicator(value: _progress),
            const SizedBox(height: 4),
            Text(
              'Uploading… chunk ${(_progress * (_getChunkCount(state.toProfilesJsonString()))).round()} / ${_getChunkCount(state.toProfilesJsonString())}',
              style: Theme.of(context).textTheme.bodySmall,
            ),
            const SizedBox(height: 8),
          ],
          FilledButton.icon(
            icon: const Icon(Icons.upload),
            label: const Text('Upload Profiles'),
            onPressed: (!_uploading && _valid && ble.isConnected)
                ? _uploadProfiles
                : null,
          ),
          const SizedBox(height: 8),
          TextButton.icon(
            icon: _uploadingConfig
                ? const SizedBox(
                    width: 18,
                    height: 18,
                    child: CircularProgressIndicator(strokeWidth: 2))
                : const Icon(Icons.settings),
            label: const Text('Upload Hardware Config'),
            onPressed:
                (!_uploadingConfig && ble.isConnected) ? _uploadConfig : null,
          ),
          const SizedBox(height: 8),
          TextButton(
            onPressed: () => context.push('/json-preview'),
            child: const Text('View JSON Preview'),
          ),
        ],
      ),
    );
  }

  // Mirrors BleService._upload's chunk size (see TASK-261). Drift here only
  // affects the progress-bar UX, not correctness, but keeping it aligned
  // means "chunk N/N" matches the real wire traffic.
  int _getChunkCount(String json) => (json.length / 180).ceil() + 1;
}
