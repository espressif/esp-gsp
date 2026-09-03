#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Release gates for benchmark scene freshness and control coverage."""

from __future__ import annotations

import json
import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
SCENE_DIR = ROOT / "examples" / "benchmark" / "scenes"


class BenchmarkSceneCoverageTests(unittest.TestCase):
    def test_generated_scenes_are_current(self):
        subprocess.run(
            [sys.executable, str(SCENE_DIR / "gen_scenes.py"), "--check"],
            cwd=ROOT,
            check=True,
        )

    def test_every_authored_control_type_is_present(self):
        result = subprocess.run(
            [str(ROOT / "ci" / "gspc-dev"), "inventory", "--format", "json"],
            cwd=ROOT,
            check=True,
            text=True,
            capture_output=True,
        )
        required = {entry["name"] for entry in json.loads(result.stdout)["widgets"]}
        scene_paths = sorted(SCENE_DIR.glob("bench_*.json"))
        scene_paths = [
            path for path in scene_paths
            if "_alt_" not in path.name
        ]
        self.assertTrue(scene_paths)
        for path in scene_paths:
            data = json.loads(path.read_text(encoding="utf-8"))
            authored = {obj["type"] for obj in data["objects"]}
            self.assertEqual(
                required - authored,
                set(),
                f"{path.name} does not cover the complete component registry",
            )

    def test_font_pack_budget_is_respected_at_every_resolution(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            data = json.loads(path.read_text(encoding="utf-8"))
            sizes = {data["default_font_size"]}
            sizes.update(
                obj["font_size"] for obj in data["objects"]
                if "font_size" in obj
            )
            self.assertLessEqual(
                len(sizes), 8,
                f"{path.name} exceeds the runtime font-pack limit: {sizes}",
            )


if __name__ == "__main__":
    unittest.main()
