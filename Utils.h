#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

int freeRam(); // Devuelve la cantidad de memoria RAM disponible en tiempo real (credito de otro)
void findI2CDevices();

void stop(); // Detiene toda ejecucion en marcha