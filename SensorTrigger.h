#pragma once

#include "Program.h"

#define TRIGGER_EQ		0
#define TRIGGER_LT		1
#define TRIGGER_LEQ		2
#define TRIGGER_GT		3
#define TRIGGER_GEQ		4
#define TRIGGER_APROX	5

class SensorTrigger
{
public:

public:
	bool enabled = false;
	ProgramLabel value;
	byte valueRef;
	ProgramLabel aproxVal;
	bool checkAbs = false;

	byte priority = 10;
	ProgramLabel label;


public:
	bool check(DataType checkValue, Program *program)
	{
		if (!enabled) return false;

		DataType d1, d2;

		double v;
		if (checkAbs && checkValue < 0)
			v = -checkValue;
		else
			v = checkValue;

		ExecutionData eD;
		eD.currentOp = value;
		d1 = program->compute(&eD);

		switch (valueRef)
		{
		case TRIGGER_EQ:
			return v == d1;

		case TRIGGER_LT:
			return v < d1;

		case TRIGGER_LEQ:
			return v <= d1;

		case TRIGGER_GEQ:
			return v >= d1;

		case TRIGGER_GT:
			return v > d1;

		case TRIGGER_APROX:
			eD.currentOp = aproxVal;
			d2 = program->compute(&eD);

			return d1 - d2 <= v && d1 + d2 >= v;
		}

		return false;
	}

	void set(ProgramLabel checkValue, byte ref, byte priority, ProgramLabel label)
	{
		set(checkValue, ref, 0, false, priority, label);
	}

	void set(ProgramLabel checkValue, byte ref, ProgramLabel range, byte priority, ProgramLabel label)
	{
		set(checkValue, ref, range, false, priority, label);
	}

	void set(ProgramLabel checkValue, byte ref, bool abs, byte priority, ProgramLabel label)
	{
		set(checkValue, ref, 0, abs, priority, label);
	}

	void set(ProgramLabel checkValue, byte ref, ProgramLabel range, bool abs, byte priority, ProgramLabel label)
	{
		enabled = true;
		value = checkValue;
		valueRef = ref;
		aproxVal = range;
		checkAbs = abs;
		this->priority = priority;
		this->label = label;
	}
};
