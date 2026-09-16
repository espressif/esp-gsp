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
from run_sim_benchmark import validate_vector_updates
from vector_cases import VECTOR_CASES, expected_commands


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
    def test_vector_manifest_and_command_contracts_match(self):
        manifest = dict(re.findall(r'^BENCH_CASE\((P_VECTOR_\w+),\s*\w+,\s*"([^"]+)"',
                                   CASE_MANIFEST.read_text(), re.MULTILINE))
        self.assertEqual(manifest, {key: value[0] for key, value in VECTOR_CASES.items()})
        for updates in range(100):
            commands = sum(4 * ((i % 32 in (0, 4, 6, 18, 20)) + (i % 8 == 0)) for i in range(updates))
            self.assertEqual(expected_commands("P_VECTOR_EYES", updates), commands)

    def test_simulator_requires_actual_vector_updates(self):
        valid = "gsp_sim: vector[P_VECTOR_SIZE] updates=60 commands=240 errors=0\n"
        validate_vector_updates(valid, ("P_VECTOR_SIZE",), 60, 60)
        for output in ("", valid.replace("commands=240", "commands=239"),
                       valid.replace("errors=0", "errors=1")):
            with self.assertRaises(RuntimeError):
                validate_vector_updates(output, ("P_VECTOR_SIZE",), 60, 60)
        validate_vector_updates("", ("P_VECTOR_SIZE",), 1, 60)
        validate_vector_updates("", ("P_RECT1", "P_VECTOR_SIZE"), 60, 60)

    def test_vector_cases_at_every_resolution(self):
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            scene = json.loads(path.read_text())
            objects = scene["objects"]
            for bind in ("p_vector_size", "p_vector_rotate", "p_vector_tint", "p_vector_morph"):
                page = next(i for i, obj in enumerate(objects) if obj.get("bind") == bind)
                images = [obj for obj in objects if obj.get("parent") == page and obj["type"] == "image"]
                self.assertEqual(len(images), 2, (path.name, bind))
                for image in images:
                    self.assertTrue((path.parent / image["image"]).is_file())
                    self.assertLessEqual(image["x"] + 96, scene["w"])
                    self.assertLessEqual(image["y"] + 96, scene["h"])
                    if bind == "p_vector_size":
                        self.assertEqual((image["w"]["min"], image["w"]["max"]), (32, 96))
                    elif bind == "p_vector_rotate":
                        self.assertEqual(image["rotation"], 0)
                    elif bind == "p_vector_morph":
                        self.assertEqual(image["svg_element"], "emblem")
                        self.assertTrue((path.parent / image["morph_to"]).is_file())
                    else:
                        self.assertIn("tint", image)

    def test_vector_feature_pages_and_eye_layers(self):
        expected = {"p_vector_move": 2, "p_vector_fit": 3, "p_vector_style": 2, "p_vector_eyes": 8}
        for path in sorted(SCENE_DIR.glob("bench_*.json")):
            if "_alt_" in path.name:
                continue
            scene = json.loads(path.read_text())
            objects = scene["objects"]
            for bind, count in expected.items():
                page = next(i for i, obj in enumerate(objects) if obj.get("bind") == bind)
                images = [obj for obj in objects if obj.get("parent") == page and obj["type"] == "image"]
                self.assertEqual(len(images), count, (path.name, bind))
                for obj in images:
                    for axis, span, limit in (("x", "w", scene["w"]), ("y", "h", scene["h"])):
                        position = obj[axis]
                        low, high = (position["min"], position["max"]) if isinstance(position, dict) else (position, position)
                        if obj.get("svg_layout") == "canvas" and isinstance(position, dict):
                            self.assertGreater(low + obj[span], 0)
                            self.assertLess(high, limit)
                            low = high = position["default"]
                        self.assertGreaterEqual(low, 0)
                        self.assertLessEqual(high + obj[span], limit)
                    self.assertTrue((path.parent / obj["image"]).is_file())
                if bind == "p_vector_fit":
                    self.assertEqual({obj["fit"] for obj in images}, {"contain", "cover", "stretch"})
                    self.assertTrue(all(obj["scalable"] for obj in images))
                if bind == "p_vector_eyes":
                    self.assertEqual([obj["svg_element"] for obj in images], ["white", "iris", "mask", "rim"] * 2)
                    for obj in images:
                        self.assertEqual(obj.get("svg_layout"), "canvas")
                        if obj["svg_element"] in {"mask", "rim"}:
                            self.assertTrue((path.parent / obj["morph_to"]).is_file())
                        if obj["svg_element"] == "iris":
                            self.assertIsInstance(obj["x"], dict)
                            self.assertIsInstance(obj["y"], dict)

    def test_eye_simulator_requires_completed_blinks(self):
        output = "gsp_sim: vector[P_VECTOR_EYES] updates=30 commands=36 errors=0\ngsp_sim: eyes open=5 closed=2 errors=0\n"
        validate_vector_updates(output, ("P_VECTOR_EYES",), 180, 180)
        for broken in (output.replace("closed=2", "closed=0"), output.replace("open=5", "open=0"), output.splitlines()[0]):
            with self.assertRaises(RuntimeError):
                validate_vector_updates(broken, ("P_VECTOR_EYES",), 180, 180)

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
