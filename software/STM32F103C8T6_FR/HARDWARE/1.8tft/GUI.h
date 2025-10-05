#include "common.h"
#include "QDTFT_demo.h"
#define LCD_WIDTH 160
#define LCD_HEIGH 128
#define FONT_HEIGH 8
#define FONT_WIDTH 6
u16 LCD_BGR2RGB(u16 c);
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);
void Gui_DrawCurve(u16 x, u16 y,u16 size_x, u16 size_y, int16_t *data, uint8_t scale, u16 color);    
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num);
void Gui_DrawFont_Num16(u16 x, u16 y, u16 fc, u16 bc, u16 num);
void LCD_Num_16x8(u16 x,u16 y,u16 fc,u16 bc,float num);
void Gui_DrawFont_Num8(u16 x, u16 y, u16 fc, u16 bc, u16 num);
// void LCD_Num_8x6(ContentItem_t *item, uint8_t isFocused);
// void LCD_Num_8x6(u16 x,u16 y,u16 fc,u16 bc,float num,float* last_num);
void Gui_DrawFont_Str16(u16 x, u16 y, u16 fc, u16 bc, const char *str);
void Gui_DrawFont_Str8x6(u16 x, u16 y, u16 fc, u16 bc, const char *str);

