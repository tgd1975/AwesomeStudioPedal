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

#ifndef MAX_CONFIG_BYTES
#define MAX_CONFIG_BYTES 32768
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

    explicit BleConfigReassembler(ProfileManager* pm,
                                  IBleKeyboard* kb,
                                  IFileSystem* fs,
                                  ILogger* logger,
                                  std::vector<ILEDController*> selectLeds,
                                  StatusCallback statusCb,
                                  DelayFn delayFn = nullptr,
                                  MillisFn millisFn = nullptr);

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

    bool transferInProgress_ = false;
    bool hwTransfer_ = false;
    uint16_t nextExpectedSeq_ = 0;
    std::string buffer_;
};
