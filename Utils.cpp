#include <Wire.h>
#include "Utils.h"

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void findI2CDevices()
{
	byte error, address;

	for (address = 1; address < 127; address++)
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print(F("I2C device found at address 0x"));
			if (address < 16)
				Serial.print(F("0"));
			Serial.print(address, HEX);
			Serial.println(F("  !"));

			//nDevices++;
		}
	}
}

void stop()
{

}