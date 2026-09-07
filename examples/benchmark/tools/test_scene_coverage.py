#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Release gates for benchmark scene freshness and control coverage."""

from __future__ import annotations

import json
import re
import subprocess
import sys
import unittest
from pathlib import Path

from PIL import ImageFont


ROOT = Path(__file__).resolve().parents[3]
SCENE_DIR = ROOT / "examples" / "benchmark" / "scenes"
CASE_MANIFEST = ROOT / "examples" / "benchmark" / "main" / "bench_cases.inc"


def manifest_scene_binds():
    pattern = re.compile(
        r"^BENCH_CASE\(\s*(P_[A-Z0-9_]+)\s*,\s*"
        r"GSP_BIND_(P_[A-Z0-9_]+)\s*,"
    )
    result = []
    for line in CASE_MANIFEST.read_text(encoding="utf-8").splitlines():
        match = pattern.match(line)
        if match is not None:
            case_id, bind = match.groups()
            if case_id != bind:
                raise AssertionError(
                    f"case {case_id} uses mismatched bind {bind}"
                )
            result.append(bind.lower())
    return result


class BenchmarkSceneCoverageTests(unittest.TestCase):
    def test_repeater_controls_have_bindable_names(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            data = json.loads(path.read_text())
            for obj in data["objects"]:
                if obj["type"] in {"list", "wheel", "grid", "message_list"}:
                    self.assertTrue(obj.get("name"), (path.name, obj.get("bind")))

    def test_capacity_ladder_changes_only_object_count(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            objects = json.loads(path.read_text())["objects"]
            tiers = []
            for count in (1, 8, 32, 64):
                parent = next(i for i, obj in enumerate(objects)
                              if obj.get("bind") == f"p_load{count}")
                boxes = [obj for obj in objects if obj.get("parent") == parent
                         and obj.get("opacity") == 128]
                self.assertEqual(len(boxes), count, path.name)
                tiers.append([{key: box[key] for key in ("x", "y", "w", "h", "bg_color")}
                              for box in boxes])
            for tier in tiers:
                self.assertEqual(tier, tiers[-1][:len(tier)], path.name)

    def test_results_cards_fit_without_text_overlap(self):
        font_path = ROOT / "examples/common/fonts/DejaVuSans.ttf"
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            scene = json.loads(path.read_text())
            objects = scene["objects"]
            expected = 6 if scene["w"] >= 600 or scene["h"] >= 600 else 3
            cards = [obj for obj in objects if obj.get("bind", "").startswith("result_card")
                     and not obj.get("hidden")]
            self.assertEqual(len(cards), expected, path.name)
            for card in cards:
                self.assertLessEqual(card["x"] + card["w"], scene["w"], path.name)
                self.assertLessEqual(card["y"] + card["h"], scene["h"], path.name)
            for index in range(expected):
                labels = {side: next(obj for obj in objects
                                    if obj.get("bind") == f"result_{side}{index}")
                          for side in ("name", "fps", "value")}
                for side, text in (("name", "fade through black"), ("fps", "999.9"),
                                   ("value", "R 999.9 ms")):
                    label = labels[side]
                    font = ImageFont.truetype(str(font_path), label["font_size"])
                    self.assertLessEqual(font.getlength(text), label["w"], (path.name, side))
                self.assertLessEqual(labels["name"]["y"] + labels["name"]["font_size"],
                                     labels["fps"]["y"], path.name)
                self.assertLessEqual(labels["fps"]["x"] + labels["fps"]["w"],
                                     labels["value"]["x"], path.name)

    def test_font_declarations_stay_hidden_and_addressable(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            objects = json.loads(path.read_text(encoding="utf-8"))["objects"]
            palette = next(obj for obj in objects
                           if obj.get("bind") == "font_palette")
            self.assertTrue(palette["hidden"], path.name)
            self.assertEqual(palette["type"], "layer", path.name)

    def test_case_manifest_matches_every_scene_page(self):
        expected = manifest_scene_binds()
        self.assertEqual(len(expected), len(set(expected)))
        self.assertTrue(expected)
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            data = json.loads(path.read_text(encoding="utf-8"))
            authored = [
                obj["bind"] for obj in data["objects"]
                if obj.get("type") == "layer" and obj.get("parent") == 0
                and obj.get("bind", "").startswith("p_")
            ]
            self.assertEqual(len(authored), len(expected), path.name)
            self.assertEqual(set(authored), set(expected), path.name)

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

    def test_rotation_case_is_present_at_every_resolution(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            data = json.loads(path.read_text(encoding="utf-8"))
            objects = data["objects"]
            page_index = next(
                index for index, obj in enumerate(objects)
                if obj.get("bind") == "p_rotate"
            )
            images = {
                obj.get("name"): obj for obj in objects
                if obj.get("parent") == page_index and obj["type"] == "image"
            }
            self.assertEqual(
                set(images), {"rotate_arbitrary", "rotate_cardinal"},
                path.name,
            )
            for image in images.values():
                self.assertEqual(image["w"], 96, path.name)
                self.assertEqual(image["h"], 96, path.name)
                self.assertEqual(image["fit"], "contain", path.name)
                self.assertEqual(
                    image["rotation"],
                    {"default": 0, "min": -180, "max": 180},
                    path.name,
                )


if __name__ == "__main__":
    unittest.main()
