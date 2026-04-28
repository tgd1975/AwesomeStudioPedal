import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'app.dart';
import 'models/profiles_state.dart';
import 'services/ble_service.dart';
import 'services/file_service.dart';
import 'services/schema_service.dart';

void main() {
  runApp(
    MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => BleService()),
        ChangeNotifierProvider(create: (_) => ProfilesState()),
        Provider(create: (_) => SchemaService()),
        ChangeNotifierProxyProvider<ProfilesState, FileService>(
          create: (_) => FileService(),
          update: (_, state, service) {
            service!.profilesState = state;
            return service;
          },
        ),
      ],
      child: const _AppRoot(),
    ),
  );
}

class _AppRoot extends StatefulWidget {
  const _AppRoot();

  @override
  State<_AppRoot> createState() => _AppRootState();
}

class _AppRootState extends State<_AppRoot> {
  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) {
      final fileService = context.read<FileService>();
      final profilesState = context.read<ProfilesState>();
      // Restore auto-saved state on launch.
      fileService.restoreAutoSave();
      fileService.restoreConfigAutoSave();
      // Auto-save on every change.
      profilesState.addListener(fileService.notifyAutoSave);
    });
  }

  @override
  Widget build(BuildContext context) => const App();
}
