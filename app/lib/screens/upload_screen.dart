import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../models/profiles_state.dart';
import '../services/ble_service.dart';
import '../services/schema_service.dart';

class UploadScreen extends StatefulWidget {
  const UploadScreen({super.key});

  @override
  State<UploadScreen> createState() => _UploadScreenState();
}

class _UploadScreenState extends State<UploadScreen> {
  bool _uploading = false;
  bool _uploadingConfig = false;
  double _progress = 0;
  String? _statusMessage;
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
      _statusMessage = null;
    });

    // Simulate chunk progress for UX — actual chunking is inside BleService.
    final chunkCount = (json.length / 510).ceil() + 1;
    for (int i = 0; i <= chunkCount; i++) {
      if (!mounted) return;
      setState(() => _progress = i / chunkCount);
      await Future.delayed(const Duration(milliseconds: 30));
    }

    final result = await ble.uploadProfiles(json);
    if (!mounted) return;
    setState(() {
      _uploading = false;
      _progress = 1;
    });

    if (result.success) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Upload successful!'),
          backgroundColor: Colors.green,
        ),
      );
    } else {
      _showError(result.errorMessage ?? 'Unknown error');
    }
  }

  Future<void> _uploadConfig() async {
    final state = context.read<ProfilesState>();
    final ble = context.read<BleService>();
    if (state.hardwareConfig == null) {
      _showError('No hardware config loaded.');
      return;
    }
    final json = state.toConfigJsonString();

    setState(() {
      _uploadingConfig = true;
      _statusMessage = null;
    });

    final result = await ble.uploadConfig(json);
    if (!mounted) return;
    setState(() => _uploadingConfig = false);

    if (result.success) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Hardware config uploaded!')),
      );
    } else {
      _showError(result.errorMessage ?? 'Unknown error');
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
              color: Colors.red.withOpacity(0.1),
              child: Padding(
                padding: const EdgeInsets.all(12),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Validation errors — fix before uploading:',
                        style: TextStyle(fontWeight: FontWeight.w600, color: Colors.red)),
                    ...(_validationErrors.map((e) => Text('• $e',
                        style: const TextStyle(fontSize: 12, color: Colors.red)))),
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
            onPressed: (!_uploading && _valid && ble.isConnected) ? _uploadProfiles : null,
          ),
          const SizedBox(height: 8),
          TextButton.icon(
            icon: _uploadingConfig
                ? const SizedBox(width: 18, height: 18, child: CircularProgressIndicator(strokeWidth: 2))
                : const Icon(Icons.settings),
            label: const Text('Upload Hardware Config'),
            onPressed: (!_uploadingConfig && ble.isConnected) ? _uploadConfig : null,
          ),
          const SizedBox(height: 8),
          TextButton(
            onPressed: () => context.go('/json-preview'),
            child: const Text('View JSON Preview'),
          ),
        ],
      ),
    );
  }

  int _getChunkCount(String json) => (json.length / 510).ceil() + 1;
}
