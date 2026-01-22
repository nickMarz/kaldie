# Wiring Diagram

## Component Connections

### MPU6050 Motion Sensor → Arduino Mega

```
┌─────────────┐              ┌──────────────────┐
│   MPU6050   │              │   Arduino Mega   │
│   GY-521    │              │      R3          │
├─────────────┤              ├──────────────────┤
│ VCC    ●────┼──────────────┼────●  5V         │
│ GND    ●────┼──────────────┼────●  GND        │
│ SCL    ●────┼──────────────┼────●  21 (SCL)   │
│ SDA    ●────┼──────────────┼────●  20 (SDA)   │
│ XDA         │              │                  │
│ XCL         │              │                  │
│ AD0         │              │                  │
│ INT         │              │                  │
└─────────────┘              └──────────────────┘
```

**Notes:**
- XDA, XCL, AD0, INT pins are not used in this project
- AD0 can be left floating or connected to GND (sets I2C address)

### WS2812B LED Strip → Arduino Mega

```
┌──────────────────┐         ┌──────────────────┐
│   WS2812B Strip  │         │   Arduino Mega   │
│   (300 LEDs)     │         │      R3          │
├──────────────────┤         ├──────────────────┤
│ 5V         ●─────┼─────────┼────●  5V*        │
│ GND        ●─────┼─────────┼────●  GND        │
│ DIN        ●─────┼─────────┼────●  6 (PWM)    │
└──────────────────┘         └──────────────────┘
```

**Important Power Notes:**
- **\* DO NOT power 300 LEDs from Arduino's 5V pin!**
- Use external 5V power supply rated for at least 12A (60W)
- Connect Arduino GND to external power supply GND (common ground)

### Recommended External Power Setup

```
┌────────────────────┐
│  5V Power Supply   │
│  60W (12A) min     │
└────────┬───────────┘
         │
    ┌────┴────┐
    │         │
    5V       GND
    │         │
    │         ├──────────────────┐
    │         │                  │
    │         │                  │
┌───┴─────────┴────┐      ┌──────┴──────────────┐
│  WS2812B Strip   │      │   Arduino Mega      │
│                  │      │   GND               │
│  5V     GND  DIN │      │    │                │
└───────────────┬──┘      └────┼────────────────┘
                │                │
                └────────────────┘
                    To Pin 6
```

### Optional: Mode Switch Button

```
┌──────────┐              ┌──────────────────┐
│  Button  │              │   Arduino Mega   │
├──────────┤              ├──────────────────┤
│  Pin 1 ●─┼──────────────┼────●  2          │
│  Pin 2 ●─┼──────────────┼────●  GND        │
└──────────┘              └──────────────────┘
```

**Notes:**
- Arduino Mega has internal pull-up resistor enabled on Pin 2
- No external resistor needed
- Button press cycles through animation modes

## Complete Assembly

```
                           ┌─────────────────────────────┐
                           │      Arduino Mega R3         │
                           │                              │
     ┌─────────────────────┤  20 (SDA)  21 (SCL)         │
     │                     │                              │
     │     ┌───────────────┤  6 (PWM)                     │
     │     │               │                              │
     │     │         ┌─────┤  2 (Digital)                 │
     │     │         │     │                              │
     │     │         │     │  5V   GND                    │
     │     │         │     └────┬────┬────────────────────┘
     │     │         │          │    │
     │     │         │          │    └──────────┬─────────────┐
     │     │         │          │               │             │
┌────┴─────┴──┐  ┌───┴───┐  ┌──┴───┐     ┌─────┴───┐   ┌─────┴─────┐
│  MPU6050    │  │ LED   │  │ 5V   │     │ Button  │   │  External │
│  SDA   SCL  │  │ Strip │  │ PSU  │     │         │   │  5V PSU   │
│             │  │ DIN   │  │ 60W  │     │ (Mode)  │   │           │
│             │  │       │  │      │     │         │   │  GND      │
└─────────────┘  └───────┘  └──┬───┘     └─────────┘   └─────┬─────┘
                                │                              │
                                └──────────────────────────────┘
                                   Common Ground Connection
```

## Pin Summary

| Component      | Arduino Pin | Function           |
|----------------|-------------|--------------------|
| MPU6050 SDA    | 20 (SDA)    | I2C Data           |
| MPU6050 SCL    | 21 (SCL)    | I2C Clock          |
| WS2812B DIN    | 6 (PWM)     | LED Data           |
| Mode Button    | 2 (Digital) | Mode Switch Input  |

## Wire Recommendations

- **Power wires (5V, GND)**: 18-20 AWG for high current
- **Signal wires (SDA, SCL, DIN)**: 22-24 AWG is fine
- Keep LED data wire (DIN) as short as possible
- Twist SDA/SCL wires together to reduce interference

## Safety Checklist

- [ ] Double-check all connections before powering on
- [ ] Verify 5V power supply polarity (don't reverse!)
- [ ] Ensure common ground between Arduino and power supply
- [ ] Start with low brightness setting (`DEFAULT_BRIGHTNESS = 100`)
- [ ] Monitor power supply temperature during first use
- [ ] Use proper gauge wire for high current connections

## Testing Order

1. **Test Arduino only** - Upload code, check Serial Monitor
2. **Add MPU6050** - Verify motion sensor readings
3. **Add LEDs with low brightness** - Test with `DEFAULT_BRIGHTNESS = 50`
4. **Gradually increase brightness** - Monitor power supply
5. **Add button** - Test mode switching

## Troubleshooting Wiring Issues

| Problem | Check |
|---------|-------|
| No Serial output | USB cable connected? Correct COM port? |
| "MPU6050 not found" | SDA/SCL connections, I2C pullup resistors |
| No LEDs | Power supply, DIN connection, LED strip orientation |
| LEDs flicker | Common ground, power supply capacity |
| Random colors | DIN connection, first LED might be damaged |
| Button doesn't work | Pin 2 connection, check button orientation |

## Advanced: Capacitor Recommendation

Add a **1000µF capacitor** across the LED strip power connection (+ to 5V, - to GND) to smooth power delivery and protect against voltage spikes.

```
     5V ───┬─── LED Strip 5V
           │
          ╱│╲  1000µF
          ╲│╱  Capacitor
           │   (+ to 5V)
    GND ───┴─── LED Strip GND
```
