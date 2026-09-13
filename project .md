# Smart Greenhouse Controller — Project Plan

## 1. Project Overview

The project is a Smart Greenhouse Controller based on the ATmega32A at 8 MHz.
The system monitors temperature, soil moisture, and light level, then controls the fan, water pump, and grow lamp using software hysteresis.

The system also provides an LCD interface, push buttons, an alarm, and UART communication for status reporting and commands.

> **Important update:** SPI has been removed from the project according to the latest project decision. Therefore, all SPI-dependent modules and hardware are removed from the implementation plan.

---

## 2. Hardware Components

### Main Controller

- **ATmega32A @ 8 MHz**
  - Main microcontroller that runs the complete greenhouse control system.

### Sensors / Inputs

- **3 Potentiometers in SimulIDE**
  - **Temperature:** connected to `PA0 / ADC0`, scaled to 0–50 °C.
  - **Soil Moisture:** connected to `PA1 / ADC1`, scaled to 0–100%.
  - **Light:** connected to `PA2 / ADC2`, scaled to 0–100%.

### Actuators / Outputs

- **Cooling Fan** → `PB0`
- **Water Pump** → `PB1`
- **Grow Lamp** → `PB2`
- **Alarm LED** → `PB3`
- **Buzzer** → `PD7`

The exact actuator driver stage (relay/transistor/H-bridge) depends on the final SimulIDE circuit implementation; the project specification defines the MCU control outputs but does not mandate a specific driver IC.

### Display

- **16×2 Character LCD**
- **PCF8574 I2C I/O Expander**
  - Allows the LCD to communicate with the ATmega32A through I2C using fewer MCU pins.

### Buttons

- **Alarm Reset Button** → `PD2 / INT0`
- **Mode Button** → `PD3 / INT1`
- **Save Button** → `PD4`

### Communication

- **UART / USART** using:
  - `PD0 / RXD`
  - `PD1 / TXD`
  - **9600 8N1**

UART is used for telemetry and text commands.

---

## 3. Drivers & Software Modules

### HAL Drivers

- **Actuators Driver** (`actuators.c / actuators.h`)
  - Controls the Fan, Pump, Lamp, and Alarm outputs through a hardware-independent interface.

- **Buttons Driver** (`buttons.c / buttons.h`)
  - Handles Mode, Save, and Alarm Reset buttons with software debouncing.

- **Sensors Driver** (`sensors.c / sensors.h`)
  - Reads the three analog inputs through the ADC and converts them into temperature, soil moisture, and light values.

- **LCD I2C Driver** (`lcd_i2c.c / lcd_i2c.h`)
  - Controls the 16×2 LCD through the PCF8574 I2C interface.

### MCAL Drivers

- **DIO Driver** (`dio.c / dio.h`)
  - Configures GPIO pins and handles digital input/output operations.

- **ADC Driver** (`adc.c / adc.h`)
  - Reads ADC0, ADC1, and ADC2 from the three analog inputs.

- **Timer Driver** (`timer.c / timer.h`)
  - Configures Timer0 in CTC mode to generate the 10 ms system tick.
  - Timer2 may be used for the buzzer PWM as a bonus feature.

- **EXTI Driver** (`exti.c / exti.h`)
  - Handles INT0 and INT1 external interrupts.

- **USART Driver** (`usart.c / usart.h`)
  - Handles UART communication at 9600 8N1.

- **I2C / TWI Driver** (`i2c.c / i2c.h`)
  - Provides I2C communication with the PCF8574 LCD interface.

> **Removed:** SPI Driver, SPI EEPROM Driver, and any other SPI-dependent module/hardware.

---

## 4. Application Modules

- **Scheduler** (`scheduler.c / scheduler.h`)
  - Runs system tasks according to their configured periods using the 10 ms system tick.

- **Greenhouse FSM** (`greenhouse_fsm.c / greenhouse_fsm.h`)
  - Manages the system states and transitions between AUTO, MANUAL, ALARM, CONFIG, and initialization states.

