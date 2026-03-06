 #include "sys.h"


#ifndef __KEY_H 
#define __KEY_H

#define KEY1_GPIO_PORT GPIOA //key1 GPIO seriers
#define KEY1_GPIO_CLK RCC_APB2Periph_GPIOA //key1 clock
#define KEY1_GPIO_PIN GPIO_Pin_10 //key1 PA10

#define KEY2_GPIO_PORT GPIOA  
#define KEY2_GPIO_CLK RCC_APB2Periph_GPIOA 
#define KEY2_GPIO_PIN GPIO_Pin_9 //key2 PA9

#define KEY3_GPIO_PORT GPIOA 
#define KEY3_GPIO_CLK RCC_APB2Periph_GPIOA  
#define KEY3_GPIO_PIN GPIO_Pin_5  //key3 PA5

void KEY_Init(void);

#endif