#ifndef CONFIG_H
#define CONFIG_H

// LED Configuration
#define LED_PIN 4              // Data pin for WS2818
#define NUM_LEDS 209           // Total LEDs on strip
#define LEDS_PER_SEGMENT 70    // LEDs per segment (209/3 = ~70, last segment has 69)
#define NUM_SEGMENTS 3         // Number of segments

// LED Brightness (0-255)
#define MAX_BRIGHTNESS 200     // Maximum brightness to avoid power issues
#define DEFAULT_BRIGHTNESS 150

// Motion Sensor Configuration
#define MPU_UPDATE_RATE 50     // Hz - how often to read sensor
#define MOTION_SMOOTHING 0.2   // 0-1, lower = more smoothing

// Motion Thresholds
#define TILT_THRESHOLD 15.0    // Degrees - minimum tilt to trigger effects
#define SHAKE_THRESHOLD 2.0    // G-force - minimum shake intensity
#define ROTATION_THRESHOLD 50  // Degrees/sec - minimum rotation speed

// Animation Configuration
#define TARGET_FPS 60          // Target frames per second
#define FRAME_DELAY (1000 / TARGET_FPS)

// Calibration
#define CALIBRATION_SAMPLES 100

#endif
