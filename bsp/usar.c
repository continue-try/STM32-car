#include "usar.h"
#include "delay.h"
/*USRA2 init for GPIO PA2,3 as TX&RX*/
void USAR2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//APB1:USART2~5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //USART2,GPIOA Clock enable
	USART_DeInit(USART2); //reset USART2

	/*GPIO Setting*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //ISART2_TX PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //both side send together
	GPIO_Init(GPIOA, &GPIO_InitStructure); //init GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART1_RX PA.3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //both side recive together
	GPIO_Init(GPIOA, &GPIO_InitStructure); //init GPIOA.3
	/*USART Setting*/
	USART_InitStructure.USART_BaudRate = bound; //bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //woed 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //one stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //no ckeck bit
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//send and receive mode
	USART_Init(USART2, &USART_InitStructure); //init USART2
	/*if Receive then Interruption Setting*/
	#if EN_USART2_RX 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//USART Interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //The last Priority in NVIC group2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ channel Enable
	NVIC_Init(&NVIC_InitStructure); //init NVIC
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //open USART interruption 
	#endif
	/*Enable USRAT2*/
	USART_Cmd(USART2, ENABLE); 
}
/*USRA3 init for GPIO PB10,11 as TX&RX*/
void USAR3_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //USART3,GPIOB Clock enable
	USART_DeInit(USART3); //reset USART3

	/*GPIO Setting*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //ISART3_TX PB.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //both side send together
	GPIO_Init(GPIOB, &GPIO_InitStructure); //init GPIOB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART1_RX PB.11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //both side recive together
	GPIO_Init(GPIOB, &GPIO_InitStructure); //init GPIOB.11
	/*USART Setting*/
	USART_InitStructure.USART_BaudRate = bound; //bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //woed 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //one stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //no ckeck bit
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//send and receive mode
	USART_Init(USART3, &USART_InitStructure); //init USART3
	/*if Recive then Interruption Setting*/
	#if EN_USART3_RX 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//USART Interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //The last Priority in NVIC group2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ channel Enable
	NVIC_Init(&NVIC_InitStructure); //init NVIC
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //open USART interruption 
	#endif
	/*Enable USRAT3*/
	USART_Cmd(USART3, ENABLE); 
}
/*--------------------Car&Phone Send&Receive----------------------*/
u8 SE=0;
void PhoneMessage_Rs()
{ 
	u8 len;
	if(USART_RX_STA&0x8000)//receive from phone and not prehandle
		{
			len=USART_RX_STA&0x3fff;
			if(len<2)
			{SE=0x80;}
			if(USART_RX_BUF[len-2]==0xcf)//command the car
			{
				if(USART_RX_BUF[len-1]&0x10)//direction control,stop mode command
				{
					RC&=0xF8;//unable mode
					RC+=(USART_RX_BUF[len-1]&0x03);
				}
				else if(USART_RX_BUF[len-1]&0x20)//mode enable and change command
				{
					RC|=0x04;//mode enable
					RC&=0xFC;
					RC+=(USART_RX_BUF[len-1]&0x03);//enter new mode value
				}
				else if(USART_RX_BUF[len-1]&0x40)//angle enable and change command
				{
					if((RC&0x08)==0)
					{
						RC&=0x0F;//last speed value need clear
						RC|=0x08;
					}
					if((USART_RX_BUF[len-1]&0x0F)>0x09)
					{
						RC&=0x0F;
						SE=0x80;//error send
					}
					else
					{
						RC+=((USART_RX_BUF[len-1]&0x0F)<<4);//enter new angle value
					}
				}
 				else if(!(USART_RX_BUF[len-1]&0xF0))//speed command
				{
					if(RC&0x08)//last angle set need clear
					{
						RC&=0x07;
					}
					if(USART_RX_BUF[len-1]==0x0F)//0X0d CON'T RECEIVE
					{
						if(RC&0xF0)//speed >= 0%
						{
							RC-=0x10;
						}
					}
					else if(USART_RX_BUF[len-1]==0x0E)
					{
						RC+=0x10;
						if((RC&0xF0)==0xD0)//speed <= 96%
						{
							RC=0xC0;
						}
					}
					else
					{
						RC&=0x0F;
						RC+=(USART_RX_BUF[len-1]&0x0F)<<4;
					}
				}
				else
				{SE=0x80;}
			}
			else if(USART_RX_BUF[len-2]==0xff)
			{
				SE=USART_RX_BUF[len-1];
			}
			else
			{SE=0x80;}
		USART_RX_STA=0;
		
	if(SE!=0)
	{
		{
			#if EN_USART1_RX
				if(SE&0x80)
				{
				USART_SendData(USART1, 0xEC);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x20)
				{
				USART_SendData(USART1, (TIM_GetCapture1(TIM3)-9)*5);//angle
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x10)
				{
				u8 HW=0;
				HW+=HW_1;		//H11 right medium
				HW+=HW_2<<4;//H12 left medium
				HW+=HW_3<<1;//H13 right right
				HW+=HW_4<<5;//H14 left left
				USART_SendData(USART1, HW);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x08)
				{
				USART_SendData(USART1, DI>>8);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				USART_SendData(USART1, DI);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x04)
				{
				USART_SendData(USART1, BV>>8);//high 8 bit
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				USART_SendData(USART1, BV);//low 8 bit
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x02)
				{
				u8 dir=0;
				dir+=PBout(3)<<4;//left go
				dir+=PAout(12);//right go
				USART_SendData(USART1, dir);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
				if(SE&0x01)
				{
				USART_SendData(USART1, TIM_GetCapture4(TIM1)/40);//left speed
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				USART_SendData(USART1, TIM_GetCapture1(TIM1)/40);//right speed
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
				}
			#endif
			#if EN_USART2_RX
				USART_SendData(USART2, SE);
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			#endif
			#if EN_USART3_RX
				USART_SendData(USART3, SE);
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			#endif
				SE=0;
			}
		}
	}
}
/*--------------------Interrupt Handler--------------------------*/
/*interrupt handler function for USART1*/
#if EN_USART1_RX //if Receive then  	
u8 USART1_RX_BUF[USART_REC_LEN]; //Receive Buff,USART_REC_LEN most
u16 USART1_RX_STA=0; //Receive situation mark:initvalue 0
																					/*bit15,		Receive finsih mark
																						bit14,		Receive 0x0d mark
																						bit13~0,	The number of access halfword*/
void USART1_IRQHandler(void) 
{
u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//RXNE interrupt reach 
	{
	Res =USART_ReceiveData(USART1); 
		if((USART1_RX_STA&0x8000)==0)//bit15:Receive unfinish
		{
			if(USART1_RX_STA&0x4000)//bit14:received 0x0d(first stop bit of receive)
			{
				if(Res!=0x0a)USART1_RX_STA=0;//reset as second stop bit miss
				else USART1_RX_STA|=0x8000;//write down bit15 as all stop bit get
			}
			else //bit14:not received 0x0d
			{
				if(Res==0x0d)USART1_RX_STA|=0x4000;//write down bit14 when Receiving 0x0d 
				else//normal receive data
				{
				USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//save bit to receive buff
					USART1_RX_STA++;//bit13~0:add once to save next bit
					if(USART1_RX_STA>(USART_REC_LEN-1))USART1_RX_STA=0;//reset when num of bit longer than init leght
				}
			}
		}
	}
	if(USART_RX_STA&0x8000)
	{
		if(RC&0x04)//stop  need interrupt reaction while mode enable
		{
				RC=0x0c;//stop command to out mode and no unexpect action
		}
	}
}
#endif
/*interrupt handler function for USART2*/
#if EN_USART2_RX //if Receive then  	
u8 USART2_RX_BUF[USART_REC_LEN]; //Receive Buff,USART_REC_LEN most
u16 USART2_RX_STA=0; //Receive situation mark	
																					/*bit15,		Receive finsih mark
																						bit14,		Receive 0x0d mark
																						bit13~0,	The number of access halfword*/
void USART2_IRQHandler(void) 
{
u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
	Res =USART_ReceiveData(USART2); 
		if((USART2_RX_STA&0x8000)==0)
		{
			if(USART2_RX_STA&0x4000)
			{
				if(Res!=0x0a)USART2_RX_STA=0;
				else USART2_RX_STA|=0x8000;
			}
			else 
			{
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
				USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
				USART2_RX_STA++;
					if(USART2_RX_STA>(USART_REC_LEN-1))USART2_RX_STA=0;
				}
			}
		}
	}
}
#endif
/*interrupt handler function for USART3*/
#if EN_USART3_RX //if Receive then  	
u8 USART3_RX_BUF[USART_REC_LEN]; //Receive Buff,USART_REC_LEN most
u16 USART3_RX_STA=0; //Receive situation mark	
																					/*bit15,		Receive finsih mark
																						bit14,		Receive 0x0d mark
																						bit13~0,	The number of access halfword*/
