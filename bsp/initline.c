#include "initline.h"

void TB6612_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//PB4,3&PA15	remap to GPIO not JTAG
	/*Configure PB4,3 for AIN1,2*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*Configure PA15,12 for BIN1,2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void TB6612_PWM_Init(u16 arr,u16 psc)
{
	/*init stuction*/
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//base setting for TIM
	TIM_OCInitTypeDef TIM_OCInitStructure;//
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//Enable GPIOA for PWM output
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //Enable TIM1 for PWMA,B both TIM1

	/*Configure PA11,8 for PWMA,B*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//not from output register
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*TIM1 Configure*/
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructure.TIM_Period = arr-1;//how many times counted after to update(max counter value)  
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;//TIMxCLK fre div:fixed T=arr*psc/TIMxCLK-fre
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM up-counter
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/*TIM1 Channe4 PWMA,TIM1 Channe1 PWMB*/
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//effective value be set when counter value less than compare value 
	TIM_OCInitStructure.TIM_Pulse=0;//init disable motor:0%
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//output channe enable
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//effective value is high
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure); //channe1 init
	TIM_OC4Init(TIM1,&TIM_OCInitStructure); //channe4 init 
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//shadow regist enable
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable); //shadow regist enable
	TIM_Cmd(TIM1,ENABLE);//Start TIM1 counter
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//Senior TIM output eable
}

void SG90_PWM_Init(u8 ang)/*pulse=125ang/180*/
{
	/*init stuction*/
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//base setting for TIM
	TIM_OCInitTypeDef TIM_OCInitStructure;//

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//Enable GPIOA for PWM output
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //Enable TIM3 for PWM(APB1 36MHz*2 as TIM3 CLK-fre)
	
	/*Configure PA6 for PWM*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//not from output register
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*TIM3 Configure for SG90 T=20ms pulse singal*/
	TIM_TimeBaseStructure.TIM_Period = 359;//how many times counted after to update(max counter value)  
	TIM_TimeBaseStructure.TIM_Prescaler = 3999;//TIMxCLK fre div:fixed T=arr*psc/TIMxCLK-fre
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM up-counter
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/*TIM3 Channe1 PWM*/
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//effective value be set when counter value less than compare value 
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//output channe enable
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//effective value is high
	TIM_OCInitStructure.TIM_Pulse=ang/5+9;//0~180 angle->duty ratio 2.5%~12.5%->Period 9~45(total 360)
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); //channe1 init

	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable); //shadow regist enable
	
	TIM_Cmd(TIM3,ENABLE);//Start TIM3 counter
}

void TCRT5000_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void BlueTeeth_USAR_Init(u32 bound)/*just USART1 init with GPIO_remap_USART1(TX:PB6,RX:PB7)*/
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOB, ENABLE); //USART1,GPIOB Clock enable
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//remap USART1 to PB6,7
	USART_DeInit(USART1); //reset USART1

	/*GPIO Setting*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //ISART1_TX PB.6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //both side send together
	GPIO_Init(GPIOB, &GPIO_InitStructure); //init GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //USART1_RX PB.7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //both side receive together
	GPIO_Init(GPIOB, &GPIO_InitStructure); //init GPIOB.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //state PB.5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //receive 1 when link access
	GPIO_Init(GPIOB, &GPIO_InitStructure); //init GPIOB.5
	/*USART Setting*/
	USART_InitStructure.USART_BaudRate = bound; //bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //woed 8 bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //one stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //no ckeck bit
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;//send and recive mode
	USART_Init(USART1, &USART_InitStructure); //init USART1
	/*if Recive then Interruption Setting*/
	#if EN_USART1_RX 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//USART Interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ; //The last Priority in NVIC group2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ channel Enable
	NVIC_Init(&NVIC_InitStructure); //init NVIC
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //open USART interruption 
	#endif
	/*Enable USRAT1*/
	USART_Cmd(USART1, ENABLE); 
}

void SR40_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//PA.0:trig output
  GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//PA.1:echo input capture channel
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,0);
}

void SR40_CAP_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//timebase
	TIM_ICInitTypeDef TIM_ICInitStructure;//input capture 
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	/*Init TIM2 TimeBase*/
	TIM_TimeBaseStructure.TIM_Period = arr-1; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  /*Init TIM2 input capture(PA.1->TIM2Channel2)*/
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//IC channel2->TIM2
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//Rising capture
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//direct TI
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//no division of fre
	TIM_ICInitStructure.TIM_ICFilter = 0x00;//no filter
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	/*NVIC Init for acculate no echo pulse time*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC2,ENABLE);//Enable TIM-updata(time add)&
																										//IC-rising(time mark) Interrupt
}

void OLED_GPIO_Init(void)
{ //I2C接口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

void MPU6050_GPIO_Init(void)
{ //I2C接口初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#if HOS
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);//remap to get I2C function to PB8~9
#endif
	
//GPIO
	GPIO_InitTypeDef GPIO_InitStructure;
#if HOS
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
#else
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
#endif
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//IIC-CLK
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//IIC-DATA
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
//I2C Init
#if HOS
	I2C_DeInit(I2C1);
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 120000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;//SCL lowlevel:highlevel=16:9 for leave more peroid to up level
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//defualt stm give response
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//stm as slave address bit num
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;//stm as slave address to be called
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1,ENABLE);
#endif
}

void Test_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}
