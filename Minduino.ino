#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

#include "Utils.h"
#include "TimedEvents.h"

#include "Sensor.h"
#include "SensorTrigger.h"
#include "GyroSensor.h"
#include "DistanceSensor.h"
#include "CompassSensor.h"
#include "TimedSensor.h"

#include "Actuator.h"
#include "ServoActuator.h"

#define VERBOSE	2

const char PROGRAM_INFO[] PROGMEM = "INFO";
const char PROGRAM_PING[] PROGMEM = "PING";
const char PROGRAM_STOP[] PROGMEM = "STOP";
const char PROGRAM_PROG[] PROGMEM = "PROG";

const byte PROGRAM_TEST[] PROGMEM = 
{
	129, // Gyro on
	1, 17, 0, // main prog
	2, // gyro/accel
		40, 0, // goto
		2, 2, 
		0, 0, 0, 250, 68, // const 2000
	128, 1, // 1 vars
	255, // end headers

	3, 0, // set var 0 = 
		1, 0, // op arit.
			2, 0, // var 0
			0, 0, 0, 128, 63, // const 1
	1, // wait
		0, 0, 0, 0, 64, // const 2
	48, // send msg
		2, 0, // var 0
	0, 17, 0, // goto begin

	48, // send msg
		1, 0, // op sum
			2, 0, // var 0
			0, 0, 0, 122, 68, // const 1000
	1, // wait
		0, 0, 0, 160, 64, // const 5
	13, // end event

	0 // Dummy
};

TimedEvents events;
Sensor *sensors[MAXSENSORS];
unsigned char numSensors = 0;
Actuator *actuators[MAXACTUATORS];

Program *program;
bool serialVerbose = false;

void updateSensors()
{
	int n = 0;
	while (n < numSensors)
	{
		if (sensors[n]->enabled)
			sensors[n]->read();
		n++;
	}
}

void checkSensors()
{
	int n = 0;
	while (n < numSensors)
	{
		if (sensors[n]->enabled)
			sensors[n]->check(program);
		n++;
	}
}

void checkSerial()
{
	if(Serial.available() >= 4)
	{
		char S[4];
		Serial.readBytes(S, 4);
		S[4] = 0;
		if (!strcmp_P(S, PROGRAM_INFO))
		{
			Serial.print(F("INFO:"));
			serialVerbose = !serialVerbose;
			Serial.println(serialVerbose ? F("Y") : F("N"));
		}
		else if (!strcmp_P(S, PROGRAM_PING))
		{
			Serial.println(F("PONG"));
		}
		else if (!strcmp_P(S, PROGRAM_STOP))
		{
			clearAll();
			Serial.println(F("OK"));
		}
		else if (!strcmp_P(S, PROGRAM_PROG))
		{
			Serial.println(F("PROG_OK"));
			unsigned short dataLen;
			Serial.readBytes((uint8_t*)&dataLen, 2);
			
			clearAll();

			delete program->data;
			program->data = new byte[dataLen];
			Serial.readBytes(program->data, dataLen);
			program->dataLen = dataLen;

			Serial.println("OK");

			program->dump();

			prepareProgram();
		}
		else
		{
			Serial.print("ERROR:");
			Serial.println(S);
		}
	}
}

void updateActuators()
{
	for (int n = 0; n < MAXACTUATORS; n++)
	{
		if (actuators[n])
			if(actuators[n]->enabled) actuators[n]->update();
	}
}

// Interpreta el microcodigo operacion por operacion
void executeOperation(TimedEvents::Event *ev)
{
	DataType nextWait = 0;
	byte numOps = 0;
	ExecutionData *eD = (ExecutionData*)ev->userData;

	while (nextWait == 0 && numOps++ < 10 && eD->continueExecution)
	{
#ifdef VERBOSE
		Serial.print(numOps);
		Serial.print("[");
		Serial.print(events.currentPriority);
		Serial.print("] > ");
		//Serial.print("Ejecuta op ");
		//Serial.println(eD->currentOp);

		Serial.print(F("pos:"));
		Serial.print(eD->currentOp);
		Serial.print(F(":"));
		Serial.println(program->data[eD->currentOp]);
#endif

		switch (program->data[eD->currentOp] >> 4) // Revisa 4 primeros bits como familia de operacion
		{
		case 0:
		case 1:
			//Serial.print(F("Logic "));
			nextWait = program-> logicOp(eD);
			break;

		case 3:
			//Serial.print(F("Bluetooth "));
			nextWait = program->bluetoothOp(eD);
			break;
		}

#ifdef VERBOSE
		Serial.println();
#endif

		if (eD->currentOp >= program->dataLen)
			eD->currentOp = program->programStart;
	}

	if(eD->continueExecution)
		events.addEvent(nextWait, executeOperation, eD, ev->priority);
	else
	{
#ifdef VERBOSE
		Serial.println(F("Thread end"));
#endif
		delete eD;
	}
}

