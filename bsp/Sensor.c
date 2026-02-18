#include "Sensor.h"
#include "adc.h"
#include "delay.h"
u16 BV=0;
u16 DI=0;

void ADCVoltage()
{
		u16 v=Get_Adc_TimesTotal(ADC_Channel_4,TS);
		BV=(float)100*v/(BVR*TS);
		Delay_ms(1000);
}

void SR40_Distance()
{
	DI=Get_Cap_TimesTotal();
}

u16 Get_Cap_TimesTotal()
{
	u16 total[ALM]={0};
	u16 temp;

	for(u8 t=0;t<ALM;t++)
	{
		total[t]=Get_Cap();
	}
	for(u8 j = 0; j < ALM-1; j++)
	{
		for(u8 i = 0; i< ALM-j-1; i++)
		{
			if(total[i] > total[i+1])
			{temp=total[i];total[i]=total[i+1];total[i+1]=temp;}
		}
	}
	return total[ALM/2];
} 

u16 Get_Cap()   
{
u32 timel=0;
u16 distance=0;	
	while(!distance)//read a no zero value then continue
	{		
		/*Trig start*/
		TRIG = 1;
		Delay_us(10);
    TRIG = 0;
		TIM2CH1_CAPTURE_STA=0;//reset situation mark time			
		Delay_ms(40);//wait interrupt 65ms/times
		if(TIM2CH1_CAPTURE_STA&0X80)//got rise
		{
			timel=TIM2CH1_CAPTURE_STA&0X3F;//save times counter
			timel*=TL;					//time acculation(once updata interrupt=65536us)
			timel+=TIM2CH1_CAPTURE_VAL;		//shorter time add(1us a unit)
			distance=timel*SOV/2;		
		}
		
	}
 return distance;
}

void Test_TCRT5000(FunctionalState on_off)
{
DENG=1;
	while(on_off)
	{
		if(HW_1==1)
		{
			DENG=!DENG;
			Delay_ms(250);
		}
		else if(HW_2==1)
		{
			DENG=!DENG;
			Delay_ms(500);
		}
		else if(HW_3==1)
		{
			DENG=!DENG;
			Delay_ms(750);
		}
		else if(HW_4==1)
		{
			DENG=!DENG;
			Delay_ms(1000);
		}
	}

}

void Test_ADCVoltage(FunctionalState on_off)
{
	while(on_off)
	{
		u16 v=Get_Adc_TimesTotal(ADC_Channel_4,TS);
		float V=(float)v/(BVR*TS);
		printf("battry voltage is:%.2f\n",V);
		DENG=!DENG;
		Delay_ms(1000);
	}
}

void Test_SR40(FunctionalState on_off)
{
	u32 timel=0;
	while(on_off)
	{
		/*Trig start*/
		Delay_ms(500);
		TRIG = 1;
		Delay_us(10);
    TRIG = 0;
		DENG =!DENG;
		if(TIM2CH1_CAPTURE_STA&0X80)//got rise
		{
			timel=TIM2CH1_CAPTURE_STA&0X3F;//save times counter
			timel*=TL;					//time acculation(once updata interrupt=65536us)
			timel+=TIM2CH1_CAPTURE_VAL;		//shorter time add(1us a unit)
			printf("HIGH:%d us\r\n",timel);	//printf high level time
			float distance=timel*SOV/2;
			printf("DISTANCE:%.2f mm\r\n",distance);
 			TIM2CH1_CAPTURE_STA=0;			//reset situation mark
 		}
	}
}