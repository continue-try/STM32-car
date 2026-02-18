#include "sys.h"
#include "delay.h"
#include "OLED.h" 
#include "OLED_Font.h"
#include <string.h>
#include <math.h>

/**
  * 数据存储格式：
  * 纵向8点，高位在下，先从左到右，再从上到下
  * 每一个Bit对应一个像素点
  * 
  *      B0 B0                  B0 B0
  *      B1 B1                  B1 B1
  *      B2 B2                  B2 B2
  *      B3 B3  ------------->  B3 B3 --
  *      B4 B4                  B4 B4  |
  *      B5 B5                  B5 B5  |
  *      B6 B6                  B6 B6  |
  *      B7 B7                  B7 B7  |
  *                                    |
  *  -----------------------------------
  *  |   
  *  |   B0 B0                  B0 B0
  *  |   B1 B1                  B1 B1
  *  |   B2 B2                  B2 B2
  *  --> B3 B3  ------------->  B3 B3
  *      B4 B4                  B4 B4
  *      B5 B5                  B5 B5
  *      B6 B6                  B6 B6
  *      B7 B7                  B7 B7
  * 
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0         X轴(八格)           127 
  *    0 .-------------------------------->
  *      |___|___|___|___|___|___|___|___|
  *      |	 |q w|	 |   |	 |	 |	 |   |
  *      |___|a_s|___|___|___|___|___|___|
  *  Y轴 |	 |	 |	 |	 |	 |	 |	 |   |
  *(四格)|___|___|___|___|___|___|___|___|
  *      |	 |	 |	 |   |	 |	 |	 |   |
  *   63 |___|___|___|___|___|___|___|___|
  *      |					16x16字体网格
  *      v(8x8字体对齐：q左上,w右上,a左下,s右下)
  * 
  */
/************OLED通信协议和命令函数***************/
/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
	* @brief  OLED多次写不同数据
  * @param  *Data要写入的数组首地址
  * @param  num  要写入的数组长度
  * @param  start要写入的开始索引
	* @param  ifsame,1:重复写入Data;2:Data[i]依次写入
  * @retval 无
  */
void OLED_WriteDataS(const uint8_t *Data,uint16_t num, uint16_t start, FunctionalState ifsame)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	if(ifsame){
		uint8_t DataT = (uint8_t)Data;
		for(uint16_t i = 0;i < num; i++)
		{
			OLED_I2C_SendByte(DataT);
		}
	}
	else
	{
		for(uint16_t i = start;i < start + num; i++)
		{
			OLED_I2C_SendByte(Data[i]);
		}
	}
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Page 以左上角为原点，向下方向的坐标（GDDROM页码），范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Page, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Page);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		OLED_WriteDataS(0x00, 128, 0,ENABLE);
	}
}
/**
	* @brief  OLED区域清屏
  * @param  x 以左上角为原点，向右方向的坐标，范围：0~127
  * @param  y 以左上角为原点，向下方向的坐标，范围：0~63
	* @param  width 要清空区域宽度，范围：1~128
	* @param  height要清空区域高度，范围：1~64
  */
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{  
	uint8_t i, j;
	for (j = y / 8; j <= (y + height - 1) / 8; j++)
	{
		OLED_SetCursor(j, x);
		OLED_WriteDataS(0x00, width, x,ENABLE);
	}
}
/*********************屏幕缓存区与更新函数**************************/
uint8_t ImageTransBuff[8][128] = {0};
/**
	* @brief  OLED缓存区清零
  * @param  x 以左上角为原点，向右方向的坐标，范围：0~127
  * @param  y 以左上角为原点，向下方向的坐标，范围：0~63
	* @param  width 要清空区域宽度，范围：1~128
	* @param  height要清空区域高度，范围：1~64
  */
