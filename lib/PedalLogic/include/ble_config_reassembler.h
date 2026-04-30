#pragma once
#include "file_system.h"
#include "i_ble_keyboard.h"
#include "i_led_controller.h"
#include "i_logger.h"
#include "profile_manager.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

// Wire ceiling for an uploaded config (profiles or hardware config). Anything
// larger is rejected by the reassembler before the parser ever sees it.
//
// JSON_DOC_CAPACITY is the matching ArduinoJson 6 DynamicJsonDocument capacity
// used by every parse/serialise site that touches an uploaded config. AJ6
// sizes the *token tree*, not the input string — rule of thumb is ~2.5× the
// JSON length for our shape (profiles → buttons → action objects), so 16 KB
// JSON needs ~40 KB of token storage; we round up to 48 KB for headroom on
// deeply-nested macros.
//
// CLI (scripts/pedal_config.py) and the Flutter app pre-flight on
// MAX_CONFIG_BYTES so the user gets a clear "file too large" message instead
// of a cryptic ERROR:parse_failed after a multi-second BLE transfer.
//
// Keep the three values aligned: bumping one without the others reintroduces
// the TASK-240 mismatch (the firmware previously advertised 32 KB on the wire
// but parsed only ~8 KB).
#ifndef MAX_CONFIG_BYTES
#define MAX_CONFIG_BYTES 16384 // NOLINT(cppcoreguidelines-macro-usage)
#endif

#ifndef JSON_DOC_CAPACITY
#define JSON_DOC_CAPACITY 49152 // NOLINT(cppcoreguidelines-macro-usage)
#endif

/**
 * @class BleConfigReassembler
 * @brief Platform-independent BLE chunked-transfer reassembly and apply logic.
 *
 * Implements the protocol from docs/developers/BLE_CONFIG_PROTOCOL.md.
 * Feed incoming BLE write packets via onChunk(); receive status callbacks
 * via setStatusCallback(). All LED and profile-apply logic is also here so
 * it can be unit-tested without NimBLE.
 */
class BleConfigReassembler
{
public:
    using StatusCallback = std::function<void(const char*)>;
    using DelayFn = std::function<void(uint32_t ms)>;
    using MillisFn = std::function<uint32_t()>;
    using RebootFn = std::function<void()>;

    explicit BleConfigReassembler(ProfileManager* pm,
                                  IBleKeyboard* kb,
                                  IFileSystem* fs,
                                  ILogger* logger,
                                  std::vector<ILEDController*> selectLeds,
                                  StatusCallback statusCb,
                                  DelayFn delayFn = nullptr,
                                  MillisFn millisFn = nullptr,
                                  RebootFn rebootFn = nullptr);

    /** Feed one BLE write packet (sequence number prefix + payload). */
    void onChunk(const uint8_t* data, size_t len, bool isHw);

    bool isApplying() const { return transferInProgress_; }

private:
    void applyTransfer();
    void applyHwTransfer();
    void resetTransfer();
    void notifyStatus(const char* s);
    void blinkSuccess();
    void blinkFailure();

    ProfileManager* pm_;
    IBleKeyboard* kb_;
    IFileSystem* fs_;
    ILogger* logger_;
    std::vector<ILEDController*> selectLeds_;
    StatusCallback statusCb_;
    DelayFn delay_;
    MillisFn millis_;
    RebootFn reboot_;

    bool transferInProgress_ = false;
    bool hwTransfer_ = false;
    uint16_t nextExpectedSeq_ = 0;
    std::string buffer_;
};