void prepareProgram()
{
	unsigned short n;

#ifdef VERBOSE
	Serial.println(F("HEADER START"));
#endif

	events.removeAll();

	for (n = 0; n < numSensors; n++)
	{
		sensors[n]->release();
		delete sensors[n];
	}

	numSensors = 0;

	program->deleteVariables();

	bool moreData = true;
	unsigned short *p_pos, pos;
	byte d1, d2;
	GyroSensor *GS = NULL;
	while (moreData)
	{
		switch (program->data[n++])
		{
		case 1:
			p_pos = (unsigned short*)&program->data[n];
			program->programStart = *p_pos;
#ifdef VERBOSE
			Serial.print(F("ProgramStart SET "));
			Serial.println(program->programStart);
#endif
			program->addEvent(0, 100, program->programStart);

			n += 2;
			break;

		case 2: // Gyro Event
			p_pos = (unsigned short*)&program->data[n];
			n += 2;
			d1 = program->data[n++];
			d2 = program->data[n++];
			pos = n; // Position for event check
			if (GS != NULL)
			{
				Serial.print("Set Event: ");
				Serial.println(d1 * 100 + d2);
				Serial.println(pos);
				Serial.println(*p_pos);
				switch (d1 * 100 + d2)
				{
				case 101: GS->triggerAccX.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				case 102: GS->triggerAccY.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				case 103: GS->triggerAccZ.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				case 201: GS->triggerGyroX.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				case 202: GS->triggerGyroY.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				case 203: GS->triggerGyroZ.set(pos, TRIGGER_GEQ, 10, *p_pos); break;
				}
			}
			break;

		case 128:
			d1 = program->data[n++];
			program->variables = new float[d1];
			memset(program->variables, 0, sizeof(float)*d1);
			break;

		case 129:
#ifdef VERBOSE
			Serial.println(F("GyroSensor ON"));
#endif
			GS = new GyroSensor();
			if (GS->exists())
			{
				sensors[numSensors++] = GS;
				GS->setup();
				GS->enabled = true;
			}
			else {
				delete GS;
				GS = NULL;
#ifdef VERBOSE
				Serial.println(F("GyroSensor OFF"));
#endif
			}
			break;

		case 255:
			moreData = false;
			break;
		}

		if (n >= program->dataLen) moreData = false;
	}

#ifdef VERBOSE
	Serial.println(F("<<< HEADER END"));
#endif
}

void stopExecution()
{
	events.removeAll();
}

// Deletes everything but the program so can be restarted with prepareProgram()
void clearAll()
{
	stopExecution();

	for (byte n = 0; n < numSensors; n++)
	{
		sensors[n]->release();
		delete sensors[n];
	}
	numSensors = 0;

	program->deleteVariables();
}

void setup()
{
	Serial.begin(9600);
#ifdef VERBOSE
	Serial.println(F("Inicio general"));
#endif
	Wire.begin();

	//findI2CDevices();

	program = new Program(executeOperation, &events);

	program->data = new byte[sizeof(PROGRAM_TEST)];
	memcpy_P(program->data, PROGRAM_TEST, sizeof(PROGRAM_TEST));
	program->dataLen = sizeof(PROGRAM_TEST);

	prepareProgram();

#ifdef VERBOSE
	Serial.println("Inicio de sensores");
#endif

/*
	GyroSensor *GS = new GyroSensor();
	if (GS->exists())
	{
		sensors[0] = GS;
		GS->setup();
		GS->enabled = true;
		numSensors++;
	}
	else {
		delete GS;
		GS = NULL;
	}

	DistanceSensor *DS = new DistanceSensor();
	if (DS->exists())
	{
		sensors[1] = DS;
		DS->setMaxDistance(100);
		numSensors++;
	}
	else {
		delete DS;
		DS = NULL;
	}

	CompassSensor *CS = new CompassSensor();
	if (CS->exists())
	{
		sensors[2] = CS;
		CS->setup();
		numSensors++;
	}
	else {
		delete CS;
		CS = NULL;
	}
	
	TimedSensor *TS = new TimedSensor();
	sensors[numSensors++] = TS;
	*/

#ifdef VERBOSE
	Serial.println("Inicio de actuadores");
#endif

	//memset(actuators, MAXACTUATORS, sizeof(Actuator*));

/*	ServoActuator *SA1 = new ServoActuator();
	actuators[ACTUATOR_CONNECTION_A] = SA1;
	SA1->setup(ACTUATOR_CONNECTION_A);*/

/*	ServoActuator *SA2 = new ServoActuator();
	actuators[ACTUATOR_CONNECTION_F] = SA2;
	SA2->setup(ACTUATOR_CONNECTION_F);*/

#ifdef VERBOSE
	Serial.println("Inicio de eventos");
#endif

/*
	if (GS)
	{
		//GS->triggerGyroX.set(5000, TRIGGER_GEQ, true, 20, 23);
		//GS->triggerGyroY.set(2000, TRIGGER_GEQ, true, 10, 14);
		//GS->triggerGyroZ.set(1000, TRIGGER_GEQ, true, 7, 14);
	}

	//TS->set(300, TIMEDSENSOR_REPEAT, 0, 14);

	// Ejecucion inicial. Rama principal
	program->addEvent(0, 100, 0);
*/
}

int loops = 0;

void loop()
{
	updateSensors(); // Actualiza info de sensores
	checkSensors(); // Actualiza acciones desde sensores (triggers)
	checkSerial(); // Actualiza acciones desde Serial

	events.update(); // Ejecuta eventos temporizados

	//updateActuators(); // Ejecuta operaciones en actuadores
}
