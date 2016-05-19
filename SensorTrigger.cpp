#include <Arduino.h>
#include "SensorTrigger.h"

/*template <typename T>
bool SensorTrigger<T>::check(double checkValue)
{
	if (!enabled) return false;
	double v;
	if (checkAbs && checkValue < 0)
		v = -checkValue;
	else
		v = checkValue;

	switch (valueRef)
	{
		case TRIGGER_EQ:
			return v == value;
		
		case TRIGGER_LT:
			return v < value;

		case TRIGGER_LEQ:
			return v <= value;

		case TRIGGER_GEQ:
			return v >= value;

		case TRIGGER_GT:
			return v > value;

		case TRIGGER_APROX:
			return value - valueRef <= v && value + valueRef >= v;
	}

	return false;
}*/
/*
template <typename T>
void SensorTrigger<T>::set(T checkValue, byte ref)
{
	set(checkValue, ref, 0, false);
}

template <typename T>
void SensorTrigger<T>::set(T checkValue, byte ref, T range)
{
	set(checkValue, ref, range, false);
}

template <typename T>
void SensorTrigger<T>::set(T checkValue, byte ref, bool abs)
{
	set(checkValue, ref, 0, abs);
}

/*template <typename T>
void SensorTrigger<T>::set(T checkValue, byte ref, T range, bool abs)
{
	enabled = true;
	value = checkValue;
	valueRef = ref;
	aproxVal = range;
	checkAbs = abs;
}
*/