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

// 单个内容项的结构体
// typedef struct {
//     int16_t x;          // 项左上角X坐标
//     int16_t y;          // 项左上角Y坐标
//     uint16_t width;     // 项宽度
//     uint16_t height;    // 项高度
//     ItemType_t type;    // 项类型
//     void *data;         // 指向项相关数据的指针
//     void *last_data;         // 指向项相关数据的指针
//     void (*drawFunction)(ContentItem_t*, uint8_t); 
// } ContentItem_t;

// 布局方向
typedef enum {
    LAYOUT_HORIZONTAL,
    LAYOUT_VERTICAL,
    LAYOUT_GRID
} LayoutDirection_t;

// 布局管理器结构体
// typedef struct {
//     ContentItem_t *items;   // 指向内容项数组的指针
//     uint8_t itemCount;      // 内容项的总数
//     uint8_t currentFocusIndex; // 当前获得焦点的项在数组中的索引
//     LayoutDirection_t direction; // 排列方向
//     int16_t spacing;        // 项与项之间的间隔
// } LayoutManager_t;

// extern LayoutManager_t mainLayout; // 全局布局管理器

void Redraw_Mainmenu(void);
void Num_Test(void);
void Font_Test(void);
void Color_Test(void);
void showimage(const unsigned char *p); 
void QDTFT_Test_Demo(void);
#endif
