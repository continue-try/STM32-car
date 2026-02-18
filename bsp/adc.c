#include "adc.h"
#include "delay.h"
u16 Get_Adc(u8 ch)   
{
  //Set the ADC rule transform channe,a sequence,cample time
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC channe,sample cycle 239.5T	  			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//Enable ADC1 softward transform start
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//wait for end of transform
	return ADC_GetConversionValue(ADC1);	//return last ADC1 transform result
}

u16 Get_Adc_TimesTotal(u8 ch,u8 times)
{
	u16 total=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		total+=Get_Adc(ch);
		Delay_ms(5);
	}
	return total;
} 
void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1, ENABLE );	  //Enable ADC1 clock
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //set ADC division:6 72M/6=12M;ADC max frequent is 14M

	//PA4 as analog input pin                          
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//analog input pin
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	ADC_DeInit(ADC1);  //Reset ADC1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC mode:ADC1&ADC2 work in independent mode
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//Digtal to analog work in sigle channe mode
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//Digtal to analog work in first transform mode
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//transform start by softward not interruption
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC digtal align with right
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//The number of rule transform ADC channe
	ADC_Init(ADC1, &ADC_InitStructure);	// ADC_InitStruct to init ADCx   

  
	ADC_Cmd(ADC1, ENABLE);	//Enable ADC1
	ADC_ResetCalibration(ADC1);	//Enable reset-align
	while(ADC_GetResetCalibrationStatus(ADC1));	//wait for restet end
	ADC_StartCalibration(ADC1);	 //Enable AD align
	while(ADC_GetCalibrationStatus(ADC1));	 //wait for align end
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//?????ADC1?????????
}