# Temperature monitoring device

Bare-metal software for a temperature monitor with OK / Warning / Critical LEDs,
supporting two hardware revisions with different sensor resolutions. Runs on a
PC against mocked hardware.

| Directory | Content |
|---|---|
| `docs/` | `architecture` (layers), `runtime` (boot, 100 µs sampling path, LED state machine), `cpp_classes` (C++ class diagram); PNG |
| `c/` | C implementation, demo and tests, see [`c/README.md`](c/README.md) |
| `cpp/` | C++ OOP implementation, demo and tests, see [`cpp/README.md`](cpp/README.md) |

```
cd c && make test && make run
cd cpp && make test && make run
```

Requires gcc/g++ and make.

The original task description is in [docs/assessment.pdf](docs/assessment.pdf).

## Behaviour

| LED | Condition |
|---|---|
| Green | 5.0 °C ≤ T < 85.0 °C |
| Yellow | 85.0 °C ≤ T < 105.0 °C |
| Red | T ≥ 105.0 °C or T < 5.0 °C |
| Red + Yellow | configuration fault (blank/corrupt EEPROM, I2C error) |

Leaving a level requires 1.0 °C of hysteresis to prevent flicker.

| EEPROM address | Content |
|---|---|
| 0x0000 | hardware revision: 0 = Rev-A (1 °C/digit), 1 = Rev-B (0.1 °C/digit) |
| 0x0001–0x0010 | serial number, ASCII, NUL-padded |
