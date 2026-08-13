#include <Arduino.h>
#include "behavior.h"
#include "config.h"
#include "escape.h"
#include "motor.h"
#include "sensors.h"
#include "scenario_a.h"
#include "telemetry.h"

void setup() {
  Serial.begin(SERIAL_BAUD);
  initSensors();
  initMotor();
  initTelemetry();
  Serial.println("STARTING...");
  delay(1000);
  initScenarioA();
}

void loop() {
  unsigned long loopStart = micros();
  LineSensors line = readLineSensors();
  if (scenarioAActive()) {
    runScenarioA(line, loopStart);
    return;
  }
  updateLeftWheelRpm();
  RobotState state = handleEscape(line) ? STATE_ESCAPE : runBehavior(millis());
  sendTelemetry(state, line, micros() - loopStart);
}
