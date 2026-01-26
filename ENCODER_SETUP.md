# KY-040 Rotary Encoder Setup Guide

## Hardware Overview

The KY-040 rotary encoder modules provide:
- **Infinite rotation** in both directions
- **20 detents per rotation** (tactile feedback)
- **Integrated push button** (press down on knob)
- **Pull-up resistors** built into module

## Wiring Connections to ELEGOO MEGA

### Encoder 1 - Animation Mode Control
```
KY-040 #1        Arduino Mega        Function
---------        ------------        --------
CLK       →      Pin 3               Interrupt for rotation
DT        →      Pin 5               Direction detection
SW        →      Pin 7               Push button (mode reset)
VCC (+)   →      5V                  Power
GND       →      GND                 Ground
```

### Encoder 2 - Effect Control (Virtual Rotation/Intensity)
```
KY-040 #2        Arduino Mega        Function
---------        ------------        --------
CLK       →      Pin 18              Interrupt for rotation (INT5)
DT        →      Pin 19              Direction detection
SW        →      Pin 17              Push button (effect toggle)
VCC (+)   →      5V                  Power
GND       →      GND                 Ground
```

### Existing Connections (Reference)
```
Component        Arduino Mega        Purpose
---------        ------------        -------
MPU-6050 SDA  →  Pin 20             I2C Data
MPU-6050 SCL  →  Pin 21             I2C Clock
LED Data      →  Pin 4              WS2818 Control
Mode Button   →  Pin 2              Manual mode switch (optional)
```

## Pin Selection Rationale

- **Pin 3**: Hardware interrupt INT1 - for smooth encoder reading
- **Pin 18**: Hardware interrupt INT5 - for second encoder
- **Pins 5, 7, 17, 19**: Digital I/O for direction and button detection
- Avoided pins 20-21 (I2C in use) and pin 2 (existing button)

## Wiring Diagram

```
        ELEGOO MEGA 2560
        ┌─────────────────────────┐
        │                         │
        │  [3] [5] [7]   ...     │  ← Encoder 1
        │                         │
        │  ...  [17][18][19]     │  ← Encoder 2
        │                         │
        │  [20][21]              │  ← MPU-6050 (I2C)
        │                         │
        │  [4]                   │  ← LED Data
        │                         │
        │  [5V] [GND]            │  ← Power
        └─────────────────────────┘

   KY-040 Encoder #1              KY-040 Encoder #2
   ┌──────────┐                   ┌──────────┐
   │ CLK → 3  │                   │ CLK → 18 │
   │ DT  → 5  │                   │ DT  → 19 │
   │ SW  → 7  │                   │ SW  → 17 │
   │ VCC → 5V │                   │ VCC → 5V │
   │ GND → GND│                   │ GND → GND│
   └──────────┘                   └──────────┘
```

## Control Functions

### Encoder 1 - Mode Control
- **Rotate CW**: Next animation mode
- **Rotate CCW**: Previous animation mode
- **Press Button**: Return to default/favorite mode
- **Hold + Rotate**: Adjust mode speed multiplier

### Encoder 2 - Effect Control
- **Rotate CW**: Increase effect intensity/virtual rotation speed
- **Rotate CCW**: Decrease effect intensity/virtual rotation speed
- **Press Button**: Toggle effect on/off
- **Hold + Rotate**: Fine adjustment mode

## Software Implementation

### Key Features
1. **Interrupt-driven reading** for smooth response
2. **Debouncing** to prevent false triggers
3. **Acceleration** - faster rotation = bigger changes
4. **Visual feedback** - brief flash on mode change
5. **Serial output** for debugging

### Configuration Options
```cpp
// In Config.h
#define ENCODER_1_CLK 3
#define ENCODER_1_DT 5
#define ENCODER_1_SW 7

#define ENCODER_2_CLK 18
#define ENCODER_2_DT 19
#define ENCODER_2_SW 17

#define ENCODER_ACCELERATION true
#define ENCODER_DEBOUNCE_MS 5
```

## Usage Tips

1. **Mounting**: Mount encoders on opposite sides for two-handed control
2. **Knobs**: Use different shaped knobs to distinguish by feel
3. **Labels**: Add labels or colors to indicate function
4. **Detents**: The clicks provide natural feedback for mode changes

## Troubleshooting

### Encoder not responding
- Check 5V and GND connections
- Verify interrupt pins (3, 18) are connected to CLK
- Check Serial Monitor for debug output

### Erratic behavior/double counting
- Increase ENCODER_DEBOUNCE_MS
- Check for loose connections
- Ensure proper grounding

### Direction reversed
- Swap CLK and DT pins in software
- Or physically swap the wire connections

## Testing Procedure

1. Upload test sketch (encoder_test.ino)
2. Open Serial Monitor (115200 baud)
3. Rotate each encoder - should see count changes
4. Press buttons - should see button press messages
5. Test both encoders simultaneously

## Integration with Motion Control

The encoders complement the motion sensors:
- **Tilt**: Controls animation speed
- **Pan**: Controls color/hue shift
- **Encoder 1**: Selects animation pattern
- **Encoder 2**: Simulates rotation/adds manual control layer

This gives you full control even when the kaleidoscope is stationary!