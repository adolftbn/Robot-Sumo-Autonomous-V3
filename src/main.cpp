#include <Arduino.h>
#include "behavior.h"
#include "escape.h"
#include "motor.h"
#include "sensors.h"
#include "telemetry.h"

void setup() {
  Serial.begin(115200);
  initSensors();
  initMotor();
  initTelemetry();
  Serial.println("STARTING...");
  delay(1000);
}

void loop() {
  unsigned long loopStart = micros();
  updateLeftWheelRpm();
  LineSensors line = readLineSensors();
  RobotState state = handleEscape(line) ? STATE_ESCAPE : runBehavior(millis());
  sendTelemetry(state, line, micros() - loopStart);
}
