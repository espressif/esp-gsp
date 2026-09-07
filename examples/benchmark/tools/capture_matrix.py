#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT

"""Reset and concurrently capture a matrix of benchmark serial ports."""

from __future__ import annotations

import argparse
import selectors
import time
from pathlib import Path

import serial


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--device", action="append", nargs=3, required=True,
        metavar=("LABEL", "PORT", "LOG"))
    parser.add_argument("--seconds", type=float, default=660.0)
    parser.add_argument("--baud", type=int, default=115200)
    args = parser.parse_args()
    if args.seconds <= 0 or args.baud <= 0:
        parser.error("seconds and baud must be positive")
    paths = [Path(device[2]).resolve() for device in args.device]
    ports = [device[1] for device in args.device]
    if len(set(paths)) != len(paths) or len(set(ports)) != len(ports):
        parser.error("each port and log path must be unique")

    selector = selectors.DefaultSelector()
    streams = []
    try:
        for label, port, log_path in args.device:
            uart = serial.Serial(port, args.baud, timeout=0)
            try:
                output = Path(log_path).open("xb")
            except Exception:
                uart.close()
                raise
            streams.append((label, uart, output))
            selector.register(uart.fileno(), selectors.EVENT_READ,
                              (label, uart, output))
        # All ports are already owned before any reset, so no board can emit
        # an unobserved prefix while another serial device is being opened.
        for _, uart, _ in streams:
            uart.reset_input_buffer()
            uart.dtr = False
            uart.rts = True
        time.sleep(0.12)
        for _, uart, _ in streams:
            uart.rts = False
        deadline = time.monotonic() + args.seconds
        while time.monotonic() < deadline:
            for key, _ in selector.select(
                    min(0.25, max(0.0, deadline - time.monotonic()))):
                label, uart, output = key.data
                data = uart.read(uart.in_waiting or 1)
                if data:
                    output.write(data)
                    output.flush()
        for label, _, output in streams:
            print(f"{label}: {output.tell()} bytes")
    finally:
        for _, uart, output in streams:
            try:
                selector.unregister(uart.fileno())
            except Exception:
                pass
            output.close()
            uart.close()
    from compare_logs import parse_log
    valid = True
    for label, _, log_path in args.device:
        try:
            result = parse_log(Path(log_path))
            print(f"{label}: VALID measured summary, {len(result.pages)} cases")
        except ValueError as error:
            print(f"{label}: INVALID: {error}")
            valid = False
    return 0 if valid else 1


if __name__ == "__main__":
    raise SystemExit(main())
