import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../models/profiles_state.dart';
import '../models/action_config.dart';

class ProfileEditorScreen extends StatelessWidget {
  const ProfileEditorScreen({super.key, required this.profileIndex});

  final int profileIndex;

  @override
  Widget build(BuildContext context) {
    final state = context.watch<ProfilesState>();
    if (profileIndex >= state.profiles.length) {
      return Scaffold(
        appBar: AppBar(title: const Text('Profile')),
        body: const Center(child: Text('Profile not found')),
      );
    }
    final profile = state.profiles[profileIndex];
    final slots = state.hardwareConfig?.buttonSlots ?? ['A', 'B', 'C', 'D'];

    return Scaffold(
      appBar: AppBar(title: Text(profile.name)),
      body: ListView.builder(
        itemCount: slots.length,
        itemBuilder: (context, i) {
          final slot = slots[i];
          final action = profile.buttons[slot];
          return ListTile(
            leading: CircleAvatar(child: Text(slot)),
            title: Text(action != null ? action.type : '(none)'),
            subtitle: action?.name != null ? Text(action!.name!) : null,
            trailing: const Icon(Icons.chevron_right),
            onTap: () => context.go(
              '/action/$slot',
              extra: {
                'action': action,
                'onSave': (ActionConfig updated) {
                  final buttons = Map<String, ActionConfig>.from(profile.buttons);
                  buttons[slot] = updated;
                  state.updateProfile(
                    profileIndex,
                    profile.copyWith(buttons: buttons),
                  );
                },
              },
            ),
          );
        },
      ),
    );
  }
}
