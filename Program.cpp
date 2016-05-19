#include "Program.h"

#define VERBOSE	2

Program::Program(void(*microcode)(TimedEvents::Event*), TimedEvents *events)
{
	this->microcode = microcode;
	this->events = events;
}

#ifdef VERBOSE
void Program::dump()
{
	Serial.print(F("{ "));
	for (int n = 0; n < dataLen; n++)
	{
		Serial.print(data[n]);
		Serial.print(F(", "));
	}
	Serial.println(F("}"));
	Serial.print(F("len = "));
	Serial.println(dataLen);
}
#endif

bool Program::canTriggerEvent(byte priority)
{
	// TODO: Verificar diferentes tipos de conducta en el lanzamiento de eventos para diferentes o iguales prioridades
	// Ahora solo se añaden eventos si es de prioridad mayor (numero menor)

#ifdef VERBOSE
	if (priority < events->currentPriority)
	{
		Serial.print("Nuevo evento prioridad ");
		Serial.print(priority);
		Serial.print("/");
		Serial.println(events->currentPriority);
	}
	else
	{
		Serial.print("No evento: ");
		Serial.print(priority);
		Serial.print("/");
		Serial.println(events->currentPriority);
	}
#endif

	return priority < events->currentPriority;
}

void Program::deleteVariables()
{
	if (variables == NULL) return;
	delete variables;
	variables = NULL;
}


void Program::addEvent(unsigned long delay, byte priority, ProgramLabel label)
{
	ExecutionData *eD = new ExecutionData();

	eD->currentOp = label;
	eD->priority = priority;

	events->addEvent(delay, microcode, eD, priority);
}

DataType Program::logicOp(ExecutionData *eD)
{
	int *i;
	DataType i2;
	byte var;

	switch (data[eD->currentOp++] & 0x1F)
	{
	case 0: // GoTo
		i = (int*)&data[eD->currentOp];
		eD->currentOp = *i;
#ifdef VERBOSE
		Serial.print(F("GoTo "));
		Serial.print(eD->currentOp);
#endif
		return 0;

	case 1: // Wait
		i2 = this->compute(eD);
#ifdef VERBOSE
		Serial.print(F("Wait:"));
		Serial.print(i2);
#endif
		if (i2 < 0) return 0;
		return i2 * (DataType)1000;

	case 3: // Set Var
		var = (byte)data[eD->currentOp++];
		variables[var] = this->compute(eD);
#ifdef VERBOSE
		Serial.print(F("VarSet["));
		Serial.print(var);
		Serial.print(F("] = "));
		Serial.print(variables[var]);
#endif
		break;

	case 13: // End trigger execution
#ifdef VERBOSE
		Serial.print(F("End trigger"));
#endif
		eD->continueExecution = false;
		break;

	default:
		Serial.print("Comando desconocido");
		break;
	}

	return 0;
}

DataType Program::bluetoothOp(ExecutionData *eD)
{
	switch (data[eD->currentOp++] & 0x0F)
	{
	case 0:
		DataType i = compute(eD);
		Serial.print(F("MSG:"));
		Serial.println(i);
		break;
	}

	return 0;
}

void Program::readDataType(ExecutionData *eD, float **output)
{
	*output = (float*)&data[eD->currentOp];
	eD->currentOp += 4;
}

void Program::readDataType(ExecutionData *eD, int **output)
{
	*output = (int*)&data[eD->currentOp];
	eD->currentOp += 2;
}

DataType Program::compute(ExecutionData *eD)
{
	int ret;
	DataType op1, op2;
	DataType *p_op;
	byte op;

	switch (data[eD->currentOp++])
	{
	case 0: // Constant
		readDataType(eD, &p_op);
#ifdef _VERBOSE
		Serial.print(*p_op);
#endif
		return *p_op;
	
	case 1: // Operation
		op = data[eD->currentOp++];
#ifdef _VERBOSE
		Serial.print(F("( "));
#endif
		op1 = compute(eD);
#ifdef _VERBOSE
		switch (op)
		{
		case 0: Serial.print(F(" + ")); break;
		case 1: Serial.print(F(" - ")); break;
		case 2: Serial.print(F(" * ")); break;
		case 3: Serial.print(F(" / ")); break;
		case 4: Serial.print(F(" ^ ")); break;
		}
#endif
		op2 = compute(eD);
#ifdef _VERBOSE
		Serial.print(F(" )"));
#endif

		switch (op)
		{
		case 0: // Add
			return op1 + op2;

		case 1: // Subtract
			return op1 - op2;

		case 2: // Multiply
			return (DataType)(op1 * op2);

		case 3: // Divide
			return (DataType)((float)op1 / (float)op2);

		case 4: // Power
			return (DataType)(pow(op1, op2));
		}
		break;

	case 2: // Read Variable
		op = data[eD->currentOp++];
#ifdef _VERBOSE
		Serial.print(F("v"));
		Serial.print(op);
		if (VERBOSE > 1)
		{
			Serial.print(F("["));
			Serial.print(variables[op]);
			Serial.print(F("]"));
		}
#endif
		return variables[op];



	default:
		Serial.println(F("Operacion desconocida"));
	}
}
