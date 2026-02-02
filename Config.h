#ifndef CONFIG_H
#define CONFIG_H

// LED Configuration
#define LED_PIN 4              // Data pin for WS2818
#define NUM_LEDS 209           // Total LEDs on strip
#define LEDS_PER_SEGMENT 70    // LEDs per segment (209/3 = ~70, last segment has 69)
#define NUM_SEGMENTS 3         // Number of segments

// LED Brightness (0-255)
#define MAX_BRIGHTNESS 50     // Maximum brightness to avoid power issues
#define DEFAULT_BRIGHTNESS 10 // Lowered to 10 for minimal power

// Motion Sensor Configuration
#define MPU_UPDATE_RATE 100    // Hz - how often to read sensor (increased for smoother response)
#define MOTION_SMOOTHING 0.25  // Increased smoothing for end-mounted sensor (was 0.15)

// Motion Thresholds (adjusted for end-mount position)
#define TILT_THRESHOLD 3.0     // More sensitive since sensor at end (was 5.0)
#define SHAKE_THRESHOLD 2.0    // Less sensitive to shake at end (was 1.5)
#define ROTATION_THRESHOLD 3   // More sensitive for rotation (was 5)

// Sensor mounting position compensation
#define SENSOR_AT_END true     // Set true if sensor at viewing end
#define SHAKE_DAMPENING 0.7    // Reduce shake amplitude from end-mount

// Animation Configuration
#define TARGET_FPS 120         // Target frames per second (doubled for smoother animation)
#define FRAME_DELAY (1000 / TARGET_FPS)

// Mode Configuration
#define AUTO_CYCLE_MODES false  // Automatically cycle through modes
#define MODE_DURATION_MS 20000 // Duration per mode in auto-cycle (20 seconds)
#define RANDOM_START_MODE true // Start with random mode instead of kaleidoscope

// Calibration
#define CALIBRATION_SAMPLES 100

// Encoder Configuration
#define USE_ENCODERS true      // Enable rotary encoder support
#define ENCODER_1_CLK 3        // Encoder 1 clock pin (interrupt)
#define ENCODER_1_DT 5         // Encoder 1 direction pin
#define ENCODER_1_SW 7         // Encoder 1 switch pin

#define ENCODER_2_CLK 18       // Encoder 2 clock pin (interrupt)
#define ENCODER_2_DT 19        // Encoder 2 direction pin
#define ENCODER_2_SW 17        // Encoder 2 switch pin

#define ENCODER_DEBOUNCE_MS 5  // Debounce time for encoders
#define ENCODER_ACCELERATION true // Speed up changes with fast rotation

// Virtual Rotation (simulated by encoder 2)
#define VIRTUAL_ROTATION_MAX 100  // Maximum virtual rotation value
#define VIRTUAL_ROTATION_STEP 10  // Step size per encoder click (10 = reach 100 in half rotation)

#endif
