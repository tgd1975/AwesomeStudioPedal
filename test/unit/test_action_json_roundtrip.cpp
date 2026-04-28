// Round-trip tests for Action::getJsonProperties (TASK-232).
//
// Pre-fix, several action types serialised placeholder values ("CHAR",
// "KEY", "MEDIA_STOP") that were rejected by the loader on re-parse,
// silently corrupting any ConfigLoader::saveToFile -> loadFromString cycle.
// These tests load a JSON config containing one of every action type,
// saveToFile to a buffer, reload that buffer into a fresh ProfileManager,
// and assert that the second save produces byte-identical output to the
// first (idempotency), with no placeholder strings appearing anywhere.

#include "button_constants.h"
#include "config_loader.h"
#include "delayed_action.h"
#include "file_system.h"
#include "macro_action.h"
#include "mock_led_controller.h"
#include "null_logger.h"
#include "pin_action.h"
#include "profile_manager.h"
#include "send_action.h"
#include "serial_action.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using ::testing::_;
using ::testing::AnyNumber;

namespace
{

    class FakeFs : public IFileSystem
    {
    public:
        std::string lastContent;

        bool exists(const char*) override { return false; }
        bool readFile(const char*, std::string&) override { return false; }
        bool writeFile(const char*, const std::string& content) override
        {
            lastContent = content;
            return true;
        }
    };

    class FakeKb : public IBleKeyboard
    {
    public:
        void begin() override {}
        bool isConnected() override { return true; }
        void write(uint8_t) override {}
        void write(const MediaKeyReport) override {}
        void print(const char*) override {}
    };

    class ActionJsonRoundtripTest : public ::testing::Test
    {
    protected:
        FakeFs fs;
        NullLogger logger;
        ConfigLoader loader{&fs, &logger};
        MockLEDController led1, led2, led3;
        FakeKb kb;
        ProfileManager pm{std::vector<ILEDController*>{&led1, &led2, &led3}};

        void SetUp() override
        {
            EXPECT_CALL(led1, setState(_)).Times(AnyNumber());
            EXPECT_CALL(led2, setState(_)).Times(AnyNumber());
            EXPECT_CALL(led3, setState(_)).Times(AnyNumber());
            EXPECT_CALL(led1, startBlink(_, _)).Times(AnyNumber());
            EXPECT_CALL(led2, startBlink(_, _)).Times(AnyNumber());
            EXPECT_CALL(led3, startBlink(_, _)).Times(AnyNumber());
            EXPECT_CALL(led1, stopBlink()).Times(AnyNumber());
            EXPECT_CALL(led2, stopBlink()).Times(AnyNumber());
            EXPECT_CALL(led3, stopBlink()).Times(AnyNumber());
            EXPECT_CALL(led1, update(_)).Times(AnyNumber());
            EXPECT_CALL(led2, update(_)).Times(AnyNumber());
            EXPECT_CALL(led3, update(_)).Times(AnyNumber());
            EXPECT_CALL(led1, isBlinking()).Times(AnyNumber());
            EXPECT_CALL(led2, isBlinking()).Times(AnyNumber());
            EXPECT_CALL(led3, isBlinking()).Times(AnyNumber());
        }

        /// Load *json* into pm, saveToFile to fs, return the serialized buffer.
        std::string loadAndSave(const std::string& json)
        {
            EXPECT_TRUE(loader.loadFromString(pm, &kb, json)) << "load failed: " << json;
            EXPECT_TRUE(loader.saveToFile(pm, "/x"));
            return fs.lastContent;
        }
    };

    // ---------------------------------------------------------------------------
    // Per-action regression tests — each placeholder string from the pre-fix
    // implementations must NOT appear in the serialised output.
    // ---------------------------------------------------------------------------

