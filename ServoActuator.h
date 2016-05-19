#pragma once
#include <Servo.h>
#include "Actuator.h"

class ServoActuator :
	public Actuator
{
public:
	byte pin;

private:
	byte pos;
	Servo servo;

public:
	void setup(byte connection);
	void update();

public:
	void setPosition(byte position);
};

