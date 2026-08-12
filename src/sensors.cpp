#include <Arduino.h>
#include "config.h"
#include "sensors.h"

static long lastLeftDist = 999, lastRightDist = 999;
static unsigned long leftDistanceTime = 0, rightDistanceTime = 0, ultrasonicTimer = 0, rpmTimer = 0;
static bool readLeftNext = true;
static volatile uint32_t leftWheelPulseCount = 0, lastLeftWheelPulseTime = 0;
static float rpm = 0;

void IRAM_ATTR countLeftWheelPulse() {
  uint32_t now = micros();
  if (now - lastLeftWheelPulseTime > 1000) { leftWheelPulseCount++; lastLeftWheelPulseTime = now; }
}
static bool lineDetected(int pin) {
  int detected = 0;
  for (int i = 0; i < 3; ++i) { if (digitalRead(pin) == LOW) ++detected; delayMicroseconds(150); }
  return detected >= 2;
}
static long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10); digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, ULTRASONIC_TIMEOUT_US);
  if (duration == 0) return 999;
  long distance = duration * 0.034 / 2;
  return (distance < MIN_TARGET_RANGE_CM || distance > TRACK_RANGE_CM) ? 999 : distance;
}
void initSensors() {
  pinMode(IR_BACK, INPUT); pinMode(IR_RIGHT, INPUT); pinMode(IR_LEFT, INPUT); pinMode(IR_FRONT_RIGHT, INPUT); pinMode(IR_FRONT_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT); pinMode(TRIG_LEFT, OUTPUT); pinMode(ECHO_LEFT, INPUT);
  pinMode(RPM_SENSOR_LEFT, INPUT); attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_LEFT), countLeftWheelPulse, RISING);
}
LineSensors readLineSensors() { return {lineDetected(IR_FRONT_LEFT), lineDetected(IR_FRONT_RIGHT), lineDetected(IR_LEFT), lineDetected(IR_RIGHT), lineDetected(IR_BACK)}; }
void updateUltrasonic() {
  if (micros() - ultrasonicTimer < ULTRASONIC_INTERVAL_US) return;
  if (readLeftNext) { lastLeftDist = readDistance(TRIG_LEFT, ECHO_LEFT); leftDistanceTime = millis(); }
  else { lastRightDist = readDistance(TRIG_RIGHT, ECHO_RIGHT); rightDistanceTime = millis(); }
  readLeftNext = !readLeftNext; ultrasonicTimer = micros();
}
long leftDistanceCm() { return lastLeftDist; }
long rightDistanceCm() { return lastRightDist; }
bool leftTargetFresh(unsigned long now) { return lastLeftDist != 999 && leftDistanceTime && now - leftDistanceTime <= SENSOR_FRESH_TIME_MS; }
bool rightTargetFresh(unsigned long now) { return lastRightDist != 999 && rightDistanceTime && now - rightDistanceTime <= SENSOR_FRESH_TIME_MS; }
void updateLeftWheelRpm() {
  unsigned long now = millis(); if (now - rpmTimer < 1000) return; rpmTimer = now;
  noInterrupts(); uint32_t pulses = leftWheelPulseCount; leftWheelPulseCount = 0; interrupts();
  rpm = (pulses * 60.0f) / PULSE_PER_REV;
}
float leftWheelRpm() { return rpm; }
