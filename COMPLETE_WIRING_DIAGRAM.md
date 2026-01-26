# Complete Kaleidoscope Wiring Diagram

## 🔌 ALL CONNECTIONS - ELEGOO MEGA 2560

```
                              ELEGOO MEGA 2560
    ┌──────────────────────────────────────────────────────────────┐
    │                                                              │
    │  POWER SECTION                    DIGITAL I/O PINS          │
    │  ┌──────────┐                                               │
    │  │ 5V   GND │                    [2] - Mode Button (optional)│
    │  └─┬────┬───┘                    [3] - Encoder 1 CLK  ←━━━━━┫ ENCODER 1
    │    │    │                         [4] - LED Data Out  ←━━━━━┫ LEDs
    │    │    │                         [5] - Encoder 1 DT   ←━━━━┫ ENCODER 1
    │    │    │                         [6] - (unused)            │
    │    │    │                         [7] - Encoder 1 SW   ←━━━━┫ ENCODER 1
    │    │    │                         ...                       │
    │    │    │                         [17] - Encoder 2 SW  ←━━━━┫ ENCODER 2
    │    │    │                         [18] - Encoder 2 CLK ←━━━━┫ ENCODER 2
    │    │    │                         [19] - Encoder 2 DT  ←━━━━┫ ENCODER 2
    │    │    │                         [20] - I2C SDA       ←━━━━┫ MPU-6050
    │    │    │                         [21] - I2C SCL       ←━━━━┫ MPU-6050
    │    │    │                                                   │
    │    │    └──────────────────────────────────────────────────┤
    │    └────────────────────────────────────────────────────────┤
    │                                                              │
    │  USB Port                                      Reset Button │
    └──────────────────────────────────────────────────────────────┘
```

## 📍 COMPONENT 1: KY-040 ROTARY ENCODER #1 (Mode Control)

```
         KY-040 ENCODER #1
         ┌─────────────┐
         │ ○ ○ ○ ○ ○   │
         │CLK DT SW + -│
         └─┬─┬─┬──┬──┬─┘
           │ │ │  │  │
           │ │ │  │  └────────── GND (Black wire)
           │ │ │  └───────────── +5V (Red wire)
           │ │ └──────────────── Pin 7 (Yellow wire) - Button
           │ └────────────────── Pin 5 (Green wire)  - Direction
           └──────────────────── Pin 3 (White wire)  - Clock/Interrupt

    Function: Rotate to change animation modes
              Press button to reset to Kaleidoscope mode
```

## 📍 COMPONENT 2: KY-040 ROTARY ENCODER #2 (Effect Control)

```
         KY-040 ENCODER #2
         ┌─────────────┐
         │ ○ ○ ○ ○ ○   │
         │CLK DT SW + -│
         └─┬─┬─┬──┬──┬─┘
           │ │ │  │  │
           │ │ │  │  └────────── GND (Black wire)
           │ │ │  │
           │ │ │  └───────────── +5V (Red wire)
           │ │ │
           │ │ └──────────────── Pin 17 (Yellow wire) - Button
           │ │
           │ └────────────────── Pin 19 (Green wire)  - Direction
           │
           └──────────────────── Pin 18 (White wire)  - Clock/Interrupt

    Function: Rotate to control virtual rotation/effect intensity
              Press button for effect toggle (future feature)
```

## 📍 COMPONENT 3: MPU-6050 MOTION SENSOR

```
         MPU-6050 GY-521
         ┌─────────────┐
         │ ● ● ● ● ● ● │
         │VCC GND SCL SDA XDA XCL│
         └─┬───┬──┬───┬──┬────┬──┘
           │   │  │   │  │    │
           │   │  │   │  └────┴── (Not connected)
           │   │  │   │
           │   │  │   └────────── Pin 20 (Blue wire)   - I2C Data
           │   │  │
           │   │  └────────────── Pin 21 (Yellow wire) - I2C Clock
           │   │
           │   └───────────────── GND (Black wire)
           │
           └───────────────────── +5V (Red wire)

    Function: Detects tilt and pan motion for animation control
```

## 📍 COMPONENT 4: WS2818 LED STRIP (209 LEDs)

```
         LED STRIP CONNECTOR
         ┌──────────────┐
         │ ● ● ●        │
         │5V GND DIN    │
         └─┬──┬──┬──────┘
           │  │  │
           │  │  └────────────── Pin 4 (Green wire) - Data
           │  │
           │  └───────────────── GND to Power Supply AND Arduino
           │
           └──────────────────── +5V from External Power Supply

    ⚠️ IMPORTANT: External 5V power supply required (10A minimum)
```

## 📍 COMPONENT 5: EXTERNAL POWER SUPPLY

```
    5V POWER SUPPLY (10A+)
    ┌────────────────┐
    │   AC IN        │
    │  ┌──┬──┐      │
    │  │L │N │      │────── Wall Outlet (120V/240V)
    │  └──┴──┘      │
    │                │
    │   DC OUT       │
    │  ┌──┬──┐      │
    │  │+ │- │      │
    │  └┬─┴─┬┘      │
    └───┼───┼───────┘
        │   │
        │   └──────────────── GND (Connect to Arduino GND + LED GND)
        │
        └──────────────────── +5V (To LED strip ONLY)

    ⚡ CRITICAL: Connect all grounds together!
```

