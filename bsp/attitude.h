#include "sys.h"
/*------------range gain constant--------------*/
#define Acce_Gain					ACR*2/0xFFFF
#define Gyro_Gain   			GYR*2/0xFFFF
#define Acce_Sensitivity	0xFFFF/(ACR*2)

/*------------digtal structure-----------------*/
typedef struct
{//GYRO&ACCL basic values
	s16 X;
	s16 Y;
	s16 Z;
}INT16_Origin_XYZ;
	INT16_Origin_XYZ zeroShift_ACCE = {0};
	INT16_Origin_XYZ zeroShift_GYRO = {0};
	INT16_Origin_XYZ lastMemory_ACCE = {0};
	INT16_Origin_XYZ lastMemory_GYRO = {0};
typedef struct
{//GYRO&ACCL physical values
	float X;
	float Y;
	float Z;
}FLOAT_Physic_XYZ;
	FLOAT_Physic_XYZ filtered_ACCE = {0};
	FLOAT_Physic_XYZ filtered_GYRO = {0};
	FLOAT_Physic_XYZ xyzGravity_a = {0};
	FLOAT_Physic_XYZ xyzAngular_v = {0};
typedef struct
{//The angle after calculate
	float rol;
	float pit;
	float yaw;
}FLOAT_Attitude_ANG;
	FLOAT_Attitude_ANG car_attitude;

/*------------Digtal Preprocess----------------*/
ErrorStatus ZeroShiftMeasure();
void OriginDigtal_Enter();//offset prime digtal zero shift and enter structure format
void PhysicDigtal_Trans();//16 bit origin digtal transfre to physical digtal with unit
/*------------GYRO&ACCEL Filter----------------*/
void Gyro_Filter(FLOAT_Physic_XYZ* gyro_values,float factor);
void Acce_Filter(FLOAT_Physic_XYZ* accl_values);

/*------------attitude calculate----------------*/

