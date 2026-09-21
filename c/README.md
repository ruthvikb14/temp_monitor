# Temperature monitor – C implementation

Bare-metal firmware structure, built and tested on a PC against a mocked HAL.

```
make test   # 62 unit/integration checks
make run    # demo: Rev-A, Rev-B and a corrupted-EEPROM case
```

## Layers

| Layer | Files | Responsibility |
|---|---|---|
| Application | `src/app/app.c` | boot sequence, main-loop step, fault handling |
| | `src/app/temp_monitor.c` | thresholds + hysteresis (pure logic, no HW) |
| Drivers | `src/drivers/temp_acquisition.c` | 100 µs sampling, ISR -> main hand-off |
| | `src/drivers/temp_sensor.c` | Rev-A / Rev-B raw -> deci-°C (strategy table) |
| | `src/drivers/eeprom_config.c` | read + validate revision and serial |
| | `src/drivers/led_indicator.c` | level -> R/Y/G GPIOs |
| Interrupts | `src/isr.c` | `ADC_IRQHandler`, `TIM_IRQHandler` |
| HAL interface | `include/hal/*.h` | timer, ADC, GPIO, I2C, IRQ lock |
| Port | `port/pc/` | mock HAL, demo, used by tests |
| | `src/main.c` | real target entry point (not in PC build) |

Upper layers never touch registers. Porting to an MCU means implementing `include/hal/*.h` in `port/<mcu>/`; nothing above changes.

## Key decisions

**Low-jitter 100 µs sampling.** A hardware timer's trigger output (TRGO) starts the ADC directly, so the sampling instant does not depend on the CPU, interrupt latency or main-loop load. The ADC end-of-conversion ISR only stores the value and increments a sequence counter. `BOARD_ADC_HW_TRIGGER = 0` selects a software fallback where the timer ISR starts the conversion.

**ISR ↔ main-loop hand-off.** `temp_acq_get_latest()` reads value and sequence number under a short critical section so they can't tear. The main loop uses the newest sample and counts overwritten ones (`samples_missed`) for diagnostics; LEDs cannot usefully change faster than that anyway. If every sample had to be evaluated (e.g. filtering), this module would switch to a ring buffer without changing its callers.

**Two hardware revisions.** All temperatures are integers in tenths of a degree (`temp_dC_t`, 85.0 °C = 850), so no floating point. The revision is read from EEPROM once at boot and selects a `temp_sensor_t` (struct with a function pointer). After that nothing checks the revision again; a Rev-C is one function plus one table entry.

**Thresholds.** Green 5.0 ≤ T < 85.0, yellow 85.0 ≤ T < 105.0, red T ≥ 105.0 or T < 5.0. Escalation is immediate; de-escalation needs 1.0 °C of hysteresis to avoid LED flicker at a threshold (`APP_HYSTERESIS_DC`, 0 disables it).

**Fail-safe.** Blank/corrupt EEPROM, unknown revision or I²C failure → sampling is not started, red + yellow LEDs on (distinguishable from a real over-temperature).

## EEPROM layout

| Address | Size | Content |
|---|---|---|
| 0x0000 | 1 | hardware revision: 0 = Rev-A, 1 = Rev-B |
| 0x0001 | 16 | serial number, ASCII, NUL-padded (e.g. `ABC1234`) |