## 🔗 COMPLETE WIRING TABLE

| Component | Pin/Wire | Arduino Pin | Wire Color | Notes |
|-----------|----------|-------------|------------|-------|
| **ENCODER 1** |||||
| | CLK | Pin 3 | White | Hardware interrupt INT1 |
| | DT | Pin 5 | Green | Direction detection |
| | SW | Pin 7 | Yellow | Push button |
| | + (VCC) | 5V | Red | Power |
| | GND | GND | Black | Ground |
| **ENCODER 2** |||||
| | CLK | Pin 18 | White | Hardware interrupt INT5 |
| | DT | Pin 19 | Green | Direction detection |
| | SW | Pin 17 | Yellow | Push button |
| | + (VCC) | 5V | Red | Power |
| | GND | GND | Black | Ground |
| **MPU-6050** |||||
| | VCC | 5V | Red | Power |
| | GND | GND | Black | Ground |
| | SCL | Pin 21 | Yellow | I2C Clock |
| | SDA | Pin 20 | Blue | I2C Data |
| | XDA | - | - | Not used |
| | XCL | - | - | Not used |
| **LED STRIP** |||||
| | DIN | Pin 4 | Green | Data signal |
| | 5V | PSU 5V | Red | From power supply |
| | GND | PSU GND + Arduino GND | Black | Common ground |
| **POWER SUPPLY** |||||
| | +5V OUT | LED 5V | Red | High current |
| | GND OUT | LED GND + Arduino GND | Black | Common ground |
| **BUTTON** (Optional) |||||
| | One side | Pin 2 | Any | Mode button |
| | Other side | GND | Black | Ground |

## ⚡ POWER DISTRIBUTION DIAGRAM

```
    WALL OUTLET (120V/240V AC)
            │
            ▼
    ┌───────────────┐
    │ 5V PSU (10A+) │
    └───┬───────┬───┘
        │       │
       +5V     GND
        │       │
        │       ├────────────────┐
        │       │                │
        │       ▼                ▼
        │   ARDUINO GND      LED GND
        │                        │
        └───────────────────────►│
                              LED +5V

    Arduino 5V ──► Encoders + MPU-6050 (low current)
    PSU 5V ────► LED Strip only (high current)

    ⚠️ ALL GROUNDS MUST BE CONNECTED TOGETHER!
```

## 🛠️ WIRING TIPS

### Power Connections:
1. **Arduino 5V**: Powers encoders and MPU-6050 (low current devices)
2. **External PSU**: Powers LED strip ONLY (high current)
3. **Common Ground**: Connect Arduino GND to PSU GND - CRITICAL!

### Wire Management:
```
    Use different colored wires for easy identification:
    • RED = Power (+5V)
    • BLACK = Ground (GND)
    • GREEN = Data signals
    • YELLOW = Button/switch signals
    • WHITE = Clock/interrupt signals
    • BLUE = I2C signals
```

### Connector Options:
- **Dupont cables**: For prototyping
- **JST connectors**: For permanent installation
- **Terminal blocks**: For power connections
- **Breadboard**: For testing only

## ✅ WIRING CHECKLIST

Before powering on, verify:

□ All 5 pins connected on Encoder 1
□ All 5 pins connected on Encoder 2
□ MPU-6050 has 4 connections (VCC, GND, SCL, SDA)
□ LED strip data wire to Pin 4
□ Power supply connected to LED strip
□ Arduino GND connected to Power Supply GND
□ No short circuits (check with multimeter)
□ Power supply rated for at least 10A
□ Correct voltage (5V) from power supply

## 🔧 TESTING SEQUENCE

1. **Power Off Everything**
2. **Connect Arduino USB** (powers Arduino only)
3. **Upload test sketch** (encoder_test.ino)
4. **Test encoders and MPU-6050** via Serial Monitor
5. **Then connect external power** for LEDs
6. **Upload main sketch**

## ⚠️ COMMON MISTAKES TO AVOID

❌ Forgetting to connect Arduino GND to PSU GND (LEDs won't work!)
❌ Powering LEDs from Arduino 5V pin (will damage Arduino!)
❌ Wrong encoder pins (must use interrupt-capable pins)
❌ Swapping I2C pins (SDA/SCL reversed)
❌ Not using external power for LEDs
❌ Loose connections (use solid connections)

## 📏 PHYSICAL LAYOUT SUGGESTION

```
    TOP VIEW OF PROJECT BOX:

    ┌─────────────────────────────────────┐
    │                                     │
    │  [ENCODER 1]          [ENCODER 2]  │
    │     Mode                Effect     │
    │                                     │
    │         ┌─────────────┐            │
    │         │ ARDUINO MEGA│            │
    │         │             │            │
    │         └─────────────┘            │
    │                                     │
    │   MPU ┌───┐      ┌──────────┐     │
    │       └───┘      │   PSU    │     │
    │                  └──────────┘     │
    │                                     │
    │  LED Output ═══════════════════►   │
    │                                     │
    └─────────────────────────────────────┘
```

---

## Need Help?

1. Check all connections with multimeter
2. Verify pin numbers match code
3. Test components individually first
4. Check Serial Monitor for debug messages
5. Ensure adequate power supply

This document contains EVERY wire connection needed for the complete kaleidoscope project!