void OLED_BuffClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t PageStart = y / 8;									//始页数（0~7）
	uint8_t Pagend = (y + height - 1) / 8;			//终页数（0~7）
	uint8_t Yshift = y % 8;											//上部残卷空白量（填满0~7）
	uint8_t Endfull = (y + height - 1) % 8 + 1;	//下部残卷填充量（1~8填满）
	if(Yshift)//1：上部未填满；0上部填满不为残卷
	{
		for(uint8_t i = Yshift; i < 8; i++)
		{
			for(uint8_t j = x; j < x + width; j++)
			{
				ImageTransBuff[PageStart][j] &= ~(0x01 << i);
			}
		}
		PageStart ++;
	}
	if(Endfull != 8)//下部残卷
	{
		for(uint8_t i = 0; i < Endfull; i++)
		for(uint8_t j = x; j < x + width; j++)
		{
			ImageTransBuff[Pagend][j] &= ~(0x01 << i);
		}
		Pagend --;
	}
	for(uint8_t i = PageStart; i <= Pagend; i++)//中间整卷
	{
		for(uint8_t j = x; j < x + width; j++)
		{
			ImageTransBuff[i][j] = 0;
		}
	}
}
void OLED_BuffClearAll(void)
{
	OLED_BuffClear(0, 0, 128, 64);
}
/**
	* @brief  OLED缓存区上屏
  * @param  x 以左上角为原点，向右方向的坐标，范围：0~127
  * @param  y 以左上角为原点，向下方向的坐标，范围：0~63
	* @param  w 要上屏区域宽度，范围：1~128
	* @param  h 要上屏区域高度，范围：1~64
  */
void OLED_BuffUpdata(uint8_t x, uint8_t y, uint8_t width, uint8_t height, FunctionalState ifclear)
{
	uint8_t PageStart = y / 8;							//始页数（0~7）
	uint8_t Pagend = (y + height - 1) / 8;	//终页数（0~7）
	for(uint8_t i = PageStart; i <= Pagend; i++)
	{
		OLED_SetCursor(i, x);
		OLED_WriteDataS(ImageTransBuff[i], width, x, DISABLE);
	}
	if(ifclear)
	{
		OLED_BuffClear(x,y,width,height);//下皮，不留影
	}
}
/**
  * @brief  OLED显示一张图片
  * @param  Xpos 以左上角为原点，向右方向的坐标，范围：0~127
  * @param  Ypos 以左上角为原点，向下方向的坐标，范围：0~63
  * @param  Width 要显示图片像素宽度，范围：1~128
  * @param  Height要显示图片像素高度，范围：1~64
	* @param  ifmove显示图片的模式：(1运动模式默认覆盖显示模式)
		1运动模式：图像上屏后Buff中清除该图像，屏幕清空要上屏位置后加空框图像再上屏；0静止模式（叠加显示）；
  * @param  Image	要显示图片地址指针
	* @retval ErrorStatus 图像越界反馈：ERROR越界:0；SUCCESS未越界:1
  */
