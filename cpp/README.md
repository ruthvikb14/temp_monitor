# Temperature monitor – C++ implementation (OOP)

Same behaviour as the C version, restructured around classes, interfaces
and dependency injection. Class diagram: `../docs/cpp_classes.png`.

```
make test   # 70 unit/integration checks
make run    # demo: Rev-A, Rev-B and a corrupted-EEPROM case
```

## How C concepts map to C++

| C version | C++ version | OOP principle |
|---|---|---|
| `temp_sensor_t` struct of function pointers | `ITemperatureSensor` + `RevASensor` / `RevBSensor` | polymorphism, open/closed |
| lookup table by revision | `SensorFactory::forRevision()` | factory |
| `hal_*.h` free functions, linked per port | `hal::IAdc`, `ISampleTimer`, `IDigitalOut`, `II2cBus`, `IInterruptControl` | dependency inversion |
| mock selected by linker | mock classes injected through `HalBundle` | dependency injection |
| module-static state | state encapsulated in objects (`TempAcquisition`, `TempMonitor`, …) | encapsulation |
| `irq_save` / `irq_restore` pairs | `hal::CriticalSection` | RAII |
| `int32_t` deci-°C | `Temperature` value type | strong typing |

`Application` owns its drivers by composition and receives all hardware as interface references, so the same class runs on target and on the PC.

## Embedded C++ rules applied

- No heap: every object is static or on the stack; sensors are static instances returned by the factory.
- `-fno-exceptions -fno-rtti`; errors are returned as `enum class` status values.
- Interfaces have protected, non-virtual destructors, since objects are never deleted through a base pointer. That avoids pulling virtual destructors and `operator delete` into the binary.
- `constexpr` thresholds and `Temperature` arithmetic, checked with `static_assert`.
- ISRs stay `extern "C"` for the vector table and forward to a handler object (`IAdcEventHandler`) bound before the interrupt is enabled. `Application`'s destructor unbinds it, so an ISR can never call a destroyed object.
- The only runtime cost over C is one virtual call per sample in the ISR and one in the main loop, a few cycles at 10 kHz.

## Structure

```
include/common/Types.hpp        HwRevision, TempLevel, Temperature
include/hal/                    HAL interfaces + CriticalSection
include/drivers/                ConfigReader, TemperatureSensors, LedIndicator, TempAcquisition, IrqEvents
include/app/                    TempMonitor, Application
src/Isr.cpp                     ADC_IRQHandler / TIM_IRQHandler -> bound objects
src/main_target.cpp             target entry point (illustrative, not in PC build)
port/pc/                        MockHal.hpp, demo_main.cpp
tests/                          test_main.cpp
```
