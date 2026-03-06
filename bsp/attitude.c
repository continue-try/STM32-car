#include "attitude.h"
#include "MPU6050&I2C.h"
#include "MathAndPhysics.h"
#include "delay.h"

ErrorStatus ZeroShiftMeasure()
{
	s16 readData[7] = {0};
	u8	endMeasCount = 30;
	u8 	measureCount = 200;
	INT16_Origin_XYZ current = {0};
	INT16_Origin_XYZ last = {0};
		for(u8 i = 0;i < measureCount;i++)
		{
			last = current;
			Delay_ms(5);
				MPU6050_SenorData(readData);
			current.X = readData[0] + readData[4];//new read digtal X,Y,Z compare with old those
			current.Y = readData[1] + readData[5];
			current.Z = readData[2] + readData[6];
			if(ABS(current.X - last.X) < 15 & ABS(current.Y - last.Y) < 15 & ABS(current.Z - last.Z) < 15)
			{				
				zeroShift_ACCE.X += readData[0];
				zeroShift_ACCE.Y += readData[1];
				zeroShift_ACCE.Z += readData[2] - Acce_Sensitivity;
				zeroShift_GYRO.X += readData[4];
				zeroShift_GYRO.Y += readData[5];
				zeroShift_GYRO.Z += readData[6];
			}
			else{i --;endMeasCount--;
			if(endMeasCount == 0)
			{
				return ERROR;
			}
			}
		}
		zeroShift_ACCE.X = zeroShift_ACCE.X/measureCount;
		zeroShift_ACCE.Y = zeroShift_ACCE.Y/measureCount;
		zeroShift_ACCE.Z = zeroShift_ACCE.Z/measureCount;
		zeroShift_GYRO.X = zeroShift_GYRO.X/measureCount;
		zeroShift_GYRO.Y = zeroShift_GYRO.Y/measureCount;
		zeroShift_GYRO.Z = zeroShift_GYRO.Z/measureCount;
		return SUCCESS;
}

void OriginDigtal_Enter()
{
	s16 readData[7] = {0};
	MPU6050_SenorData(readData);
	lastMemory_ACCE.X = readData[0] - zeroShift_ACCE.X;
	lastMemory_ACCE.Y = readData[1] - zeroShift_ACCE.Y;
	lastMemory_ACCE.Z = readData[2] - zeroShift_ACCE.Z;
	lastMemory_GYRO.X = readData[4] - zeroShift_GYRO.X;
	lastMemory_GYRO.Y = readData[5] - zeroShift_GYRO.Y;
	lastMemory_GYRO.Z = readData[6] - zeroShift_GYRO.Z;
}

void PhysicDigtal_Trans()
{
	
}

void Gyro_Filter(FLOAT_Physic_XYZ* gyro_values,float factor)
{
	s16 readData[3] = {0};
	MPU6050_GyroData(readData);
	gyro_values->X = LOWPASSF(readData[0], lastMemory_GYRO.X, factor);
	gyro_values->Y = LOWPASSF(readData[1], lastMemory_GYRO.Y, factor);
	gyro_values->Z = LOWPASSF(readData[2], lastMemory_GYRO.Z, factor);
}

void Acce_Filter(FLOAT_Physic_XYZ* accl_values)
{
	s16 readData[3] = {0};
	MPU6050_AcceData(readData);
	//accl_values->X = ;
	//accl_values->Y = ;
	//accl_values->Z = ;
}