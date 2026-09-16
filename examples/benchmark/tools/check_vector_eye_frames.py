#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Verify real benchmark eye frames: closure occludes pupils; gaze moves both eyes."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

from PIL import Image


def eye_pixels(path, rgb888=False):
    image = Image.open(path).convert("RGB")
    result = []
    for side in range(2):
        pupils = []
        whites = 0
        for y in range(image.height // 8, image.height):
            for x in range(side * image.width // 2, (side + 1) * image.width // 2):
                r, g, b = image.getpixel((x, y))
                whites += r > 210 and g > 210 and b > 210
                if (r, g, b) == ((11, 32, 41) if rgb888 else (8, 32, 41)):
                    pupils.append((x, y))
        center = [sum(p[i] for p in pupils) / len(pupils) for i in (0, 1)] if pupils else None
        result.append({"white_pixels": whites, "pupil_pixels": len(pupils), "pupil_center": center})
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--rgb888", action="store_true", help="Check the 800x480 RGB888 scene instead of 240x240 RGB565")
    args = parser.parse_args()
    samples = {}
    for name, frames in (("left", 20), ("right", 90), ("down", 168), ("closed", 48)):
        directory = args.output_dir / name
        size = "800" if args.rgb888 else "240"
        command = [sys.executable, str(Path(__file__).with_name("run_sim_benchmark.py")),
                   "--size", size, "--case", "P_VECTOR_EYES", "--frames", str(frames),
                   "--build-dir", str(args.build_dir), "--output-dir", str(directory)]
        if args.rgb888:
            command.append("--rgb888")
        subprocess.run(command, check=True)
        samples[name] = eye_pixels(directory / size / "final.ppm", args.rgb888)
    for side in range(2):
        for name in ("left", "right", "down"):
            assert samples[name][side]["white_pixels"] > 50, (name, side, samples)
            assert samples[name][side]["pupil_pixels"] > 20, (name, side, samples)
        assert samples["closed"][side]["white_pixels"] == 0, samples
        assert samples["closed"][side]["pupil_pixels"] == 0, samples
        assert samples["right"][side]["pupil_center"][0] - samples["left"][side]["pupil_center"][0] > 2, samples
        assert samples["down"][side]["pupil_center"][1] - samples["left"][side]["pupil_center"][1] > 2, samples
    (args.output_dir / "pixel-check.json").write_text(json.dumps(samples, indent=2) + "\n")
    print("PASS: both pupils move horizontally/vertically and fully closed lids hide all eye whites and pupils")


if __name__ == "__main__":
    main()
