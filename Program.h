#pragma once
#include <Arduino.h>

#include "TimedEvents.h"

#define ProgramLabel	uint16_t
#define DataType		float

typedef struct ExecutionData
{
	ProgramLabel currentOp = 0;

	byte continueExecution = true; // false = Stop executing actions for this "thread" (kill "thread")

	byte priority = 255;
};

class Program
{
public:
	byte *data;
	unsigned int dataLen;
	unsigned int programStart = 0;
	float *variables = NULL;

private:
	void(*microcode)(TimedEvents::Event*);

	TimedEvents *events;

public:
	Program(void(*microcode)(TimedEvents::Event*), TimedEvents *events);

	void loadFromEEPROM();

	void loadFromStream(Stream* S);

	void dump();

	bool canTriggerEvent(byte priority);

	void addEvent(unsigned long delay, byte priority, ProgramLabel label);

	void readDataType(ExecutionData *eD, float **output);
	void readDataType(ExecutionData *eD, int **output);

	void deleteVariables();

public: // Devuelven el tiempo a esperar en centésimas de seg
	DataType logicOp(ExecutionData *eD);
	DataType bluetoothOp(ExecutionData *eD);

	DataType compute(ExecutionData *eD);
};
