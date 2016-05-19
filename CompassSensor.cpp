#include <Arduino.h>
#include "CompassSensor.h"

bool CompassSensor::exists()
{
	return false;
}

void CompassSensor::setup()
{
	Serial.println("Init CompassSensor");

	x = y = z = 0.0f;
}

void CompassSensor::read()
{
	x += 1.0f;
	if (x >= 100)
	{
		Serial.println(F("Read Compass"));
		x = 0.0f;
	}
}

void CompassSensor::check(Program *program)
{

}

void CompassSensor::removeTriggers()
{

}
