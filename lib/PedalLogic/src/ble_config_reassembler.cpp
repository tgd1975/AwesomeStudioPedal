#include "ble_config_reassembler.h"
#include "config_loader.h"

#ifndef HOST_TEST_BUILD
#include <Arduino.h>
#else
#include "arduino_shim.h"
#endif

static constexpr uint16_t END_SEQ = 0xFFFF;

BleConfigReassembler::BleConfigReassembler(ProfileManager* pm,
                                           IBleKeyboard* kb,
                                           IFileSystem* fs,
                                           ILogger* logger,
                                           std::vector<ILEDController*> selectLeds,
                                           StatusCallback statusCb,
                                           DelayFn delayFn,
                                           MillisFn millisFn)
    : pm_(pm), kb_(kb), fs_(fs), logger_(logger), selectLeds_(std::move(selectLeds)),
      statusCb_(std::move(statusCb)), delay_(std::move(delayFn)), millis_(std::move(millisFn))
{
#ifndef HOST_TEST_BUILD
    if (! delay_)
        delay_ = [](uint32_t ms) { delay(ms); };
    if (! millis_)
        millis_ = []() -> uint32_t { return static_cast<uint32_t>(millis()); };
#endif
}

void BleConfigReassembler::onChunk(const uint8_t* data, size_t len, bool isHw)
{
    if (len < 2)
        return;

    uint16_t seq = (static_cast<uint16_t>(data[0]) << 8) | data[1];

    if (seq == 0x0000)
    {
        if (transferInProgress_)
        {
            notifyStatus("BUSY");
            return;
        }
        transferInProgress_ = true;
        hwTransfer_ = isHw;
        nextExpectedSeq_ = 0;
        buffer_.clear();
    }

    if (! transferInProgress_)
        return;

    if (seq == END_SEQ)
    {
        if (hwTransfer_)
            applyHwTransfer();
        else
            applyTransfer();
        return;
    }

    if (seq != nextExpectedSeq_)
    {
        notifyStatus("ERROR:bad_sequence");
        resetTransfer();
        return;
    }

    size_t payloadLen = len - 2;
    if (buffer_.size() + payloadLen > MAX_CONFIG_BYTES)
    {
        notifyStatus("ERROR:too_large");
        resetTransfer();
        return;
    }

    buffer_.append(reinterpret_cast<const char*>(data + 2), payloadLen);
    nextExpectedSeq_++;
}

void BleConfigReassembler::applyTransfer()
{
    ConfigLoader loader(fs_, logger_);
    bool ok = loader.loadFromString(*pm_, kb_, buffer_);
    if (! ok)
    {
        notifyStatus("ERROR:parse_failed");
        blinkFailure();
        resetTransfer();
        return;
    }
    loader.saveToFile(*pm_, "/profiles.json");
    blinkSuccess();
    notifyStatus("OK");
    resetTransfer();
}

void BleConfigReassembler::applyHwTransfer()
{
    if (! fs_->writeFile("/config.json", buffer_))
    {
        notifyStatus("ERROR:write_failed");
        blinkFailure();
        resetTransfer();
        return;
    }
    blinkSuccess();
    notifyStatus("OK");
    resetTransfer();
}

void BleConfigReassembler::resetTransfer()
{
    transferInProgress_ = false;
    hwTransfer_ = false;
    nextExpectedSeq_ = 0;
    buffer_.clear();
}

void BleConfigReassembler::notifyStatus(const char* s)
{
    if (statusCb_)
        statusCb_(s);
}

void BleConfigReassembler::blinkSuccess()
{
    for (int i = 0; i < 3; i++)
    {
        for (auto* led : selectLeds_)
            led->setState(true);
        delay_(150);
        for (auto* led : selectLeds_)
            led->setState(false);
        delay_(150);
    }
    pm_->update(millis_());
}

void BleConfigReassembler::blinkFailure()
{
    for (auto* led : selectLeds_)
        led->setState(true);
    delay_(500);
    for (auto* led : selectLeds_)
        led->setState(false);
}
