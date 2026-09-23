# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: LicenseRef-Espressif-Modified-MIT
"""Command-count contract shared by simulator and hardware log validation."""

VECTOR_CASES = {
    "P_VECTOR_SIZE": ("vector resize", 4),
    "P_VECTOR_ROTATE": ("vector rotation", 2),
    "P_VECTOR_TINT": ("vector tint", 2),
    "P_VECTOR_MORPH": ("vector morph", 2),
    "P_VECTOR_MOVE": ("vector move", 2),
    "P_VECTOR_FIT": ("vector fit", 3),
    "P_VECTOR_STYLE": ("vector paints", 1),
    "P_VECTOR_EYES": ("vector eyes", None),
}


def expected_commands(case, updates):
    per_update = VECTOR_CASES[case][1]
    if per_update is not None:
        return updates * per_update
    cycles, remainder = divmod(updates, 32)
    # Four eyelid tweens per transition, four iris tweens per gaze target.
    return cycles * 36 + 4 * sum(phase < remainder for phase in (0, 4, 6, 18, 20, 0, 8, 16, 24))
