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
//     // 1. ����������������飨������3����ťΪ����
//     static ContentItem_t myItems[] = {
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn1" }, // Item 0
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn2" }, // Item 1
//         { .x = 0, .y = 0, .width = 60, .height = 30, .type = ITEM_TYPE_BUTTON, .data = "Btn3" }  // Item 2
//         // ע�⣺x,y��������Ϊ0���ɲ��ּ��㺯���Զ�����ʵ��λ��
//     };

//     // 2. ��ʼ�����ֹ�����
//     mainLayout.items = myItems;
//     mainLayout.itemCount = sizeof(myItems) / sizeof(ContentItem_t);
//     mainLayout.currentFocusIndex = 0; // Ĭ�Ͻ����ڵ�һ������
//     mainLayout.direction = LAYOUT_VERTICAL; // ��ֱ����
//     mainLayout.spacing = 10; // ��֮����10����

//     // 3. ���ؼ������ݲ��ַ����Զ�����ÿ�����ʵ����Ļ����
//     Calculate_Layout(&mainLayout);

//     // 4. ���ݼ���õ������꣬�����������ݵ���Ļ
//     Draw_AllItems(&mainLayout);

//     // 5. ���ݵ�ǰ��ý���������ѡ���
//     Draw_SelectionBox_ForItem(&(mainLayout.items[mainLayout.currentFocusIndex]));
// }

// void Calculate_Layout(LayoutManager_t *layout) {
//     int16_t currentX = START_X; // ��ʼ���꣬����20
//     int16_t currentY = START_Y; // ��ʼ���꣬����20

//     for(int i = 0; i < layout->itemCount; i++) {
//         // ���ݲ��ַ��򣬼���ÿ���������
//         switch(layout->direction) {
//             case LAYOUT_VERTICAL:
//                 layout->items[i].x = currentX;
//                 layout->items[i].y = currentY;
//                 currentY += layout->items[i].height + layout->spacing; // ��һ�����Y����
//                 break;

//             case LAYOUT_HORIZONTAL:
//                 layout->items[i].x = currentX;
//                 layout->items[i].y = currentY;
//                 currentX += layout->items[i].width + layout->spacing; // ��һ�����X����
//                 break;

//             case LAYOUT_GRID:
//                 // �����ӵ�������㣬��Ҫ�����Ȳ���
//                 // layout->items[i].x = currentX;
//                 // layout->items[i].y = currentY;
//                 // currentX += ...;
//                 // if(/* �����ж� */) { currentX = START_X; currentY += ...; }
//                 break;
//         }
//     }
// }

// void Draw_Item(ContentItem_t *item, uint8_t isFocused) {
//     // 1. �Ȼ����������ݣ�����һ����ť��
//     ST7739_FillRect(item->x, item->y, item->width, item->height, BG_COLOR);
//     ST7739_DrawString(item->x + 5, item->y + 5, (char*)item->data, TEXT_COLOR, BG_COLOR);

//     // 2. ��������ý��㣬������������ӻ���ѡ���
//     if(isFocused) {
//         // ����һ�����򣬿��Ա��������Դ�һ�㣬Ч������
//         int16_t border = 2; // �߿�������չ2����
//         ST7739_DrawRect(item->x - border,
//                         item->y - border,
//                         item->width + (2 * border),
//                         item->height + (2 * border),
//                         FOCUS_COLOR); // ������ɫ�����ɫ���ɫ
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
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"������ʾ����");

	delay_ms(1000);
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,30,YELLOW,GRAY0,"���Ӽ���");
	Gui_DrawFont_GBK16(16,50,BLUE,GRAY0,"רעҺ������");
	Gui_DrawFont_GBK16(16,70,RED,GRAY0, "ȫ�̼���֧��");
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

//ȡģ��ʽ ˮƽɨ�� ������ ��λ��ǰ
void showimage(const unsigned char *p) //��ʾ40*40 QQͼƬ
{
  	int i,j,k; 
	unsigned char picH,picL;
	Lcd_Clear(WHITE); //����  
	
	for(k=0;k<4;k++)
	{
	   	for(j=0;j<3;j++)
		{	
			Lcd_SetRegion(40*j+2,40*k,40*j+39,40*k+39);		//��������
		    for(i=0;i<40*40;i++)
			 {	
			 	picL=*(p+i*2);	//���ݵ�λ��ǰ
				picH=*(p+i*2+1);				
				LCD_WriteData_16Bit(picH<<8|picL);  						
			 }	
		 }
	}		
}
void QDTFT_Test_Demo(void)
{
	Lcd_Init();
	LCD_LED_SET;//ͨ��IO���Ʊ�����				
	Redraw_Mainmenu();//�������˵�(�����������ڷֱ��ʳ�������ֵ�����޷���ʾ)
	Color_Test();//�򵥴�ɫ������
	Num_Test();//������������
	Font_Test();//��Ӣ����ʾ����		
	showimage(gImage_qq);//ͼƬ��ʾʾ��
	delay_ms(1200);
	LCD_LED_CLR;//IO���Ʊ�����	
	
}