ErrorStatus OLED_ShowImg(uint8_t Xpos, uint8_t Ypos, uint8_t Width, uint8_t Height, FunctionalState ifmove, const uint8_t *Image)
{
	if(Xpos + Width <= 128 && Ypos + Height <= 64)//检查越界
	{
		uint8_t Pagend = (Height - 1) / 8;//占页数 - 1
		uint8_t PageStart = Ypos / 8;			//始页数（0~7）
		uint8_t Yshift = Ypos % 8;				//页内偏移量
		if(Yshift)//相对Page横栏偏移：否，一般处理；是，切割重组
			{	
//				if(ifmove){OLED_ClearArea(Xpos, Ypos, Width, Height);}//
				for(uint8_t i = 0; i <= Pagend; i++)//重组8位数据
				{
					for(uint8_t j = 0; j < Width; j++)
					{
						ImageTransBuff[PageStart + i][j + Xpos] |= Image[i * Width + j] << Yshift;//下移切割为上半部分:原来的页数不变
						ImageTransBuff[PageStart + i + 1][j + Xpos] |= Image[i * Width + j] >> (8 - Yshift);//上移切割为下半部分：页数加一
					}
				}
				if(ifmove)
				{
					Xpos = Xpos ? Xpos - 1 : 0;
					Ypos = Ypos ? Ypos - 1 : 0;
					Width = (Xpos + Width == 128) ? Width : Width + 2; 
					Height = (Ypos + Height == 64) ? Height : Height + 2;//去除运动残影并清出下一步运动空间
				}
				OLED_BuffUpdata(Xpos, Ypos, Width, Height, ifmove);
			}
		else
			{
				if(Height % 8)//是否满PAGE:1,no先缓存；0,yes直接上屏
				{
//					if(ifmove){OLED_ClearArea(Xpos, Ypos, Width, Height);}
					for(uint8_t i = 0; i <= Pagend; i++)
					{
						for(uint8_t j = 0; j < Width; j++)
						{
							ImageTransBuff[PageStart + i][j + Xpos] |= Image[i * Width + j];//原来的页数不变
						}
					}
					if(ifmove)
					{
						Xpos = Xpos ? Xpos - 1 : 0;
						Ypos = Ypos ? Ypos - 1 : 0;
						Width = (Xpos + Width == 128) ? Width : Width + 2; 
						Height = (Ypos + Height == 64) ? Height : Height + 2;//去除运动残影并清出下一步运动空间
					}
					OLED_BuffUpdata(Xpos, Ypos, Width, Height, ifmove);
				}
				else
				{
					for(uint8_t i = 0; i <= Pagend; i++)
					{
						OLED_SetCursor(PageStart + i, Xpos);
						OLED_WriteDataS(Image, Width, i * Width, DISABLE);
					}
				}
			}
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char, FunctionalState FontSize)
{      	
	uint8_t i;
	if(FontSize)
	{
		OLED_ShowImg((Column - 1) * 8, (Line - 1) * 8, 8, 16, 0, OLED_F8x16[Char - ' ']);
	}
	else
	{
		OLED_ShowImg((Column - 1) * 6, (Line - 1) * 8, 6, 8, 0, OLED_F6x8[Char - ' ']);
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String, FunctionalState FontSize)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i], FontSize);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}
//去前面的零
void OLED_ShowCNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize)
{
	uint8_t i=0;
			while((Number / OLED_Pow(10, Length - i - 1) % 10) == 0 && i < Length-1)
			{
				OLED_ShowChar(Line, Column + i, ' ', FontSize);
				i++;
			}
			for(; i < Length; i++)
			{
			OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
			}
}
/**
  * @brief  OLED显示带小数点数字（十进制，正数，省去前零）
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @param  Point 要显示小数点位置，范围：1~Length-1
	* @retval 无
	*/
void OLED_ShowRPNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, uint8_t Point, FunctionalState FontSize)
{
	uint8_t i = 0;
	uint8_t pointseat = Length - Point;
	while((Number / OLED_Pow(10, Length - i - 1) % 10)==0 && i < pointseat-1)
	{
		OLED_ShowChar(Line, Column + i, ' ', FontSize);
		i++;
	}
	for(; i < pointseat; i++)
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
	OLED_ShowChar(Line, Column + i,'.', FontSize); i++;
	for (; i < Length + 1; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i) % 10 + '0', FontSize);
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length, FunctionalState FontSize)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+', FontSize);
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-', FontSize);
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0', FontSize);
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0', FontSize);
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A', FontSize);
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 行位置，范围：1~4，8x16大小字体；1~8，6x8大小字体
  * @param  Column 列位置，范围：1~16，8x16大小字体；1~21，6x8大小字体
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0', FontSize);
	}
}

/**
  * @brief  OLED显示汉字
  * @param  Line 行位置，范围：1~7，16x16大小字体；
  * @param  Column 列位置，范围：1~15，16x16大小字体；
	* @param  Char要显示的汉字在字库中的序号，范围：0~字模数量
	* @param  *ChineseChar要显示的汉字的UTF8码数组，范围：(直接输入汉字字符串)UTF8:0xXX,0xXX,0xXX,0x00
  * @retval 无
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Char)//输出自己的文字库
{      	
	uint8_t i;
	OLED_SetCursor(Line - 1, (Column - 1) * 8);		//设置光标位置在上半部分
	OLED_WriteDataS(OLED_CF16x16[Char].FontData,16, 0, DISABLE);			//显示上半部分内容
	OLED_SetCursor(Line, (Column - 1) * 8);	//设置光标位置在下半部分
	OLED_WriteDataS(OLED_CF16x16[Char].FontData, 16, 16, DISABLE);		//显示下半部分内容
}
void OLED_ShowChineseN(uint8_t Line, uint8_t Column, char *ChineseChar)//输出自己的文字库
{      	
	char singleChinese[4] = {0};// 0,as '\0' end of chinese string code(UTF8)
	uint8_t indexTemp = 0;
	uint8_t lenght = 0;
	
	for(uint8_t i = 0; ChineseChar[i] != '\0'; i++)//汉字字符串编码遍历
	{
		singleChinese[indexTemp] = ChineseChar[i];
		indexTemp ++;
		if(indexTemp >= 3)//字符串分割单字编码
		{
			lenght ++;
			for(indexTemp = 0; strcmp(OLED_CF16x16[indexTemp].Index, "") != 0; indexTemp ++)//查找到空字符串结束，遍历字库
			{
				if(strcmp(OLED_CF16x16[indexTemp].Index, singleChinese) == 0)//索引匹配
				{
					break;
				}
			}
			OLED_ShowImg(Column*8 + lenght*16 - 24, Line*8 - 8, 16, 16, 0,OLED_CF16x16[indexTemp].FontData);
			indexTemp = 0;
		}
	}
}
/**
	* @brief  OLED画点（默认为叠加模式）
  * @param  x横坐标：范围0~127；
  * @param  y纵坐标：范围0~63；
  * @retval 无
  */
