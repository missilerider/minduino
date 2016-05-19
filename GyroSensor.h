#pragma once
#include "SensorTrigger.h"
#include "Sensor.h"
#include "Program.h"

class GyroSensor :
	public Sensor
{
public:
	typedef union accel_t_gyro_union
	{
		struct
		{
			uint8_t x_accel_h;
			uint8_t x_accel_l;
			uint8_t y_accel_h;
			uint8_t y_accel_l;
			uint8_t z_accel_h;
			uint8_t z_accel_l;
			uint8_t t_h;
			uint8_t t_l;
			uint8_t x_gyro_h;
			uint8_t x_gyro_l;
			uint8_t y_gyro_h;
			uint8_t y_gyro_l;
			uint8_t z_gyro_h;
			uint8_t z_gyro_l;
		} reg;
		struct
		{
			int16_t x_accel;
			int16_t y_accel;
			int16_t z_accel;
			int16_t temperature;
			int16_t x_gyro;
			int16_t y_gyro;
			int16_t z_gyro;
		} value;
	};

	float temperature;
	int accX, accY, accZ;
	int gyroX, gyroY, gyroZ;

	SensorTrigger triggerAccX;
	SensorTrigger triggerAccY;
	SensorTrigger triggerAccZ;

	SensorTrigger triggerGyroX;
	SensorTrigger triggerGyroY;
	SensorTrigger triggerGyroZ;

	SensorTrigger triggerTemp;

private:
	accel_t_gyro_union data;

public:
	bool exists();
	void read();
	void check(Program *program);
	void removeTriggers();
	void release();

public:
	void setup();

private:
	int MPU6050_read(int start, uint8_t *buffer, int size);
	int MPU6050_write(int start, const uint8_t *pData, int size);
	int MPU6050_write_reg(int reg, uint8_t data);
};
