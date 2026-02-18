#include "sys.h"
/********************slave address****************************/
#define MPU6050_ADDRESS 0xD0
/********************scale constant**************************/
#if GYR == 250
    #define GYRO_SCALE (0x00)
#elif GYR == 500
    #define GYRO_SCALE (0x08)
#elif GYR == 1000
    #define GYRO_SCALE (0x10)
#elif GYR == 2000
    #define GYRO_SCALE (0x18)
#else
    #error "GYR must be 250, 500, 1000, or 2000!"
#endif
#if ACR == 2
    #define ACCE_SCALE (0x00)
#elif ACR == 4
    #define ACCE_SCALE (0x08)
#elif ACR == 8
    #define ACCE_SCALE (0x10)
#elif ACR == 16
    #define ACCE_SCALE (0x18)
#else
    #error "ACR must be 2, 4, 8, or 16!"
#endif
/********************Register Map****************************/
#define SMPLRT_DIV			0x19//fre division
#define CONFIG					0x1A
#define GYRO_CONFIG 		0x1B//self_test,scale
#define ACCEL_CONFIG		0x1C//self_test,scale
#define PWR_MGMT_1			0x6B//awake,woke mode,clock choose
#define	PWR_MGMT_2			0x6C//cycle fre,axis unable
//^setting,_record REG
#define	ACCEL_XOUT_H		0x3B
#define	ACCEL_XOUT_L		0x3C
#define	ACCEL_YOUT_H		0x3D
#define	ACCEL_YOUT_L		0x3E
#define	ACCEL_ZOUT_H		0x3F
#define	ACCEL_ZOUT_L		0x40
#define	TEMP_OUT_H			0x41
#define	TEMP_OUT_L			0x42
#define	GYRO_XOUT_H			0x43
#define	GYRO_XOUT_L			0x44
#define	GYRO_YOUT_H			0x45
#define	GYRO_YOUT_L			0x46
#define	GYRO_ZOUT_H			0x47
#define	GYRO_ZOUT_L			0x48
#define WHO_AM_I				0x75
/***********Basic IIC Bit Write&Read to CLK&DATA*************/
void MPUI2C_SCL_WR(u8 BitValue);
void MPUI2C_SDA_WR(u8 BitValue);
FlagStatus MPUI2C_SDA_RE(void);

/***********IIC Transform Rules for Start&End***********/
void MPUI2C_Start();
void MPUI2C_End();
void MPUI2C_Send(u8 data);
u8   MPUI2C_Receive();
FlagStatus MPUI2C_Ack(FunctionalState);//end of single byte need a boit of response
/*FutionalState:1 Master react to slave for go,0 wait for slave reacting to Master then return
																															or Master react to slave for hold*/
/********IIC Transform protocol format for slave-Read&slave-Write********/
void MPUI2C_SingleByteW(u8 data,u8 address);
u8 MPUI2C_SingleByteR(u8 address);
void MPUI2C_SequenceByteR(u8 num,u8 begin_address,u8* save_address);
//Register num,Register begin address,save array first address

/*********************command init&data award****************************/
void MPU6050_Register_Init();
void MPU6050_SenorData(s16* fdata);
void MPU6050_GyroData(s16* fdata);
void MPU6050_AcceData(s16* fdata);

/*********************test program***************************************/
void MPUI2C_Test(FunctionalState);//FS:0 digtal read test,1 slave response test