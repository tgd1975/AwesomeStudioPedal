import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import '../services/ble_service.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final ble = context.watch<BleService>();
    return Scaffold(
      appBar: AppBar(title: const Text('AwesomeStudioPedal')),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            _ActionCard(
              icon: Icons.bluetooth,
              title: 'Connect to pedal',
              subtitle: ble.isConnected ? 'Connected' : 'Not connected',
              color: ble.isConnected ? Colors.green : null,
              onTap: () => context.go('/scan'),
            ),
            const SizedBox(height: 12),
            _ActionCard(
              icon: Icons.tune,
              title: 'Edit profiles',
              subtitle: 'Create and edit button profiles',
              onTap: () => context.go('/profiles'),
            ),
            const SizedBox(height: 12),
            _ActionCard(
              icon: Icons.upload,
              title: 'Upload',
              subtitle: 'Send profiles to the pedal via BLE',
              enabled: ble.isConnected,
              onTap: () => context.go('/upload'),
            ),
          ],
        ),
      ),
    );
  }
}

class _ActionCard extends StatelessWidget {
  const _ActionCard({
    required this.icon,
    required this.title,
    required this.subtitle,
    required this.onTap,
    this.color,
    this.enabled = true,
  });

  final IconData icon;
  final String title;
  final String subtitle;
  final VoidCallback onTap;
  final Color? color;
  final bool enabled;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    return Card(
      child: ListTile(
        leading: Icon(icon, color: color ?? (enabled ? theme.colorScheme.primary : Colors.grey)),
        title: Text(title),
        subtitle: Text(subtitle),
        trailing: enabled ? const Icon(Icons.chevron_right) : null,
        enabled: enabled,
        onTap: enabled ? onTap : null,
      ),
    );
  }
}
