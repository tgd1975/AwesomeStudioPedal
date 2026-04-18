"""Unit tests for pedal_config.py (no BLE hardware required)."""

import json
import pathlib
import struct
import sys
import tempfile
import unittest
from unittest.mock import MagicMock, patch

# Make scripts/ importable
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))
import pedal_config


# ---------------------------------------------------------------------------
# make_chunks
# ---------------------------------------------------------------------------

class TestMakeChunks(unittest.TestCase):

    def _seq(self, chunk: bytes) -> int:
        return struct.unpack(">H", chunk[:2])[0]

    def _payload(self, chunk: bytes) -> bytes:
        return chunk[2:]

    def test_empty_data_produces_only_sentinel(self):
        chunks = pedal_config.make_chunks(b"")
        self.assertEqual(len(chunks), 1)
        self.assertEqual(self._seq(chunks[0]), 0xFFFF)
        self.assertEqual(self._payload(chunks[0]), b"")

    def test_single_chunk_data(self):
        data = b"hello"
        chunks = pedal_config.make_chunks(data)
        self.assertEqual(len(chunks), 2)  # data chunk + sentinel
        self.assertEqual(self._seq(chunks[0]), 0)
        self.assertEqual(self._payload(chunks[0]), data)
        self.assertEqual(self._seq(chunks[1]), 0xFFFF)

    def test_exact_payload_size_is_one_chunk(self):
        data = b"x" * pedal_config.PAYLOAD_SIZE
        chunks = pedal_config.make_chunks(data)
        self.assertEqual(len(chunks), 2)
        self.assertEqual(self._seq(chunks[0]), 0)
        self.assertEqual(len(self._payload(chunks[0])), pedal_config.PAYLOAD_SIZE)

    def test_one_byte_over_splits_into_two_data_chunks(self):
        data = b"x" * (pedal_config.PAYLOAD_SIZE + 1)
        chunks = pedal_config.make_chunks(data)
        self.assertEqual(len(chunks), 3)  # 2 data + sentinel
        self.assertEqual(self._seq(chunks[0]), 0)
        self.assertEqual(self._seq(chunks[1]), 1)
        self.assertEqual(self._seq(chunks[2]), 0xFFFF)

    def test_sequence_numbers_are_contiguous(self):
        data = b"x" * (pedal_config.PAYLOAD_SIZE * 5 + 7)
        chunks = pedal_config.make_chunks(data)
        for i, chunk in enumerate(chunks[:-1]):  # skip sentinel
            self.assertEqual(self._seq(chunk), i)

    def test_reassembled_data_matches_input(self):
        data = b"abc" * 1000
        chunks = pedal_config.make_chunks(data)
        reassembled = b"".join(self._payload(c) for c in chunks[:-1])
        self.assertEqual(reassembled, data)

    def test_sentinel_always_last_and_empty(self):
        for size in [0, 1, pedal_config.PAYLOAD_SIZE, pedal_config.PAYLOAD_SIZE * 3]:
            with self.subTest(size=size):
                chunks = pedal_config.make_chunks(b"z" * size)
                self.assertEqual(self._seq(chunks[-1]), 0xFFFF)
                self.assertEqual(self._payload(chunks[-1]), b"")


# ---------------------------------------------------------------------------
# cmd_validate
# ---------------------------------------------------------------------------

VALID_PROFILES = {
    "profiles": [
        {
            "name": "Test",
            "buttons": {
                "A": {"type": "SendStringAction", "value": "hello"}
            }
        }
    ]
}

VALID_CONFIG = {
    "numProfiles": 4,
    "numSelectLeds": 2,
    "numButtons": 2,
    "ledBluetooth": 26,
    "ledPower": 25,
    "ledSelect": [5, 18],
    "buttonSelect": 21,
    "buttonPins": [13, 12]
}


