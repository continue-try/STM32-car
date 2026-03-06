#include "sys.h"
#include <stdio.h>

void USAR2_Init(u32 bound);
void USAR3_Init(u32 bound);
void PhoneMessage_Rs();
int fputc(int ch, FILE *f);//retarget printf
int fgetc(FILE *f);//retarget scanf
void Test_USART_Rs(FunctionalState);
void Test_USART_UTF8();
