import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../models/profiles_state.dart';
import '../services/schema_service.dart';

class ValidationBanner extends StatefulWidget {
  const ValidationBanner({super.key});

  @override
  State<ValidationBanner> createState() => _ValidationBannerState();
}

class _ValidationBannerState extends State<ValidationBanner> {
  List<String> _errors = [];
  bool _isValid = true;
  bool _checked = false;

  int _lastProfileCount = -1;

  Future<void> _validate(ProfilesState state) async {
    if (state.profiles.isEmpty) {
      if (mounted) {
        setState(() {
          _isValid = true;
          _errors = [];
          _checked = true;
        });
      }
      return;
    }
    final service = context.read<SchemaService>();
    final json = state.toProfilesJson();
    final result = await service.validateProfiles(json);
    if (!mounted) return;
    setState(() {
      _isValid = result.isValid;
      _errors = result.errors;
      _checked = true;
    });
  }

  @override
  Widget build(BuildContext context) {
    final state = context.watch<ProfilesState>();
    final count = state.profiles.length;
    if (count != _lastProfileCount) {
      _lastProfileCount = count;
      WidgetsBinding.instance.addPostFrameCallback((_) => _validate(state));
    }
    if (!_checked) return const SizedBox.shrink();
    final color = _isValid ? Colors.green : Colors.red;
    final label = _isValid
        ? 'Valid ✓'
        : '${_errors.length} error${_errors.length == 1 ? '' : 's'}';

    return GestureDetector(
      onTap: _isValid
          ? null
          : () => _showErrors(context),
      child: Container(
        width: double.infinity,
        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
        color: color.withValues(alpha: 0.15),
        child: Row(
          children: [
            Icon(
              _isValid ? Icons.check_circle : Icons.error,
              color: color,
              size: 16,
            ),
            const SizedBox(width: 8),
            Text(label, style: TextStyle(color: color, fontWeight: FontWeight.w600)),
          ],
        ),
      ),
    );
  }

  void _showErrors(BuildContext context) {
    showDialog<void>(
      context: context,
      builder: (_) => AlertDialog(
        title: const Text('Validation Errors'),
        content: SizedBox(
          width: double.maxFinite,
          child: ListView(
            shrinkWrap: true,
            children: _errors
                .map((e) => ListTile(
                      leading: const Icon(Icons.error_outline, color: Colors.red),
                      title: Text(e, style: const TextStyle(fontSize: 13)),
                    ))
                .toList(),
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: const Text('Close'),
          ),
        ],
      ),
    );
  }
}
