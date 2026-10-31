# UltrasonicTankSensor

Tank level sensor for boats: an ESP8266/ESP32 reads a **DS1603L ultrasonic level sensor** and publishes the fill level to a [Signal K](https://signalk.org) server, built on [SensESP](https://github.com/SignalK/SensESP).

The DS1603L (XKC-DS1603L.V1) measures the liquid level **from the outside** of the tank, through the tank wall (plastic or metal). Nothing touches the water or fuel, so the sensor also works for drinking water tanks.

## How it works

```
DS1603L ──TTL serial──▶ ESP (SoftwareSerial) ──▶ UltrasonicLevel sensor class
        ──▶ Linear (mm → %) ──▶ MovingAverage(10) ──▶ SKOutputNumber("tanks.<type>.<name>.currentLevel")
```

- `src/ultrasonic_level.{h,cpp}` – a SensESP `NumericSensor` that polls the DS1603L and emits the raw level in millimetres. `read_delay` is configurable in the SensESP web UI.
- `src/main.cpp` – wires the sensor into SensESP: Wi‑Fi, Signal K server, Signal K path, linear scaling to a percentage, moving average.
- `lib/DS1603L/` – vendored copy of [wvmarle/Arduino_DS1603L](https://github.com/wvmarle/Arduino_DS1603L) (MIT).

## Hardware

| Part | Notes |
|------|-------|
| Wemos D1 mini (ESP8266) | default PlatformIO env; `esp32dev` and `esp-wrover-kit` envs are included too |
| DS1603L ultrasonic level sensor, TTL version | 9600 baud serial output |
| 5 V supply | sensor is 5 V, ESP is 3.3 V |

Wiring (D1 mini): sensor TX → `D3`, sensor RX → `D4` (see `TX_PIN`/`RX_PIN` in `main.cpp`). Glue the sensor flat against the **bottom** of the tank, pointing upward.

## Build and flash

1. Install [PlatformIO](https://platformio.org/) (VS Code extension is recommended in `.vscode/extensions.json`).
2. Edit `src/main.cpp`:
   - `set_wifi("SSID", "Password")` – your Wi‑Fi
   - `set_sk_server(...)` – your Signal K server, or remove the call to use mDNS discovery
   - `sk_path` – e.g. `tanks.freshWater.port.currentLevel` or `tanks.fuel.daytank.currentLevel`
   - `empty_value` / `full_value` – tank depth in mm for the mm → percent conversion
3. `pio run -t upload` and `pio device monitor`.

Calibration follows the SensESP wiki: [Using AnalogInput to Calculate a Tank Level](https://github.com/SignalK/SensESP/wiki/Using-AnalogInput-to-Calculate-a-Tank-Level) (the same linear multiplier/offset math, applied to millimetres instead of ADC counts).

## Status

Written in 2021 against **SensESP 1.x** (`ReactESP app([](){...})`, `sensesp_app->enable()`). SensESP 2 and 3 changed the API, so the code needs porting before it builds against current SensESP releases. The sensor class itself is small and should be straightforward to adapt. Pull requests welcome.

## License

MIT, see [LICENSE](LICENSE). The bundled `Arduino_DS1603L` library is MIT, © 2018 wvmarle.