ErrorStatus OLED_DrawPoint(uint8_t x, uint8_t y)
{
	if(x >= 0 && x <= 127 && y >= 0 && y <= 63)
	{
		ImageTransBuff[y / 8][x] |= 0x01 << (y % 8);
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}
/**
	* @brief  OLED查点
  * @param  x横坐标：范围0~127；
  * @param  y纵坐标：范围0~63；
  * @retval 该坐标有点：返回1		
  */
FlagStatus OLED_GetPoint(uint8_t x, uint8_t y)
{
	if (x >= 0 && x <= 127 && y >=0 && y <= 63)		//超出屏幕的内容不读取
	{
		return ImageTransBuff[y / 8][x] & 0x01 << (y % 8);//		/*判断指定位置的数据*/
	}
	return RESET;		//否则，返回0
}
/**
	* @brief  OLED画直线（默认为叠加模式）
  * @param  x0 起点横坐标：范围0~127；
  * @param  y0 起点纵坐标：范围0~63；
  * @param  x1 终点横坐标：范围0~127；
  * @param  y1 终点纵坐标：范围0~63；
  * @retval 无
  */
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	/*以下为Bresenham算法画直线*/
	if(x0 == x1)//直线在y轴上
	{
		if(y0 > y1){uint8_t temp = y1; y1 = y0; y0 = temp;}
		for(; y0 <= y1; y0++)
		{
			OLED_DrawPoint(x0,y0);
		}
	}
	else if(y0 == y1)//直线在x轴上
	{
		if(x0 > x1){uint8_t temp = x1; x1 = x0; x0 = temp;}
		for(; x0 <= x1; x0++)
		{
			OLED_DrawPoint(x0,y0);
		}
	}
	else/*算法要求，画线方向必须为第一象限0~45度范围*/
	{
		FlagStatus flagX = 0;
		FlagStatus flagY = 0;
		FlagStatus flagK = 0;
		uint8_t dX = x1 - x0;
		uint8_t dY = y1 - y0;

		if(x1 < x0){dX = x0 - x1;flagX = 1;}//直线在起点为原点坐标系的左半象限
		if(y1 < y0){dY = y0 - y1; flagY = 1;}//直线在起点为原点坐标系的下半象限
		if(dY > dX){uint8_t temp = dX; dX = dY; dY = temp; flagK = 1;}//直线斜率过一
		
		int16_t x = 0;
		int16_t y = 0;
		int16_t d = 2 * dY - dX;
		do
		{
			OLED_DrawPoint(flagK ? (x0 + y - 2 * flagX * y) : (x0 + x - 2 * flagX * x),
										 flagK ? (y0 + x - 2 * flagY * x) : (y0 + y - 2 * flagY * y));//逆变换，增量部分取反方向
			x++;
			if(d > 0)
			{
				y++;
				d -= 2 * dX;
			}
			d += 2 * dY;
		}
		while(x <= dX);	
	}
}
/**
	* @brief  OLED画矩形（默认为叠加模式）
  * @param  x0 起点横坐标：范围0~127；
  * @param  y0 起点纵坐标：范围0~63；
  * @param  w0 终点横坐标：范围0~127；
  * @param  h0 终点纵坐标：范围0~63；
  * @retval 无
  */
void OLED_DrawRect(uint8_t x0, uint8_t y0, uint8_t w0, uint8_t h0)
{
	OLED_DrawLine(x0, y0, x0 + w0 - 1, y0);
	OLED_DrawLine(x0 + w0 - 1, y0, x0 + w0 - 1, y0 + h0 - 1);
	OLED_DrawLine(x0 + w0 - 1, y0 + h0 - 1, x0, y0 + h0 - 1);
	OLED_DrawLine(x0, y0 + h0 - 1, x0, y0);
}

