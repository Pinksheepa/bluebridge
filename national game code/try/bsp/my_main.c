#include "my_main.h"




/*** key ***/
extern struct keyblock key[5];




/**** led **/
uint8_t led_sta = 0x00;

void led_disp(uint8_t sta)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,sta << 8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void led_chg(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1);
    led_sta = ( led_sta & ~pos ) | ( pos * sta );
    led_disp(led_sta);
    
}




void setup()
{
    /****led **/
    led_disp(0x00);
    
    /**** lCd ***/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /**key**/
    HAL_TIM_Base_Start_IT(&htim6);
    
}


void loop()
{
    led_chg(1,1);

    
    if(key[1].short_flag == 1)
    {
        led_chg(2,1);
        key[1].short_flag = 0;
        
    }
    if(key[2].long_flag == 1)
    {
        led_chg(3,1);
        key[2].long_flag = 0;
    }
    if(key[3].double_flag == 1)
    {
        led_chg(7,1);
        key[3].double_flag = 0;
    }

}



/****** IT ***/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        key_serv();
    }

}

