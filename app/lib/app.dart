import 'dart:async';

import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:provider/provider.dart';
import 'models/action_config.dart';
import 'models/hardware_config.dart';
import 'models/profiles_state.dart';
import 'screens/action_editor_screen.dart';
import 'screens/community_profiles_screen.dart';
import 'screens/home_screen.dart';
import 'screens/how_to_screen.dart';
import 'screens/independent_actions_screen.dart';
import 'screens/info_about_screen.dart';
import 'screens/json_preview_screen.dart';
import 'screens/profile_editor_screen.dart';
import 'screens/profile_list_screen.dart';
import 'screens/scanner_screen.dart';
import 'screens/upload_screen.dart';
import 'services/first_run.dart';
import 'theme/asp_theme.dart';

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
            final profilesState = context.read<ProfilesState>();
            final board =
                profilesState.hardwareConfig?.boardTarget ?? BoardTarget.esp32;
            return ActionEditorScreen(
              buttonId: buttonId,
              initial: action,
              onSave: onSave ?? (_) {},
              board: board,
            );
          },
        ),
        GoRoute(
          path: '/upload',
          name: 'upload',
          builder: (_, __) => const UploadScreen(),
        ),
        GoRoute(
          path: '/community-profiles',
          name: 'community-profiles',
          builder: (_, __) => const CommunityProfilesScreen(),
        ),
        GoRoute(
          path: '/json-preview',
          name: 'json-preview',
          builder: (_, __) => const JsonPreviewScreen(),
        ),
        GoRoute(
          path: '/independent-actions',
          name: 'independent-actions',
          builder: (_, __) => const IndependentActionsScreen(),
        ),
        GoRoute(
          path: '/info',
          name: 'info',
          builder: (_, __) => const InfoAboutScreen(),
        ),
        GoRoute(
          path: '/how-to',
          name: 'how-to',
          builder: (context, state) {
            final firstRun = state.uri.queryParameters['firstRun'] == '1';
            return HowToScreen(firstRun: firstRun);
          },
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
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      if (await firstRunGate.shouldAutoShowHowTo()) {
        unawaited(_router.push('/how-to?firstRun=1'));
      }
    });
  }

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
      theme: AspTheme.dark,
      themeMode: ThemeMode.dark,
    );
  }
}
