#pragma once
#include "Sensor.h"
class CompassSensor :
	public Sensor
{
public:
	float x, y, z;

public:
	bool exists();
	void read();
	void check(Program *program);
	void removeTriggers();

public:
	void setup();
};

