#pragma once
#include "types.h"

void initTelemetry();
void sendTelemetry(RobotState state, const LineSensors &line, unsigned long loopTimeUs);
