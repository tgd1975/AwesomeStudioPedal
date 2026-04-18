// clang-format off
#include <BleKeyboard.h>  // must precede i_ble_keyboard.h
// clang-format on
#include "ble_config_service.h"
#include "ble_config_reassembler.h"
#include "file_system.h"

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

// UUIDs from docs/developers/BLE_CONFIG_PROTOCOL.md
static const char* SERVICE_UUID = "516515c0-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_WRITE_UUID = "516515c1-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_WRITE_HW_UUID = "516515c2-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_STATUS_UUID = "516515c3-4b50-447b-8ca3-cbfce3f4d9f8";

static NimBLECharacteristic* statusChar_ = nullptr;
static BleConfigReassembler* reassembler_ = nullptr;

// ---- NimBLE callbacks ----

class ProfileWriteCallback : public NimBLECharacteristicCallbacks
{
    bool isHw_;

public:
    explicit ProfileWriteCallback(bool isHw) : isHw_(isHw) {}

    void onWrite(NimBLECharacteristic* c) override
    {
        const std::string& val = c->getValue();
        if (reassembler_)
            reassembler_->onChunk(reinterpret_cast<const uint8_t*>(val.data()), val.size(), isHw_);
    }
};

// ---- BleConfigService::begin ----

void BleConfigService::begin(ProfileManager* pm,
                             IBleKeyboard* kb,
                             std::vector<ILEDController*> leds)
{
    IFileSystem* fs = createFileSystem();
    ILogger* logger = createLogger();

    auto notifyCb = [](const char* s)
    {
        if (statusChar_)
            statusChar_->notify(reinterpret_cast<const uint8_t*>(s), strlen(s));
    };

    reassembler_ = new BleConfigReassembler(
        pm,
        kb,
        fs,
        logger,
        leds,
        notifyCb,
        [](uint32_t ms) { delay(ms); },
        []() -> uint32_t { return millis(); });

    NimBLEServer* server = NimBLEDevice::getServer();
    if (! server)
    {
        Serial.println("BleConfigService: NimBLEDevice not initialised");
        return;
    }

    NimBLEService* svc = server->createService(SERVICE_UUID);

    svc->createCharacteristic(CHAR_WRITE_UUID, NIMBLE_PROPERTY::WRITE_NR)
        ->setCallbacks(new ProfileWriteCallback(false));

    svc->createCharacteristic(CHAR_WRITE_HW_UUID, NIMBLE_PROPERTY::WRITE_NR)
        ->setCallbacks(new ProfileWriteCallback(true));

    statusChar_ = svc->createCharacteristic(CHAR_STATUS_UUID, NIMBLE_PROPERTY::NOTIFY);

    svc->start();
    Serial.println("BleConfigService: GATT service started");
}

void BleConfigService::loop()
{
    // Nothing to poll — work is driven by BLE write callbacks.
}

bool BleConfigService::isApplying() const { return reassembler_ && reassembler_->isApplying(); }

// Forward declaration for platform-specific factory
IFileSystem* createFileSystem();
ILogger* createLogger();
