# Product scenarios

Product flows built with native GSP controls, compiled assets and shared
application controllers:

- [Coffee machine](coffee_machine/README.md): 800×480, S3/S31 RGB565 with touch
  or S31 RGB888; drink collection, a direct-select product catalog, recipes and preparation.
- [Smartwatch](smartwatch/README.md): 360×360, S3 QSPI touch; dial, activity,
  notifications, charging, direct route selection and a guided breathing session.
- [Industrial panel](industrial_panel/README.md): 1024×600, P4 MIPI-DSI touch;
  process telemetry, a draggable history inspector, draft setpoints and alerts.
- [Thermostat](thermostat/README.md): 240×240, C3 SPI with rotary input;
  temperature, climate mode, sleep timer and previewable heating presets.
- [Washing machine](washing_machine/README.md): 320×240, S3 SPI touch;
  draggable fabric programs, settings, washing stages and cycle details.

The interfaces use English and explicit glyph subsets. Larger numeric
readouts carry numeric glyphs rather than the entire body alphabet. Product
motion follows state: page transitions, a seconds marker, the active wash
load and user-started breathing guidance.
Idle appliance pages have no ambient pulses, sweeps or decorative loops. Each scenario keeps
its existing display size, pixel format and input method.

The examples use simulated appliance and sensor data. Replace the application
service layer when connecting real equipment. Device initialization covers
the display and its touch or rotary input; these scenarios do not initialize
audio, radios, motors or heaters. Each README provides the build,
input and PC preview instructions for its profiles.

See [usage](../README.md#usage) for focused API examples and
[benchmark](../performance/benchmark/README.md) for controlled measurements.
