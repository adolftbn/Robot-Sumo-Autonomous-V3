#pragma once

// Motor driver (ESP32 + dual H-bridge)
constexpr int ENA = 14;
constexpr int IN1 = 26;
constexpr int IN2 = 27;
constexpr int ENB = 25;
constexpr int IN3 = 32;
constexpr int IN4 = 33;
constexpr uint8_t PWM_CHANNEL_A = 0;
constexpr uint8_t PWM_CHANNEL_B = 1;

// IR line sensors. A LOW level means the white boundary is detected.
constexpr int IR_BACK = 35;
constexpr int IR_RIGHT = 34;
constexpr int IR_LEFT = 21;
constexpr int IR_FRONT_RIGHT = 23;
constexpr int IR_FRONT_LEFT = 22;

// HC-SR04 ultrasonic sensors
constexpr int TRIG_RIGHT = 5;
constexpr int ECHO_RIGHT = 17;
constexpr int TRIG_LEFT = 18;
constexpr int ECHO_LEFT = 19;

// Only the left wheel has an encoder in the current hardware.
constexpr int RPM_SENSOR_LEFT = 4;
constexpr uint32_t PULSE_PER_REV = 40;

// Motion parameters
constexpr int PWM_ESCAPE_BACK = 180;
constexpr int PWM_ESCAPE_TURN = 190;
constexpr int PWM_SEARCH = 130;
constexpr int PWM_SEARCH_ROTATE = 135;
constexpr int PWM_TRACK = 190;
constexpr int PWM_APPROACH = 180;
constexpr int PWM_ATTACK = 230;

constexpr long MIN_TARGET_RANGE_CM = 4;
constexpr long TRACK_RANGE_CM = 120;
constexpr long ATTACK_RANGE_CM = 30;

constexpr uint32_t ULTRASONIC_TIMEOUT_US = 8000;
constexpr uint32_t ULTRASONIC_INTERVAL_US = 12000;
constexpr uint32_t SENSOR_FRESH_TIME_MS = 80;
constexpr uint32_t TARGET_HOLD_TIME_MS = 350;

constexpr uint32_t SEARCH_FORWARD_TIME_MS = 60;
constexpr uint32_t SEARCH_ROTATE_TIME_MS = 140;
constexpr uint32_t ESCAPE_FRONT_MIN_BACK_TIME_MS = 100;
constexpr uint32_t ESCAPE_FRONT_MAX_BACK_TIME_MS = 180;
constexpr uint32_t ESCAPE_FRONT_BRAKE_TIME_MS = 40;
constexpr uint32_t REAR_CONFIRM_TIME_MS = 30;
constexpr uint32_t ESCAPE_FRONT_TURN_TIME_MS = 450;
constexpr uint32_t ESCAPE_SIDE_TURN_TIME_MS = 280;
constexpr uint32_t ESCAPE_BACK_FORWARD_TIME_MS = 140;

constexpr bool DEBUG_TELEMETRY = true;
constexpr uint32_t DEBUG_INTERVAL_MS = 200;
constexpr uint32_t SERIAL_BAUD = 230400;

// Scenario A: set ENABLED to true for one straight-line PWM/RPM/overshoot trial.
// Change TARGET_PWM and TRIAL_ID before uploading for the next trial.
constexpr bool SCENARIO_A_ENABLED = false;
constexpr bool SCENARIO_B_ENABLED = true;
// Motor-channel diagnostic is complete; use Scenario A behavior again.
constexpr bool MOTOR_CHANNEL_DIAGNOSTIC_ENABLED = false;
constexpr uint16_t SCENARIO_A_TRIAL_ID = 1;
constexpr int SCENARIO_A_TARGET_PWM = 255;  // Allowed research levels: 127, 191, 255.
// Calibration test: keep the encoder-equipped left wheel at TARGET_PWM and
// reduce the faster right wheel to improve the straight-line trajectory.
constexpr int SCENARIO_A_LEFT_PWM_OFFSET = 0;
constexpr int SCENARIO_A_RIGHT_PWM_OFFSET = -40;
constexpr uint32_t SCENARIO_A_RPM_SAMPLE_INTERVAL_MS = 50;
constexpr uint32_t SCENARIO_A_RPM_STABLE_TIME_MS = 500;
constexpr uint32_t SCENARIO_A_MAX_BRAKE_TIME_MS = 1000;
constexpr uint32_t SCENARIO_A_MIN_BACK_TIME_MS = 140;

constexpr int SCENARIO_B_TARGET_PWM = 191;
constexpr uint16_t SCENARIO_B_TRIAL_ID = 1;
constexpr int SCENARIO_B_LEFT_PWM_OFFSET = 0;
constexpr int SCENARIO_B_RIGHT_PWM_OFFSET = -40;
constexpr uint32_t SCENARIO_B_MAX_BRAKE_TIME_MS = 1000;
constexpr uint32_t SCENARIO_B_BACK_TIME_MS = 140;
constexpr int SCENARIO_B_TURN_PWM = 190;
constexpr uint32_t SCENARIO_B_TURN_TIME_MS = 450;
