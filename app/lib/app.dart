import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'models/action_config.dart';
import 'screens/action_editor_screen.dart';
import 'screens/home_screen.dart';
import 'screens/json_preview_screen.dart';
import 'screens/profile_editor_screen.dart';
import 'screens/profile_list_screen.dart';
import 'screens/scanner_screen.dart';
import 'screens/upload_screen.dart';

const Color _kAccent = Color(0xFF2563EB);
const Color _kBackground = Color(0xFFF5F5F5);
const Color _kDarkSurface = Color(0xFF1E1E1E);

GoRouter _buildRouter() => GoRouter(
  initialLocation: '/',
  routes: [
    GoRoute(
      path: '/',
      name: 'home',
      builder: (_, __) => const HomeScreen(),
    ),
    GoRoute(
      path: '/scan',
      name: 'scan',
      builder: (_, __) => const ScannerScreen(),
    ),
    GoRoute(
      path: '/profiles',
      name: 'profiles',
      builder: (_, __) => const ProfileListScreen(),
    ),
    GoRoute(
      path: '/profile/:id',
      name: 'profile',
      builder: (context, state) {
        final id = int.parse(state.pathParameters['id']!);
        return ProfileEditorScreen(profileIndex: id);
      },
    ),
    GoRoute(
      path: '/action/:buttonId',
      name: 'action',
      builder: (context, state) {
        final buttonId = state.pathParameters['buttonId']!;
        final extra = state.extra as Map<String, dynamic>?;
        final action = extra?['action'] as ActionConfig?;
        final onSave = extra?['onSave'] as void Function(ActionConfig)?;
        return ActionEditorScreen(
          buttonId: buttonId,
          initial: action,
          onSave: onSave ?? (_) {},
        );
      },
    ),
    GoRoute(
      path: '/upload',
      name: 'upload',
      builder: (_, __) => const UploadScreen(),
    ),
    GoRoute(
      path: '/json-preview',
      name: 'json-preview',
      builder: (_, __) => const JsonPreviewScreen(),
    ),
  ],
);

class App extends StatefulWidget {
  const App({super.key});

  @override
  State<App> createState() => _AppState();
}

class _AppState extends State<App> {
  late final GoRouter _router = _buildRouter();

  @override
  void dispose() {
    _router.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp.router(
      title: 'AwesomeStudioPedal',
      routerConfig: _router,
      theme: ThemeData(
        useMaterial3: true,
        colorScheme: ColorScheme.fromSeed(
          seedColor: _kAccent,
          brightness: Brightness.light,
          surface: _kBackground,
        ),
      ),
      darkTheme: ThemeData(
        useMaterial3: true,
        colorScheme: ColorScheme.fromSeed(
          seedColor: _kAccent,
          brightness: Brightness.dark,
          surface: _kDarkSurface,
        ),
      ),
      themeMode: ThemeMode.system,
    );
  }
}
