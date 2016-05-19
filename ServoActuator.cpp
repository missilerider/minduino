#include <Arduino.h>
#include "ServoActuator.h"

void ServoActuator::setup(byte connection)
{
	switch (connection)
	{
	case ACTUATOR_CONNECTION_A: pin = 3; break;
	case ACTUATOR_CONNECTION_B: pin = 5; break;
	case ACTUATOR_CONNECTION_C: pin = 6; break;
	case ACTUATOR_CONNECTION_D: pin = 9; break;
	case ACTUATOR_CONNECTION_E: pin = 10; break;
	case ACTUATOR_CONNECTION_F: pin = 11; break;
	default: pin = 0;
	}

	servo.attach(pin);
	enabled = true;
}

void ServoActuator::update()
{
	servo.write(pos);
}

void ServoActuator::setPosition(byte position)
{
	pos = position % 181;
}