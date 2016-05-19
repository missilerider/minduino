#pragma once

#include <Arduino.h>

typedef struct ExecutionData
{
	byte *data;
	byte currentOp = 0;
};