#pragma once
#include "Sensor.h"

#define DISTANCESENSOR_TRIGGER			2
#define DISTANCESENSOR_ECHO				3
#define DISTANCESENSOR_TRIGGERPULL		2
#define DISTANCESENSOR_BEAMWIDTH		10

class DistanceSensor :
	public Sensor
{
public:
	long distance;

private:
	long pingTimeout;

public:
	bool exists();
	void read();
	void check(Program *program);
	void removeTriggers();

public:
	void setMaxDistance(int centimeters);
};

