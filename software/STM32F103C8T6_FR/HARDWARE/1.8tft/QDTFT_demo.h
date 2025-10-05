#ifndef __QDTFT_DEMO_H
#define __QDTFT_DEMO_H 
#include "Lcd_Driver.h"
#include "GUI.h"
#include "delay.h"
// #include "Picture.h"

#define START_X 20
#define START_Y 20
typedef enum {
    ITEM_TYPE_NUM,
    ITEM_TYPE_TEXT,
    ITEM_TYPE_BUTTON,
    ITEM_TYPE_ICON,
    ITEM_TYPE_MENU_ENTRY,
    ITEM_TYPE_TOGGLE
} ItemType_t;

// ����������Ľṹ��
// typedef struct {
//     int16_t x;          // �����Ͻ�X����
//     int16_t y;          // �����Ͻ�Y����
//     uint16_t width;     // ����
//     uint16_t height;    // ��߶�
//     ItemType_t type;    // ������
//     void *data;         // ָ����������ݵ�ָ��
//     void *last_data;         // ָ����������ݵ�ָ��
//     void (*drawFunction)(ContentItem_t*, uint8_t); 
// } ContentItem_t;

// ���ַ���
typedef enum {
    LAYOUT_HORIZONTAL,
    LAYOUT_VERTICAL,
    LAYOUT_GRID
} LayoutDirection_t;

// ���ֹ������ṹ��
// typedef struct {
//     ContentItem_t *items;   // ָ�������������ָ��
//     uint8_t itemCount;      // �����������
//     uint8_t currentFocusIndex; // ��ǰ��ý�������������е�����
//     LayoutDirection_t direction; // ���з���
//     int16_t spacing;        // ������֮��ļ��
// } LayoutManager_t;

// extern LayoutManager_t mainLayout; // ȫ�ֲ��ֹ�����

void Redraw_Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void showimage(const unsigned char *p); 
void QDTFT_Test_Demo(void);
#endif
