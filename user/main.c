#include "sys.h"
#include "initline.h"
#include "exti.h"
#include "usar.h"
#include "adc.h"
#include "delay.h"
#include "Motor.h"
#include "Sensor.h"
#include "OLED.h"
#include "menu.h"
#include "MPU6050&I2C.h"
#include "key.h"


int main(void)
	{
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//outside interrupt&remap need enable AFIO
	KEY_Init();//1.key GPIO init
	EXTIX_Init();//2.key EXIT&NVIC init
	TB6612_GPIO_Init();
	TB6612_PWM_Init(ARR,360);
	SG90_PWM_Init(0);
	TCRT5000_GPIO_Init();
	BlueTeeth_USAR_Init(115200);
	SR40_GPIO_Init();
	SR40_CAP_Init(TL,TF);		//1Mhz timebase
	Adc_Init();
	OLED_GPIO_Init();
	OLED_Init();
	MPU6050_GPIO_Init();
	MPU6050_Register_Init();
	Test_GPIO_Init();//
		while(1)
		{
			Test_USART_UTF8();
		}
	//MPUI2C_Test(DISABLE);
//	while(1)
//			{
//				if(BSTA)
// 				{PhoneMessage_Rs();}
//				
//				if((OC&0x0F)==0)
//				{
//					ADCVoltage();
//				}
//				if((OC&0x0F)==4)
//				{
//					SR40_Distance();
//				}
//				Menu();
//				
//				if(RC&0x04)//mode run enabe
//				{
//						switch(RC&0x03)
//						{
//							case 0:{Pack();Angle(0);}break;//stop car:xxxxx100
//							case 1:{Angle(90);MoveFollow();Pack();}break;//Mode3:xxxxx101
//							case 2:{MarkFollow();Pack();}break;//Mode2:xxxxx110
//							case 3:{AvoidObstacle();Pack();}break;//Mode3:xxxxx111
//						}					
//				}
//				else
//					{
//					Direction((RC&0x02)>>1,RC&0x01);
//						if(RC&0x08)
//						{
//							Angle((RC&0xF0)+((RC&0xF0)>>2));//angle:0~180 degree(20 degree a bit within 0~9)
//						}
//						else
//						{
//							Speed((RC&0xF0)>>1,(RC&0xF0)>>1);//speed:0%~96%(8% a bit within 0~C)
//						}
//					}
//			}
}