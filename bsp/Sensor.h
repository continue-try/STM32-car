#include "sys.h"
#include <stdio.h>
/*----------Variable Tab-----------*/
extern u8  TIM2CH1_CAPTURE_STA;	/*IC situation mark:captured mark,bit15:fall,14:rise;*/
extern u16	TIM2CH1_CAPTURE_VAL;									/*updata times counter,bit13~0*/
/*----------Sensors function-----------*/
void ADCVoltage();
void SR40_Distance();
u16	Get_Cap();
u16  Get_Cap_TimesTotal();
/*----------Test function-----------*/
void Test_TCRT5000(FunctionalState);
void Test_ADCVoltage(FunctionalState);
void Test_SR40(FunctionalState);