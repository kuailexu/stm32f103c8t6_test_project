#ifndef __KEY_H
#define __KEY_H
#include "common.h"
#include "sys.h"

/******************************************************************************************/
/* ���� ���� */

#define KEY_GPIO_PORT                  	GPIOB
#define KEY_GPIO_PIN0                   GPIO_PIN_0
#define KEY_GPIO_PIN1                   GPIO_PIN_1
#define KEY_GPIO_PIN2                   GPIO_PIN_2
#define KEY_GPIO_PIN5                   GPIO_PIN_5
#define KEY_GPIO_PIN6                   GPIO_PIN_6
#define KEY_GPIO_PIN7                   GPIO_PIN_7
#define KEY_INT_IRQn                    EXTI9_5_IRQn
#define KEY_INT_IRQHandler              EXTI9_5_IRQHandler /*6-7外部中断*/
#define GPIOB_CLK_ENABLE()              do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PE��ʱ��ʹ�� */

/******************************************************************************************/

#define KEY0        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN0)     /* ��ȡKEY0���� */
#define KEY1        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN1)     /* ��ȡKEY1���� */
#define KEY2        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN2)     /* ��ȡWKUP���� */
#define KEY5        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN5)     /* ��ȡKEY0���� */
#define KEY6        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN6)     /* ��ȡKEY1���� */
#define KEY7        HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN7)     /* ��ȡWKUP���� */

#define KEY0_PRES    1              /* KEY0���� */
#define KEY1_PRES    2              /* KEY1���� */
#define KEY2_PRES    3              /* KEY2���� */
#define KEY5_PRES    5              /* KEY0���� */
#define KEY6_PRES    6              /* KEY1���� */
#define KEY7_PRES    7              /* KEY2���� */


// �����¼�����
typedef enum {
    KEY_EVENT_NONE = 0,    // ���¼�
    KEY_EVENT_PRESSED,     // �������£�������
    KEY_EVENT_RELEASED     // �����ͷţ�������
} KeyEvent;

// ������ţ�����ʵ��Ӳ�����壩
typedef enum {
    KEY_ID_0 = 0,  // ��ӦKEY0
    KEY_ID_1,      // ��ӦKEY1
    KEY_ID_2,       // ��ӦWKUP
	  KEY_ID_5,  // ��ӦKEY0
    KEY_ID_6,      // ��ӦKEY1
    KEY_ID_7       // ��ӦWKUP
} KeyID;

// ��������״̬��
typedef enum {
    BTN_IDLE,       // ��ʼ״̬��δ���£�
    BTN_PRESS_DB,   // ����������
    BTN_PRESSED,    // ����ȷ�ϣ��ȶ�״̬��
    BTN_RELEASE_DB  // �ͷ�������
} BtnState;

// ����ʱ�䣨��λ��ms������Ӳ��������
#define DEBOUNCE_TIME_MS  20

uint8_t Read_KEY0(void);
uint8_t Read_KEY1(void);
uint8_t Read_KEY2(void);
uint8_t Read_KEY5(void);
uint8_t Read_KEY6(void);
uint8_t Read_KEY7(void);

// ����ָ�����ͣ���ȡ����״̬�ĺ���
typedef uint8_t (*KeyReadFunc)(void);

// ������������ԭ��
KeyEvent Button_Debounce(KeyReadFunc key_read, KeyID key_id);


typedef uint8_t (*KeyReadFunc)(void);
void key_init(void);                /* ������ʼ������ */


#endif


















