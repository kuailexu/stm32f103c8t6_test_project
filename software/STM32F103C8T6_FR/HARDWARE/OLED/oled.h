#ifndef _OLED_H
#define _OLED_H

#include "sys.h"

//#define DC   PAout(4)	
//#define RES  PAout(5)	
//#define SDA  PAout(6)	
//#define SCL  PAout(7)	


//#define OLED_GPIO    GPIOA
//#define OLED_RCCEN   RCC_APB2Periph_GPIOA
//#define OLED_SCL     GPIO_Pin_7      //D0
//#define OLED_SDA     GPIO_Pin_6      //D1
//#define OLED_RST     GPIO_Pin_5      //RST
//#define OLED_DC      GPIO_Pin_4      //DC

#define OLED_GPIO    GPIOB
//#define OLED_GPIO1   GPIOA
//#define OLED_RCCEN   RCC_APB2Periph_GPIOB
//#define OLED_RCCEN1  RCC_APB2Periph_GPIOA
#define OLED_SCL     GPIO_PIN_8      //D0
#define OLED_SDA     GPIO_PIN_9      //D1
#define OLED_RST     GPIO_PIN_10      //RST
#define OLED_DC      GPIO_PIN_11    //DC
#define OLED_CS      GPIO_PIN_12    //DC


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define OLED_D0_OL()		HAL_GPIO_WritePin(OLED_GPIO, OLED_SCL, GPIO_PIN_RESET)	//D0 IO口输出低电平
#define OLED_D0_OH()		HAL_GPIO_WritePin(OLED_GPIO, OLED_SCL, GPIO_PIN_SET)  	//D0 IO口输出高电平

#define OLED_D1_OL()		HAL_GPIO_WritePin(OLED_GPIO, OLED_SDA, GPIO_PIN_RESET)	//D1 IO口输出低电平
#define OLED_D1_OH()		HAL_GPIO_WritePin(OLED_GPIO, OLED_SDA, GPIO_PIN_SET) 	//D1 IO口输出高电平

#define OLED_RST_OL()		HAL_GPIO_WritePin(OLED_GPIO, OLED_RST, GPIO_PIN_RESET)	//RST IO口输出低电平
#define OLED_RST_OH()		HAL_GPIO_WritePin(OLED_GPIO, OLED_RST, GPIO_PIN_SET) //RST IO口输出高电平

#define OLED_DC_OL()		HAL_GPIO_WritePin(OLED_GPIO, OLED_DC, GPIO_PIN_RESET)	//DC IO口输出低电平
#define OLED_DC_OH()		HAL_GPIO_WritePin(OLED_GPIO, OLED_DC, GPIO_PIN_SET) 	//DC IO口输出高电平

#define OLED_RST_Clr()  HAL_GPIO_WritePin(OLED_GPIO, OLED_RST, GPIO_PIN_RESET) //RST IO口输出低电平
#define OLED_RST_Set()  HAL_GPIO_WritePin(OLED_GPIO, OLED_RST, GPIO_PIN_SET) 	//RST IO口输出高电平

#define OLED_RS_Clr()   HAL_GPIO_WritePin(OLED_GPIO, OLED_DC, GPIO_PIN_RESET)	//DC IO口输出低电平
#define OLED_RS_Set()   HAL_GPIO_WritePin(OLED_GPIO, OLED_DC, GPIO_PIN_SET) 	  //DC IO口输出高电平

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_GPIO, OLED_SCL, GPIO_PIN_RESET)	//D0 IO口输出低电平
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_GPIO, OLED_SCL, GPIO_PIN_SET)  	//D0 IO口输出高电平

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_GPIO, OLED_SDA, GPIO_PIN_RESET)	//D1 IO口输出低电平
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_GPIO, OLED_SDA, GPIO_PIN_SET) 	//D1 IO口输出高电平

extern const unsigned char F16X16_hanzi[];
extern const unsigned char F12X16_hanzi[];











void setgpio(void);
void OLED_Init(void);
void OLED_CLS(void);
void OLED_4num(unsigned char x,unsigned char y,int number);
void OLED_3num_6x8(unsigned char x,unsigned char y,unsigned int number);
void OLED_3num_8x16(unsigned char x,unsigned char y,unsigned int number);
void OLED_Num(unsigned char x,unsigned char y,unsigned char asc);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
//void OLED_P14x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_Print(unsigned char x, unsigned char y, unsigned char ch[]);
void OLED_PutPixel(unsigned char x,unsigned char y);
void OLED_Rectangle(int16_t acc_x,int16_t acc_y);
//void Draw_Logo(void);
void Draw_BMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char bmp[]); 
void OLED_Fill(unsigned char dat);
//void Dis_String(unsigned char y, unsigned char x, unsigned char ch[]);
void Dis_Char(unsigned char y,unsigned char x,unsigned char asc);
void Dis_Num(unsigned char y, unsigned char x, unsigned int num,unsigned char N);
void Dis_Float(unsigned char Y,unsigned char X,double real,unsigned char N);
void Dis_Float2(unsigned char Y,unsigned char X,double real,unsigned char N1,unsigned char N2);
void OLED_P6x8Num_8bit(unsigned char x,unsigned char y,unsigned char Number); 
void OLED_Num5(unsigned char x,unsigned char y,unsigned int number);
void OLED_WR_Byte1(uint8_t dat,uint8_t cmd);
void OLED_Clear1(void);  
void OLED_Set_Pos1(uint8_t x,uint8_t y);
void OLED_ShowChar1(uint8_t x,uint8_t y,uint8_t a);
void OLED_ShowString1(uint8_t x,uint8_t y,uint8_t *chr);
void OLED_ShowCHinese1(uint8_t x,uint8_t y,uint8_t no);
void display_page1(void);
void display_page2(void);
void display_page3(void);
void display_page4(void);
void OLED_P16x16Str(uint8_t x, uint8_t y, const unsigned char ch[],int adder);
void OLED_P16x12Str(uint8_t x, uint8_t y, const unsigned char ch[],int adder);
void OLED_Num1(unsigned char x,unsigned char y,unsigned char asc);
void OLED_P16x16kongge(uint8_t x, uint8_t y);



#endif


