#include "key.h"

/*key hardward initation*/
void KEY_Init(void) //IO init
{ 
 GPIO_InitTypeDef GPIO_InitStructure; 
 
 RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK 
 | KEY2_GPIO_CLK 
 | KEY3_GPIO_CLK ,ENABLE); //define name easy to change,read,remember
 
GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN; 
GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure); 
}