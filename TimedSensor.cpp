#include "TimedSensor.h"

bool TimedSensor::exists()
{
	return true;
}

void TimedSensor::read()
{
}


void TimedSensor::check(Program *program)
{
	if (millis() >= timeout)
	{
		program->addEvent(0, priority, label);

		if (mode == TIMEDSENSOR_REPEAT)
			timeout = millis() + delay * 10;
		else
			enabled = false;
	}
}

void TimedSensor::set(unsigned int delay, byte mode, byte priority, ProgramLabel label)
{
	this->delay = delay;
	this->mode = mode;
	this->label = label;
	this->priority = priority;
	timeout = millis() + delay * 10;
	this->enabled = true;
}

void TimedSensor::removeTriggers()
{
	this->enabled = false;
}