#include "ble_config_reassembler.h"
#include "config_loader.h"
#include "file_system.h"
#include "i_logger.h"
#include "mock_led_controller.h"
#include "null_logger.h"
#include "profile_manager.h"
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Exactly;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::vector<uint8_t> makePacket(uint16_t seq, const std::string& payload = "")
{
    std::vector<uint8_t> pkt;
    pkt.push_back(static_cast<uint8_t>(seq >> 8));
    pkt.push_back(static_cast<uint8_t>(seq & 0xFF));
    pkt.insert(pkt.end(), payload.begin(), payload.end());
    return pkt;
}

static void feedChunks(BleConfigReassembler& r, const std::string& json, bool isHw = false)
{
    constexpr size_t PAYLOAD = 510;
    uint16_t seq = 0;
    size_t off = 0;
    while (off < json.size())
    {
        auto pkt = makePacket(seq++, json.substr(off, PAYLOAD));
        r.onChunk(pkt.data(), pkt.size(), isHw);
        off += PAYLOAD;
    }
    // sentinel
    auto sentinel = makePacket(0xFFFF);
    r.onChunk(sentinel.data(), sentinel.size(), isHw);
}

// ---------------------------------------------------------------------------
// Fakes
// ---------------------------------------------------------------------------

class FakeFS : public IFileSystem
{
public:
    std::string lastPath;
    std::string lastContent;
    bool writeOk = true;
    std::string readContent;
    bool readOk = false;

    bool exists(const char*) override { return false; }
    bool readFile(const char*, std::string& out) override
    {
        out = readContent;
        return readOk;
    }
    bool writeFile(const char* path, const std::string& content) override
    {
        lastPath = path;
        lastContent = content;
        return writeOk;
    }
};

class MockKeyboard : public IBleKeyboard
{
public:
    void begin() override {}
    bool isConnected() override { return true; }
    void write(uint8_t) override {}
    void write(const MediaKeyReport) override {}
    void print(const char*) override {}
};

static const char* VALID_JSON = R"({
    "profiles": [{
        "name": "Test",
        "buttons": {
            "A": {"type": "SendStringAction", "value": "hello"}
        }
    }]
})";

// ---------------------------------------------------------------------------
// Test fixture
// ---------------------------------------------------------------------------

class BleConfigServiceTest : public ::testing::Test
{
protected:
    MockLEDController led1, led2, led3;
    ProfileManager pm{std::vector<ILEDController*>{&led1, &led2, &led3}};
    MockKeyboard kb;
    FakeFS fs;
    NullLogger logger;
    std::string lastStatus;
    std::vector<uint32_t> delayLog;

    std::unique_ptr<BleConfigReassembler> makeReassembler()
    {
        lastStatus.clear();
        delayLog.clear();
        return std::make_unique<BleConfigReassembler>(
            &pm,
            &kb,
            &fs,
            &logger,
            std::vector<ILEDController*>{&led1, &led2, &led3},
            [this](const char* s) { lastStatus = s; },
            [this](uint32_t ms) { delayLog.push_back(ms); },
            []() -> uint32_t { return 0; });
    }

    void SetUp() override
    {
        // Suppress LED strict-mock errors by allowing any calls
        EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
        EXPECT_CALL(led1, update(_)).Times(AnyNumber());
        EXPECT_CALL(led2, update(_)).Times(AnyNumber());
        EXPECT_CALL(led3, update(_)).Times(AnyNumber());
        EXPECT_CALL(led1, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led2, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led3, startBlink(_, _)).Times(AnyNumber());
        EXPECT_CALL(led1, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led2, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led3, stopBlink()).Times(AnyNumber());
        EXPECT_CALL(led1, isBlinking()).Times(AnyNumber()).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(led2, isBlinking()).Times(AnyNumber()).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(led3, isBlinking()).Times(AnyNumber()).WillRepeatedly(::testing::Return(false));
        EXPECT_CALL(led1, setup(_)).Times(AnyNumber());
        EXPECT_CALL(led2, setup(_)).Times(AnyNumber());
        EXPECT_CALL(led3, setup(_)).Times(AnyNumber());
        EXPECT_CALL(led1, toggle()).Times(AnyNumber());
        EXPECT_CALL(led2, toggle()).Times(AnyNumber());
        EXPECT_CALL(led3, toggle()).Times(AnyNumber());
    }
};

// ---------------------------------------------------------------------------
// Happy path
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, HappyPath_OkNotified)
{
    auto r = makeReassembler();
    EXPECT_FALSE(r->isApplying());

    auto pkt0 =
        makePacket(0x0000, std::string(VALID_JSON, std::min((size_t) 510, strlen(VALID_JSON))));
    r->onChunk(pkt0.data(), pkt0.size(), false);
    EXPECT_TRUE(r->isApplying());

    auto sentinel = makePacket(0xFFFF);
    r->onChunk(sentinel.data(), sentinel.size(), false);

    EXPECT_EQ(lastStatus, "OK");
    EXPECT_FALSE(r->isApplying());
}

TEST_F(BleConfigServiceTest, HappyPath_ConfigureProfilesCalledOnce)
{
    auto r = makeReassembler();
    feedChunks(*r, VALID_JSON, false);
    EXPECT_EQ(lastStatus, "OK");
    // Profile "Test" loaded
    ASSERT_NE(pm.getProfile(0), nullptr);
    EXPECT_EQ(pm.getProfile(0)->getName(), "Test");
}

