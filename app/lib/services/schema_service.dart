import 'dart:convert';
import 'package:flutter/services.dart';
import 'package:json_schema/json_schema.dart';

class ValidationResult {
  ValidationResult.valid() : isValid = true, errors = const [];
  ValidationResult.invalid(this.errors) : isValid = false;

  final bool isValid;
  final List<String> errors;
}

class SchemaService {
  JsonSchema? _profilesSchema;
  JsonSchema? _configSchema;

  Future<void> _loadSchemas() async {
    if (_profilesSchema != null) return;
    final profilesRaw =
        await rootBundle.loadString('../data/profiles.schema.json');
    final configRaw = await rootBundle.loadString('../data/config.schema.json');
    _profilesSchema = JsonSchema.create(jsonDecode(profilesRaw));
    _configSchema = JsonSchema.create(jsonDecode(configRaw));
  }

  Future<ValidationResult> validateProfiles(Map<String, dynamic> json) async {
    await _loadSchemas();
    final result = _profilesSchema!.validate(json);
    if (result.isValid) return ValidationResult.valid();
    return ValidationResult.invalid(
      result.errors.map((e) => e.toString()).toList(),
    );
  }

  Future<ValidationResult> validateConfig(Map<String, dynamic> json) async {
    await _loadSchemas();
    final result = _configSchema!.validate(json);
    if (result.isValid) return ValidationResult.valid();
    return ValidationResult.invalid(
      result.errors.map((e) => e.toString()).toList(),
    );
  }
}
