#include "stm32f10x.h"
/*equation:
	AliasAddr = 
	bit_band_base(fisrt addr num+0x02000000)+(byte_offset(last 5 addr num)*2^5+bit_num*2^2)*/
						
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr&0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr)) 
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum)) 

/*input&output register address(addr) GPIOx*/
#define GPIOA_ODR_Addr (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr (GPIOC_BASE+12) //0x4001100C 
#define GPIOA_IDR_Addr (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr (GPIOC_BASE+8) //0x40011008 

/*bit_num under register address(bitnum) Pin_num*/
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr,n) //output
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr,n) //input
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr,n)
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr,n)


/*---------------const num--------------------------------*/
#define ARR	0x0FA0   //counter value:4000(100*n)
#define AR  ARR/100  //duty cycle value=speed value(0~100%)
#define BVR 0xF8		 //battery voltage transform ratio complement:248(4096/(3.3V*5),<1% error)
#define TS  10			 //the times of ADC cample to caculate average(1~16)
#define TF  72			 //the frequent division of ehco timer:1MHz->1us(TIM-fre:72MHz/Prescaler(TF):72)
#define TL  0x7FFF   //the once updata interrupt need time for ehco timer:32767(us)=TF*0x7FFF/TIM-fre
#define SOV 0.34     //sonic velocity(0.34mm/us)
#define ALM 8			   //distance cample times(anrry long)
#define HOS 0				 //MPU6050 I2C pattern:0 software I2C,1 hardware I2C
#define ACR 16			 //MPU6050 acceleration meter range:+-2g(2),+-4g(4),+-8g(8),+-16g(16)
#define GYR 2000		 //MPU6050 gyroscope meter range:+-250deg/s(250),+-500deg/s(500),+-1000deg/s(1000),+-2000deg/s(2000)
#define EN_USART1_RX  	1	//enable usar1 interrupt
#define	EN_USART2_RX 		0 //enable usar2 interrupt
#define	EN_USART3_RX 		0 //enable usar3 interrupt(1/3 could enable)
#define USART_REC_LEN  200 //receive buff char long
#define	USART_RX_STA	USART1_RX_STA//Receive situation mark of Test USART 
#define	USART_RX_BUF	USART1_RX_BUF//USART Receive buff of Test

/*--------------GPIO define-------------------------------*/
#define KEY1 PAin(10) // PA10 
#define KEY2 PAin(9)  // PA9 
#define KEY3 PAin(5)  // PA5 
#define	HW_1 PBin(15) //HongWaiDuiGuan
#define	HW_2 PBin(14)
#define	HW_3 PBin(13)
#define	HW_4 PBin(12)
#define BSTA PBin(5)  //LanYa_state
#define TRIG PAout(0) //ChaoShengBo_trigger
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOC, GPIO_Pin_14, (BitAction)(x))//OLED CLK&DATA
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOC, GPIO_Pin_15, (BitAction)(x))
#define	DENG PCout(13)//Test Light
/*--------------Variable share-----------------------------*/
extern u16 USART_RX_STA;//USART situation Mark
extern u16 BV;//battry voltage:2 decimal places of 4 DEC(10mv a unit)
extern u16 DI; //obstacle distance:mm unit
extern u8 USART_RX_BUF[USART_REC_LEN];//receive buff

extern u8 SE; 
/*car to phone message:bit7:illegal ask,bit6:,bit5:angle,
bit4:on off HW_1~4,bit3:distance,bit2:bvoltage,bit1:direction,bit0:speed*/
extern u8 OC; //Oled Change:bit7~4:line pointer,bit3:screen changed mark,bit2~0:screen mark
extern u8 RC; 
/*Run Command:bit7~4:Speed or Angle value,
bit3:Angle or Speed enter mark,bit2:Mode enable,bit1~0:Mode or Dirction*/