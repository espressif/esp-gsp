# Simulator Preview and Testing

The standalone simulator host provides three ways to verify UI behavior
before flashing firmware: interactive browser preview, scripted CLI testing,
and API-driven automation. Each approach proves portable scene behavior but
cannot substitute for panel wiring, touch mapping, or visual acceptance on
target hardware.

For the complete command-line, API, and backend reference see the
[Simulator reference](../reference/simulator.md).

## Obtaining the simulator

Install the simulator through the `esp-gsp-tools` toolchain manager:

```sh
python -m pip install -U esp-gsp-tools
python -m gsp.execute --version '<ESP-GSP version>' sim --capabilities
```

`<ESP-GSP version>` is the `version` field from `idf_component.yml`.
Alternatively, set `GSP_SIM_EXECUTABLE` to point to an existing executable.
See [Simulator reference — Obtaining the simulator](../reference/simulator.md#obtaining-the-simulator)
for details.

Commands below use `gsp_sim_host` for the simulator executable. When
invoking through the manager, substitute
`python -m gsp.execute --version '<ESP-GSP version>' sim`.

## Interactive browser preview

Start the simulator with no frame limit to keep the preview running:

```sh
gsp_sim_host --bundle app.gspb --frames 0
```

The host prints the preview URL to stderr and opens a browser page with:

- a live canvas rendering the simulated display;
- mouse click and drag mapped to pointer input;
- 25 %–400 % zoom;
- scene navigation buttons (◀ ▶ ↺);
- a tabbed log panel showing callback events and runtime output.

Use this mode during authoring to see layout changes, animation, and
navigation in real time without a target build. Close the browser or press
Ctrl-C to stop the simulator.

## Headless testing and screenshots

For CI or automated checks, run without a browser and capture the final
frame:

```sh
gsp_sim_host --bundle app.gspb --headless \
    --frames 3 --dump final.png --dump-format png
```

Add scripted input to verify interactive paths:

```sh
gsp_sim_host --bundle app.gspb --headless \
    --tap 160 120 --wait 5 --tap 80 200 \
    --dump result.png --dump-format png
```

Scripted `--tap`, `--drag`, and `--wait` actions execute in order. For
complex sequences, prefer the API channel.

## API-driven automation

Enable the JSON-RPC control channel for programmatic access:

```sh
gsp_sim_host --bundle app.gspb --frames 0 --api-enable
```

Then drive the simulator from any JSON-RPC client. A typical verification
flow:

1. `capabilities` — confirm display size and scene count.
2. `tap` / `drag` — inject pointer input.
3. `wait` — let the animation settle.
4. `screenshot` — capture the result.
5. `quit` — shut down.

The channel supports stdio (default), loopback TCP, and Unix sockets. Set
`--input-mode api-exclusive` when the automation must be the sole input
source and the browser preview is open simultaneously.

## Full-stack simulation with a backend

To test application logic alongside the UI, connect an application backend:

```sh
gsp_sim_host --bundle app.gspb --frames 0 \
    --backend-listen tcp://127.0.0.1:8684 \
    --backend-required --api-enable
```

The backend receives `callback` notifications when the user interacts with
the UI and drives the display through `set_text`, `set_value`, `goto_scene`,
and other state methods. While a backend is attached, scene navigation is
backend-exclusive: browser scene buttons and API `goto_scene` calls are
rejected.

This mode is suitable for verifying data binding, event handling, and
multi-scene flow without target hardware.

## Choosing the right mode

| Goal | Recommended approach |
|---|---|
| Visual authoring and layout iteration | Interactive browser preview |
| Quick render check in CI | Headless with `--dump` |
| Regression test for interactions | Headless with `--tap` / `--drag` scripting |
| Complex automation or AI agent testing | API channel (`--api-enable`) |
| End-to-end application logic | Backend + API channel |

## Evidence boundary

A simulator run proves that the compiled scene renders and responds to input
correctly in the portable runtime. It does **not** prove:

- panel byte order, rotation, or tearing behavior;
- touch controller coordinate mapping;
- PSRAM timing or DMA throughput;
- final display color accuracy or font rendering fidelity.

Report simulator evidence and hardware evidence as separate layers.
