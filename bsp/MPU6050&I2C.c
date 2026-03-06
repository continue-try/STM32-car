#include "MPU6050&I2C.h"
#include "delay.h"
#include "OLED.h"

void MPUI2C_SCL_WR(u8 BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(BitValue));
}

void MPUI2C_SDA_WR(u8 BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(BitValue));
}

FlagStatus MPUI2C_SDA_RE(void)
{
	FlagStatus BV;
	BV = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8);
	return BV;
}

void MPUI2C_Start()//SDA change from 1 to 0->as SCL=1 time for slave to enter SDA level ->mean IIC start transform
{
	MPUI2C_SDA_WR(1);
	MPUI2C_SCL_WR(1);
	MPUI2C_SDA_WR(0);
	MPUI2C_SCL_WR(0);
}

void MPUI2C_End()//SDA change from 0 to 1-> as SCL=1 time for slave to enter SDA level->mean IIC end transform
{
	MPUI2C_SDA_WR(0);
	MPUI2C_SCL_WR(1);
	MPUI2C_SDA_WR(1);	
}

void MPUI2C_Send(u8 data)
{
	u8 i;
	for(i=0; i<8; i++)
	{
		MPUI2C_SDA_WR(data & (0x80 >> i));
		MPUI2C_SCL_WR(1);//slave read bit of byte
		MPUI2C_SCL_WR(0);
	}
}

u8 MPUI2C_Receive()
{
	u8 data = 0x00;
	MPUI2C_SDA_WR(1);
	for(int i=0; i<8; i++)
	{
		MPUI2C_SCL_WR(1);//slave send bit of byte
		if(MPUI2C_SDA_RE() == 1)
		{
			data = data | (0x80 >> i);
		}
		MPUI2C_SCL_WR(0);
	}
	return data;
}


FlagStatus MPUI2C_ACK(FunctionalState m)//ACK(ask call back)
{
	FlagStatus tip = 0;
	if(m == 0)//wait for slave response& Master no response for end transfer
	{
		MPUI2C_SDA_WR(1);//realse for end or wait
		MPUI2C_SCL_WR(1);
		tip = !MPUI2C_SDA_RE();//0:slave response,1:slave no reaction->tip:true,false
		MPUI2C_SCL_WR(0);
	}
	else{//Master response for continue date transfer
			MPUI2C_SDA_WR(0);
			MPUI2C_SCL_WR(1);
		  MPUI2C_SCL_WR(0);
	}
	return tip;
}

void MPUI2C_SingleByteW(u8 data,u8 address)
{
#if HOS
	I2C_GenerateSTART(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);//I2C situation register to diffrent event
	I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Transmitter);//- for check:began,sent addr,regs enpty and acked
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);//- sending and acked,sent no new in
	I2C_SendData(I2C1,address);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
	I2C_SendData(I2C1,data);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	I2C_GenerateSTOP(I2C1,ENABLE);

#else
	MPUI2C_Start();
	MPUI2C_Send(MPU6050_ADDRESS);//slave address choose devise MPU6050,end bit:write slave
	MPUI2C_ACK(0);//slave response:0 no reaction,1:get!
	MPUI2C_Send(address);//enter regsister address
	MPUI2C_ACK(0);
	MPUI2C_Send(data);//Data send
	MPUI2C_ACK(0);//Ask call back:Master onreaction end
	MPUI2C_End();
#endif
}

u8 MPUI2C_SingleByteR(u8 address)
{
	u8 data = 0x00;
#if HOS
	I2C_GenerateSTART(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);//I2C situation register to diffrent event
	I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Transmitter);//- for check:began,sent addr,received
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS); 
	I2C_SendData(I2C1,address);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	I2C_GenerateSTART(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Receiver);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	I2C_AcknowledgeConfig(I2C1,DISABLE);//no ack and set stopping before recieve
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	data = I2C_ReceiveData(I2C1);
	I2C_AcknowledgeConfig(I2C1,ENABLE);//set back defualt ack

#else
	MPUI2C_Start();
	MPUI2C_Send(MPU6050_ADDRESS);//slave address choose devise MPU6050,end bit:write slave
	MPUI2C_ACK(0);//slave response:0 no reaction,1:get!
	MPUI2C_Send(address);//enter regsister address
	MPUI2C_ACK(0);
	MPUI2C_Start();//restart
	MPUI2C_Send(MPU6050_ADDRESS | 0x01);//slave address choose devise MPU6050,end bit:read slave
	MPUI2C_ACK(0);
	data = MPUI2C_Receive();//data enter
	MPUI2C_ACK(0);
	MPUI2C_End();
#endif
	return data;
}
	
