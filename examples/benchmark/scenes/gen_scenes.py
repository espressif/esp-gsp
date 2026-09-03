#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Generates the per-resolution benchmark scenes and validates image assets.

One scene, one full-screen page layer per benchmark case, mirroring
lv_demo_benchmark where its scenes map onto GSP render classes and
extending it to cover every widget, image codec path and font size the
framework supports. The app cycles page visibility, samples the
rendered-frame counter per page and prints a duration-weighted summary.
"""

import argparse
import io
import json
import os
import sys
from pathlib import Path

from PIL import Image

RESOLUTIONS = [(1024, 600), (800, 480), (480, 800), (360, 360),
               (320, 240), (240, 240)]
FONT = "../../common/fonts/DejaVuSans.ttf"

LOREM = ("The quick brown fox jumps over the lazy dog. "
         "Pack my box with five dozen liquor jugs. " * 6)

FONT_PALETTES = {
    240: (8, 9, 11, 12, 13, 14, 16, 20),
    360: (11, 12, 13, 16, 18, 24, 30),
    480: (12, 15, 16, 18, 21, 24, 32, 41),
    600: (12, 16, 18, 20, 23, 27, 40, 51),
    800: (12, 16, 20, 26, 30, 40, 54, 68),
}


def validate_assets():
    expected = {
        "bench_rgb.png": ("RGB", (640, 480)),
        "bench_scale.png": ("RGB", (96, 64)),
        "bench_argb.png": ("RGBA", (320, 240)),
        "bench_anim.gif": ("P", (120, 120)),
    }
    for name, (mode, size) in expected.items():
        with Image.open(name) as image:
            if image.mode != mode:
                raise ValueError(
                    f"{name}: expected {mode}, found {image.mode}")
            if image.size != size:
                raise ValueError(
                    f"{name}: expected {size}, found {image.size}")
    with Image.open("bench_argb.png") as image:
        if image.getchannel("A").getextrema() != (0, 255):
            raise ValueError("bench_argb.png must contain real alpha coverage")
    with Image.open("bench_anim.gif") as image:
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


def scale_asset_bytes():
    """Build the small, deterministic runtime-scaling benchmark source."""
    with Image.open("bench_rgb.png") as source:
        image = source.convert("RGB").resize(
            (96, 64), Image.Resampling.LANCZOS)
    output = io.BytesIO()
    image.save(output, format="PNG", optimize=False)
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
        self.font_palette = FONT_PALETTES[height]
        self.objects = [
            {"type": "container", "parent": -1, "x": 0, "y": 0,
             "w": width, "h": height, "bg_color": "#101820"},
        ]

    def page(self, bind, hidden=True, tag=True):
        self.objects.append({"type": "layer", "parent": 0, "x": 0,
                             "y": 0, "w": self.w, "h": self.h,
                             "hidden": hidden, "bind": bind})
        layer = len(self.objects) - 1
        if tag:
            # Top-left tag naming the scenario under test. Full-surface image
            # grids opt out so this label cannot be mistaken for a stale or
            # missing decoded tile.
            self.add({"type": "label", "parent": layer, "x": 4, "y": 2,
                      "w": self.w // 2, "h": max(14, self.h // 30),
                      "text": bind[2:].replace("_", " "),
                      "font_size": max(11, self.h // 40),
                      "fg_color": "#8098C0"})
        return layer

    def add(self, obj):
        if "font_size" in obj:
            requested = obj["font_size"]
            obj["font_size"] = min(
                self.font_palette, key=lambda size: abs(size - requested))
        self.objects.append(obj)
        return len(self.objects) - 1

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
        b.add({"type": "rect" if index == 0 else "container", "parent": p,
               "x": m + (index % 3) * (card_w + gap),
               "y": height // 8 + (index // 3) * (card_h + gap),
               "w": card_w, "h": card_h, "bg_color": "#203048",
               "bind": f"cc{index}", "radius": 14,
               "border_color": "#5070B0", "border_width": 3})

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
        b.add({"type": "label", "parent": p, "x": m, "y": y,
               "w": inner, "h": line, "text": f"line {index}",
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
           "text": LOREM[: max(80, (width * height) // 2400)],
           "font_size": max(13, height // 26), "fg_color": "#C8D2E8"})

    # P10 scroll simulation: a column of rows rewritten every tick.
    p = b.page("p_scroll")
    y = height // 14
    line = max(16, height // 11)
    for index in range(8):
        b.add({"type": "label", "parent": p, "x": m, "y": y,
               "w": inner, "h": line, "text": ".",
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

    # P12 RGB image grid re-blit under a sweep.
    p = b.page("p_imgrgb")
    for index in range(9):
        x = (index % 3) * max(1, (width - 160)) // 2
        y2 = (index // 3) * max(1, (height - 120)) // 2
        parent = p
        if index % 2 == 0:
            parent = b.add({"type": "layer", "parent": p, "x": x,
                            "y": y2, "w": 160, "h": 120,
                            "bind": f"iv{index // 2}"})
            x, y2 = 0, 0
        b.add({"type": "image", "parent": parent, "x": x, "y": y2,
               "w": 160, "h": 120, "image": "bench_rgb.png",
               "codec": "raw"})

    # P13 ARGB image grid (alpha blend) under a sweep.
    p = b.page("p_imgargb")
    for index in range(9):
        x = (index % 3) * max(1, (width - 160)) // 2
        y2 = (index // 3) * max(1, (height - 120)) // 2
        parent = p
        if index % 2 == 0:
            parent = b.add({"type": "layer", "parent": p, "x": x,
                            "y": y2, "w": 160, "h": 120,
                            "bind": f"av{index // 2}"})
            x, y2 = 0, 0
        b.add({"type": "image", "parent": parent, "x": x, "y": y2,
               "w": 160, "h": 120, "image": "bench_argb.png",
               "codec": "raw"})

    # P13b QOI-decoded grid: tiles are stored compressed (QOI/RLE16 +
    # DECODE_LRU) except dyn0's flash-mapped initial placeholder.
    # Small panels shrink the decoded tiles: no-PSRAM chips must fit
    # the two cache entries next to the frame buffer.
    qw, qh = (160, 120) if width >= 320 else (80, 60)
    p = b.page("p_qoi", tag=False)
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
                "image": "bench_argb.png" if index % 2 else
                         "bench_rgb.png"}
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
    p = b.page("p_scale", tag=False)
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

    # P13d canvas stream: a placeholder image drawn by the app's
    # synthetic camera / video / custom-stream producer.
    p = b.page("p_stream")
    # The no-PSRAM 240x240 target uses a smaller surface while retaining
    # the same 25 fps Canvas update path.
    cw, ch = ((64, 32) if width < 320 else
              (width // 2, height // 3))
    b.add({"type": "image", "parent": p,
           "x": (width - cw) // 2, "y": (height - ch) // 2,
           "w": cw, "h": ch, "image": "bench_rgb.png",
           "codec": "lossless", "bind": "cv0"})
    b.add({"type": "label", "parent": p, "x": m, "y": height // 12,
           "w": inner, "h": row, "text": "canvas stream 25 fps",
           "fg_color": "#8098C0", "text_align": "center"})

    # P14 wallpaper composite: tiled images fully covered by a
    # translucent color-tween cover (full-screen composite per frame).
    p = b.page("p_wall")
    for ty in range(0, height, 120):
        for tx in range(0, width, 160):
            b.add({"type": "image", "parent": p, "x": tx, "y": ty,
                   "w": 160, "h": 120, "image": "bench_rgb.png",
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
               "w": image_w, "h": image_h, "image": "bench_rgb.png"})
        b.add({"type": "label", "parent": mover, "x": 6,
               "y": mover_h - 20, "w": mover_w - 12, "h": 16,
               "text": f"static SRAM {index}", "font_size": 12,
               "fg_color": "#FFFFFF"})
    b.add({"type": "image", "parent": p, "x": width // 3,
           "y": height // 3, "w": 160, "h": 120,
           "image": "bench_argb.png"})

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
           "w": 80, "h": 60, "image": "bench_rgb.png"})
    b.add({"type": "container", "parent": 0, "x": 0, "y": 0,
           "w": 140, "h": 140, "bg_color": "#E06090",
           "opacity": 110, "radius": 20, "template": "veil"})
    b.add({"type": "image", "parent": 0, "x": 0, "y": 0,
           "w": 160, "h": 120, "image": "bench_argb.png",
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
           "image": "bench_rgb.png"})

    # Moving page shows a tiled backdrop so instance motion composites
    # over real content, not a flat background.
    b.add({"type": "image", "parent": move_page, "x": width // 4,
           "y": height // 4, "w": 160, "h": 120,
           "image": "bench_argb.png"})

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
           "w": width, "h": height, "bg_color": "#402010",
           "bind": "wall"})
    spin = max(24, min(width, height) // 6)
    b.add({"type": "spinner", "parent": p,
           "x": width // 2 - spin // 2, "y": height // 2 - spin // 2,
           "w": spin, "h": spin, "speed": 900, "fg_color": "#E05050"})

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
               "items": [f"item {n}" for n in range(8)],
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
        "bg_color": "#050505",
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
            "bg_color": "#F4F5F7", "radius": drawer_col // 8,
        })
        b.add({
            "type": "container", "parent": card, "x": 0, "y": 0,
            "w": drawer_col, "h": max(12, drawer_card_h // 5),
            "bg_color": "#242424",
            "radius": min(drawer_col // 8, drawer_card_h // 5),
        })
        b.add({
            "type": "label", "parent": card, "x": 0,
            "y": drawer_card_h * 2 // 3,
            "w": drawer_col, "h": drawer_card_h // 3,
            "text": f"{percent}%",
            "fg_color": "#080808", "text_align": "center",
        })
    b.add({
        "type": "slider", "parent": panel, "name": "drawer_level",
        "x": drawer_pad, "y": drawer_rows_y,
        "w": drawer_col, "h": drawer_row_h, "value": 80,
        "bg_color": "#F4F5F7", "fg_color": "#F05020",
        "track_size": max(4, drawer_row_h // 8),
    })
    toggle_card = b.add({
        "type": "container", "parent": panel,
        "x": drawer_pad + drawer_col + drawer_gap, "y": drawer_rows_y,
        "w": drawer_col, "h": drawer_row_h,
        "bg_color": "#F4F5F7", "radius": drawer_row_h // 2,
    })
    toggle_h = max(20, drawer_row_h * 2 // 3)
    b.add({
        "type": "toggle", "parent": toggle_card, "name": "drawer_toggle",
        "x": (drawer_col - 2 * toggle_h) // 2,
        "y": (drawer_row_h - toggle_h) // 2,
        "w": 2 * toggle_h, "h": toggle_h, "checked": True,
        "bg_color": "#404040", "fg_color": "#F05020",
    })
    for index, caption in enumerate(("Mode A", "Mode B")):
        b.add({
            "type": "button", "parent": panel,
            "x": drawer_pad + index * (drawer_col + drawer_gap),
            "y": drawer_rows_y + drawer_row_h + drawer_gap,
            "w": drawer_col, "h": drawer_row_h,
            "text": caption,
            "bg_color": "#F4F5F7", "fg_color": "#202020",
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
               "h": min(120, flow_h // 2), "image": "bench_rgb.png"})
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
               "w": image_w, "h": image_h, "image": "bench_rgb.png"})
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
    p = b.page("p_messages", tag=False)
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
           "text": "GSP Chat", "font_size": chrome_font,
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
    b.add({"type": "list", "parent": p, "x": m + col + gap, "y": y,
           "w": col, "h": height // 5,
           "items": ["alpha", "beta", "gamma", "delta", "epsilon",
                     "zeta"],
           "item_height": max(18, height // 14), "bind": "w_list",
           "font_size": small})
    y += height // 5 + gap
    b.add({"type": "wheel", "parent": p, "x": m, "y": y, "w": col,
           "h": height // 6,
           "items": ["one", "two", "three", "four", "five"],
           "item_height": max(18, height // 14), "cyclic": True,
           "bind": "w_whl", "font_size": small})
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
           "options": ["Throughput", "Latency", "Memory"],
           "font_size": max(9, height // 30)})
    b.add({"type": "table", "parent_name": "bench_tabs_tab0",
           "x": m, "y": max(24, height // 11) + 2 * gap,
           "w": inner, "columns": ["path", "load", "state"],
           "rows": [["render", "100%", "active"],
                    ["present", "100%", "active"],
                    ["input", "100%", "active"]],
           "font_size": max(9, height // 32)})
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
           "w": media_w, "h": media_h, "image": "bench_rgb.png"})
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

    return {
        "screen": "bench",
        "w": width, "h": height,
        "screen_bg": "#101820",
        "themes": {
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
        "objects": b.objects,
    }


def alt_scene(width, height):
    """Contrast screen for the full-screen slide-transition test."""
    objects = [
        {"type": "container", "parent": -1, "x": 0, "y": 0,
         "w": width, "h": height, "bg_color": "#3A1818"},
        {"type": "container", "parent": 0, "x": width // 8,
         "y": height // 8, "w": width - width // 4,
         "h": height - height // 4, "bg_color": "#602828",
         "radius": 18, "border_color": "#C06060", "border_width": 3},
        {"type": "container", "parent": 1, "x": 24,
         "y": 24, "w": 48, "h": 48, "bg_color": "#C06060",
         "radius": 24},
    ]
    return {
        "screen": "bench_alt",
        "w": width, "h": height,
        "screen_bg": "#3A1818",
        "objects": objects,
    }


def main():
    os.chdir(Path(__file__).resolve().parent)
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check", action="store_true",
        help="fail if checked-in generated scenes are stale")
    args = parser.parse_args()
    scale_asset = scale_asset_bytes()
    scale_asset_path = Path("bench_scale.png")
    if args.check:
        if (not scale_asset_path.exists() or
                scale_asset_path.read_bytes() != scale_asset):
            print("stale benchmark asset: bench_scale.png", file=sys.stderr)
            return 1
    else:
        scale_asset_path.write_bytes(scale_asset)
        print("wrote bench_scale.png")
    validate_assets()
    generated = {}
    for width, height in RESOLUTIONS:
        name = f"bench_{width}.json"
        generated[name] = json.dumps(scene(width, height), indent=2) + "\n"
        alt = f"bench_alt_{width}.json"
        generated[alt] = json.dumps(alt_scene(width, height), indent=2) + "\n"
    generated["bench_rgb888_1024.json"] = json.dumps(
        scene(1024, 600, rgb888=True), indent=2) + "\n"
    generated["bench_rgb888_800.json"] = json.dumps(
        scene(800, 480, rgb888=True), indent=2) + "\n"
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
