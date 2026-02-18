#include "Motor.h"
#include "Sensor.h"
#include "delay.h"
u8 RC=0;
void Direction(FunctionalState left,FunctionalState right)
{
	/*							Left Motor		Right Motor		Car
	(left,right)	&(AIN1,AIN2)	&(BIN1,BIN2)
				(0,0)			back(1,0)			back(1,0)			back
				(0,1)			back(1,0)			forward(0,1)	left
				(1,0)			forward(0,1)	back(1,0)			right
				(1,1)			forward(0,1)	forward(0,1)	forward
	
	#so: AIN1=!left,AIN2=left;BIN1=!right,BIN2=right;
	*/
	AIN1=!left;
	AIN2=left;
	BIN1=!right;
	BIN2=right;
}
void Pack()
{
		Speed(0,0);
		AIN1=0;
		AIN2=0;
		BIN1=0;
		BIN2=0;
}	
void Speed(u8 ls,u8 rs)/*speed value 0~100(%)*/
{
	u16 leftSpeed=AR*ls;
	u16 rightSpeed=AR*rs;
	/*ls->TIM1channe4->PA11->PWMA,rs->TIM1channe1->PA8->PWMB*/
	TIM_SetCompare1(TIM1,rightSpeed);//channe1 refence compare value
	TIM_SetCompare4(TIM1,leftSpeed);//channe4 refence compare value
}
	
void Angle(u8 ang)/*angle value 0~180(degree)*/
{
	/*0~180 angle->duty ratio 2.5%~12.5%->Period 9~45(total 360)*/
	u8 angle=ang/5+9;
	TIM_SetCompare1(TIM3,angle);
}
	
void MarkFollow()
{
	Speed(20,20);
	while((RC&0x07)==6)
	{
		if(HW_4 == 0 && HW_2 == 0 && HW_1 == 0 && HW_3 == 0)//left:even,right:odd,small:medium,big:side
		{
			Direction(1,1);
			Delay_ms(50);
		}
		/*reach left*/
		if(HW_4 == 0 && HW_2 == 1 && HW_1 == 0 && HW_3 == 0)
		{
			Direction(0,1);//turn left
			Delay_ms(100);
		}
		if(HW_4 == 1 && HW_2 == 0 && HW_1 == 0 && HW_3 == 0)
		{
			Direction(0,1);
			Delay_ms(300);//large angle moveback
		}
		if(HW_4 == 1 && HW_2 == 1 && HW_1 == 0 && HW_3 == 0)
		{
			Direction(0,1);
			Delay_ms(600);//largest angle moveback
		}
		/*reach right*/
		if(HW_4 == 0 && HW_2 == 0 && HW_1 == 1 && HW_3 == 0)
		{
			Direction(1,0);//turn right
			Delay_ms(100);
		}
		if(HW_4 == 0 && HW_2 == 0 && HW_1 == 0 && HW_3 == 1)
		{
			Direction(1,0);
			Delay_ms(300);//large angle moveback
		}
		if(HW_4 == 0 && HW_2 == 0 && HW_1 == 1 && HW_3 == 1)
		{
			Direction(1,0);
			Delay_ms(600);//largest angle moveback
		}
	}
}	

void AvoidObstacle()
{
	while((RC&0x07)==7)
	{
		Angle(90);
		Delay_ms(200);
		SR40_Distance();
		if(DI>250)//no obstacle in font of car
		{
			Direction(1,1);
			Speed(30,30);
			SR40_Distance();
		}
		if(DI<250) //can't go strait
		{
			Pack();
			Angle(30);
			Delay_ms(200);
			SR40_Distance();
			if(DI>250)//no right obstacle
			{
				Direction(1,0);
				Speed(30,30);
				Delay_ms(500);
			}
			else 
				{
					Angle(150);
					Delay_ms(200);
					SR40_Distance();
					if(DI>250)//no left obstacle
					{
						Direction(0,1);
						Speed(30,30);
						Delay_ms(500);
					}
					else// back
					{
						Angle(180);
						Delay_ms(200);
						do
						{
							SR40_Distance();
							Direction(0,0);
							Speed(30,30);
							Delay_ms(500);
						}while(DI<250);
						Direction(0,1);//turn left
						Delay_ms(900);
					}
				}
		}
	}
}
void MoveFollow()
{
	while((RC&0x07)==5)
	{
		SR40_Distance();
		if(DI>250)
		{
			Direction(1,1);
			Speed(30,30);
			Delay_ms(50);
		}
		else if(DI<150)
		{
			Direction(0,0);
			Speed(30,30);
			Delay_ms(50);
		}
		else {Pack();}
	}
}