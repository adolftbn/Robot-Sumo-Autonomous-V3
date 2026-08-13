#include <Arduino.h>
#include "behavior.h"
#include "config.h"
#include "escape.h"
#include "motor.h"
#include "sensors.h"
#include "scenario_a.h"
#include "telemetry.h"

static void runMotorChannelDiagnostic() {
  static unsigned long diagnosticStartMs = 0;
  static bool started = false;
  static uint8_t announcedStage = 0;
  if (!started) {
    diagnosticStartMs = millis();
    started = true;
    Serial.println("DIAG,MOTOR_CHANNEL_A_ONLY");
  }

  unsigned long elapsedMs = millis() - diagnosticStartMs;
  if (elapsedMs < 2000) {
    forwardChannelAOnly(127);
  } else if (elapsedMs < 4000) {
    if (announcedStage != 1) {
      Serial.println("DIAG,MOTOR_CHANNEL_B_ONLY");
      announcedStage = 1;
    }
    forwardChannelBOnly(127);
  } else {
    stopMotor();
    if (announcedStage != 2) {
      Serial.println("DIAG,COMPLETE");
      announcedStage = 2;
    }
  }
}

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
  if (MOTOR_CHANNEL_DIAGNOSTIC_ENABLED) {
    runMotorChannelDiagnostic();
    return;
  }
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
