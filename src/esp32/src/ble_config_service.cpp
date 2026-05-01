// clang-format off
#include <BleKeyboard.h>  // must precede i_ble_keyboard.h
// clang-format on
#include "ble_config_service.h"
#include "ble_config_reassembler.h"
#include "ble_keyboard_adapter.h"
#include "config.h"
#include "file_system.h"
#include "version.h"

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <esp_system.h>
#include <string>

// UUIDs from docs/developers/BLE_CONFIG_PROTOCOL.md
static const char* SERVICE_UUID = "516515c0-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_WRITE_UUID = "516515c1-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_WRITE_HW_UUID = "516515c2-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_STATUS_UUID = "516515c3-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_HW_IDENTITY_UUID = "516515c4-4b50-447b-8ca3-cbfce3f4d9f8";
static const char* CHAR_FIRMWARE_VERSION_UUID = "516515c5-4b50-447b-8ca3-cbfce3f4d9f8";

static NimBLECharacteristic* statusChar_ = nullptr;
static BleConfigReassembler* reassembler_ = nullptr;

IFileSystem* createFileSystem();
ILogger* createLogger();

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

class StatusCharCallback : public NimBLECharacteristicCallbacks
{
public:
    void onSubscribe(NimBLECharacteristic* c, ble_gap_conn_desc* desc, uint16_t subValue) override
    {
        Serial.printf("BleConfigService: onSubscribe subValue=%d\n", subValue);
    }
};

// ---- GATT setup helper ----

static void setupGattService(BLEServer* pServer)
{
    // NOTE: do NOT replace pServer->setCallbacks here. BleKeyboard's
    // onConnect() sets its internal `connected` flag, which BleKeyboard::
    // isConnected() returns. main.cpp gates attachInterrupts() and the
    // blue LED on that flag, so replacing the callbacks would make HID
    // input silently broken. NimBLE already restarts advertising on
    // disconnect by default (m_advertiseOnDisconnect = true), so we do
    // not need a custom onDisconnect either.

    NimBLEService* svc = pServer->createService(SERVICE_UUID);

    svc->createCharacteristic(CHAR_WRITE_UUID, NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::WRITE)
        ->setCallbacks(new ProfileWriteCallback(false));

    svc->createCharacteristic(CHAR_WRITE_HW_UUID,
                              NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::WRITE)
        ->setCallbacks(new ProfileWriteCallback(true));

    statusChar_ = svc->createCharacteristic(CHAR_STATUS_UUID, NIMBLE_PROPERTY::NOTIFY);
    statusChar_->setCallbacks(new StatusCharCallback());

    NimBLECharacteristic* hwChar =
        svc->createCharacteristic(CHAR_HW_IDENTITY_UUID, NIMBLE_PROPERTY::READ);
    // Wrap in std::string — NimBLE's template setValue(const T&) with
    // T=const char* stores sizeof(pointer) bytes (the address) instead of the
    // string contents. std::string has c_str()/length() so it takes the
    // string-specific overload. See TASK-235.
    hwChar->setValue(std::string(hardwareConfig.hardware));

    NimBLECharacteristic* fwChar =
        svc->createCharacteristic(CHAR_FIRMWARE_VERSION_UUID, NIMBLE_PROPERTY::READ);
    fwChar->setValue(std::string(FIRMWARE_VERSION));

    svc->start();
    Serial.println("BleConfigService: GATT service registered");
}

static void initReassembler(ProfileManager* pm, IBleKeyboard* kb, std::vector<ILEDController*> leds)
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
        []() -> uint32_t { return millis(); },
        []()
        {
            // CONFIG_WRITE_HW path: pin/LED/button-count assignments are loaded
            // at boot. esp_restart() reapplies the freshly-written /config.json
            // on the next boot. See TASK-273.
            Serial.println("BleConfigService: rebooting to apply new hardware config");
            Serial.flush();
            esp_restart();
        });
}

// ---- BleConfigService::begin (production — via BleKeyboardAdapter onStarted hook) ----
//
// Must be called BEFORE bleAdapter->begin().  We register an onStarted callback
// on the adapter so our GATT service is created inside BleKeyboard::begin(),
// after hid->startServices() but before adv->start().  This is the only safe
// window to add services when USE_NIMBLE is defined: ble_gatts_start() is called
// once, locking in all services atomically.

void BleConfigService::begin(ProfileManager* pm,
                             IBleKeyboard* kb,
                             std::vector<ILEDController*> leds)
{
    initReassembler(pm, kb, leds);

    auto* adapter = static_cast<BleKeyboardAdapter*>(kb);
    adapter->setOnStartedCallback(
        [](BLEServer* pServer)
        {
            // Override the primary advertisement to only include our config service UUID.
            // BleKeyboard adds HID UUID (0x1812) which triggers BlueZ's HID daemon to
            // auto-connect and read encrypted HID characteristics — failing without a bond
            // and causing disconnect.
            setupGattService(pServer);
        });
}

void BleConfigService::loop() {}

bool BleConfigService::isApplying() const { return reassembler_ && reassembler_->isApplying(); }