class TestCmdValidate(unittest.TestCase):

    def _make_args(self, path: str, hw: bool = False) -> MagicMock:
        args = MagicMock()
        args.file = path
        args.hw = hw
        return args

    def test_valid_profiles_json_returns_0(self):
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump(VALID_PROFILES, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 0)

    def test_valid_config_json_returns_0(self):
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump(VALID_CONFIG, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path, hw=True))
        self.assertEqual(result, 0)

    def test_file_not_found_returns_1(self):
        result = pedal_config.cmd_validate(self._make_args("/nonexistent/file.json"))
        self.assertEqual(result, 1)

    def test_malformed_json_returns_1(self):
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            f.write("{bad json{{")
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 1)

    def test_profiles_json_missing_required_field_returns_1(self):
        # profiles array is required
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump({"not_profiles": []}, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 1)

    def test_config_json_missing_required_field_returns_1(self):
        bad = dict(VALID_CONFIG)
        del bad["numProfiles"]
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump(bad, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path, hw=True))
        self.assertEqual(result, 1)

    def test_profiles_json_empty_profiles_returns_1(self):
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump({"profiles": []}, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 1)

    def test_shipped_profiles_json_is_valid(self):
        repo_root = pathlib.Path(__file__).resolve().parent.parent.parent
        shipped = repo_root / "data" / "profiles.json"
        result = pedal_config.cmd_validate(self._make_args(str(shipped)))
        self.assertEqual(result, 0)

    def test_missing_type_field_in_button_action_returns_1(self):
        bad = {
            "profiles": [{
                "name": "Bad",
                "buttons": {
                    "A": {"value": "hello"}  # missing "type"
                }
            }]
        }
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump(bad, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 1)

    def test_invalid_action_type_string_returns_1(self):
        bad = {
            "profiles": [{
                "name": "Bad",
                "buttons": {
                    "A": {"type": "NotARealAction", "value": "hello"}
                }
            }]
        }
        with tempfile.NamedTemporaryFile(suffix=".json", mode="w", delete=False) as f:
            json.dump(bad, f)
            path = f.name
        result = pedal_config.cmd_validate(self._make_args(path))
        self.assertEqual(result, 1)


# ---------------------------------------------------------------------------
# cmd_upload characteristic routing
# ---------------------------------------------------------------------------

class TestCmdUploadRouting(unittest.TestCase):
    """Verify upload vs upload-config targets the right GATT characteristic."""

    def test_upload_uses_profile_char(self):
        captured = {}

        async def fake_upload(file_path, char_uuid):
            captured["char_uuid"] = char_uuid
            return 0

        with patch("pedal_config._upload", fake_upload):
            args = MagicMock()
            args.file = "profiles.json"
            args.hw = False
            pedal_config.cmd_upload(args)

        self.assertEqual(captured["char_uuid"], pedal_config.CHAR_WRITE_UUID)

    def test_upload_config_uses_hw_char(self):
        captured = {}

        async def fake_upload(file_path, char_uuid):
            captured["char_uuid"] = char_uuid
            return 0

        with patch("pedal_config._upload", fake_upload):
            args = MagicMock()
            args.file = "config.json"
            args.hw = True
            pedal_config.cmd_upload(args)

        self.assertEqual(captured["char_uuid"], pedal_config.CHAR_WRITE_HW_UUID)


# ---------------------------------------------------------------------------
# build_parser / subcommand routing
# ---------------------------------------------------------------------------

class TestBuildParser(unittest.TestCase):

    def test_upload_subcommand_parsed(self):
        parser = pedal_config.build_parser()
        args = parser.parse_args(["upload", "profiles.json"])
        self.assertEqual(args.command, "upload")
        self.assertEqual(args.file, "profiles.json")

    def test_upload_config_subcommand_parsed(self):
        parser = pedal_config.build_parser()
        args = parser.parse_args(["upload-config", "config.json"])
        self.assertEqual(args.command, "upload-config")
        self.assertEqual(args.file, "config.json")

    def test_validate_subcommand_no_hw_flag(self):
        parser = pedal_config.build_parser()
        args = parser.parse_args(["validate", "profiles.json"])
        self.assertEqual(args.command, "validate")
        self.assertFalse(args.hw)

    def test_validate_subcommand_with_hw_flag(self):
        parser = pedal_config.build_parser()
        args = parser.parse_args(["validate", "config.json", "--hw"])
        self.assertTrue(args.hw)

    def test_scan_subcommand_parsed(self):
        parser = pedal_config.build_parser()
        args = parser.parse_args(["scan"])
        self.assertEqual(args.command, "scan")

    def test_missing_subcommand_raises(self):
        parser = pedal_config.build_parser()
        with self.assertRaises(SystemExit):
            parser.parse_args([])


# ---------------------------------------------------------------------------
# Constants sanity
# ---------------------------------------------------------------------------

class TestConstants(unittest.TestCase):

    def test_payload_size_is_mtu_minus_2(self):
        self.assertEqual(pedal_config.PAYLOAD_SIZE, pedal_config.MTU - 2)

    def test_end_seq_value(self):
        self.assertEqual(pedal_config.END_SEQ, 0xFFFF)

    def test_service_uuid_format(self):
        import re
        uuid_re = re.compile(
            r'^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}$'
        )
        for uuid in [
            pedal_config.SERVICE_UUID,
            pedal_config.CHAR_WRITE_UUID,
            pedal_config.CHAR_WRITE_HW_UUID,
            pedal_config.CHAR_STATUS_UUID,
        ]:
            with self.subTest(uuid=uuid):
                self.assertRegex(uuid, uuid_re)


if __name__ == "__main__":
    unittest.main()