uint64_t num_len(uint32_t  Num)
{
  uint16_t len=1;
  for(;Num>9;++len)
  Num/=10;
  return len;
}

void OLED_Test()
{	
	while(1)
	{
		OLED_ShowImg(0,0,128,64,0,Image_longmao);
		Delay_ms(3000);
		OLED_Clear();
		OLED_ShowString(FLINEMIDDLE,FCOLRIGHT_6X(13),"123!@#$%^&*()",FONTSIZE_6X8);
		OLED_ShowString(FLINEMIDDLE+1,FCOLMIDDLE_6X(13),"123!@#$%^&*()",FONTSIZE_6X8);
		OLED_ShowString(1,FCOLMIDDLE_8X(12),"abcdEFG_123!",FONTSIZE_8X16);
		OLED_ShowChinese(FLINEDNS16_X8(3),FCOLMIDDLE_16(3),0);
		OLED_ShowChinese(FLINEDNS16_X8(3),FCOLMIDDLE_16(3) + 2,1);
		OLED_ShowChinese(FLINEDNS16_X8(3),FCOLMIDDLE_16(3) + 4,2);
		Delay_ms(3000);
		OLED_Clear();
		OLED_ShowString(1,FCOLRIGHT_8X(12),"abcdEFG_123!",FONTSIZE_8X16);
		OLED_ShowString(FLINEUPS16_X8(2),FCOLRIGHT_6X(13),"123!@#$%^&*()",FONTSIZE_6X8);
		OLED_ShowString(FLINEDNS16_X8(3),FCOLRIS16_6X(6)-12,"123!@#$%^&*()",FONTSIZE_6X8);
		OLED_ShowChineseN(FLINEDNS16_X8(2),FCOLMIDDLE_16(6),"显示调试结果");
		OLED_ShowChinese(FLINEDOWN_16,FCOLRIGHT_16(3),0);
		OLED_ShowChinese(FLINEDOWN_16,FCOLRIGHT_16(3) + 2,1);
		OLED_ShowChinese(FLINEDOWN_16,FCOLRIGHT_16(3) + 4,2);
		Delay_ms(3000);
		OLED_Clear();
		OLED_DrawRect(0,0,128,60);
		for(uint8_t t = 0; t < 128;)
		{
			OLED_DrawLine(63,31,30*cos(t/64.0*3.14)+63,30*sin(t/64.0*3.14)+31);
			t += 8;
		}
		OLED_DrawLine(63,31,20,20);
		OLED_DrawLine(63,31,127,20);
		OLED_DrawLine(63,31,20,54);
		OLED_BuffUpdata(0,0,128,64,1);
		Delay_ms(3000);
		OLED_Clear();
		OLED_ShowImg(10,0,30,30,0,Image_laugh);
		OLED_ShowImg(0,ICOLDOWN(30),30,30,0,Image_laugh);
		OLED_ShowImg(ILINEMIDDLE(30),0,30,30,0,Image_laugh);
		OLED_ShowImg(ILINEMIDDLE(30),ICOLDOWN(30),30,30,0,Image_laugh);
		OLED_ShowImg(ILINERIGHT(30),ICOLDOWN(30),30,30,0,Image_laugh);
		OLED_ShowImg(ILINERIGHT(30),0,30,30,0,Image_laugh);
		OLED_BuffClearAll();
		Delay_ms(3000);
		for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(i,i,30,30,1,Image_laugh);
			Delay_ms(200 - 6*i);
		}
		for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(i + 30,30 - i,30,30,1,Image_laugh);
			Delay_ms(100 - 3*i);
		}
		for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(i + 60,i,30,30,1,Image_laugh);
			Delay_ms(40 - i);
		}
		for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(90 - i,i,30,30,1,Image_laugh);
			Delay_ms(10);
		}
			for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(60 - i,30 - i,30,30,1,Image_laugh);
			Delay_ms(10);
		}
		for(uint8_t i = 0; i <= 30;i++)
		{
			OLED_ShowImg(30 - i,i,30,30,1,Image_laugh);
			Delay_ms(10);
		}
	}
}

void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
