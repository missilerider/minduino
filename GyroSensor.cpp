#include <Arduino.h>
#include <Wire.h>
//#include <MPU6050.h>
#include "GyroSensor.h"

#define GYROSENSOR_I2CADDRESS	0x68
#define MPU6050_PWR_MGMT_1		0x6B
#define MPU6050_ACCEL_XOUT_H	0x3B

#define ACCEL_CONFIG			0x1C
#define GYRO_CONFIG				0x1B

#define accSens					3	// 0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
#define gyroSens				3	// 0 = 250rad/s, 1 = 500rad/s, 2 1000rad/s, 3 = 2000rad/s

// Incluye codigo extraido de http://dummyscodes.blogspot.com.es/2014/07/mpu6050-gy-521-breakout-arduino-mega.html
// Credito del codigo original: 


int GyroSensor::MPU6050_read(int start, uint8_t *buffer, int size)
{
	int i, n, error;
	Wire.beginTransmission(GYROSENSOR_I2CADDRESS);
	n = Wire.write(start);
	if (n != 1)
		return (-10);

	n = Wire.endTransmission(false);    // hold the I2C-bus
	if (n != 0)
		return (n);

	// Third parameter is true: relase I2C-bus after data is read.
	Wire.requestFrom(GYROSENSOR_I2CADDRESS, size, true);
	i = 0;
	while (Wire.available() && i<size)
	{
		buffer[i++] = Wire.read();
	}

	if (i != size)
		return (-11);

	return (0);  // return : no error
}

int GyroSensor::MPU6050_write(int start, const uint8_t *pData, int size)
{
	int n, error;

	Wire.beginTransmission(GYROSENSOR_I2CADDRESS);
	n = Wire.write(start);        // write the start address
	if (n != 1)
		return (-20);

	n = Wire.write(pData, size);  // write data bytes
	if (n != size)
		return (-21);

	error = Wire.endTransmission(true); // release the I2C-bus
	if (error != 0)
		return (error);

	return (0); // return : no error
}

int GyroSensor::MPU6050_write_reg(int reg, uint8_t data)
{
	int error;

	error = MPU6050_write(reg, &data, 1);

	return (error);
}

bool GyroSensor::exists()
{
	Wire.beginTransmission(GYROSENSOR_I2CADDRESS);
	return Wire.endTransmission() == 0;
}

void GyroSensor::setup()
{
	Serial.println("Init GyroSensor");

	int error;
	uint8_t c;

/*	error = MPU6050_read(MPU6050_WHO_AM_I, &c, 1);
	Serial.print(F("WHO_AM_I : "));
	Serial.print(c, HEX);
	Serial.print(F(", error = "));
	Serial.println(error, DEC);

	// According to the datasheet, the 'sleep' bit
	// should read a '1'.
	// That bit has to be cleared, since the sensor
	// is in sleep mode at power-up. 
	error = MPU6050_read(MPU6050_PWR_MGMT_1, &c, 1);
	Serial.print(F("PWR_MGMT_1 : "));
	Serial.print(c, HEX);
	Serial.print(F(", error = "));
	Serial.println(error, DEC);/**/

	Serial.println("Gyro accSens");
	MPU6050_write_reg(ACCEL_CONFIG, accSens << 3); // Specifying output scaling of accelerometer
	Serial.println("Gyro gyroSens");
	MPU6050_write_reg(GYRO_CONFIG, gyroSens << 3); // Specifying output scaling of gyroscope

	Serial.println("Gyro sleep");
	// Clear the 'sleep' bit to start the sensor.
	MPU6050_write_reg(MPU6050_PWR_MGMT_1, 0);
	Serial.println("Gyro setup done");
}

void GyroSensor::read()
{
	int error;

	error = MPU6050_read(MPU6050_ACCEL_XOUT_H, (uint8_t *)&data, sizeof(data));

	// Swap all high and low bytes.
	// After this, the registers values are swapped, 
	// so the structure name like x_accel_l does no 
	// longer contain the lower byte.
	uint8_t swap;
#define SWAP(x,y) swap = x; x = y; y = swap

	SWAP(data.reg.x_accel_h, data.reg.x_accel_l);
	SWAP(data.reg.y_accel_h, data.reg.y_accel_l);
	SWAP(data.reg.z_accel_h, data.reg.z_accel_l);
	SWAP(data.reg.t_h, data.reg.t_l);
	SWAP(data.reg.x_gyro_h, data.reg.x_gyro_l);
	SWAP(data.reg.y_gyro_h, data.reg.y_gyro_l);
	SWAP(data.reg.z_gyro_h, data.reg.z_gyro_l);

	temperature = ((double)data.value.temperature + 12412.0) / 340.0;
	// 16384 - data.value.x_accel
	accX = data.value.x_accel;
	accY = data.value.y_accel;
	accZ = data.value.z_accel;

	gyroX = data.value.x_gyro;
	gyroY = data.value.y_gyro;
	gyroZ = data.value.z_gyro;
}

void GyroSensor::check(Program *program)
{
	if (triggerTemp.check((DataType)(temperature * 100), program))
		if (program->canTriggerEvent(triggerTemp.priority))
			program->addEvent(0, triggerTemp.priority, triggerTemp.label);

	if (triggerAccX.check(accX, program))
		if (program->canTriggerEvent(triggerAccX.priority))
			program->addEvent(0, triggerAccX.priority, triggerAccX.label);

	if (triggerAccY.check(accY, program))
		if (program->canTriggerEvent(triggerAccY.priority))
			program->addEvent(0, triggerAccY.priority, triggerAccY.label);

	if (triggerAccZ.check(accZ, program))
		if (program->canTriggerEvent(triggerAccZ.priority))
			program->addEvent(0, triggerAccZ.priority, triggerAccZ.label);

	if (triggerGyroX.check(gyroX, program))
		if (program->canTriggerEvent(triggerGyroX.priority))
			program->addEvent(0, triggerGyroX.priority, triggerGyroX.label);

	if (triggerGyroY.check(gyroY, program))
		if (program->canTriggerEvent(triggerGyroY.priority))
			program->addEvent(0, triggerGyroY.priority, triggerGyroY.label);

	if (triggerGyroZ.check(gyroZ, program))
		if (program->canTriggerEvent(triggerGyroZ.priority))
			program->addEvent(0, triggerGyroZ.priority, triggerGyroZ.label);
}

void GyroSensor::removeTriggers()
{
	triggerAccX.enabled = false;
	triggerAccY.enabled = false;
	triggerAccZ.enabled = false;
	triggerGyroX.enabled = false;
	triggerGyroY.enabled = false;
	triggerGyroZ.enabled = false;
	triggerTemp.enabled = false;
}

void GyroSensor::release()
{
	removeTriggers();
	MPU6050_write_reg(MPU6050_PWR_MGMT_1, 0x20); // bit6 ON - SLEEP
}