TEST_F(BleConfigServiceTest, HappyPath_SavesToLittleFS)
{
    auto r = makeReassembler();
    feedChunks(*r, VALID_JSON, false);
    EXPECT_EQ(lastStatus, "OK");
    EXPECT_EQ(fs.lastPath, "/profiles.json");
    EXPECT_FALSE(fs.lastContent.empty());
}

// ---------------------------------------------------------------------------
// LED blink — success: 3 × on(150ms)/off(150ms)
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, BlinkSuccess_SixDelays)
{
    auto r = makeReassembler();
    feedChunks(*r, VALID_JSON, false);
    EXPECT_EQ(lastStatus, "OK");
    // 3 on/off cycles = 6 delay calls of 150 ms each
    ASSERT_EQ(delayLog.size(), 6u);
    for (auto ms : delayLog)
        EXPECT_EQ(ms, 150u);
}

// ---------------------------------------------------------------------------
// LED blink — failure: single 500 ms blink
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, BlinkFailure_OneDelay500)
{
    auto r = makeReassembler();
    // Feed invalid JSON
    auto pkt = makePacket(0x0000, "not json{{{{");
    r->onChunk(pkt.data(), pkt.size(), false);
    auto sentinel = makePacket(0xFFFF);
    r->onChunk(sentinel.data(), sentinel.size(), false);
    EXPECT_NE(lastStatus, "OK");
    ASSERT_EQ(delayLog.size(), 1u);
    EXPECT_EQ(delayLog[0], 500u);
}

// ---------------------------------------------------------------------------
// Out-of-order sequence
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, OutOfOrderSeq_ErrorBadSequence)
{
    auto r = makeReassembler();
    auto pkt0 = makePacket(0x0000, "chunk0");
    r->onChunk(pkt0.data(), pkt0.size(), false);
    // Send seq=2 instead of seq=1
    auto pkt2 = makePacket(0x0002, "chunk2");
    r->onChunk(pkt2.data(), pkt2.size(), false);
    EXPECT_EQ(lastStatus, "ERROR:bad_sequence");
    EXPECT_FALSE(r->isApplying()); // transfer reset
}

// ---------------------------------------------------------------------------
// Oversized payload
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, OversizedPayload_ErrorTooLarge)
{
    auto r = makeReassembler();
    std::string big(MAX_CONFIG_BYTES + 1, 'x');
    auto pkt0 = makePacket(0x0000, big.substr(0, 510));
    r->onChunk(pkt0.data(), pkt0.size(), false);
    // Fill up until over limit
    uint16_t seq = 1;
    size_t total = 510;
    while (total <= MAX_CONFIG_BYTES)
    {
        auto pkt = makePacket(seq++, std::string(510, 'x'));
        r->onChunk(pkt.data(), pkt.size(), false);
        total += 510;
    }
    EXPECT_EQ(lastStatus, "ERROR:too_large");
    EXPECT_FALSE(r->isApplying());
}

// ---------------------------------------------------------------------------
// Invalid JSON
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, InvalidJson_ErrorParseFailed)
{
    auto r = makeReassembler();
    auto pkt = makePacket(0x0000, "{bad json");
    r->onChunk(pkt.data(), pkt.size(), false);
    auto sentinel = makePacket(0xFFFF);
    r->onChunk(sentinel.data(), sentinel.size(), false);
    EXPECT_EQ(lastStatus, "ERROR:parse_failed");
}

// ---------------------------------------------------------------------------
// Concurrent upload rejection
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, ConcurrentUpload_BusyNotified)
{
    auto r = makeReassembler();
    auto pkt0 = makePacket(0x0000, "first");
    r->onChunk(pkt0.data(), pkt0.size(), false);
    EXPECT_TRUE(r->isApplying());

    // Second transfer start (seq=0)
    auto pkt0b = makePacket(0x0000, "second");
    r->onChunk(pkt0b.data(), pkt0b.size(), false);
    EXPECT_EQ(lastStatus, "BUSY");
    EXPECT_TRUE(r->isApplying()); // ongoing transfer unaffected
}

// ---------------------------------------------------------------------------
// Hardware config transfer
// ---------------------------------------------------------------------------

TEST_F(BleConfigServiceTest, HwTransfer_SavesConfigJson)
{
    auto r = makeReassembler();
    std::string hwJson =
        R"({"numProfiles":4,"numSelectLeds":2,"numButtons":2,"ledBluetooth":26,"ledPower":25,"ledSelect":[5,18],"buttonSelect":21,"buttonPins":[13,12]})";
    feedChunks(*r, hwJson, true);
    EXPECT_EQ(lastStatus, "OK");
    EXPECT_EQ(fs.lastPath, "/config.json");
    EXPECT_EQ(fs.lastContent, hwJson);
}

TEST_F(BleConfigServiceTest, HwTransfer_WriteFail_ErrorNotified)
{
    fs.writeOk = false;
    auto r = makeReassembler();
    feedChunks(*r, "{}", true);
    EXPECT_EQ(lastStatus, "ERROR:write_failed");
}
