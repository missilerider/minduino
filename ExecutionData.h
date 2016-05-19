#pragma once

#include <Arduino.h>

typedef struct ExecutionData
{
	uint16_t currentOp = 0;

	byte continueExecution = true; // false = Stop executin actions for this "thread"

	byte priority = 255;
};
