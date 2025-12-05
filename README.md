# pic16f877a-fire-smoke-sms-alert
Fire and smoke over-temperature SMS alert using PIC16F877A, LM35, MQ-2 and GSM module (CCS C)

# Fire and Smoke SMS Alert using PIC16F877A (CCS C)

This is a small embedded project using a **PIC16F877A** microcontroller, **CCS C** compiler and a **GSM module** to monitor temperature and gas/smoke level and send **SMS alerts** when dangerous conditions occur.  
The system reads temperature from an **LM35 sensor** using the ADC, checks for gas/smoke using an **MQ-2 gas sensor**, displays information on a **16x2 LCD**, drives a **buzzer**, and sends different SMS messages via the GSM module using AT commands. :contentReference[oaicite:1]{index=1}

## Features

- Measure ambient **temperature (°C)** using an **LM35** analog temperature sensor.
- Display live temperature on a **16x2 character LCD**.
- Detect gas/smoke using an **MQ-2** gas sensor (digital output).
- Combine **over-temperature** and **gas** conditions:
  - Normal: temperature below threshold and no gas.
  - Over-temperature only.
  - Gas only.
  - Both over-temperature and gas/smoke present.
- Use a **buzzer** for local audible alarm.
- Send **SMS notifications** to a predefined phone number:
  - System startup message.
  - Over-temperature + gas alert (includes temperature value).
  - Over-temperature only alert (includes temperature value).
  - Gas-only alert.
  - (Optional) temperature status SMS.
- Avoids sending duplicate SMS messages by tracking the last alarm state.

## Hardware

- Microcontroller: **PIC16F877A** (20 MHz external crystal)
- Temperature sensor: **LM35** (connected to ADC channel AN0)
- Gas/smoke sensor: **MQ-2** gas sensor module (digital output on RA1)
- 16x2 character LCD (4-bit mode) using `lcd.c`
- Buzzer (connected to `PIN_D2`)
- GSM module (e.g. SIM900/SIM800) using UART on `RC6` (TX) and `RC7` (RX)
- Power supply (5 V for PIC, suitable supply for GSM module)
- Miscellaneous: resistors, wires, breadboard or PCB

Typical pin usage in the example code:

- LM35 analog input: `AN0`
- MQ-2 digital output: `PIN_A1`
- Buzzer: `PIN_D2`
- UART to GSM module: `PIN_C6` (TX), `PIN_C7` (RX)
- LCD:
  - `LCD_ENABLE_PIN` → `PIN_B2`
  - `LCD_RS_PIN`     → `PIN_B4`
  - `LCD_RW_PIN`     → `PIN_B3`
  - `LCD_DATA4`      → `PIN_D7`
  - `LCD_DATA5`      → `PIN_D6`
  - `LCD_DATA6`      → `PIN_D5`
  - `LCD_DATA7`      → `PIN_D4`

## Software & Tools

- **Compiler**: CCS C for PIC (`#include <16F877A.h>`)  
- Clock: 20 MHz (`#use delay(clock = 20000000)`)
- LCD library: `lcd.c` (standard CCS LCD driver)
- UART configuration: `#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)` for GSM module
- IDE: CCS IDE or MPLAB X with CCS plug-in
- Programmer: PICkit or compatible programmer for PIC16F877A
- Optional: Proteus for simulation of LM35, MQ-2, GSM module and LCD

## Code Overview

Main source file: `fire_smoke_sms_alert.c`

Key points:

- **GSM initialization (`khoi_dong_sim`)**:
  - Shows a startup message on the LCD: “KHOI DONG SIM…”.
  - Sends a sequence of AT commands to the GSM module:
    - `AT`, `ATE1` (echo on), `AT+IPR=9600` (set baud rate),
    - `AT&W` (save settings),
    - `AT+CMGF=1` (text mode SMS),
    - `AT+CNMI=2,2,0,0,0` (new SMS indication),
    - `AT+CMGD=1,4` (delete all stored messages).
  - Sends an initial SMS (`gui_tin_nhan(1)`) such as “HE THONG BAT DAU HOAT DONG”.

- **SMS sending (`gui_tin_nhan`)**:
  - Uses `AT+CMGS="<phone_number>"` followed by the message text and then Ctrl+Z (ASCII 26).
  - Different message types are selected via the `tn` parameter:
    - `1`: system startup message.
    - `2`: over-temperature + gas alert with temperature value.
    - `3`: over-temperature only with temperature.
    - `4`: gas-only alert.
    - `5`: temperature status message (`NHIET DO:%ld`).

- **LM35 temperature reading (`doc_lm35`)**:
  - Reads ADC channel AN0 100 times and averages the result.
  - Converts the averaged ADC value to millivolts and then to °C using:
    - 5 V reference, 10-bit ADC, LM35 resolution of 10 mV/°C.
  - Stores the result in `nd` (temperature in °C).
  - Displays temperature on the LCD: `NHIET DO:xxC`.

- **Alarm logic and state flags**:
  - `tt`, `tt1`, `tt2` are used as state flags to avoid sending the same SMS repeatedly.
  - In the main loop:
    - If `nd > 40` and gas detected (`input(mq2) == 0`):
      - Turn ON buzzer.
      - Show `"QUA NHIET-KHI GA"` on LCD.
      - If this is a new state, send SMS type 2.
    - Else if `nd > 40` only:
      - Turn ON buzzer.
      - Show `"    QUA NHIET    "`.
      - If this is a new state, send SMS type 3.
    - Else if gas detected only:
      - Turn ON buzzer.
      - Show `"    KHI GAS    "`.
      - If this is a new state, send SMS type 4.
    - Else (normal):
      - Turn OFF buzzer.
      - Show `"  BINH THUONG   "`.
      - Reset state flags so that a new alarm will generate a new SMS.

## How It Works (High-Level)

1. **Initialize:**
   - Configure TRIS registers:
     - Port A as inputs for LM35 and MQ-2.
     - Port D and B as outputs for LCD and buzzer.
     - Port C with TX/RX pins for UART.
   - Initialize the LCD and display project information (fire/gas warning and author).
   - Call `khoi_dong_sim()` to initialize the GSM module and send the startup SMS.
   - Configure the ADC for LM35:
     - `SETUP_ADC(ADC_CLOCK_DIV_32);`
     - `SETUP_ADC_PORTS(sAN0);`
     - `SET_ADC_CHANNEL(0);`
   - Initialize alarm state flags (`tt`, `tt1`, `tt2`) and turn OFF the buzzer.

2. **In the main loop:**
   - Call `doc_lm35()` to read and display the current temperature.
   - Check gas/smoke via `input(mq2)`:
     - `0` = gas/smoke detected (depending on module).
   - Evaluate conditions:
     - Over-temperature (e.g. `nd > 40`).
     - Gas only.
     - Both.
     - Normal.
   - According to the current condition:
     - Update LCD status line.
     - Control buzzer (ON for any alarm, OFF for normal).
     - Use `tt`, `tt1`, `tt2` flags to decide whether to send the corresponding SMS alert (so that only one SMS is sent when the state changes).
   - Loop continuously to provide real-time monitoring and SMS alert functionality.
