/* Includes ------------------------------------------------------------------*/
#include "QDTFT_demo.h"
#include "Picture.h"
unsigned char Num[10]={0,1,2,3,4,5,6,7,8,9};

void Redraw_Mainmenu(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,40,BLUE,GRAY0,"asdsadas");

	DisplayButtonUp(15,60,113,90); //x1,y1,x2,y2
	Gui_DrawFont_GBK16(16,62,YELLOW,GRAY0,"asdasd");
}

// void Application_Init(void) {
//     // 1. 定义你的内容项数组（这里以3个按钮为例）
//     static ContentItem_t myItems[] = {
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn1" }, // Item 0
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn2" }, // Item 1
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn3" }  // Item 2
//         // 注意：x,y可以先设为0，由布局计算函数自动计算实际位置
//     };

//     // 2. 初始化布局管理器
//     mainLayout.items = myItems;
//     mainLayout.itemCount = sizeof(myItems) / sizeof(ContentItem_t);
//     mainLayout.currentFocusIndex = 0; // 默认焦点在第一个项上
//     mainLayout.direction = LAYOUT_VERTICAL; // 垂直排列
//     mainLayout.spacing = 10; // 项之间间隔10像素

//     // 3. 【关键】根据布局方向，自动计算每个项的实际屏幕坐标
//     Calculate_Layout(&mainLayout);

//     // 4. 根据计算好的项坐标，绘制所有内容到屏幕
//     Draw_AllItems(&mainLayout);

//     // 5. 根据当前获得焦点的项，绘制选择框
//     Draw_SelectionBox_ForItem(&(mainLayout.items[mainLayout.currentFocusIndex]));
// }

// void Calculate_Layout(LayoutManager_t *layout) {
//     int16_t currentX = START_X; // 起始坐标，例如20
//     int16_t currentY = START_Y; // 起始坐标，例如20

//     for(int i = 0; i < layout->itemCount; i++) {
//         // 根据布局方向，计算每个项的坐标
//         switch(layout->direction) {
//             case LAYOUT_VERTICAL:
//                 layout->items[i].x = currentX;
//                 layout->items[i].y = currentY;
//                 currentY += layout->items[i].height + layout->spacing; // 下一个项的Y坐标
//                 break;

//             case LAYOUT_HORIZONTAL:
//                 layout->items[i].x = currentX;
//                 layout->items[i].y = currentY;
//                 currentX += layout->items[i].width + layout->spacing; // 下一个项的X坐标
//                 break;

//             case LAYOUT_GRID:
//                 // 更复杂的网格计算，需要列数等参数
//                 // layout->items[i].x = currentX;
//                 // layout->items[i].y = currentY;
//                 // currentX += ...;
//                 // if(/* 换行判断 */) { currentX = START_X; currentY += ...; }
//                 break;
//         }
//     }
// }

// void Draw_Item(ContentItem_t *item, uint8_t isFocused) {
//     // 1. 先绘制项本身的内容（例如一个按钮）
//     ST7739_FillRect(item->x, item->y, item->width, item->height, BG_COLOR);
//     ST7739_DrawString(item->x + 5, item->y + 5, (char*)item->data, TEXT_COLOR, BG_COLOR);

//     // 2. 如果该项获得焦点，再在它上面叠加绘制选择框
//     if(isFocused) {
//         // 绘制一个方框，可以比内容项稍大一点，效果更好
//         int16_t border = 2; // 边框向外扩展2像素
//         ST7739_DrawRect(item->x - border,
//                         item->y - border,
//                         item->width + (2 * border),
//                         item->height + (2 * border),
//                         FOCUS_COLOR); // 焦点颜色，如红色或白色
//     }
// }


void Num_Test(void)
{
	u8 i=0;
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,20,RED,GRAY0,"Num Test");
	delay_ms(1000);
	Lcd_Clear(GRAY0);

	for(i=0;i<10;i++)
	{
	Gui_DrawFont_Num32((i%3)*40,32*(i/3)+5,RED,GRAY0,Num[i+1]);
	delay_ms(100);
	}
	
}

void Font_Test(void)
{
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"文字显示测试");

	delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,30,YELLOW,GRAY0,"电子技术");
	Gui_DrawFont_GBK16(16,50,BLUE,GRAY0,"专注液晶批发");
	Gui_DrawFont_GBK16(16,70,RED,GRAY0, "全程技术支持");
	Gui_DrawFont_GBK16(0,100,BLUE,GRAY0,"Tel:11111111111");
	Gui_DrawFont_GBK16(0,130,RED,GRAY0, "QQ:11111111");	
	delay_ms(1800);	
}

void Color_Test(void)
{
	u8 i=1;
	Lcd_Clear(GRAY0);
	
	Gui_DrawFont_GBK16(20,10,BLUE,GRAY0,"Color Test");
	delay_ms(200);

	while(i--)
	{
		Lcd_Clear(WHITE);
		Lcd_Clear(BLACK);
		Lcd_Clear(RED);
	  	Lcd_Clear(GREEN);
	  	Lcd_Clear(BLUE);
	}		
}

//取模方式 水平扫描 从左到右 低位在前
void showimage(const unsigned char *p) //显示40*40 QQ图片
{
  	int i,j,k; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //清屏  
	
	for(k=0;k<4;k++)
	{
	   	for(j=0;j<3;j++)
		{	
			Lcd_SetRegion(40*j+2,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//数据低位在前
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}
void QDTFT_Test_Demo(void)
{
	Lcd_Init();
	LCD_LED_SET;//通过IO控制背光亮				
	Redraw_Mainmenu();//绘制主菜单(部分内容由于分辨率超出物理值可能无法显示)
	Color_Test();//简单纯色填充测试
	Num_Test();//数码管字体测试
	Font_Test();//中英文显示测试		
	showimage(gImage_qq);//图片显示示例
	delay_ms(1200);
	LCD_LED_CLR;//IO控制背光灭	
	
}
