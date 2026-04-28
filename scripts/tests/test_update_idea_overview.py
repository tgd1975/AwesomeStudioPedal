"""Unit tests for update_idea_overview.py — no filesystem side effects on repo."""

import io
import os
import pathlib
import sys
import tempfile
import textwrap
import unittest
from contextlib import redirect_stdout
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))
import update_idea_overview as uio


def _write(path: pathlib.Path, content: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(textwrap.dedent(content).lstrip(), encoding="utf-8")


class TestParseIdeaFile(unittest.TestCase):

    def test_parses_frontmatter(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = pathlib.Path(tmp) / "idea-042-sample.md"
            _write(p, """
                ---
                id: IDEA-042
                title: Sample idea
                description: A short description.
                ---

                # Body
            """)
            result = uio.parse_idea_file(str(p))
            self.assertEqual(result["id"], "IDEA-042")
            self.assertEqual(result["title"], "Sample idea")
            self.assertEqual(result["description"], "A short description.")
            self.assertEqual(result["_file"], "idea-042-sample.md")

    def test_returns_none_without_frontmatter(self):
        with tempfile.TemporaryDirectory() as tmp:
            p = pathlib.Path(tmp) / "no-frontmatter.md"
            _write(p, "# Just a body\n")
            self.assertIsNone(uio.parse_idea_file(str(p)))


class TestRenderOverview(unittest.TestCase):

    def test_empty_folders(self):
        out = uio.render_overview([], [])
        self.assertIn("**Open: 0** | **Archived: 0**", out)
        self.assertIn("_No open ideas._", out)
        self.assertNotIn("## Archived Ideas", out)

    def test_single_open_idea_renders_row(self):
        idea = {"id": "IDEA-001", "title": "First", "description": "Does things.",
                "_file": "idea-001-first.md"}
        out = uio.render_overview([idea], [])
        self.assertIn("| [IDEA-001](open/idea-001-first.md) | First | Does things. |", out)

    def test_missing_description_renders_empty_cell(self):
        idea = {"id": "IDEA-002", "title": "No desc", "_file": "idea-002.md"}
        out = uio.render_overview([idea], [])
        self.assertIn("| [IDEA-002](open/idea-002.md) | No desc |  |", out)

    def test_pipe_in_description_is_escaped(self):
        idea = {"id": "IDEA-003", "title": "Piped",
                "description": "Has | pipe", "_file": "idea-003.md"}
        out = uio.render_overview([idea], [])
        self.assertIn("Has \\| pipe", out)

    def test_archived_section_only_when_non_empty(self):
        archived = {"id": "IDEA-099", "title": "Done", "_file": "idea-099.md"}
        out = uio.render_overview([], [archived])
        self.assertIn("## Archived Ideas", out)
        self.assertIn("[IDEA-099](archived/idea-099.md)", out)


class TestMainIdempotent(unittest.TestCase):

    def test_dry_run_prints_and_does_not_write(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_p = pathlib.Path(tmp)
            _write(tmp_p / "open" / "idea-001.md",
                   "---\nid: IDEA-001\ntitle: A\n---\n")
            overview = tmp_p / "OVERVIEW.md"

            buf = io.StringIO()
            with patch.object(uio, "OPEN_DIR", str(tmp_p / "open")), \
                 patch.object(uio, "ARCHIVED_DIR", str(tmp_p / "archived")), \
                 patch.object(uio, "OVERVIEW", str(overview)), \
                 redirect_stdout(buf):
                rc = uio.main(["--dry-run"])
            self.assertEqual(rc, 0)
            self.assertFalse(overview.exists())
            self.assertIn("IDEA-001", buf.getvalue())

    def test_write_is_idempotent(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp_p = pathlib.Path(tmp)
            _write(tmp_p / "open" / "idea-001.md",
                   "---\nid: IDEA-001\ntitle: A\n---\n")
            overview = tmp_p / "OVERVIEW.md"

            with patch.object(uio, "OPEN_DIR", str(tmp_p / "open")), \
                 patch.object(uio, "ARCHIVED_DIR", str(tmp_p / "archived")), \
                 patch.object(uio, "OVERVIEW", str(overview)), \
                 redirect_stdout(io.StringIO()):
                uio.main([])
                first = overview.read_text()
                uio.main([])
                second = overview.read_text()
            self.assertEqual(first, second)


if __name__ == "__main__":
    unittest.main()
