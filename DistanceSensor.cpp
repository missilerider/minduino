#include <Arduino.h>
#include "DistanceSensor.h"

bool DistanceSensor::exists()
{
	return false;
}

void DistanceSensor::read()
{
	digitalWrite(DISTANCESENSOR_TRIGGER, LOW);
	delayMicroseconds(DISTANCESENSOR_TRIGGERPULL);
	digitalWrite(DISTANCESENSOR_TRIGGER, HIGH);
	delayMicroseconds(DISTANCESENSOR_BEAMWIDTH);
	digitalWrite(DISTANCESENSOR_TRIGGER, LOW);
	unsigned long ulPing = pulseIn(DISTANCESENSOR_ECHO, HIGH, pingTimeout);
	if (!ulPing)
		ulPing = pingTimeout;

	distance = (long)((ulPing / 2) / 29.1);
}

void DistanceSensor::check(Program *program)
{

}

void DistanceSensor::setMaxDistance(int centimeters)
{
	pingTimeout = centimeters * 2 * 29.1;
}

void DistanceSensor::removeTriggers()
{

}
