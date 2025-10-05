#include "key.h"
#include "delay.h"
#include "freertos_demo.h"

/**
 * @brief       ������ʼ������
 * @param       ��
 * @retval      ��
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    GPIOB_CLK_ENABLE();

    gpio_init_struct.Pin = KEY_GPIO_PIN0|KEY_GPIO_PIN1|KEY_GPIO_PIN2|KEY_GPIO_PIN5;                       /* KEY0���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;              /* ���� */
    HAL_GPIO_Init(KEY_GPIO_PORT, &gpio_init_struct);           /* KEY0����ģʽ����,�������� */

   gpio_init_struct.Pin = KEY_GPIO_PIN6|KEY_GPIO_PIN7;                       
   gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;                  
   gpio_init_struct.Pull = GPIO_PULLUP;                                   
   HAL_GPIO_Init(KEY_GPIO_PORT, &gpio_init_struct);          
   HAL_NVIC_SetPriority(KEY_INT_IRQn, 7, 2);                
   HAL_NVIC_EnableIRQ(KEY_INT_IRQn);                          
}
void EXTI9_5_IRQHandler(void)
{
    if(KEY6 == RESET) {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
    }
    if(KEY7 == RESET) {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
    }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint8_t lastAB = 0;
    static uint32_t lastISRTime = 0;
    uint32_t currentTime = HAL_GetTick();
    
    // Ӳ����������С5ms�����
    if(currentTime - lastISRTime < 10) return;
    lastISRTime = currentTime;

    if(GPIO_Pin == GPIO_PIN_6 || GPIO_Pin == GPIO_PIN_7) { // �������ж�
        uint8_t currentAB = (KEY6 << 1) | KEY7;
        switch(lastAB) {
            case 3:  
                if(currentAB == 2) {ledrpwmval ++;} 
                else if(currentAB == 1) {ledrpwmval--;}
                break;
            case 1: 
                if(currentAB == 3) {ledrpwmval ++;} 
                else if(currentAB == 0) {ledrpwmval--;}
                break;
            case 0: 
                if(currentAB == 1) {ledrpwmval ++;} 
                else if(currentAB == 2) {ledrpwmval--;}
                break;
            case 2: 
                if(currentAB == 0) {ledrpwmval ++;} 
                else if(currentAB == 3) {ledrpwmval--;}
                break;
    }
    lastAB = currentAB;
  }
}

// ���尴����ȡ�İ�װ����������HAL�⣩
uint8_t Read_KEY0(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN0); }
uint8_t Read_KEY1(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN1); }
uint8_t Read_KEY2(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN2); }
uint8_t Read_KEY5(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN5); }
uint8_t Read_KEY6(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN6); }
uint8_t Read_KEY7(void) { return HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_GPIO_PIN7); }

KeyEvent Button_Debounce(KeyReadFunc key_read, KeyID key_id) {
    static BtnState state = BTN_IDLE;
    static uint32_t last_time = 0;
    static uint8_t prev_state = 1; // Ĭ����������
    KeyEvent event = KEY_EVENT_NONE;

    uint8_t current_state = key_read(); // ͨ������ָ���ȡ����״̬
    uint32_t now = HAL_GetTick();       // ��ȡ��ǰʱ���

    switch (state) {
        case BTN_IDLE:
            if (current_state == 0 && prev_state == 1) { // ����½��أ����£�
                state = BTN_PRESS_DB;
                last_time = now;
            }
            break;
        case BTN_PRESS_DB:
            if (now - last_time >= DEBOUNCE_TIME_MS) {
                if (current_state == 0) { // ��������Ϊ����
                    state = BTN_PRESSED;
                    event = KEY_EVENT_PRESSED;
                } else { // ���������ؿ���״̬
                    state = BTN_IDLE;
                }
            }
            break;
        case BTN_PRESSED:
            if (current_state == 1 && prev_state == 0) { // ��������أ��ͷţ�
                state = BTN_RELEASE_DB;
                last_time = now;
            }
            break;
        case BTN_RELEASE_DB:
            if (now - last_time >= DEBOUNCE_TIME_MS) {
                if (current_state == 1) { // ��������Ϊ�ͷ�
                    state = BTN_IDLE;
                    event = KEY_EVENT_RELEASED;
                } else { // ���������ذ���״̬
                    state = BTN_PRESSED;
                }
            }
            break;
    }

    prev_state = current_state;
    return event;
}