void MPUI2C_SequenceByteR(u8 num,u8 begin_address,u8* save_address)
{
#if HOS
		I2C_GenerateSTART(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);//I2C situation register to diffrent event
	I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Transmitter);//- for check:began,sent addr,received
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS); 
	I2C_SendData(I2C1,begin_address);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	I2C_GenerateSTART(I2C1,ENABLE);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Receiver);
	
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	
	for(int i=0; i < num; i++)
	{
		if(i == num-1){
			I2C_AcknowledgeConfig(I2C1,DISABLE);//no ack and set stopping before recieve
			I2C_GenerateSTOP(I2C1,ENABLE);
		}
		while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
		*(save_address+i) = I2C_ReceiveData(I2C1);
	}
	I2C_AcknowledgeConfig(I2C1,ENABLE);//set back defualt ack
}
#else
	MPUI2C_Start();
	MPUI2C_Send(MPU6050_ADDRESS);//slave address choose devise MPU6050,end bit:write slave
	MPUI2C_ACK(0);//slave response:0 no reaction,1:get!
	MPUI2C_Send(begin_address);//enter regsister address
	MPUI2C_ACK(0);
	MPUI2C_Start();//restart
	MPUI2C_Send(MPU6050_ADDRESS | 0x01);//slave address choose devise MPU6050,end bit:read slave
	MPUI2C_ACK(0);
	for(int i=0; i < num; i++)
	{
		*(save_address+i) = MPUI2C_Receive();//data enter
		if(i == num-1){MPUI2C_ACK(0);}
		else	{MPUI2C_ACK(1);}
	}	
	MPUI2C_End();
#endif
}

void MPU6050_Register_Init()
{
	MPUI2C_SingleByteW(PWR_MGMT_1, 0x80); 						//¸´Î»MPU6050
	Delay_ms(100);
	MPUI2C_SingleByteW(0x01,PWR_MGMT_1);//GYRO clock choosed
	MPUI2C_SingleByteW(0x00,PWR_MGMT_2);//cycle unable,all axis work
	MPUI2C_SingleByteW(0x09,SMPLRT_DIV);//10 fre division
	MPUI2C_SingleByteW(0x06,CONFIG);//fitter low best
	MPUI2C_SingleByteW(GYRO_SCALE,GYRO_CONFIG);//max scale
	MPUI2C_SingleByteW(ACCE_SCALE,ACCEL_CONFIG);//max scale
}

void MPU6050_SenorData(s16* fdata)
{
	u8 databag[14]={0};
	MPUI2C_SequenceByteR(14,ACCEL_XOUT_H,databag);
	for(int i = 0; i < 7; i++)
	{
		*(fdata+i) = (databag[2*i] << 8) | databag[2*i+1];//u8 HL->u16
	}
}

void MPU6050_GyroData(s16* fdata)
{
	u8 databag[6]={0};
	MPUI2C_SequenceByteR(6,GYRO_XOUT_H,databag);
	for(int i = 0; i < 3; i++)
	{
		*(fdata+i) = (databag[2*i] << 8) | databag[2*i+1];//u8 HL->u16
	}
}

void MPU6050_AcceData(s16* fdata)
{
	u8 databag[6]={0};
	MPUI2C_SequenceByteR(6,GYRO_XOUT_H,databag);
	for(int i = 0; i < 3; i++)
	{
		*(fdata+i) = (databag[2*i] << 8) | databag[2*i+1];//u8 HL->u16
	}
}

void MPUI2C_Test(FunctionalState on_off)
{
	s16 data[7]={0};
	u8 ID = MPUI2C_SingleByteR(WHO_AM_I);
	OLED_ShowString(1,1,"ID:",FONTSIZE_8X16);
	OLED_ShowHexNum(1,4,ID,2,FONTSIZE_8X16);
	DENG = 0;
if(on_off)
{
	while(1)
	{FlagStatus temp = 0;
	#if HOS
		I2C_GenerateSTART(I2C1,ENABLE);
		while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
		I2C_Send7bitAddress(I2C1,MPU6050_ADDRESS,I2C_Direction_Transmitter);
		while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS); 
		temp = I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE);
		I2C_GenerateSTOP(I2C1,ENABLE);
	#else
		MPUI2C_Start();
		MPUI2C_Send(0xD0);//slave address choose devise MPU6050
		temp = MPUI2C_ACK(0);
		MPUI2C_End();
	#endif
		if(temp){Delay_ms(1000);DENG = !DENG;}
	}
}
else{
	OLED_ShowString(2,1,"X:",FONTSIZE_8X16);
	OLED_ShowString(3,1,"Y:",FONTSIZE_8X16);
	OLED_ShowString(4,1,"Z:",FONTSIZE_8X16);
	OLED_ShowString(1,10,"GYRO",FONTSIZE_8X16);
	while(1)
	{
		MPU6050_SenorData(data);
		OLED_ShowSignedNum(2,3,data[0],5,FONTSIZE_8X16);
		OLED_ShowSignedNum(3,3,data[1],5,FONTSIZE_8X16);
		OLED_ShowSignedNum(4,3,data[2],5,FONTSIZE_8X16);
		OLED_ShowSignedNum(2,10,data[4],5,FONTSIZE_8X16);
		OLED_ShowSignedNum(3,10,data[5],5,FONTSIZE_8X16);
		OLED_ShowSignedNum(4,10,data[6],5,FONTSIZE_8X16);
	}
}
}

