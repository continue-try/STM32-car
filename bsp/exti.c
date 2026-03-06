#include "exti.h"
#include "key.h"
#include "delay.h"
/*exti & nvic init*/
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure; //Init struction
	NVIC_InitTypeDef NVIC_InitStructure;

	/*determine which GPIO Pin for outside interruption*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource10); //Select GPIOA for EXTI Line10(key 1)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource9); //Select GPIOA for EXTI Line9(key 2)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); //Select GPIOA for EXTI Line5(key 3)
	
	/*EXIT Init Struction Setting*/
	EXTI_InitStructure.EXTI_Line=EXTI_Line10|EXTI_Line9|EXTI_Line5; //setting of all channels 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//go up to reach interrupt
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure); //EXTI Init function
	
	/*NVIC Init Struction Setting*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//interrupt Priority group setting	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//key 1(PA10)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //first respond
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//key 2/3(PA9/PA5)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//second  respond
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 
}

/*interrupt handler function for key*/
void EXTI9_5_IRQHandler(void) 
{ 	
Delay_ms(20); //button shanke wait
	if(KEY2==1)
	{
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13));
	if((OC&0x08)==0)//last screen change finish avoid loss scrren
	{
		OC|=0x08;//changed mark:unchange
		OC++;
	}
	if((OC&0x07)==7){OC&=0xF8;}//screen choose circle
	}
	if(KEY3==1)
	{
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13));
	OC=0x18;//fresh screen
	RC=0x04;//end Mode->park car
	}
	EXTI_ClearITPendingBit(EXTI_Line9);//clear key2 interrupt mark
	EXTI_ClearITPendingBit(EXTI_Line5);//clear key3 interrupt mark
} 
void EXTI15_10_IRQHandler(void) 
{ 
Delay_ms(20); //button shanke wait
	if(KEY1==1)
	{
	u8 H=OC&0xF0;
	u8 L=OC&0x0F;
	H=H<<1;
	if(H==0){H|=0x10;}
	OC=H+L;//line pointer circle
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,1-GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13));
	}
	EXTI_ClearITPendingBit(EXTI_Line10); //clear key1 interrupt mark
}

/*echo time acculate interrupt handler function*/
u8  TIM2CH1_CAPTURE_STA=0x80;	/*IC situation mark:captured mark,bit15:fall,14:rise;
																								updata times counter,bit13~0*/     				
u16	TIM2CH1_CAPTURE_VAL;	  //IC time value
 
//TIM2 interrupt handle
void TIM2_IRQHandler(void)
{ 

if((TIM2CH1_CAPTURE_STA&0X80)==0)//#5:no capture,not(1,1)|(1,0)
{	  
		if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)//#1,3:capture channel.1 captured
		{	
			if(TIM2CH1_CAPTURE_STA&0X40)		//#4:last got a rising 		
			{	  			
				TIM_Cmd(TIM2,DISABLE);
				TIM2CH1_CAPTURE_STA|=0X80;		//mark got falling(1,1):mark end
				TIM2CH1_CAPTURE_VAL=TIM_GetCapture2(TIM2);//save time value
		   	TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC2P=0 reset to capture rising
			}else  								//#2:first scan,get the first rising
			{
				TIM_SetCounter(TIM2,0);
				TIM_Cmd(TIM2,ENABLE);
				TIM2CH1_CAPTURE_VAL=0;
				TIM2CH1_CAPTURE_STA|=0X40;		//mark got rising(0,1)
		   	TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Falling);		//CC2P=1 set to capture falling
			}		    
		}	
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //5#:over timer(distance >10m)
		{	    
			if(TIM2CH1_CAPTURE_STA&0X40)//got rise voltlevel so time counter continue
			{
				if((TIM2CH1_CAPTURE_STA&0X3F)==0X3F)//#Def:high voltlevel too long(bit13~0:get max 0x3F->4.19s)
				{
					TIM_Cmd(TIM2,DISABLE);
					TIM2CH1_CAPTURE_STA|=0X80;//mark end(1,1)
					TIM2CH1_CAPTURE_VAL=0XFFFF;
				}else TIM2CH1_CAPTURE_STA++;//6#:(0,1)->got rise->time counter record->wait #4
			}
			else//echo miss error clear
			{
				TIM_Cmd(TIM2,DISABLE);
				TIM2CH1_CAPTURE_STA=0X80;//mark end(1,1)
				TIM2CH1_CAPTURE_VAL=0;
				TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising); //CC2P=0 reset to capture rising
			}
		}		
 }
 
 TIM_ClearITPendingBit(TIM2, TIM_IT_CC2|TIM_IT_Update); //clear interrupt mark
 
}
