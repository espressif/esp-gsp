#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Generates the per-resolution benchmark scenes and validates image assets.

One full-screen layer per authored case, with a separate transition scene.
The app cycles visibility and reports measured frame/time counters. See
COVERAGE.md for the exercised controls, rendering paths and test boundaries.
"""

import argparse
import io
import json
import math
import os
import re
import sys
from pathlib import Path

from PIL import Image, ImageDraw

RESOLUTIONS = [(1024, 600), (800, 480), (480, 800), (360, 360),
               (320, 240), (240, 240)]
FONT = "../fonts/DejaVuSans.ttf"

LOREM = ("GSP / RENDER LAB. Retained scenes, native controls and explicit "
         "damage tracking. Images, glyphs and translucent layers share one "
         "composition pipeline. Measure frames against elapsed time. "
         "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789. " * 12)

CASE_PATTERN = re.compile(
    r'^BENCH_CASE\((\w+),\s*\w+,\s*"([^"]+)",\s*"([^"]+)"')
CASE_LINES = (Path(__file__).resolve().parents[1] /
              "main/bench_cases.inc").read_text().splitlines()
CASE_MATCHES = [match for line in CASE_LINES
                if (match := CASE_PATTERN.match(line))]
CASES = {match[1].lower(): (index + 1, match[2], match[3])
         for index, match in enumerate(CASE_MATCHES)}

PALETTE = {
    "#101820": "#07121E", "#203048": "#10283B",
    "#1C2534": "#102334", "#26324A": "#153447",
    "#5090E0": "#19D3FF", "#5070B0": "#267B9B",
    "#60C080": "#52E8BA", "#8098C0": "#86AFC6",
    "#D0D8F0": "#DBF4FF", "#F0B040": "#FFAF4D",
    "#E05050": "#FF745A", "#40C080": "#155D70",
}

FONT_PALETTES = {
    240: (8, 9, 11, 12, 13, 14, 16, 20),
    360: (11, 12, 13, 16, 18, 24, 30),
    480: (12, 15, 16, 18, 21, 24, 32, 41),
    600: (12, 16, 18, 20, 23, 27, 40, 51),
    800: (12, 16, 20, 26, 30, 40, 54, 68),
}


def validate_assets():
    expected = {
        "bench_scale.png": ("RGB", (96, 64)),
        "bench_industrial.png": ("RGB", (640, 480)),
        "bench_industrial_argb.png": ("RGBA", (320, 240)),
        "bench_anim.gif": ("P", (120, 120)),
        "bench_anim_sparse.gif": ("P", (120, 120)),
    }
    for name, (mode, size) in expected.items():
        with Image.open(name) as image:
            if image.mode != mode:
                raise ValueError(
                    f"{name}: expected {mode}, found {image.mode}")
            if image.size != size:
                raise ValueError(
                    f"{name}: expected {size}, found {image.size}")
    with Image.open("bench_industrial_argb.png") as image:
        if image.getchannel("A").getextrema() != (0, 255):
            raise ValueError(
                "bench_industrial_argb.png must contain real alpha coverage")
    with Image.open("bench_anim.gif") as image:
        if image.info.get("loop") != 0:
            raise ValueError("bench_anim.gif must loop indefinitely")
        if image.n_frames != 24:
            raise ValueError(
                f"bench_anim.gif: expected 24 frames, found {image.n_frames}")
        durations = []
        for frame in range(image.n_frames):
            image.seek(frame)
            durations.append(image.info.get("duration"))
        if any(duration != 50 for duration in durations):
            raise ValueError(
                "bench_anim.gif: every frame must last exactly 50 ms")
    with Image.open("bench_anim_sparse.gif") as image:
        if image.info.get("loop") != 0:
            raise ValueError("bench_anim_sparse.gif must loop indefinitely")
        if image.n_frames != 24:
            raise ValueError(
                "bench_anim_sparse.gif: expected 24 frames, "
                f"found {image.n_frames}")
        durations = []
        previous = None
        expected_changes = {
            *((x, y) for y in range(16, 24) for x in range(8, 16)),
            *((x, y) for y in range(96, 104) for x in range(104, 112)),
        }
        for frame in range(image.n_frames):
            image.seek(frame)
            durations.append(image.info.get("duration"))
            current = image.convert("RGB")
            if previous is not None:
                changed = {
                    (x, y)
                    for y in range(image.height)
                    for x in range(image.width)
                    if current.getpixel((x, y)) != previous.getpixel((x, y))
                }
                if changed != expected_changes:
                    raise ValueError(
                        "bench_anim_sparse.gif: each frame must change "
                        "exactly two distant 8x8 regions")
            previous = current
        if any(duration != 50 for duration in durations):
            raise ValueError(
                "bench_anim_sparse.gif: every frame must last exactly 50 ms")


def scale_asset_bytes():
    """Build the small, deterministic runtime-scaling benchmark source."""
    with Image.open("bench_industrial.png") as source:
        image = source.convert("RGB").resize(
            (96, 64), Image.Resampling.LANCZOS)
    output = io.BytesIO()
    image.save(output, format="PNG", optimize=False)
    return output.getvalue()


def sparse_animation_bytes():
    """Build two distant 8x8 changes over an orientation-readable backdrop."""
    width = height = 120
    colors = [
        (18, 28, 44),    # background
        (32, 49, 72),    # grid
        (38, 61, 88),    # lower triangle
        (50, 76, 106),   # upper triangle
        (0, 220, 170),   # top-left phase A
        (120, 255, 90),  # top-left phase B
        (255, 176, 48),  # bottom-right phase A
        (255, 88, 144),  # bottom-right phase B
        (224, 236, 248), # border
    ]
    palette = [channel for color in colors for channel in color]
    palette.extend([0] * (256 * 3 - len(palette)))
    background = []
    for y in range(height):
        for x in range(width):
            if x in (0, width - 1) or y in (0, height - 1):
                color = 8
            elif x % 16 == 0 or y % 16 == 0:
                color = 1
            else:
                color = 2 if x < y else 3
            background.append(color)
    frames = []
    for index in range(24):
        frame = Image.new("P", (width, height))
        frame.putpalette(palette)
        frame.putdata(background)
        pixels = frame.load()
        left = 4 if index % 2 == 0 else 5
        right = 6 if index % 2 == 0 else 7
        for y in range(16, 24):
            for x in range(8, 16):
                pixels[x, y] = left
        for y in range(96, 104):
            for x in range(104, 112):
                pixels[x, y] = right
        frames.append(frame)
    output = io.BytesIO()
    frames[0].save(output, format="GIF", save_all=True,
                   append_images=frames[1:], duration=50, loop=0,
                   disposal=1, optimize=False)
    return output.getvalue()


def scanner_animation_bytes():
    """Fixed 24-frame industrial scan diagnostic, not a captured UI movie."""
    palette = [7, 18, 30, 21, 52, 71, 25, 211, 255, 82, 232, 186,
               255, 175, 77]
    palette.extend([0] * (768 - len(palette)))
    frames = []
    for phase in range(24):
        frame = Image.new("P", (120, 120))
        frame.putpalette(palette)
        draw = ImageDraw.Draw(frame)
        for radius in (22, 38, 54):
            draw.ellipse((60 - radius, 60 - radius, 60 + radius, 60 + radius),
                         outline=1, width=2)
        draw.line((6, 60, 114, 60), fill=1)
        draw.line((60, 6, 60, 114), fill=1)
        angle = phase * 15
        draw.arc((6, 6, 114, 114), angle - 65, angle, fill=2, width=4)
        x = round(60 + 44 * math.cos(math.radians(angle)))
        y = round(60 + 44 * math.sin(math.radians(angle)))
        draw.line((60, 60, x, y), fill=3, width=2)
        draw.ellipse((x - 4, y - 4, x + 4, y + 4), fill=4)
        frames.append(frame)
    output = io.BytesIO()
    frames[0].save(output, format="GIF", save_all=True,
                   append_images=frames[1:], duration=50, loop=0,
                   disposal=1, optimize=False)
    return output.getvalue()


class Builder:
    def __init__(self, width, height):
        self.w = width
        self.h = height
        self.margin = width // 12
        self.inner = width - 2 * self.margin
        self.row = max(14, height // 14)
        self.gap = max(6, height // 26)
        self.sweep_count = 0
        self.pages = []
        self.font_palette = FONT_PALETTES[height]
        self.objects = [
            {"type": "container", "parent": -1, "x": 0, "y": 0,
             "w": width, "h": height, "bg_color": "#07121E",
             "bind": "pressure_bg"},
        ]
        # Packed scenes share font ordinals and glyph sets. Declare the same
        # bounded palette before either scene's visible labels.
        fonts = self.add({"type": "layer", "parent": 0, "x": 0, "y": 0,
                          "w": width, "h": height, "hidden": True,
                          "bind": "font_palette"})
        charset = "".join(chr(code) for code in range(32, 127)) + "\u25be"
        for size in self.font_palette:
            self.add({"type": "label", "parent": fonts, "x": 0, "y": 0,
                      "w": width, "h": size + 4, "font_size": size,
                      "text": charset, "fg_color": "#DBF4FF"})
        # Shared vector backdrop: the cost is included in every page sample.
        for index in range(1, 8):
            self.add({"type": "container", "parent": 0,
                      "x": index * width // 8, "y": 0,
                      "w": 1, "h": height, "bg_color": "#112536"})
        for index in range(1, 5):
            self.add({"type": "container", "parent": 0,
                      "x": 0, "y": index * height // 5,
                      "w": width, "h": 1, "bg_color": "#112536"})

    def page(self, bind, hidden=True):
        self.objects.append({"type": "layer", "parent": 0, "x": 0,
                             "y": 0, "w": self.w, "h": self.h,
                             "hidden": hidden, "bind": bind})
        layer = len(self.objects) - 1
        self.pages.append((layer, bind))
        return layer

    def add(self, obj):
        for key, value in obj.items():
            if isinstance(value, str):
                obj[key] = PALETTE.get(value, value)
        if "font_size" in obj:
            requested = obj["font_size"]
            obj["font_size"] = min(
                self.font_palette, key=lambda size: abs(size - requested))
        self.objects.append(obj)
        return len(self.objects) - 1

    def finish(self):
        """Consistent case identification; no synthetic on-screen FPS."""
        bar_h = max(14, self.h // 28)
        for page, bind in self.pages:
            if bind in {"p_storm", "p_messages"}:
                continue  # These pages have integrated case headers.
            number, title, category = CASES[bind]
            self.add({"type": "container", "parent": page,
                      "x": 0, "y": 0, "w": self.w, "h": bar_h,
                      "bg_color": "#07121EF0"})
            self.add({"type": "label", "parent": page, "x": 5, "y": 1,
                      "w": self.w * 7 // 10, "h": bar_h - 1,
                      "text": f"{number:02d} / {title.upper()}",
                      "font_size": max(9, self.h // 48),
                      "fg_color": "#DBF4FF"})
            self.add({"type": "label", "parent": page,
                      "x": self.w * 7 // 10, "y": 1,
                      "w": self.w * 3 // 10 - 5, "h": bar_h - 1,
                      "text": category.upper(), "text_align": "right",
                      "font_size": max(8, self.h // 55),
                      "fg_color": "#19D3FF"})
            self.add({"type": "container", "parent": page,
                      "x": 0, "y": bar_h, "w": self.w, "h": 1,
                      "bg_color": "#267B9B"})
        return self.objects

    def sweep(self, parent, y, color="#5090E0"):
        """Full-width tween strip forcing redraw on this page (each
        page gets its own bind: bind names are scene-unique)."""
        bind = f"sv{self.sweep_count}"
        self.sweep_count += 1
        self.add({"type": "slider", "parent": parent, "x": self.margin,
                  "y": y, "w": self.inner, "h": self.row, "value": 0,
                  "bind": bind, "bg_color": "#1C2534",
                  "fg_color": color})


def scene(width, height, rgb888=False):
    b = Builder(width, height)
    m, inner, row, gap = b.margin, b.inner, b.row, b.gap

    # P0 single rect tween. There is deliberately no idle page: every scored
    # workload continuously invalidates pixels and applies back-pressure.
    p = b.page("p_rect1", hidden=False)
    b.add({"type": "slider", "parent": p, "x": m,
           "y": height // 2 - row // 2, "w": inner, "h": row,
           "value": 0, "bind": "rv0", "bg_color": "#26324A",
           "fg_color": "#5090E0"})
    b.add({"type": "label", "parent": p, "x": m,
           "y": height // 3, "w": inner, "h": row * 2,
           "text": "RASTER / 01", "font_size": max(20, height // 15),
           "fg_color": "#DBF4FF"})
    for index in range(21):
        b.add({"type": "container", "parent": p,
               "x": m + index * (inner - 1) // 20,
               "y": height // 2 + row, "w": 1,
               "h": row // 2 if index % 5 == 0 else row // 4,
               "bg_color": "#267B9B"})
    b.add({"type": "label", "parent": p, "x": m,
           "y": height * 2 // 3, "w": inner, "h": row * 2,
           "text": "SOLID FILL / LIVE VALUE RAMP", "font_size": max(9, height // 40),
           "fg_color": "#86AFC6"})

    # P2 multiple rects.
    p = b.page("p_rects")
    y = height // 10
    for index in range(8):
        b.add({"type": "slider", "parent": p, "x": m, "y": y,
               "w": inner, "h": max(10, row * 2 // 3),
               "value": (13 * index) % 101, "bind": f"v{index}",
               "bg_color": "#1C2534", "fg_color": "#5090E0",
               "knob": True})
        y += max(10, row * 2 // 3) + gap // 2
    b.add({"type": "progress", "parent": p, "x": m, "y": y,
           "w": inner, "h": row, "value": 0, "bind": "load",
           "bg_color": "#1C2534", "fg_color": "#60C080",
           "radius": row // 3})

    # P3 containers: rounded bordered cards above a tween.
    p = b.page("p_cards")
    card_w = inner // 3 - gap
    card_h = height // 3 - gap
    for index in range(6):
        card = b.add({"type": "rect" if index == 0 else "container", "parent": p,
               "x": m + (index % 3) * (card_w + gap),
               "y": height // 8 + (index // 3) * (card_h + gap),
               "w": card_w, "h": card_h, "bg_color": "#203048",
               "bind": f"cc{index}", "radius": 14,
               "border_color": "#5070B0", "border_width": 3})
        b.add({"type": "label", "parent": card, "x": 6, "y": card_h // 4,
               "w": card_w - 12, "h": card_h // 2,
               "text": f"0{index + 1}", "font_size": max(16, card_h // 3),
               "fg_color": "#DBF4FF", "text_align": "center"})

    # P3b decorative vector shapes. One dynamic theme drives every primitive
    # so the page continuously measures rect/round-rect/ellipse/line raster
    # cost instead of only checking that static shapes compile.
    p = b.page("p_shapes")
    shape_top = max(20, height // 11)
    shape_gap = max(4, gap // 2)
    shape_row = max(24, (height - shape_top - 3 * shape_gap) // 3)
    shape_col = (inner - shape_gap) // 2
    stroke = max(2, min(width, height) // 100)
    accent = {"theme": "shape_accent"}
    b.add({"type": "shape", "shape": "rect", "parent": p,
           "x": m, "y": shape_top, "w": shape_col, "h": shape_row,
           "bg_color": accent, "opacity": 176,
           "border_color": "#D8E4FF", "border_width": stroke})
    b.add({"type": "shape", "shape": "round_rect", "parent": p,
           "x": m + shape_col + shape_gap, "y": shape_top,
           "w": shape_col, "h": shape_row, "radius": shape_row // 4,
           "bg_color": accent})
    circle = min(shape_col, shape_row)
    second_y = shape_top + shape_row + shape_gap
    b.add({"type": "shape", "shape": "circle", "parent": p,
           "x": m + (shape_col - circle) // 2, "y": second_y,
           "w": circle, "h": circle, "bg_color": accent})
    b.add({"type": "shape", "shape": "ellipse", "parent": p,
           "x": m + shape_col + shape_gap, "y": second_y,
           "w": shape_col, "h": shape_row,
           "border_color": accent, "border_width": 2 * stroke})
    third_y = second_y + shape_row + shape_gap
    b.add({"type": "shape", "shape": "ellipse", "parent": p,
           "x": m, "y": third_y, "w": shape_col, "h": shape_row,
           "bg_color": accent, "opacity": 192})
    b.add({"type": "shape", "shape": "line", "parent": p,
           "x": m + shape_col + shape_gap, "y": third_y,
           "w": shape_col, "h": shape_row, "line_direction": "bl_tr",
           "border_color": accent, "border_width": 2 * stroke})

    # P3c analog clock. The frame driver updates all three direct angle
    # properties every 16 ms, exercising dynamic needle rasterization and
    # invalidation independently from the decorative shape color tween.
    p = b.page("p_clock")
    clock_size = min(inner, height * 3 // 4)
    b.add({"type": "clock", "parent": p, "name": "bench_clock",
           "x": (width - clock_size) // 2,
           "y": (height - clock_size) // 2,
           "w": clock_size, "h": clock_size,
           "bg_color": "#182438", "border_color": "#6078A0",
           "border_width": max(2, min(width, height) // 120),
           "fg_color": "#E8EEF8", "tick_color": "#91A8C8",
           "hour_color": "#F5C451", "minute_color": "#68B8F0",
           "second_color": "#FF5263",
           "hand_thickness": max(2, min(width, height) // 80),
           "hour_angle": 0, "minute_angle": 0, "second_angle": 0})
    # Overlay the standalone needle API as a fourth, gauge-style hand so the
    # benchmark covers both authored controls rather than only clock's
    # internally expanded needle primitives.
    b.add({"type": "needle", "parent": p, "name": "bench_needle",
           "x": (width - clock_size) // 2,
           "y": (height - clock_size) // 2,
           "w": clock_size, "h": clock_size,
           "fg_color": "#54E0A4",
           "thickness": max(1, min(width, height) // 120),
           "tail_length": max(2, clock_size // 24), "angle": 0})

    # P4 gradient fills. Keep cases that expose the RGB565 failure modes:
    # a vivid multi-channel ramp, a grayscale vertical ramp, a red-only
    # ramp (32 native levels), and a low-contrast translucent ramp.
    p = b.page("p_grad")
    gradient_cases = (
        ("#102040", "#E06020", "horizontal", 255),
        ("#101010", "#F0F0F0", "vertical", 255),
        ("#000000", "#F80000", "horizontal", 255),
        ("#202428", "#303840", "horizontal", 176),
    )
    band = (height - height // 3) // len(gradient_cases)
    for index, (color0, color1, direction, opacity) in \
            enumerate(gradient_cases):
        wrap = b.add({"type": "layer", "parent": p, "x": m,
                      "y": height // 12 + index * (band + gap),
                      "w": inner, "h": band, "bind": f"gv{index}"})
        b.add({"type": "container", "parent": wrap, "x": 0, "y": 0,
               "w": inner, "h": band, "bg_color": color0,
               "bg_gradient": color1, "gradient_dir": direction,
               "opacity": opacity})
    b.sweep(p, height - row - gap)

    # P5 translucent containers (blend fills).
    p = b.page("p_opa")
    for index in range(6):
        b.add({"type": "container", "parent": p,
               "x": m + (index % 3) * (inner // 4),
               "y": height // 8 + (index % 4) * (height // 10),
               "w": inner // 2, "h": height // 3,
               "bg_color": "#60A0E0" if index % 2 else "#E08060",
               "bind": f"oc{index}",
               "opacity": 120 + 20 * (index % 3), "radius": 10})

    # P6 stacked translucent layers (nested blends).
    page_opal = b.page("p_opal")
    parent = page_opal
    x, y, w, h = m, height // 8, inner, height - height // 3
    for _ in range(3):
        parent = b.add({"type": "container", "parent": parent, "x": x,
                        "y": y, "w": w, "h": h,
                        "bg_color": "#4080C0", "opacity": 110,
                        "bind": f"lc{len(b.objects) % 3}",
                        "radius": 12})
        x, y = m // 2, max(8, h // 8)
        w, h = w - m, h - 2 * max(8, h // 8)

    # P7 overlay scrim: cards under a full-screen translucent tinted
    # cover whose color tweens (full-screen blend every frame).
    p = b.page("p_over")
    for index in range(3):
        b.add({"type": "container", "parent": p,
               "x": m + index * (inner // 3 - gap), "y": height // 5,
               "w": inner // 3 - gap, "h": height // 2,
               "bg_color": "#203048", "radius": 12,
               "border_color": "#5070B0", "border_width": 2})
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#201030",
           "opacity": 90, "bind": "wall2"})

    # P8 dynamic labels (glyph rewrites, two sizes).
    p = b.page("p_text")
    y = height // 8
    line = max(18, height // 9)
    for index in range(4):
        b.add({"type": "container", "parent": p, "x": m - 5, "y": y - 2,
               "w": inner + 10, "h": line + 4, "radius": 4,
               "bg_color": "#10283B"})
        b.add({"type": "label", "parent": p, "x": m, "y": y,
               "w": inner, "h": line, "text": f"GLYPH / CHANNEL {index:02d}",
               "font_size": max(12, min(height // 22, width // 22)),
               "bind": f"t{index}", "fg_color": "#D0D8F0"})
        y += line + gap

    # P9 screen-sized static text re-blended under a color tween.
    p = b.page("p_bigtext")
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#182030",
           "bind": "wall3"})
    b.add({"type": "label", "parent": p, "x": m // 2,
           "y": height // 20, "w": width - m,
           "h": height - height // 10,
           "text": LOREM,
           "font_size": max(13, height // 26), "fg_color": "#C8D2E8"})

    # P10 scroll simulation: a column of rows rewritten every tick.
    p = b.page("p_scroll")
    y = height // 14
    line = max(16, height // 11)
    for index in range(8):
        b.add({"type": "label", "parent": p, "x": m, "y": y,
               "w": inner, "h": line, "text": f"{index + 1:02d} / pipeline ready",
               "bind": f"s{index}", "fg_color": "#B8C4DC"})
        y += line + gap // 2

    # P11 multiple arcs.
    p = b.page("p_arcs")
    side = min(width, height) // 4
    for index in range(6):
        b.add({"type": "arc", "parent": p,
               "x": m + (index % 3) * (inner // 3),
               "y": height // 4 - side // 2 +
                    (index // 3) * (height // 2), "w": side,
               "h": side, "start_angle": 120, "sweep": 300,
               "thickness": max(6, side // 9),
               "value": (17 * index) % 101, "bind": f"a{index}",
               "fg_color": "#F0B040", "bg_color": "#282018"})

    # Responsive nine-tile grids keep every image visible at every size.
    # Five card colors change continuously, invalidating their image subtree.
    image_gap = max(3, gap // 2)
    image_top = max(18, height // 16)
    tile_w = (width - 2 * image_gap) // 3
    tile_h = (height - image_top - 3 * image_gap) // 3
    for page_bind, prefix, source in (
            ("p_imgrgb", "iv", "bench_industrial.png"),
            ("p_imgargb", "av", "bench_industrial_argb.png")):
        p = b.page(page_bind)
        for index in range(9):
            card = {"type": "container", "parent": p,
                    "x": (index % 3) * (tile_w + image_gap),
                    "y": image_top + (index // 3) * (tile_h + image_gap),
                    "w": tile_w, "h": tile_h, "bg_color": "#153447"}
            if index % 2 == 0:
                card["bind"] = f"{prefix}{index // 2}"
            parent = b.add(card)
            b.add({"type": "image", "parent": parent, "x": 2, "y": 2,
                   "w": tile_w - 4, "h": tile_h - 4,
                   "image": source, "codec": "raw"})

    # P13b QOI-decoded grid: tiles are stored compressed (QOI/RLE16 +
    # DECODE_LRU) except dyn0's flash-mapped initial placeholder.
    # Small panels shrink the decoded tiles: no-PSRAM chips must fit
    # the two cache entries next to the frame buffer.
    qw, qh = (160, 120) if width >= 320 else (80, 60)
    p = b.page("p_qoi")
    for index in range(9):
        x = (index % 3) * max(1, (width - qw)) // 2
        y2 = (index // 3) * max(1, (height - qh)) // 2
        parent = p
        if index % 2 == 0:
            parent = b.add({"type": "layer", "parent": p, "x": x,
                            "y": y2, "w": qw, "h": qh,
                            "bind": f"qv{index // 2}"})
            x, y2 = 0, 0
        tile = {"type": "image", "parent": parent, "x": x, "y": y2,
                "w": qw, "h": qh, "compress": True,
                "image": "bench_industrial_argb.png" if index % 2 else
                         "bench_industrial.png"}
        if index == 0:
            tile["bind"] = "dyn0"
            del tile["compress"]
            tile["codec"] = "raw"
        # Hardware-JPEG profiles exercise JPEG and JPEG_A8 through the same
        # cache; RGB888 keeps the alpha tile lossless.
        hardware_jpeg_panel = (width >= 1000 or
                               (width == 480 and height == 800) or
                               (rgb888 and width == 800 and height == 480))
        if hardware_jpeg_panel and index in (6, 7):
            del tile["compress"]
            # RGB888 keeps transparent content lossless until its
            # JPEG+A8 output contract is defined; the opaque tile still
            # exercises native 24-bit hardware JPEG.
            tile["codec"] = "lossless" if rgb888 and index == 7 \
                else "jpeg"
        b.add(tile)

    # P13c programmable image scaling. All four images keep their authored
    # target boxes while the application drives scale at the semantic frame
    # rate. The first three cover every fit policy with a static resource;
    # scale_dynamic is replaced with encoded QOI at page entry so the same
    # measured workload also covers the decoded-cache path.
    p = b.page("p_scale")
    scale_top = max(18, height // 14)
    scale_gap = max(4, gap // 2)
    scale_w = max(24, (width - 2 * m - scale_gap) // 2)
    scale_h = max(24, (height - scale_top - m - scale_gap) // 2)
    scale_cases = (
        ("scale_stretch", "stretch", 0, 0, None),
        ("scale_contain", "contain", 1, 0, None),
        ("scale_cover", "cover", 0, 1, None),
        ("scale_dynamic", "contain", 1, 1, "scale_dyn"),
    )
    for name, fit, column, row_index, bind in scale_cases:
        image = {
            "type": "image", "parent": p, "name": name,
            "x": m + column * (scale_w + scale_gap),
            "y": scale_top + row_index * (scale_h + scale_gap),
            "w": scale_w, "h": scale_h, "image": "bench_scale.png",
            "codec": "raw", "fit": fit, "scalable": True,
            "scale": 1.0, "min_scale": 0.5, "max_scale": 2.0,
        }
        if bind is not None:
            image["bind"] = bind
        b.add(image)

    # P13d image rotation. Both objects retain exact 1:1 contain geometry:
    # the app continuously drives one through arbitrary angles and cycles the
    # other through cardinal angles so the same page covers scalar and PPA
    # routes with a resolution-independent workload.
    p = b.page("p_rotate")
    rotate_size = 96
    rotate_gap = 8
    rotate_x = (width - 2 * rotate_size - rotate_gap) // 2
    rotate_y = (height - rotate_size) // 2
    for index, name in enumerate(("rotate_arbitrary", "rotate_cardinal")):
        b.add({
            "type": "image", "parent": p, "name": name,
            "x": rotate_x + index * (rotate_size + rotate_gap),
            "y": rotate_y, "w": rotate_size, "h": rotate_size,
            "image": "bench_scale.png", "codec": "raw", "fit": "contain",
            "rotation": {"default": 0, "min": -180, "max": 180},
        })

    # P13e canvas stream: a placeholder image drawn by the app's
    # synthetic camera / video / custom-stream producer.
    p = b.page("p_stream")
    # The no-PSRAM 240x240 target uses a smaller surface while retaining
    # the same high-rate Canvas update path.
    cw, ch = ((64, 32) if width < 320 else
              (width // 2, height // 3))
    b.add({"type": "image", "parent": p,
           "x": (width - cw) // 2, "y": (height - ch) // 2,
           "w": cw, "h": ch, "image": "bench_industrial.png",
           "codec": "lossless", "bind": "cv0"})
    b.add({"type": "label", "parent": p, "x": m, "y": height // 12,
           "w": inner, "h": row, "text": "CANVAS / DIRTY STREAM",
           "font_size": max(10, min(height // 26, width // 25)),
           "fg_color": "#8098C0", "text_align": "center"})

    # P14 wallpaper composite: tiled images fully covered by a
    # translucent color-tween cover (full-screen composite per frame).
    p = b.page("p_wall")
    for ty in range(0, height, 120):
        for tx in range(0, width, 160):
            b.add({"type": "image", "parent": p, "x": tx, "y": ty,
                   "w": 160, "h": 120, "image": "bench_industrial.png",
                   "codec": "raw"})
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#102030",
           "opacity": 120, "bind": "wall4"})

    # P16 animated images: compiled GIF playback (frame-diff QOI
    # patches decode into persistent buffers; only patch rects dirty).
    p = b.page("p_anim")
    # A 120x120 RGB565 composed frame costs 28.8 KB. Generate a 60x60
    # asset for the 240x240 no-PSRAM profile so the animation actually
    # runs instead of silently turning this page into an idle test.
    anim_size = 60 if width < 320 else 120
    for index in range(4):
        b.add({"type": "image", "parent": p,
               "x": (index % 2) * max(1, width - anim_size),
               "y": (index // 2) * max(1, height - anim_size),
               "w": anim_size, "h": anim_size,
               "image": "bench_anim.gif"})

    # P16b static composite motion: one compiled subtree (background,
    # image, text and hit target) is translated from SRAM-backed x/y.
    # This isolates static-transform cost from template instance motion.
    p = b.page("p_static_move")
    mover_w = min(120, width // 2)
    mover_h = min(100, height // 2)
    image_w = min(80, mover_w - 12)
    image_h = min(60, mover_h - 28)
    mover_colors = ["#184060", "#603018", "#285028", "#502850"]
    for index in range(4):
        mover = b.add({
            "type": "container", "parent": p,
            "name": f"static_mover{index}",
            "x": {
                "default": index * (width - mover_w) // 3,
                "min": 0, "max": width - mover_w,
            },
            "y": {
                "default": index * (height - mover_h) // 3,
                "min": 0, "max": height - mover_h,
            },
            "w": mover_w, "h": mover_h,
            "bg_color": mover_colors[index],
            "radius": 12, "callback": "on_static_mover",
        })
        b.add({"type": "image", "parent": mover, "x": 6, "y": 6,
               "w": image_w, "h": image_h, "image": "bench_industrial.png"})
        b.add({"type": "label", "parent": mover, "x": 6,
               "y": mover_h - 20, "w": mover_w - 12, "h": 16,
               "text": f"static SRAM {index}", "font_size": 12,
               "fg_color": "#FFFFFF"})
    b.add({"type": "image", "parent": p, "x": width // 3,
           "y": height // 3, "w": 160, "h": 120,
           "image": "bench_industrial_argb.png"})

    # P16c moving instances: template widgets swept across the full
    # screen by the app (the lvgl moving-wallpaper counterpart).
    p = b.page("p_move")
    move_page = p
    b.add({"type": "label", "parent": p, "x": m,
           "y": height // 2 - row, "w": inner, "h": 2 * row,
           "text": "template instances in motion",
           "fg_color": "#405068"})

    # Declared templates (instantiated at runtime, movable).
    tpl = b.add({"type": "container", "parent": 0, "x": 0, "y": 0,
                 "w": 100, "h": 100, "bg_color": "#40C080",
                 "radius": 16, "template": "box", "max_instances": 4})
    b.add({"type": "image", "parent": tpl, "x": 10, "y": 10,
           "w": 80, "h": 60, "image": "bench_industrial.png"})
    b.add({"type": "container", "parent": 0, "x": 0, "y": 0,
           "w": 140, "h": 140, "bg_color": "#E06090",
           "opacity": 110, "radius": 20, "template": "veil"})
    b.add({"type": "image", "parent": 0, "x": 0, "y": 0,
           "w": 160, "h": 120, "image": "bench_industrial_argb.png",
           "template": "sprite"})

    # A real album-style Grid keeps the public component in every benchmark
    # resolution. Each authored cell is independently clickable while the
    # viewport itself exercises the shared vertical drag/inertia path.
    grid_gap = max(3, min(width, height) // 60)
    grid_columns = 3
    grid_cell_w = (inner - (grid_columns - 1) * grid_gap) // grid_columns
    grid_cell_h = grid_cell_w
    gallery_cell = b.add({
        "type": "container", "parent": 0, "x": 0, "y": 0,
        "w": grid_cell_w, "h": grid_cell_h,
        "template": "gallery_cell", "callback": "on_gallery_cell",
        "bg_color": "#203048", "radius": max(4, grid_cell_w // 12),
    })
    b.add({"type": "image", "parent": gallery_cell, "x": 0, "y": 0,
           "w": grid_cell_w, "h": grid_cell_h,
           "image": "bench_industrial.png"})

    # Moving page shows a tiled backdrop so instance motion composites
    # over real content, not a flat background.
    b.add({"type": "image", "parent": move_page, "x": width // 4,
           "y": height // 4, "w": 160, "h": 120,
           "image": "bench_industrial_argb.png"})

    p = b.page("p_grid")
    b.add({
        "type": "grid", "parent": p, "name": "bench_grid",
        "x": m, "y": m, "w": inner, "h": height - 2 * m,
        "column_count": grid_columns,
        "column_gap": grid_gap, "row_gap": grid_gap,
        "cell_template": "gallery_cell", "item_count": 24,
        "scroll_snapshot": True,
    })

    # P17 full-page motion: color tween + spinner.
    p = b.page("p_motion")
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#07121E",
           "bind": "wall"})
    spin = max(40, min(width, height) // 4)
    for index in range(3):
        diameter = spin + (index + 1) * max(14, min(width, height) // 12)
        b.add({"type": "arc", "parent": p,
               "x": (width - diameter) // 2, "y": (height - diameter) // 2,
               "w": diameter, "h": diameter, "start_angle": 0, "sweep": 360,
               "thickness": 1, "value": 100, "fg_color": "#267B9B"})
    b.add({"type": "spinner", "parent": p,
           "x": width // 2 - spin // 2, "y": height // 2 - spin // 2,
           "w": spin, "h": spin, "speed": 900, "fg_color": "#52E8BA"})
    b.add({"type": "label", "parent": p, "x": m, "y": height * 5 // 6,
           "w": inner, "h": row, "text": "MOTION / CONTINUOUS COMPOSITION",
           "font_size": max(8, height // 42), "text_align": "center",
           "fg_color": "#DBF4FF"})

    # P16 wheels: three momentum-driven rollers (repeater scroll_blit
    # path under load) over a translucent center selection bar.
    p = b.page("p_wheel")
    wheel_h = height * 2 // 3
    wheel_y = height // 5
    wheel_w = (inner - 2 * gap) // 3
    sel_h = max(20, height // 10)
    b.add({"type": "container", "parent": p, "x": m,
           "y": wheel_y + wheel_h // 2 - sel_h // 2, "w": inner,
           "h": sel_h, "bg_color": "#5090E040"})
    for index in range(3):
        item = max(20, height // 10 + index * (height // 28))
        b.add({"type": "wheel", "parent": p,
               "x": m + index * (wheel_w + gap), "y": wheel_y,
               "w": wheel_w, "h": wheel_h,
               "items": [f"{n:02d}" for n in range(8)],
               "item_height": item, "name": f"whl{index}",
               "bind": f"whl{index}", "font_size": max(12, item // 2),
               "fg_color": "#C8D4E8", "bg_color": "#161E2C"})

    # P18 top-edge Drawer follows Mosaic's ownership model: the structural
    # component belongs to the page subtree, starts closed, and owns one
    # content layer. Hiding p_drawer therefore also removes the overlay from
    # rendering and edge-gesture eligibility.
    p = b.page("p_drawer")
    b.add({"type": "label", "parent": p, "x": m,
           "y": height // 2 - row, "w": inner, "h": 2 * row,
           "text": "pull down from the top edge",
           "fg_color": "#8098C0", "text_align": "center"})
    drawer = b.add({
        "type": "drawer", "parent": p, "name": "quick_drawer",
        "x": 0, "y": 0, "w": width, "h": height,
        "edge": "top", "open": False,
    })
    panel = b.add({
        "type": "layer", "parent": drawer, "name": "quick_main",
        "x": 0, "y": 0, "w": width, "h": height,
        "bg_color": "#07121E",
    })
    drawer_pad = max(8, width // 14)
    drawer_gap = max(6, height // 32)
    drawer_col = (width - 2 * drawer_pad - drawer_gap) // 2
    drawer_card_h = max(52, height * 2 // 5)
    drawer_rows_y = drawer_pad + drawer_card_h + drawer_gap
    drawer_row_h = max(
        28, (height - drawer_rows_y - 2 * drawer_pad - drawer_gap) // 2)
    for index, percent in enumerate((80, 60)):
        card_x = drawer_pad + index * (drawer_col + drawer_gap)
        card = b.add({
            "type": "container", "parent": panel,
            "x": card_x, "y": drawer_pad,
            "w": drawer_col, "h": drawer_card_h,
            "bg_color": "#153447", "radius": drawer_col // 8,
        })
        b.add({
            "type": "container", "parent": card, "x": 0, "y": 0,
            "w": drawer_col, "h": max(12, drawer_card_h // 5),
            "bg_color": "#19D3FF",
            "radius": min(drawer_col // 8, drawer_card_h // 5),
        })
        b.add({
            "type": "label", "parent": card, "x": 0,
            "y": drawer_card_h * 2 // 3,
            "w": drawer_col, "h": drawer_card_h // 3,
            "text": f"{percent}%",
            "fg_color": "#DBF4FF", "text_align": "center",
        })
    b.add({
        "type": "slider", "parent": panel, "name": "drawer_level",
        "x": drawer_pad, "y": drawer_rows_y,
        "w": drawer_col, "h": drawer_row_h, "value": 80,
        "bg_color": "#153447", "fg_color": "#52E8BA",
        "track_size": max(4, drawer_row_h // 8),
    })
    toggle_card = b.add({
        "type": "container", "parent": panel,
        "x": drawer_pad + drawer_col + drawer_gap, "y": drawer_rows_y,
        "w": drawer_col, "h": drawer_row_h,
        "bg_color": "#153447", "radius": drawer_row_h // 2,
    })
    toggle_h = max(20, drawer_row_h * 2 // 3)
    b.add({
        "type": "toggle", "parent": toggle_card, "name": "drawer_toggle",
        "x": (drawer_col - 2 * toggle_h) // 2,
        "y": (drawer_row_h - toggle_h) // 2,
        "w": 2 * toggle_h, "h": toggle_h, "checked": True,
        "bg_color": "#267B9B", "fg_color": "#19D3FF",
    })
    for index, caption in enumerate(("Mode A", "Mode B")):
        b.add({
            "type": "button", "parent": panel,
            "x": drawer_pad + index * (drawer_col + drawer_gap),
            "y": drawer_rows_y + drawer_row_h + drawer_gap,
            "w": drawer_col, "h": drawer_row_h,
            "text": caption,
            "bg_color": "#153447", "fg_color": "#DBF4FF",
            "radius": drawer_row_h // 2,
            "events": [{"event": "click", "action": "call",
                        "target_name": "drawer_choice", "arg": index}],
        })

    # P19 PageFlow: a clipped, cyclic three-page viewport moved through the
    # public component API. Each page mixes opaque fills, text and an image so
    # the transform path cannot pass by moving a flat rectangle only.
    p = b.page("p_flow")
    flow_y = max(22, height // 12)
    flow_h = height - flow_y
    flow = b.add({
        "type": "page_flow", "parent": p, "name": "bench_flow",
        "x": 0, "y": flow_y, "w": width, "h": flow_h,
        "page_count": 3, "selected": 0, "axis": "horizontal",
        "cyclic": True,
    })
    flow_colors = ("#143A5A", "#4A234F", "#234A38")
    for index, color in enumerate(flow_colors):
        page = b.add({
            "type": "layer", "parent": flow,
            "name": f"bench_flow_tab{index}",
            "x": index * width, "y": 0, "w": width, "h": flow_h,
            "bg_color": color,
        })
        b.add({"type": "label", "parent": page, "x": m, "y": flow_h // 8,
               "w": inner, "h": row * 2, "text": f"PageFlow {index + 1}",
               "text_align": "center", "fg_color": "#FFFFFF"})
        b.add({"type": "image", "parent": page,
               "x": (width - min(160, inner)) // 2,
               "y": flow_h // 4, "w": min(160, inner),
               "h": min(120, flow_h // 2), "image": "bench_industrial.png"})
        metric_y = flow_h * 3 // 4
        metric_w = max(30, (inner - 2 * gap) // 3)
        for metric, caption in enumerate(("FPS", "DMA", "CACHE")):
            card = b.add({
                "type": "container", "parent": page,
                "x": m + metric * (metric_w + gap), "y": metric_y,
                "w": metric_w, "h": max(24, flow_h - metric_y - gap),
                "bg_color": "#162538", "opacity": 220, "radius": 6,
                "border_color": "#3D7DA8", "border_width": 1,
            })
            b.add({
                "type": "label", "parent": card, "x": 2, "y": 0,
                "w": metric_w - 4, "h": max(20, flow_h - metric_y - gap),
                "text": caption, "text_align": "center",
                "font_size": max(8, height // 36),
                "fg_color": "#B8E6FF",
            })

    # P20 StackView: fixed-capacity forward/back navigation with animated
    # viewport motion and deterministic public-API driving.
    p = b.page("p_stack")
    stack_y = max(22, height // 12)
    stack_h = height - stack_y
    stack = b.add({
        "type": "stackview", "parent": p, "name": "bench_stack",
        "x": 0, "y": stack_y, "w": width, "h": stack_h,
        "page_count": 3, "initial_page": 0, "capacity": 3,
        "axis": "horizontal",
    })
    for index, color in enumerate(("#24344A", "#45304D", "#24483C")):
        page = b.add({
            "type": "layer", "parent": stack,
            "name": f"bench_stack_page{index}",
            "x": index * width, "y": 0, "w": width, "h": stack_h,
            "bg_color": color,
        })
        b.add({"type": "label", "parent": page, "x": m,
               "y": stack_h // 10, "w": inner, "h": row * 2,
               "text": f"Stack depth {index}", "text_align": "center",
               "fg_color": "#FFFFFF"})
        image_w = min(160, inner // 2)
        image_h = min(120, stack_h // 3)
        b.add({"type": "image", "parent": page,
               "x": (width - image_w) // 2, "y": stack_h // 4,
               "w": image_w, "h": image_h, "image": "bench_industrial.png"})
        card_y = stack_h * 2 // 3
        card_w = max(32, (inner - 2 * gap) // 3)
        for card_index, value in enumerate(("CPU", "BUS", "LCD")):
            card = b.add({
                "type": "container", "parent": page,
                "x": m + card_index * (card_w + gap), "y": card_y,
                "w": card_w, "h": max(26, stack_h - card_y - gap),
                "bg_color": "#172536", "radius": 6,
                "border_color": "#4B8A76", "border_width": 1,
            })
            b.add({"type": "label", "parent": card, "x": 2, "y": 0,
                   "w": card_w - 4, "h": max(20, stack_h - card_y - gap),
                   "text": value, "text_align": "center",
                   "font_size": max(8, height // 36),
                   "fg_color": "#BFEEDA"})

    # P21 variable-height conversation history. Static chat chrome makes the
    # benchmark read like a real application while the message viewport stays
    # an independently styled, reusable component.
    p = b.page("p_messages")
    short_side = min(width, height)
    header_h = max(34, height // 11)
    composer_h = max(34, height // 12)
    avatar = max(20, min(header_h - 10, short_side // 12))
    chrome_font = max(9, short_side // 28)
    subtitle_font = max(8, short_side // 40)
    b.add({"type": "container", "parent": p,
           "x": 0, "y": 0, "w": width, "h": header_h,
           "bg_color": "#162330"})
    b.add({"type": "shape", "shape": "circle", "parent": p,
           "x": m, "y": (header_h - avatar) // 2,
           "w": avatar, "h": avatar, "bg_color": "#2D7DFF"})
    b.add({"type": "label", "parent": p,
           "x": m, "y": (header_h - avatar) // 2,
           "w": avatar, "h": avatar, "text": "G",
           "text_align": "center", "font_size": chrome_font,
           "fg_color": "#FFFFFF"})
    title_x = m + avatar + max(6, gap // 2)
    b.add({"type": "label", "parent": p,
           "x": title_x, "y": max(3, header_h // 7),
           "w": width - title_x - m, "h": header_h // 2,
           "text": f"{CASES['p_messages'][0]:02d} / MESSAGES", "font_size": chrome_font,
           "fg_color": "#F4F7FB"})
    b.add({"type": "label", "parent": p,
           "x": title_x, "y": header_h // 2,
           "w": width - title_x - m, "h": header_h // 3,
           "text": "Connected", "font_size": subtitle_font,
           "fg_color": "#62D49A"})

    composer_y = height - composer_h
    b.add({"type": "container", "parent": p,
           "x": 0, "y": composer_y, "w": width, "h": composer_h,
           "bg_color": "#162330"})
    send_size = max(22, min(composer_h - 8, short_side // 10))
    send_x = width - m - send_size
    input_y = composer_y + max(4, (composer_h - send_size) // 2)
    input_w = max(20, send_x - m - max(5, gap // 2))
    b.add({"type": "container", "parent": p,
           "x": m, "y": input_y, "w": input_w, "h": send_size,
           "bg_color": "#223243", "radius": send_size // 2})
    b.add({"type": "label", "parent": p,
           "x": m + max(7, send_size // 3), "y": input_y,
           "w": input_w - max(12, send_size // 2), "h": send_size,
           "text": "Message...", "font_size": subtitle_font,
           "fg_color": "#8FA3B8"})
    b.add({"type": "shape", "shape": "circle", "parent": p,
           "x": send_x, "y": input_y, "w": send_size, "h": send_size,
           "bg_color": "#2D7DFF"})
    b.add({"type": "label", "parent": p,
           "x": send_x, "y": input_y, "w": send_size, "h": send_size,
           "text": ">", "text_align": "center",
           "font_size": chrome_font, "fg_color": "#FFFFFF"})

    message_top = header_h + max(3, gap // 3)
    message_bottom = composer_y - max(3, gap // 3)
    b.add({
        "type": "message_list", "parent": p, "name": "bench_messages",
        "x": m, "y": message_top, "w": inner,
        "h": message_bottom - message_top,
        "incoming_color": "#263A4F", "outgoing_color": "#2D7DFF",
        "message_text_color": "#EEF4FA",
        "outgoing_text_color": "#FFFFFF",
        "bubble_radius": max(7, min(16, short_side // 24)),
        "bubble_padding_x": max(7, short_side // 32),
        "bubble_padding_y": max(5, short_side // 48),
        "message_gap": max(5, short_side // 48),
        "side_margin": max(6, width // 32),
        "max_bubble_width": inner * 74 // 100,
        "max_message_height": max(72, min(220, height // 2)),
        "font_size": max(9, short_side // 24),
    })

    # P22 retained widgets: continuously driven value, boolean, list and
    # drawing controls. Compile-time composites live on the final page so all
    # macro-expanded subtrees remain contiguous.
    p = b.page("p_widgets")
    col = inner // 2 - gap
    y = height // 16
    small = max(12, height // 26)
    # The click action gives the button a hit box so injected taps
    # (app-driven stress) show the press feedback.
    b.add({"type": "button", "parent": p, "x": m, "y": y, "w": col,
           "h": row + 6, "text": "button", "font_size": small,
           "bg_color": "#2A3550", "fg_color": "#FFFFFF", "radius": 4,
           "events": [{"event": "click", "action": "call",
                       "target_name": "bench_btn"}]})
    b.add({"type": "toggle", "parent": p, "x": m + col + gap, "y": y,
           # Keep the rail wider than the knob on tall/narrow panels or
           # the compiler drops the slide track (and its private bind).
           # radius=0 exercises the rectangular-switch path (square
           # track and knob instead of the default pill).
           "w": max(40, col // 3, row * 2), "h": row, "checked": False,
           "radius": 0, "bind": "w_tog", "fg_color": "#50B070",
           "bg_color": "#283040"})
    y += row + 2 * gap
    b.add({"type": "checkbox", "parent": p, "x": m, "y": y,
           "w": row, "h": row, "checked": True, "bind": "w_chk",
           "fg_color": "#60C080"})
    b.add({"type": "radio", "parent": p, "x": m + 2 * row, "y": y,
           "w": row, "h": row, "checked": False, "bind": "w_rad",
           "fg_color": "#E0A050"})
    b.add({"type": "spinner", "parent": p, "x": m + 4 * row, "y": y,
           "w": row + 6, "h": row + 6, "speed": 900,
           "fg_color": "#E05050"})
    y += row + 2 * gap
    b.add({"type": "chart", "parent": p, "x": m, "y": y, "w": col,
           "h": height // 5, "points": [5, 30, 18, 60, 42, 88, 66],
           "fg_color": "#5090E0", "bg_color": "#141C2C",
           "grid_lines": 3})
    b.add({"type": "list", "name": "widget_list", "parent": p, "x": m + col + gap, "y": y,
           "w": col, "h": height // 5,
           "items": ["alpha", "beta", "gamma", "delta", "epsilon",
                     "zeta"],
           "item_height": max(18, height // 14), "bind": "w_list",
           "font_size": small, "fg_color": "#DBF4FF", "bg_color": "#10283B"})
    y += height // 5 + gap
    b.add({"type": "wheel", "name": "widget_wheel", "parent": p, "x": m, "y": y, "w": col,
           "h": height // 6,
           "items": ["one", "two", "three", "four", "five"],
           "item_height": max(18, height // 14), "cyclic": True,
           "bind": "w_whl", "font_size": small,
           "fg_color": "#DBF4FF", "bg_color": "#10283B"})
    b.add({"type": "progress", "parent": p, "x": m + col + gap,
           "y": y, "w": col, "h": max(18, height // 14),
           "value": 50, "bind": "w_progress",
           "bg_color": "#202A3C", "fg_color": "#45B6FF"})

    # P23 compile-time composites: a full-screen TabView cycles continuously.
    # Its pages contain Dropdown/Table, the real Keyboard composite, and an
    # image-backed modal workload. Keeping all composites in the final authored
    # subtree preserves the compiler's append-only macro index contract.
    p = b.page("p_composites")
    tabs_y = max(18, height // 18)
    tabs_h = height - tabs_y
    bar_h = max(26, height // 9)
    b.add({
        "type": "tabview", "parent": p, "name": "bench_tabs",
        "x": 0, "y": tabs_y, "w": width, "h": tabs_h,
        "tabs": ["Data", "Input", "Modal"], "selected": 0,
        "bar_height": bar_h, "cyclic": True,
        "bg_color": "#141D2B", "active_color": "#45B6FF",
        "font_size": max(9, height // 30),
    })
    content_h = tabs_h - bar_h
    b.add({"type": "dropdown", "parent_name": "bench_tabs_tab0",
           "x": m, "y": gap, "w": inner, "h": max(24, height // 11),
           "options": ["Demo data", "Controls", "Media"],
           "font_size": max(9, height // 30)})
    b.add({"type": "table", "parent_name": "bench_tabs_tab0",
           "x": m, "y": max(24, height // 11) + 2 * gap,
           "w": inner, "columns": ["path", "data", "mode"],
           "rows": [["draw", "shape", "demo"],
                    ["media", "image", "demo"],
                    ["input", "keys", "demo"]],
           "font_size": max(9, height // 32),
           "row_height": max(24, content_h // 6)})
    input_h = max(80, content_h * 3 // 4)
    b.add({"type": "label", "parent_name": "bench_tabs_tab1",
           "x": m, "y": 2, "w": inner, "h": max(18, content_h // 5),
           "text": "", "bind": "kt0", "input": True,
           "bg_color": "#141C2C", "fg_color": "#E8F0FF",
           "radius": 6})
    b.add({"type": "keyboard", "parent_name": "bench_tabs_tab1",
           "name": "bench_keyboard", "x": 0,
           "y": max(20, content_h // 5), "w": width, "h": input_h,
           "font_size": max(8, min(16, height // 32)),
           "bg_color": "#101626", "fg_color": "#FFFFFF"})
    media_w = min(160, inner)
    media_h = min(120, max(60, content_h * 2 // 3))
    b.add({"type": "image", "parent_name": "bench_tabs_tab2",
           "x": (width - media_w) // 2, "y": gap,
           "w": media_w, "h": media_h, "image": "bench_industrial.png"})
    b.add({"type": "label", "parent_name": "bench_tabs_tab2",
           "x": m, "y": media_h + 2 * gap, "w": inner,
           "h": max(18, content_h - media_h - 2 * gap),
           "text": "Full-screen modal alpha stress",
           "text_align": "center", "fg_color": "#DCE8FF",
           "font_size": max(9, height // 30)})
    b.add({
        "type": "msgbox", "parent": p, "name": "bench_modal",
        "title": "Performance run",
        "text": "Renderer, presenter and controls remain under load.",
        "buttons": ["Cancel", "Continue"], "callback": "bench_modal_choice",
        "w": min(width - 2 * max(8, m), 480),
        "h": min(height - 2 * max(8, gap), 260),
        "font_size": max(9, height // 32), "hidden": True,
        "dismissable": True,
    })

    # Append the sparse animation page so existing authored object indexes
    # remain stable. Two distant 8x8 regions change on every source frame.
    p = b.page("p_anim_sparse")
    b.add({"type": "image", "parent": p,
           "x": (width - anim_size) // 2,
           "y": (height - anim_size) // 2,
           "w": anim_size, "h": anim_size,
           "image": "bench_anim_sparse.gif"})

    # Deliberately dense mixed-content saturation page. It combines a
    # full-screen opaque image, translucent HUD surfaces and four independently
    # translated alpha images. The 4 ms driver updates positions and meters to
    # keep raster, blend, dirty-region and submission paths under back-pressure.
    p = b.page("p_storm")
    b.add({"type": "image", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "image": "bench_industrial.png",
           "fit": "cover"})
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#06111C90"})
    header_h = max(28, height // 9)
    footer_h = max(46, height // 5)
    b.add({"type": "container", "parent": p, "x": 0, "y": 0,
           "w": width, "h": header_h, "bg_color": "#081827D8",
           "border_color": "#1DD6FF", "border_width": 1})
    b.add({"type": "label", "parent": p, "x": m,
           "y": max(2, header_h // 7), "w": inner * 2 // 3,
           "h": header_h * 2 // 3,
           "text": f"{CASES['p_storm'][0]:02d} / RENDER STORM",
           "font_size": max(11, height // 24), "fg_color": "#F2FAFF"})
    b.add({"type": "label", "parent": p,
           "x": width - m - inner // 3, "y": max(2, header_h // 7),
           "w": inner // 3, "h": header_h * 2 // 3,
           "text": "SATURATION", "text_align": "right",
           "font_size": max(8, height // 38), "fg_color": "#FFAD3D"})
    b.add({"type": "container", "parent": p,
           "x": 0, "y": height - footer_h, "w": width, "h": footer_h,
           "bg_color": "#081827E8", "border_color": "#1DD6FF",
           "border_width": 1})
    storm_accent = {"theme": "shape_accent"}
    beam_w = max(2, width // 180)
    for index in range(4):
        b.add({"type": "shape", "shape": "line", "parent": p,
               "x": index * width // 4, "y": header_h,
               "w": width // 3, "h": max(1, height - header_h - footer_h),
               "line_direction": "tl_br" if index % 2 == 0 else "bl_tr",
               "border_color": storm_accent, "border_width": beam_w,
               "opacity": 144})
    meter_gap = max(3, gap // 2)
    meter_w = (inner - 3 * meter_gap) // 4
    meter_h = max(10, footer_h // 4)
    for index in range(4):
        meter_x = m + index * (meter_w + meter_gap)
        b.add({"type": "progress", "parent": p,
               "x": meter_x, "y": height - footer_h // 2,
               "w": meter_w, "h": meter_h,
               "value": 0, "bind": f"storm_v{index}",
               "bg_color": "#193147", "fg_color": "#19D3FF",
               "radius": max(2, meter_h // 3)})
    mover_w = max(58, min(136, width // 3))
    mover_h = max(46, min(96, (height - header_h - footer_h) * 2 // 3))
    x_max = max(0, width - mover_w)
    y_min = header_h
    y_max = max(y_min, height - footer_h - mover_h)
    for index in range(4):
        mover = b.add({
            "type": "container", "parent": p,
            "name": f"storm_mover{index}",
            "x": {"default": index * x_max // 3, "min": 0,
                  "max": x_max},
            "y": {"default": y_min + index * (y_max - y_min) // 3,
                  "min": y_min, "max": y_max},
            "w": mover_w, "h": mover_h, "bg_color": "#0A2330B8",
            "radius": max(5, mover_h // 10), "border_color": "#34D7FF",
            "border_width": 1,
        })
        b.add({"type": "image", "parent": mover, "x": 2, "y": 2,
               "w": mover_w - 4, "h": mover_h - 4,
               "image": "bench_industrial_argb.png"})

    p = b.page("p_dropdown")
    b.add({"type": "label", "parent": p, "x": m, "y": height // 7,
           "w": inner, "h": max(20, height // 12), "text": "OUTPUT PIPELINE",
           "fg_color": "#52E8BA", "font_size": max(11, height // 30)})
    b.add({"type": "dropdown", "parent": p, "name": "bench_select",
           "x": m, "y": height // 4, "w": inner, "h": max(26, height // 12),
           "options": ["RGB / scanout", "SPI / DMA", "DSI / framebuffer"],
           "item_height": max(20, height // 12), "selected": 0,
           "font_size": max(11, height // 30), "bg_color": "#153447",
           "panel_color": "#10283B", "fg_color": "#DBF4FF"})
    b.add({"type": "label", "parent": p, "x": m, "y": height * 5 // 6,
           "w": inner, "h": height // 10, "text": "OPEN / SELECT / VERIFY / LOOP",
           "font_size": max(8, height // 45), "fg_color": "#86AFC6"})

    for count in (1, 8, 32, 64):
        p = b.page(f"p_load{count}")
        top = max(50, height // 5)
        box_w, box_h = width // 4, max(20, (height - top - 20) // 3)
        b.add({"type": "label", "parent": p, "x": m, "y": max(20, height // 12),
               "w": inner, "h": max(20, height // 12),
               "text": f"{count:02d} RECT / ALPHA 128",
               "font_size": max(11, height // 28), "fg_color": "#52E8BA"})
        for index in range(count):
            # Prefix-stable positions and fixed geometry: only count changes.
            x = (index * 37 + width // 3) % (width - box_w)
            y = top + (index * 53) % max(1, height - top - box_h - 12)
            b.add({"type": "container", "parent": p, "x": x, "y": y,
                   "w": box_w, "h": box_h, "opacity": 128,
                   "bg_color": {"theme": "load_accent"}})

    stage = b.add({"type": "layer", "parent": 0, "x": 0, "y": 0,
                   "w": width, "h": height, "hidden": True,
                   "bind": "transition_stage"})
    transition_dashboard(b, stage, False)
    # Outside scored windows. Small displays show three readable cards,
    # landscape displays use two columns, and tall displays use six rows.
    results = b.add({"type": "layer", "parent": 0, "x": 0, "y": 0,
                     "w": width, "h": height, "hidden": True,
                     "bind": "results_overlay"})
    b.add({"type": "container", "parent": results, "x": 0, "y": 0,
           "w": width, "h": height, "bg_color": "#07121E"})
    result_count = 6 if width >= 600 or height >= 600 else 3
    columns = 2 if width >= 600 else 1
    rows = result_count // columns
    result_margin = max(8, width // 40)
    title_h = max(20, height // 16)
    top = max(46, height // 7)
    footer_h = max(18, height // 20)
    card_gap = max(5, height // 60)
    card_w = (width - (columns + 1) * result_margin) // columns
    card_h = (height - top - footer_h - (rows - 1) * card_gap) // rows
    name_font = max(12, min(card_w // 18, card_h // 4))
    value_font = max(9, min(card_w // 26, card_h // 5))
    fps_font = max(20, min(card_w // 9, card_h * 2 // 5))
    b.add({"type": "label", "parent": results, "x": result_margin, "y": 3,
           "w": width - 2 * result_margin, "h": title_h,
           "font_size": max(14, title_h * 2 // 3),
           "text": "GSP / PERFORMANCE LAB", "fg_color": "#DBF4FF"})
    b.add({"type": "label", "parent": results, "x": result_margin, "y": title_h + 3,
           "w": width - 2 * result_margin, "h": top - title_h - 6,
           "font_size": max(9, title_h // 2), "text": "MEASURED RESULTS",
           "bind": "result_title", "fg_color": "#19D3FF"})
    for index in range(6):
        x = result_margin + (index % columns) * (card_w + result_margin)
        y = top + (index // columns) * (card_h + card_gap)
        card = b.add({"type": "layer", "parent": results, "x": x, "y": y,
                      "w": card_w, "h": card_h, "hidden": index >= result_count,
                      "bind": f"result_card{index}"})
        b.add({"type": "container", "parent": card, "x": 0, "y": 0,
               "w": card_w, "h": card_h, "radius": max(5, card_gap),
               "bg_color": "#10283B"})
        b.add({"type": "container", "parent": card, "x": 0, "y": 8,
               "w": 2, "h": card_h - 16, "bg_color": "#19D3FF"})
        b.add({"type": "label", "parent": card, "x": 8, "y": 3,
               "w": card_w - 16, "h": card_h // 3,
               "font_size": name_font, "text": "", "bind": f"result_name{index}",
               "fg_color": "#DBF4FF"})
        b.add({"type": "label", "parent": card, "x": 8, "y": card_h // 3,
               "w": card_w // 2 - 8, "h": card_h * 2 // 3 - 2,
               "font_size": fps_font, "text": "", "bind": f"result_fps{index}",
               "fg_color": "#52E8BA"})
        b.add({"type": "label", "parent": card, "x": card_w // 2, "y": card_h // 3,
               "w": card_w // 2 - 6, "h": card_h * 2 // 3 - 2,
               "font_size": value_font, "text": "", "bind": f"result_value{index}",
               "fg_color": "#86AFC6"})
    b.add({"type": "label", "parent": results, "x": result_margin,
           "y": height - footer_h + 2, "w": width - 2 * result_margin,
           "h": footer_h - 2, "font_size": max(8, footer_h // 2),
           "text": "FPS / RENDER / SUBMIT ms  |  AUTO LOOP", "fg_color": "#86AFC6"})

    return {
        "screen": "bench",
        "w": width, "h": height,
        "screen_bg": "#101820",
        "themes": {
            "load_accent": {
                "type": "color", "default": "#19D3FF", "dynamic": True,
            },
            "shape_accent": {
                "type": "color",
                "default": "#45B6FF",
                "dynamic": True,
            },
        },
        "font": FONT,
        "default_font_size": min(
            b.font_palette,
            key=lambda size: abs(size - max(14, height // 22))),
        "objects": b.finish(),
    }


def transition_dashboard(b, parent, alternate):
    """Two distinct, dense scenes make direction and fade progress readable."""
    w, h, m = b.w, b.h, max(8, b.w // 24)
    accent = "#FFAF4D" if alternate else "#19D3FF"
    b.add({"type": "container", "parent": parent, "x": 0, "y": 0,
           "w": w, "h": h, "bg_color": "#231B20" if alternate else "#07121E"})
    if alternate:
        side = min(w // 5, h // 4)
        for index in range(6):
            b.add({"type": "arc", "parent": parent,
                   "x": w * 2 // 5 + index % 3 * (w // 5),
                   "y": h // 4 + index // 3 * (h // 3),
                   "w": side, "h": side, "start_angle": 120, "sweep": 300,
                   "value": 35 + index * 11, "thickness": max(3, side // 10),
                   "fg_color": accent, "bg_color": "#3D3440"})
    else:
        b.add({"type": "image", "parent": parent, "x": w // 3,
               "y": h // 6, "w": w * 2 // 3, "h": h * 2 // 3,
               "image": "bench_industrial.png", "fit": "cover"})
    b.add({"type": "container", "parent": parent, "x": m, "y": h // 5,
           "w": w // 3, "h": h * 3 // 5, "bg_color": "#0A1E30E0",
           "border_color": accent, "border_width": 1, "radius": max(4, h // 40)})
    b.add({"type": "label", "parent": parent, "x": m, "y": h // 20,
           "w": w - 2 * m, "h": h // 10,
           "text": "GSP / AUTOMATION", "font_size": max(13, h // 24),
           "fg_color": accent})
    b.add({"type": "label", "parent": parent, "x": 2 * m, "y": h // 4,
           "w": w // 3 - 2 * m, "h": h // 5,
           "text": "02" if alternate else "01",
           "font_size": max(20, h // 12), "fg_color": "#DBF4FF"})
    for index in range(4):
        b.add({"type": "progress", "parent": parent, "x": 2 * m,
               "y": h // 2 + index * max(7, h // 18),
               "w": w // 3 - 2 * m, "h": max(3, h // 60),
               "value": ((index + (2 if alternate else 1)) * 23) % 100,
               "bg_color": "#153447", "fg_color": accent})
    b.add({"type": "label", "parent": parent, "x": m, "y": h * 7 // 8,
           "w": w - 2 * m, "h": h // 12,
           "text": "SCENE B / TRANSITION STRESS" if alternate else
                   "SCENE A / TRANSITION STRESS",
           "font_size": max(9, h // 40), "fg_color": "#86AFC6"})


def alt_scene(width, height):
    b = Builder(width, height)
    transition_dashboard(b, 0, True)
    return {
        "screen": "bench_alt",
        "w": width, "h": height,
        "screen_bg": "#07121E",
        "font": FONT,
        "default_font_size": min(
            b.font_palette,
            key=lambda size: abs(size - max(14, height // 22))),
        "objects": b.objects,
    }


def main():
    os.chdir(Path(__file__).resolve().parent)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check", action="store_true",
        help="fail if checked-in generated scenes are stale")
    args = parser.parse_args()
    generated_assets = {
        "bench_anim.gif": scanner_animation_bytes(),
        "bench_scale.png": scale_asset_bytes(),
        "bench_anim_sparse.gif": sparse_animation_bytes(),
    }
    for name, content in generated_assets.items():
        path = Path(name)
        if args.check:
            if not path.exists() or path.read_bytes() != content:
                print(f"stale benchmark asset: {name}", file=sys.stderr)
                return 1
        else:
            path.write_bytes(content)
            print(f"wrote {name}")
    validate_assets()
    generated = {}
    for width, height in RESOLUTIONS:
        main_scene = scene(width, height)
        name = f"bench_{width}.json"
        generated[name] = json.dumps(main_scene, indent=2) + "\n"
        alt = f"bench_alt_{width}.json"
        generated[alt] = json.dumps(alt_scene(width, height), indent=2) + "\n"
    for width, height in ((1024, 600), (800, 480)):
        main_scene = scene(width, height, rgb888=True)
        generated[f"bench_rgb888_{width}.json"] = json.dumps(
            main_scene, indent=2) + "\n"
    stale = []
    for name, content in generated.items():
        path = Path(name)
        if args.check:
            if not path.exists() or path.read_text(encoding="utf-8") != content:
                stale.append(name)
        else:
            path.write_text(content, encoding="utf-8")
            print(f"wrote {name}")
    if stale:
        print("stale benchmark scenes: " + ", ".join(stale), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