void USART3_IRQHandler(void) 
{
u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
	{
	Res =USART_ReceiveData(USART3); 
		if((USART3_RX_STA&0x8000)==0)
		{
			if(USART3_RX_STA&0x4000)
			{
				if(Res!=0x0a)USART3_RX_STA=0;
				else USART3_RX_STA|=0x8000;
			}
			else 
			{
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
				{
				USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
				USART3_RX_STA++;
					if(USART3_RX_STA>(USART_REC_LEN-1))USART3_RX_STA=0;
				}
			}
		}
	}
}
#endif
/*--------------------Retaget Printf&Scan--------------------------*/
int fputc(int ch, FILE *f)//only one USART could use
{
/*Send a word from USART to computer*/
#if EN_USART1_RX
USART_SendData(USART1, (uint8_t) ch);
	/*wait send mark register to be 0*/
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
#endif
#if EN_USART2_RX
USART_SendData(USART2, (uint8_t) ch);
	/*wait send mark register to be 0*/
	while (USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
#endif
#if EN_USART3_RX
USART_SendData(USART3, (uint8_t) ch);
	/*wait send mark register to be 0*/
	while (USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
#endif

return (ch);
}

int fgetc(FILE *f)//only one USART could use
{ 
/*wait USART input interrupt finish*/
#if EN_USART1_RX
	while (USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
return 
(int)USART_ReceiveData(USART1);
#endif
#if EN_USART2_RX
	while (USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == RESET);
return 
(int)USART_ReceiveData(USART2);
#endif
#if EN_USART3_RX
	while (USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == RESET);
return 
(int)USART_ReceiveData(USART3);
#endif
}

void Test_USART_Rs(FunctionalState on_off)
{
	u8 t;
	u8 len;
	u16 times=0;
	while(on_off)
	{
	if(USART_RX_STA&0x8000)
		{
		len=USART_RX_STA&0x3fff;
		printf("\r\n The message you sent is:\r\n");
			for(t=0;t<len;t++)
			{
			#if EN_USART1_RX
				USART_SendData(USART1, USART_RX_BUF[t]);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
			#endif
			#if EN_USART2_RX
				USART_SendData(USART2, USART_RX_BUF[t]);
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			#endif
			#if EN_USART3_RX
				USART_SendData(USART3, USART_RX_BUF[t]);
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
			#endif
			}
		printf("\r\n\r\n");
		USART_RX_STA=0;
		}else
		{
		times++;
			if(times%600==0)
			{
				printf("\r\nUSART TEST\r\n");
				printf("QingDao Technology and Scinece University-Automation College@QUST\r\n\r\n\r\n");

			}
			if(times%300==0)printf("Please enter word,Enter to end.\r\n");
			if(times%30==0)DENG=!DENG;
		Delay_ms(10);
		}
	}
}

void Test_USART_UTF8()
{
	u8 UTF_Chinese[] = {0xE5,0xBF,0x83};
	for(int i = 0; i < 3; i++)
	{
		USART_SendData(USART1, UTF_Chinese[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
	
}