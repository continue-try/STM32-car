#include "sys.h"
/*----------Blocks Init Programs-----------------------------*/
void TB6612_GPIO_Init(void);					//MaDaQuDong
void TB6612_PWM_Init(u16 arr,u16 psc);//MaDaQuDong_PWM
void SG90_PWM_Init(u8 ang);						//DuoJi
void TCRT5000_GPIO_Init(void);				//HongWai
void BlueTeeth_USAR_Init(u32 bound);	//lan ya
void SR40_GPIO_Init(void);						//ChaoShengBo
void SR40_CAP_Init(u16 arr,u16 psc);	//ChaoShengBo_CAP
void OLED_GPIO_Init(void);						//OLED
void MPU6050_GPIO_Init(void);				  //6 axis ACC TuoLuoYi:0 hard,1 soft
void Test_GPIO_Init(void);						//test light(PC13) init function