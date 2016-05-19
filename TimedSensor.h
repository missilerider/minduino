#pragma once
#include "Sensor.h"

#define TIMEDSENSOR_ONCE	0
#define TIMEDSENSOR_REPEAT	1


class TimedSensor :
	public Sensor
{
public:
	unsigned long timeout; // millis en los que tiene que entrar en funcionamiento la proxima vez

	unsigned int delay; // Espera entre eventos, en centesimas de segundo
	byte mode = TIMEDSENSOR_REPEAT;
	ProgramLabel label;
	byte priority;

public:
	bool exists();
	void read();
	void check(Program *program);
	void removeTriggers();

public:
	void set(unsigned int delay, byte mode, byte priority, ProgramLabel label);
};

