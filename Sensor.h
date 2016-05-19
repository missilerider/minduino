#pragma once
#define MAXSENSORS	4

#include "Program.h"

class Sensor
{
public:
	bool enabled = false;
//	void *userData;
	void(*eventHandler)(void*);

public:
	virtual void release() {}

	virtual bool exists();
	virtual void read();
	virtual void check(Program *program);
	virtual void removeTriggers();
};

