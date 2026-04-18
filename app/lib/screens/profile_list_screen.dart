import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../models/profile.dart';
import '../models/profiles_state.dart';
import '../services/file_service.dart';
import '../services/schema_service.dart';
import '../widgets/validation_banner.dart';

class ProfileListScreen extends StatelessWidget {
  const ProfileListScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final state = context.watch<ProfilesState>();
    return Scaffold(
      appBar: AppBar(
        title: const Text('Profiles'),
        actions: [
          IconButton(
            icon: const Icon(Icons.file_upload_outlined),
            tooltip: 'Import JSON',
            onPressed: () => _import(context),
          ),
          IconButton(
            icon: const Icon(Icons.share),
            tooltip: 'Export JSON',
            onPressed: () => _export(context),
          ),
          IconButton(
            icon: const Icon(Icons.data_object),
            tooltip: 'JSON Preview',
            onPressed: () => context.go('/json-preview'),
          ),
        ],
      ),
      body: Column(
        children: [
          const ValidationBanner(),
          Expanded(
            child: state.profiles.isEmpty
                ? const Center(child: Text('No profiles yet. Tap + to add one.'))
                : ReorderableListView.builder(
                    itemCount: state.profiles.length,
                    onReorder: (oldIndex, newIndex) {
                      if (newIndex > oldIndex) newIndex--;
                      state.reorderProfiles(oldIndex, newIndex);
                    },
                    itemBuilder: (context, i) {
                      final profile = state.profiles[i];
                      return ListTile(
                        key: ValueKey(i),
                        title: Text(profile.name),
                        subtitle: profile.description != null
                            ? Text(profile.description!)
                            : null,
                        trailing: Row(
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            IconButton(
                              icon: const Icon(Icons.delete_outline),
                              onPressed: () => state.removeProfile(i),
                            ),
                            const Icon(Icons.drag_handle),
                          ],
                        ),
                        onTap: () => context.go('/profile/$i'),
                      );
                    },
                  ),
          ),
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () => _addProfile(context),
        child: const Icon(Icons.add),
      ),
    );
  }

  void _addProfile(BuildContext context) {
    final state = context.read<ProfilesState>();
    showDialog<void>(
      context: context,
      builder: (ctx) {
        final nameCtrl = TextEditingController();
        final descCtrl = TextEditingController();
        return AlertDialog(
          title: const Text('New Profile'),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              TextField(
                controller: nameCtrl,
                decoration: const InputDecoration(labelText: 'Name'),
              ),
              TextField(
                controller: descCtrl,
                decoration: const InputDecoration(labelText: 'Description (optional)'),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(ctx).pop(),
              child: const Text('Cancel'),
            ),
            FilledButton(
              onPressed: () {
                if (nameCtrl.text.isNotEmpty) {
                  state.addProfile(Profile(
                    name: nameCtrl.text.trim(),
                    description: descCtrl.text.isEmpty ? null : descCtrl.text.trim(),
                    buttons: {},
                  ));
                  Navigator.of(ctx).pop();
                }
              },
              child: const Text('Add'),
            ),
          ],
        );
      },
    );
  }

  Future<void> _import(BuildContext context) async {
    final fileService = context.read<FileService>();
    final schema = context.read<SchemaService>();
    final result = await fileService.importProfiles(schema);
    if (!context.mounted) return;
    if (!result.success) {
      await showDialog<void>(
        context: context,
        builder: (_) => AlertDialog(
          title: const Text('Import Failed'),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: result.errors.map((e) => Text('• $e')).toList(),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: const Text('OK'),
            ),
          ],
        ),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Profiles imported')),
      );
    }
  }

  Future<void> _export(BuildContext context) async {
    final fileService = context.read<FileService>();
    await fileService.exportProfiles();
  }
}
