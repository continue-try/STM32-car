#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
#include "stdint.h"
//字体大小和对齐命令字符预定义
#define STEP(x, FIR, SEC)  ((x < FIR) ? 0 : ( (x > SEC) ? 2 : 1 )) 
#define FONTSIZE_8X16				1
#define FONTSIZE_6X8				0
#define FLINEMIDDLE					4									//字体居中行数(x8字体上偏一格)
#define FLINEDOWN_16				7									//?x8字体置底	
#define FLINEDOWN_X8				8									//?x8字体置底				
#define	FCOLRIGHT_16(L)			17 - L * 2				//16x?字体右对齐(字符长度)
#define	FCOLRIGHT_8X(L)			17 - L						//8x?字体右对齐(字符长度)
#define	FCOLRIGHT_6X(L)			22 - L						//6x?字体右对齐(字符长度)
#define FCOLMIDDLE_16(L)		9 -  L						//16x?字体居中列数(字符长度)
#define FCOLMIDDLE_8X(L)		8.5 - (L - 1)/2		//8x?字体居中列数(字符长度)
#define FCOLMIDDLE_6X(L)		11.5 - (L - 1)/2	//6x?字体居中列数(字符长度)
#define ILINEMIDDLE(WID)		64 - WID/2				//图片横向居中
#define ICOLMIDDLE(HEI)		  32 - HEI/2				//图片纵向居中
#define ILINERIGHT(WID)			128 - WID					//图片右对齐
#define ICOLDOWN(HEI)		  	64 - HEI					//图片置底
#define FLINEUPS16_X8(LIN)	LIN * 2 - 1				//?x8字体以16x16字体网格排列后居上(16x16网格行数)
#define FLINEDNS16_X8(LIN)	LIN * 2						//?x8字体以16x16字体网格排列后居下(16x16网格行数)
#define FCOLLES16_8X(COL)		COL * 2 - 1				//8x?字体以16x16字体网格排列后居左(16x16网格列数)
#define FCOLRIS16_8X(COL)		COL * 2						//8x?字体以16x16字体网格排列后居右(16x16网格列数)
#define FCOLLES16_6X(COL)		COL * 3 - 2	- STEP(COL, 2, 5) 
																							//6x?字体以16x16字体网格排列后居左(16x16网格列数)
#define FCOLRIS16_6X(COL)		COL * 3 - 1	- STEP(COL, 4, 7) + (COL == 1) 						
																							//6x?字体以16x16字体网格排列后居右(16x16网格列数)

//函数
/*-------------------------基础读写OLED屏和Buff区函数---------------------------------*/
void OLED_Init(void);
void OLED_Clear(void);
void OLED_BuffClearAll(void);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_BuffClear(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_BuffUpdata(uint8_t x, uint8_t y, uint8_t width, uint8_t height, FunctionalState ifclear);
ErrorStatus OLED_DrawPoint(uint8_t x, uint8_t y);
FlagStatus OLED_GetPoint(uint8_t x, uint8_t y);
ErrorStatus OLED_ShowImg(uint8_t Xpos, uint8_t Ypos, uint8_t Width, uint8_t Height, FunctionalState ifmove, const uint8_t *Image);
/*-------------------------字符与画图功能函数---------------------------------*/
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char, FunctionalState FontSize);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String, FunctionalState FontSize);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize);
void OLED_ShowCNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize);
void OLED_ShowRPNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length,uint8_t Point, FunctionalState FontSize);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length, FunctionalState FontSize);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length, FunctionalState FontSize);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Char);
void OLED_ShowChineseN(uint8_t Line, uint8_t Column, char *ChineseChar);
void OLED_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void OLED_DrawRect(uint8_t x0, uint8_t y0, uint8_t w0, uint8_t h0);
uint64_t num_len(uint32_t  Num);
/*-------------------------测试---------------------------------*/
void OLED_Test();

#endif
