#pragma once
#include "types.h"

void initTelemetry();
void sendTelemetry(RobotState state, const LineSensors &line, unsigned long loopTimeUs);
void sendScenarioAEvent(const char *event, unsigned long tUs, float rpm, unsigned long responseUs = 0);
void sendScenarioASample(unsigned long tUs, float rpm, unsigned long loopTimeUs);