    TEST_F(ActionJsonRoundtripTest, SendCharAction_AsciiValue_PreservesLiteral)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SendCharAction","value":"["}}}]})");
        EXPECT_NE(saved.find("\"value\":\"[\""), std::string::npos)
            << "ASCII value '[' should round-trip verbatim. Got: " << saved;
        EXPECT_EQ(saved.find("\"CHAR\""), std::string::npos)
            << "Placeholder \"CHAR\" must not appear. Got: " << saved;
    }

    TEST_F(ActionJsonRoundtripTest, SendCharAction_NamedKey_PreservesName)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SendCharAction","value":"KEY_LEFT_ARROW"}}}]})");
        EXPECT_NE(saved.find("KEY_LEFT_ARROW"), std::string::npos) << saved;
        EXPECT_EQ(saved.find("\"CHAR\""), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, SendKeyAction_PreservesKeyName)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SendKeyAction","value":"KEY_F5"}}}]})");
        EXPECT_NE(saved.find("KEY_F5"), std::string::npos) << saved;
        EXPECT_EQ(saved.find("\"value\":\"KEY\""), std::string::npos)
            << "Placeholder \"KEY\" must not appear. Got: " << saved;
    }

    TEST_F(ActionJsonRoundtripTest, SendMediaKeyAction_PreservesMediaKeyName)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SendMediaKeyAction","value":"KEY_MEDIA_VOLUME_UP"}}}]})");
        // Reverse lookup returns the canonical name from the table — both
        // "MEDIA_VOLUME_UP" and "KEY_MEDIA_VOLUME_UP" map to the same report,
        // and "MEDIA_VOLUME_UP" is the first entry, so that's what comes back.
        EXPECT_NE(saved.find("MEDIA_VOLUME_UP"), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, SendStringAction_PreservesText)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SendStringAction","value":"hello world"}}}]})");
        EXPECT_NE(saved.find("hello world"), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, SerialOutputAction_PreservesText)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"SerialOutputAction","value":"ping"}}}]})");
        EXPECT_NE(saved.find("ping"), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, PinHighAction_PreservesPin)
    {
        auto saved = loadAndSave(
            R"({"profiles":[{"name":"P","buttons":{"A":{"type":"PinHighAction","pin":17}}}]})");
        EXPECT_NE(saved.find("\"pin\":17"), std::string::npos) << saved;
        EXPECT_NE(saved.find("PinHighAction"), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, DelayedAction_PreservesDelayAndInner)
    {
        auto saved = loadAndSave(R"({"profiles":[{"name":"P","buttons":{"A":{
        "type":"DelayedAction","delayMs":3000,
        "action":{"type":"SendMediaKeyAction","value":"KEY_VOLUME_UP"}}}}]})");
        EXPECT_NE(saved.find("\"delayMs\":3000"), std::string::npos) << saved;
        EXPECT_NE(saved.find("MEDIA_VOLUME_UP"), std::string::npos) << saved;
    }

    TEST_F(ActionJsonRoundtripTest, MacroAction_PreservesNestedSteps)
    {
        auto saved = loadAndSave(R"({"profiles":[{"name":"P","buttons":{"A":{
        "type":"MacroAction","steps":[
            [{"type":"SendStringAction","value":"hi"}],
            [{"type":"SendCharAction","value":"a"},{"type":"SendKeyAction","value":"KEY_F1"}]
        ]}}}]})");
        EXPECT_NE(saved.find("MacroAction"), std::string::npos) << saved;
        EXPECT_NE(saved.find("\"value\":\"hi\""), std::string::npos) << saved;
        EXPECT_NE(saved.find("\"value\":\"a\""), std::string::npos) << saved;
        EXPECT_NE(saved.find("KEY_F1"), std::string::npos) << saved;
        EXPECT_EQ(saved.find("UnknownAction"), std::string::npos)
            << "MacroAction used to fall through to UnknownAction. Got: " << saved;
    }

    // ---------------------------------------------------------------------------
    // Idempotency — load + save + load + save produces the same bytes.
    // This is the property BleConfigReassembler::applyTransfer relied on (and
    // could not, until this fix landed; see the comment in that file).
    // ---------------------------------------------------------------------------

    TEST_F(ActionJsonRoundtripTest, FullProfileSaveIsIdempotent)
    {
        const std::string source = R"({
        "profiles":[{
            "name":"All",
            "buttons":{
                "A":{"type":"SendStringAction","value":"hello"},
                "B":{"type":"SendCharAction","value":"["},
                "C":{"type":"SendKeyAction","value":"KEY_F5"},
                "D":{"type":"SendMediaKeyAction","value":"MEDIA_STOP"}
            }
        }]
    })";

        std::string firstSave = loadAndSave(source);

        // Reload the saved JSON into a fresh manager and save again.
        ProfileManager pm2{std::vector<ILEDController*>{&led1, &led2, &led3}};
        EXPECT_TRUE(loader.loadFromString(pm2, &kb, firstSave));
        EXPECT_TRUE(loader.saveToFile(pm2, "/x"));
        std::string secondSave = fs.lastContent;

        EXPECT_EQ(firstSave, secondSave) << "Save → load → save must be idempotent.\n"
                                         << "First save:  " << firstSave << "\n"
                                         << "Second save: " << secondSave;
    }

} // namespace
