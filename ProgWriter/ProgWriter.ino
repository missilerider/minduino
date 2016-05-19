#include "ExecutionData.h"
/*
 Name:		ProgWriter.ino
 Created:	1/9/2016 10:08:04 PM
 Author:	Alfonso
*/

int n = 0;

byte data[1024];
int dataLen = 0;

ExecutionData eD;

void addByte(byte bit8, byte bit7, byte bit6, byte bit5, byte bit4, byte bit3, byte bit2, byte bit1)
{
	addByte((byte)(bit1 + bit2 * 2 + bit3 * 4 + bit4 * 8 + bit5 * 16 + bit6 * 32 + bit7 * 64 + bit8 * 128));
}

void addByte(byte d)
{
	data[dataLen++] = d;
}

void addBytes(const char *S)
{
	for (int n = 0; n < strlen(S); n++)
	{
		addByte(S[n]);
	}
}

void addBytesUInt(unsigned int i)
{
	byte *b = (byte*)&i;
	data[dataLen++] = b[0];
	data[dataLen++] = b[1];
}

void bluetoothOp(ExecutionData *exec)
{
	switch (exec->data[exec->currentOp] & 0x0F)
	{
	case 0:
		byte n = exec->data[++exec->currentOp];

		while (n-- > 0)
			Serial.print((char)exec->data[++exec->currentOp]);

		Serial.println();
		break;
	}
}

void dump()
{
	Serial.println(F("{ "));
	for (int n = 0; n < dataLen; n++)
	{
		Serial.print(eD.data[n]);
		Serial.print(F(", "));
	}
	Serial.println(F("}"));
	Serial.print(F("len = "));
	Serial.println(dataLen);
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	addByte(0,0,1,1,0,0,0,0); // Bluetooth, print text
	addByte(3);
	addBytes("abc");

	addByte(0, 0, 0, 0, 0, 0, 0, 1); // Logic - Wait
	addBytesUInt(100);

	eD.currentOp = 0;
	eD.data = data;

	dump();
}

// the loop function runs over and over again until power down or reset
void loop() {
	int n = eD.currentOp;
	Serial.print(data[n]);
	Serial.print(F(" => "));
	Serial.print(data[n] >> 4);
	Serial.print(F(" | "));
	Serial.println(data[n] & 0x0F);

	switch (data[n] >> 4)
	{
	case 3:
		Serial.println(F("Bluetooth"));
		bluetoothOp(&eD);
		break;
	}
	if (n >= dataLen) n = 0;
	eD.currentOp = n;
	delay(1000);
}
