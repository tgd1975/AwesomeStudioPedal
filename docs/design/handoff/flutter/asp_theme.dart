/// ASP — Flutter ThemeData scaffold
///
/// Generated from the design system. Apply to your MaterialApp:
///
///   MaterialApp(
///     theme: AspTheme.dark,
///     ...
///   );
///
/// Fonts: add to pubspec.yaml (recommended):
///   fonts:
///     - family: Metropolis
///       fonts:
///         - asset: assets/fonts/Metropolis-Regular.ttf
///         - asset: assets/fonts/Metropolis-Medium.ttf
///           weight: 500
///         - asset: assets/fonts/Metropolis-SemiBold.ttf
///           weight: 600
///         - asset: assets/fonts/Metropolis-Bold.ttf
///           weight: 700
///     - family: JetBrainsMono
///       fonts:
///         - asset: assets/fonts/JetBrainsMono-Regular.ttf
///         - asset: assets/fonts/JetBrainsMono-Medium.ttf
///           weight: 500

import 'package:flutter/material.dart';

class AspTokens {
  // Palette
  static const bg            = Color(0xFF0F0F12);
  static const surface       = Color(0xFF17171E);
  static const surfaceRaised = Color(0xFF1E1E28);

  static const border        = Color(0x1AFFFFFF); // rgba(255,255,255,0.10)
  static const borderStrong  = Color(0x2EFFFFFF); // rgba(255,255,255,0.18)

  static const text          = Color(0xFFE8E8F0);
  static const textMuted     = Color(0x8CE8E8F0); // alpha 0.55
  static const textDim       = Color(0x59E8E8F0); // alpha 0.35

  // Accent — warm amber
  static const accent        = Color(0xFFF0B030);
  static const accentInk     = Color(0xFF1A1208);

  // Semantic
  static const success       = Color(0xFF34C87A);
  static const error         = Color(0xFFE84040);
  static const warning       = Color(0xFFF0B030);

  // LED indicator colours
  static const ledPower      = Color(0xFF44FF44);
  static const ledBluetooth  = Color(0xFF4488FF);
  static const ledProfile    = Color(0xFFFFB020);

  // Radius
  static const radiusSm     = 2.0;
  static const radiusMd     = 3.0;
  static const radiusCard   = 4.0;
  static const radiusDialog = 6.0;
}

class AspTheme {
  static ThemeData get dark {
    const display = 'Metropolis';
    const mono    = 'JetBrainsMono';

    final scheme = const ColorScheme.dark(
      brightness: Brightness.dark,
      primary: AspTokens.accent,
      onPrimary: AspTokens.accentInk,
      secondary: AspTokens.accent,
      onSecondary: AspTokens.accentInk,
      surface: AspTokens.surface,
      onSurface: AspTokens.text,
      background: AspTokens.bg,
      onBackground: AspTokens.text,
      error: AspTokens.error,
      onError: AspTokens.text,
    );

    return ThemeData(
      useMaterial3: true,
      brightness: Brightness.dark,
      colorScheme: scheme,
      scaffoldBackgroundColor: AspTokens.bg,
      canvasColor: AspTokens.surface,
      dividerColor: AspTokens.border,
      fontFamily: display,

      textTheme: const TextTheme(
        // Display / titles — Metropolis light, tight tracking
        displayLarge:  TextStyle(fontFamily: display, fontWeight: FontWeight.w300, fontSize: 56, letterSpacing: 0.56, color: AspTokens.text),
        displayMedium: TextStyle(fontFamily: display, fontWeight: FontWeight.w400, fontSize: 44, letterSpacing: 0.44, color: AspTokens.text),
        displaySmall:  TextStyle(fontFamily: display, fontWeight: FontWeight.w400, fontSize: 32, letterSpacing: 0.32, color: AspTokens.text),

        headlineMedium: TextStyle(fontFamily: display, fontWeight: FontWeight.w600, fontSize: 22, letterSpacing: 0.22, color: AspTokens.text),
        titleLarge:     TextStyle(fontFamily: display, fontWeight: FontWeight.w600, fontSize: 18, letterSpacing: 0.18, color: AspTokens.text),

        bodyLarge:  TextStyle(fontFamily: display, fontWeight: FontWeight.w400, fontSize: 15, color: AspTokens.text),
        bodyMedium: TextStyle(fontFamily: display, fontWeight: FontWeight.w400, fontSize: 13, color: AspTokens.text),

        // Labels are JetBrains Mono small-caps, wide tracking.
        labelLarge:  TextStyle(fontFamily: mono, fontWeight: FontWeight.w500, fontSize: 12, letterSpacing: 1.4, color: AspTokens.text),
        labelMedium: TextStyle(fontFamily: mono, fontWeight: FontWeight.w500, fontSize: 11, letterSpacing: 1.3, color: AspTokens.textMuted),
        labelSmall:  TextStyle(fontFamily: mono, fontWeight: FontWeight.w500, fontSize:  9, letterSpacing: 1.2, color: AspTokens.textMuted),
      ),

      appBarTheme: const AppBarTheme(
        backgroundColor: AspTokens.bg,
        surfaceTintColor: Colors.transparent,
        foregroundColor: AspTokens.text,
        elevation: 0,
        centerTitle: false,
        titleTextStyle: TextStyle(
          fontFamily: display, fontWeight: FontWeight.w600,
          fontSize: 20, color: AspTokens.text,
        ),
      ),

      cardTheme: CardTheme(
        color: AspTokens.surface,
        surfaceTintColor: Colors.transparent,
        elevation: 0,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(AspTokens.radiusCard),
          side: const BorderSide(color: AspTokens.border, width: 1),
        ),
      ),

      elevatedButtonTheme: ElevatedButtonThemeData(
        style: ElevatedButton.styleFrom(
          backgroundColor: AspTokens.accent,
          foregroundColor: AspTokens.accentInk,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(AspTokens.radiusSm),
          ),
          padding: const EdgeInsets.symmetric(horizontal: 18, vertical: 12),
          textStyle: const TextStyle(
            fontFamily: mono, fontWeight: FontWeight.w500,
            fontSize: 12, letterSpacing: 1.2,
          ),
        ),
      ),

      outlinedButtonTheme: OutlinedButtonThemeData(
        style: OutlinedButton.styleFrom(
          foregroundColor: AspTokens.text,
          side: const BorderSide(color: AspTokens.borderStrong),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(AspTokens.radiusSm),
          ),
        ),
      ),

      inputDecorationTheme: InputDecorationTheme(
        filled: true,
        fillColor: AspTokens.bg,
        contentPadding: const EdgeInsets.symmetric(horizontal: 14, vertical: 12),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(AspTokens.radiusSm),
          borderSide: const BorderSide(color: AspTokens.border),
        ),
        enabledBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(AspTokens.radiusSm),
          borderSide: const BorderSide(color: AspTokens.border),
        ),
        focusedBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(AspTokens.radiusSm),
          borderSide: const BorderSide(color: AspTokens.accent, width: 1.5),
        ),
        labelStyle: const TextStyle(
          fontFamily: mono, fontSize: 11, letterSpacing: 1.3,
          color: AspTokens.textMuted,
        ),
      ),

      chipTheme: const ChipThemeData(
        backgroundColor: AspTokens.surface,
        side: BorderSide(color: AspTokens.border),
        labelStyle: TextStyle(
          fontFamily: mono, fontWeight: FontWeight.w500,
          fontSize: 11, letterSpacing: 1.0, color: AspTokens.text,
        ),
      ),

      dividerTheme: const DividerThemeData(
        color: AspTokens.border, thickness: 1, space: 0,
      ),
    );
  }
}
