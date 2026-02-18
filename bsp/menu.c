#include "menu.h"
#include "OLED.h" 
#include "key.h"

u8 OC=0x10;//bit7~4:line pointer,bit3:screen changed mark,bit2~0:screen mark
void Menu()
{
	if(OC&0x08)//sceern unchange
	{
		Skip();
		OLED_Clear();
		OC&=0xF7;//clear sceern over
	}
	
	if((OC&0x0F)==0)//screen 0
	{
	OLED_ShowChinese(1,6,0);
	OLED_ShowChinese(1,8,1);
	OLED_ShowChinese(3,2,2);
	OLED_ShowChinese(3,4,3);
	OLED_ShowChinese(3,6,4);
	OLED_ShowChinese(3,8,5);
	OLED_ShowChinese(5,2,6);
	OLED_ShowChinese(5,4,7);
	OLED_ShowChinese(5,6,8);
	OLED_ShowChinese(5,8,9);
	OLED_ShowChinese(7,2,10);
	OLED_ShowChinese(7,4,11);
	OLED_ShowChinese(7,6,12);
	OLED_ShowChinese(7,8,13);
	/*DianChiDianYaXianShi*/
		OLED_ShowRPNum(3,10,BV,4,2,FONTSIZE_8X16);
		OLED_ShowChar(6,20,'V',FONTSIZE_6X8);		
	if(BSTA==1)//LanYaLianJieXianShi
	{
		OLED_ShowChar(2,10,'"',FONTSIZE_8X16);
	}
	else
	{
		OLED_ShowChar(2,10,'_',FONTSIZE_8X16);
	}
	/*Mode show*/
	if(RC&0x04)
	{
		OLED_ShowChar(4,10,':',FONTSIZE_8X16);
		if((RC&0x03)==0)
		{
		OLED_ShowChar(8,15,'S',FONTSIZE_6X8);
		OLED_ShowChar(8,16,'T',FONTSIZE_6X8);
		OLED_ShowChar(8,17,'O',FONTSIZE_6X8);
		OLED_ShowChar(8,18,'P',FONTSIZE_6X8);
		}
		if((RC&0x03)==1)
		{
		OLED_ShowChinese(7,11,24);
		OLED_ShowChinese(7,13,25);
		}
		if((RC&0x03)==2)
		{
		OLED_ShowChinese(7,11,20);
		OLED_ShowChinese(7,13,21);
		}
		if((RC&0x03)==3)
		{
		OLED_ShowChinese(7,11,22);
		OLED_ShowChinese(7,13,23);
		}
	}
	else
	{
		OLED_ShowChar(4,10,' ',FONTSIZE_8X16);
		OLED_ShowChar(4,11,' ',FONTSIZE_8X16);
		OLED_ShowChar(4,12,' ',FONTSIZE_8X16);
		OLED_ShowChar(4,13,' ',FONTSIZE_8X16);
		OLED_ShowChar(4,14,' ',FONTSIZE_8X16);
	}
	}
	/*Mode enable screen*/
	if((OC&0x0F)==1)//OC:xxxxx001 enter screen1
	{
	OLED_ShowChinese(1,2,20);
	OLED_ShowChinese(1,4,21);
	OLED_ShowChinese(3,2,22);
	OLED_ShowChinese(3,4,23);
	OLED_ShowChinese(5,2,24);
	OLED_ShowChinese(5,4,25);
	OLED_ShowChinese(7,2,26);
	OLED_ShowChinese(7,4,27);
		if((RC&0x07)==5)
		{
		OLED_ShowChar(6,8,'#',FONTSIZE_6X8);
		}
		else
		{
		OLED_ShowChar(6,8,'_',FONTSIZE_6X8);
		}
		if((RC&0x07)==6)
		{
		OLED_ShowChar(2,8,'#',FONTSIZE_6X8);
		}
		else
		{
		OLED_ShowChar(2,8,'_',FONTSIZE_6X8);
		}
		if((RC&0x07)==7)
		{
		OLED_ShowChar(4,8,'#',FONTSIZE_6X8);
		}
		else
		{
		OLED_ShowChar(4,8,'_',FONTSIZE_6X8);
		}
	}
	/*setting and monitor screen*/
	if((OC&0x0F)==2)//OC:xxxxx010 enter screen2
	{
	RC=0x04;//this screen can't receive command
	OLED_ShowChinese(1,6,18);
	OLED_ShowChinese(1,8,19);
	OLED_ShowChinese(3,2,28);
	OLED_ShowChinese(3,4,30);
	OLED_ShowChinese(3,6,31);
	OLED_ShowChinese(3,8,32);
	OLED_ShowChinese(5,2,48);
	OLED_ShowChinese(5,4,49);
	OLED_ShowChinese(5,6,50);
	OLED_ShowChinese(5,8,51);
	OLED_ShowChinese(7,11,66);
	OLED_ShowChinese(7,13,67);
	OLED_ShowChinese(7,15,68);
	}
	/*setting screen*/
	if((OC&0x0F)==3)//OC:xxxxx011 enter screen3
	{
	RC&=0xFB;//this screen can't enable mode
	OLED_ShowChinese(1,2,37);
	OLED_ShowChinese(1,4,38);
	OLED_ShowChinese(1,6,39);
	OLED_ShowChinese(1,8,40);
	OLED_ShowChinese(5,2,33);
	OLED_ShowChinese(5,4,34);
	OLED_ShowChinese(5,6,35);
	OLED_ShowChinese(5,8,36);
	OLED_ShowChinese(3,2,29);
	OLED_ShowChinese(3,4,30);
	OLED_ShowChinese(3,6,52);
	OLED_ShowChinese(3,8,53);
	OLED_ShowChinese(7,2,26);
	OLED_ShowChinese(7,4,27);
		
		OLED_ShowCNum(7,14,TIM_GetCapture4(TIM1)/40,3,FONTSIZE_6X8);//speed show
		OLED_ShowCNum(7,18,TIM_GetCapture1(TIM1)/40,3,FONTSIZE_6X8);
		OLED_ShowChinese(5,12,41);
		OLED_ShowChinese(5,15,42);    
		OLED_ShowChar(7,17,'%',FONTSIZE_6X8);
		OLED_ShowChar(7,21,'%',FONTSIZE_6X8);
		
		OLED_ShowCNum(2,11,(TIM_GetCapture1(TIM3)-9)*5,3,FONTSIZE_8X16);//angle show
		OLED_ShowChar(4,19,'d',FONTSIZE_6X8);
		OLED_ShowChar(4,20,'e',FONTSIZE_6X8);
		OLED_ShowChar(4,21,'g',FONTSIZE_6X8);
		
		OLED_ShowChar(1,10,':',FONTSIZE_8X16);//dirction show
		OLED_ShowChar(2,10,':',FONTSIZE_8X16);
		OLED_ShowChar(3,10,':',FONTSIZE_8X16);
		switch(RC&0x03)
		{
			
			
			case 1:
			{
				OLED_ShowChinese(1,11,41);
				OLED_ShowChinese(1,13,43);
			}break;
			case 2:
			{
				OLED_ShowChinese(1,11,42);
				OLED_ShowChinese(1,13,43);
			}break;
			case 3:
			{
				OLED_ShowChinese(1,11,44);
				OLED_ShowChinese(1,13,45);
			}break;
			case 0:
			{
				OLED_ShowChinese(1,11,46);
				OLED_ShowChinese(1,13,47);
			}break;
		}
	}
	/*monitor screen*/
	if((OC&0x0F)==4)//OC:xxxxx100 enter screen4
	{
	RC&=0xFB;//this screen can't enable mode
	OLED_ShowChinese(1,2,54);
	OLED_ShowChinese(1,4,55);
	OLED_ShowChinese(1,6,56);
	OLED_ShowChinese(1,8,57);
	OLED_ShowChar(1,10,':',FONTSIZE_8X16);
	OLED_ShowChinese(3,2,58);
	OLED_ShowChinese(3,4,59);
	OLED_ShowChinese(3,6,60);
	OLED_ShowChinese(3,8,61);
	OLED_ShowChar(2,10,':',FONTSIZE_8X16);
	OLED_ShowChinese(5,2,62);
	OLED_ShowChinese(5,4,63);
	OLED_ShowChinese(5,6,64);
	OLED_ShowChinese(5,8,65);
	OLED_ShowChar(3,10,':',FONTSIZE_8X16);
	OLED_ShowChinese(7,2,26);
	OLED_ShowChinese(7,4,27);
	//HongWaiDuiGuan
		if(HW_4)
		{
		OLED_ShowChar(1,11,'1',FONTSIZE_8X16);
		}
		else
		{
		OLED_ShowChar(1,11,'0',FONTSIZE_8X16);
		}
		if(HW_2)
		{
		OLED_ShowChar(1,12,'1',FONTSIZE_8X16);
		}
		else
		{
		OLED_ShowChar(1,12,'0',FONTSIZE_8X16);
		}
		if(HW_1)
		{
		OLED_ShowChar(1,13,'1',FONTSIZE_8X16);
		}
		else
		{
		OLED_ShowChar(1,13,'0',FONTSIZE_8X16);
		}
		if(HW_3)
		{
		OLED_ShowChar(1,14,'1',FONTSIZE_8X16);
		}
		else
		{
		OLED_ShowChar(1,14,'0',FONTSIZE_8X16);
		}
	//ChaoShengCeJu
		OLED_ShowRPNum(4,15,DI,4,1,FONTSIZE_6X8);
		OLED_ShowChar(4,20,'c',FONTSIZE_6X8);
		OLED_ShowChar(4,21,'m',FONTSIZE_6X8);
	//ChuangKou
		if(USART_RX_STA&0x8000)
		{
		u8 len=USART_RX_STA&0x3fff;
		OLED_ShowHexNum(3,11,USART_RX_BUF[len-2],2,FONTSIZE_8X16);
		OLED_ShowHexNum(3,13,USART_RX_BUF[len-1],2,FONTSIZE_8X16);
		}
	}
	/*gyro&accl screen*/
	if((OC&0x0F)==5)//OC:xxxxx101 enter screen5
	{
		RC&=0xFB;//this screen can't enable mode
		OLED_ShowChinese(7,13,26);
		OLED_ShowChinese(7,15,27);
	}
	
	if(OC&0x10)//OC:0001xxxx point first line
	{
		OLED_ShowChar(2,1,'*',FONTSIZE_6X8);
		OLED_ShowChar(4,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(6,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(8,1,' ',FONTSIZE_6X8);
	}	
	if(OC&0x20)//OC:0010xxxx point second line
	{ OLED_ShowChar(2,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(4,1,'*',FONTSIZE_6X8);
		OLED_ShowChar(6,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(8,1,' ',FONTSIZE_6X8);
	}	
	if(OC&0x40)//OC:0100xxxx point third line
	{ OLED_ShowChar(2,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(4,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(6,1,'*',FONTSIZE_6X8);
		OLED_ShowChar(8,1,' ',FONTSIZE_6X8);
	}	
	if(OC&0x80)//OC;1000xxxx point forth line
	{ OLED_ShowChar(2,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(4,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(6,1,' ',FONTSIZE_6X8);
		OLED_ShowChar(8,1,'*',FONTSIZE_6X8);
	}
	
}

void Skip()
{
	if((OC&0x07)==1)//at screen0 to change:xxxxx000->xxxxx001
	{	
		if(OC&0x80)//at screen0point4 to change:1000x0000->1000x0001
		{
			OC&=0xF8;
			OC+=1;//target screen
		}
		else if(OC&0x10)//at screen0point1 to change:0001x0000->0001x0010
		{
			OC&=0xF8;
			OC+=2;//target screen
		}
		else OC--;
	}
	else if((OC&0x07)==2)//at screen1
	{
		if(OC&0x80)//at screen1point4 to change:1000x0000->1000x0001
		{
			OC&=0xF8;
			OC+=0;//target screen
		}
		if(OC&0x40)//at screen1point3
		{
			RC&=0xF8;
			RC|=0x04;
			RC+=1;//Mode1
			OC--;//screen remain
		}
		if(OC&0x20)//at screen1point2
		{
			RC&=0xF8;
			RC|=0x04;
			RC+=3;//Mode3
			OC--;//screen remain
		}
		if(OC&0x10)//at screen1point1
		{
			RC&=0xF8;
			RC|=0x04;
			RC+=2;//Mode2
			OC--;//screen remain
		}
	}
	else if((OC&0x07)==3)//at screen2
	{
		if(OC&0x10)//at screen2point1 to change:0001x0010->0001x0001
		{
			OC&=0xF8;
			OC+=0;//target screen
		}
		else if(OC&0x20)
		{
			OC&=0x08;
			OC+=0x13;//target screen and pointer
		}
		else if(OC&0x40)
		{
			OC&=0x08;
			OC+=0x14;//target screen
		}
		else if(OC&0x80)
		{
			OC&=0x08;
			OC+=0x85;//target screen
		}
		else
		{
			OC--;
		}
	}
	
	else if((OC&0x07)==4)//at screen3
	{
		if(OC&0x10)//dirction setting
		{
			RC+=1;
			OC--;
		}
		else if(OC&0x40)//speed setting
		{
			if(RC&0x08)//last angle set need clear
			{
				RC&=0x07;
			}
			RC+=0x10;
			
			if((RC&0xF0)==0xD0)
			{
			RC&=0x0F;
			}
			OC--;
		}
		else if(OC&0x20)//angle setting
		{
			if((RC&0x08)==0)
			{
				RC&=0x0F;//last speed value need clear
				RC|=0x08;
			}
			RC+=0x10;
			if((RC&0xF0)==0xA0)
			{
			RC&=0x0F;
			}
			OC--;
		}
		else
		{
			OC&=0x08;
			OC+=0x22;//target screen
		}
	}
	
	else if((OC&0x07)==5)//at screen4
	{
		if(OC&0x80)
		{
			OC&=0x08;
			OC+=0x42;//target screen
		}
	else
		{
			OC--;
		}
	}
	else if((OC&0x07)==6)
	{
		if(OC&0x80)
		{
			OC&=0x08;
			OC+=0x82;//target screen
		}
	else
		{
			OC--;
		}
	}
}
