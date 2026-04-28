class UploadResult {
  const UploadResult.success()
      : success = true,
        errorMessage = null;
  const UploadResult.failure(String message)
      : success = false,
        errorMessage = message;

  final bool success;
  final String? errorMessage;
}