- **Control** (`control.c / control.h`)
  - Implements hysteresis control for the Fan, Pump, and Lamp.

- **Report** (`report.c / report.h`)
  - Sends greenhouse status through UART every 5 seconds.

- **Console** (`console.c / console.h`)
  - Receives and parses UART commands.

---

## 5. Control Logic

| Loop | Sensor | Actuator | ON Condition | OFF Condition |
|---|---|---|---|---|
| Thermal | Temperature | Fan | `> 35 °C` | `< 32 °C` |
| Irrigation | Soil Moisture | Pump | `< 40 %` | `> 60 %` |
| Photoperiod | Light | Lamp | `< 25 %` | `> 40 %` |

The gap between the ON and OFF thresholds is the **hysteresis band**, which prevents rapid ON/OFF switching around a single threshold.

Alarm conditions:

- Temperature `> 45 °C`
- Soil moisture `< 15 %`

The alarm remains latched until the reset action is performed.

---

## 6. Driver Dependency Flow

```text
APP
 ├── scheduler
 ├── greenhouse_fsm
 ├── control
 ├── report
 └── console
        │
        ▼
HAL
 ├── Sensors ────────▶ ADC
 ├── Actuators ──────▶ DIO
 ├── Buttons ────────▶ DIO / EXTI
 └── LCD_I2C ────────▶ I2C
        │
        ▼
MCAL
 ├── DIO
 ├── ADC
 ├── Timer
 ├── EXTI
 ├── USART
 └── I2C
```

---

## 7. Task Timing

### Deadline

**Start:** Sunday, September 14, 2026

**Final deadline:** Thursday night, September 17, 2026

The work is divided into four days, with integration and testing completed before the final deadline.

| Day | Main Tasks | Estimated Work |
|---|---|---:|
| **Day 1 — Sun, Sep 14** | Finalize hardware connections, verify ATmega32A pin mapping, finish/check DIO, ADC, I2C, USART and Timer drivers | **6–8 hours** |
| **Day 2 — Mon, Sep 15** | Finish HAL drivers: Sensors, Actuators, Buttons, LCD; verify each driver independently in SimulIDE | **6–8 hours** |
| **Day 3 — Tue, Sep 16** | Implement/integrate Scheduler, FSM, Control, Console and Report; connect all modules together | **7–9 hours** |
| **Day 4 — Wed, Sep 17** | Full system integration, test all scenarios, fix bugs, verify hysteresis/alarm/UART/LCD behavior, prepare final demo | **7–9 hours** |

### Recommended Daily Order

```text
Hardware / MCAL
      ↓
HAL Drivers
      ↓
Scheduler + FSM
      ↓
Control Logic
      ↓
UART + LCD
      ↓
Integration
      ↓
Testing + Demo
```

### Priority

1. **Must finish:** DIO, ADC, Timer, Sensors, Actuators, Scheduler, Control.
2. **Then:** LCD, Buttons, EXTI, FSM.
3. **Then:** USART, Console, Report.
4. **Bonus:** Timer2 buzzer PWM and extra refinements.

---

## 8. Important Architecture Rule

Only the **MCAL layer** accesses ATmega32A hardware registers directly.

The HAL layer uses MCAL APIs, while the Application layer uses HAL and service APIs instead of accessing hardware registers directly.

---

## 9. Final Demo Checklist

- [ ] Temperature sensor changes correctly.
- [ ] Soil moisture sensor changes correctly.
- [ ] Light sensor changes correctly.
- [ ] Fan responds with hysteresis.
- [ ] Pump responds with hysteresis.
- [ ] Lamp responds with hysteresis.
- [ ] Alarm activates at the specified limits.
- [ ] Alarm reset works.
- [ ] Mode button works.
- [ ] LCD displays live values and system state.
- [ ] UART sends status every 5 seconds.
- [ ] UART commands are received correctly.
- [ ] No SPI-related code or hardware remains.


**Prepared by: Karim Ehab Gamal | Ahmed Mohsen El-Shabrawy**