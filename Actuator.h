#pragma once

#define MAXACTUATORS			6

#define ACTUATOR_CONNECTION_A	0
#define ACTUATOR_CONNECTION_B	1
#define ACTUATOR_CONNECTION_C	2
#define ACTUATOR_CONNECTION_D	3
#define ACTUATOR_CONNECTION_E	4
#define ACTUATOR_CONNECTION_F	5

class Actuator
{
public:
	bool enabled = false;

public:
	virtual void setup(byte connection);
	virtual void update();
};

