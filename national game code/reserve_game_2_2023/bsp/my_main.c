#include "my_main.h"

uint8_t mode = 0;

float pa1_freq = 1000;
float pa1_duty = 10;
float pa7_freq = 1000;
float pa7_duty = 10;

extern struct keyblock key[5];

/*************************************************/


/****led***/
uint8_t led_sta = 0x00;

int blink;
uint8_t sta = 0;


void disp_led(uint8_t num)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,num << 8 ,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void chg_led(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1);
    led_sta = (led_sta & ~pos) | ( pos * sta);
    disp_led(led_sta);
}

/**** lcd*****/
char text[50];
uint8_t page = 1;

void interface()
{
    if(page == 1)
    {
        sprintf(text,"        PA1       ");
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"      F:%dHz       ",(int)pa1_freq);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"      D:%d%%       ",(int)pa1_duty);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
        
    }
    else if(page == 2)
    {
        sprintf(text,"        PA7       ");
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"      F:%dHz       ",(int)pa7_freq);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"      D:%d%%       ",(int)pa7_duty);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);     
    
    }


}

/***** uart****/
char rx_uart[50];
char tx_uart[50];


/***********************************************/

void setup()
{
    /***led***/
    disp_led(0x00);
    
    
    /***** key ***/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /**** lcd ***/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /******* pwm ****/
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    /*** uart**/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,sizeof(rx_uart));
    
    /*** systick**/
    blink = uwTick;
}

void loop()
{
    interface();
    __HAL_TIM_SetAutoreload(&htim2,1000000.0f / pa1_freq - 1);
    __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pa1_duty / 100.0f * (1000000.0f / pa1_freq));
    
    __HAL_TIM_SetAutoreload(&htim17,1000000.0f / pa7_freq - 1);
    __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,pa7_duty / 100.0f * (1000000.0f / pa7_freq));    
    
    if(mode == 0)   chg_led(3,1);
    else chg_led(3,0);
    

    
    
    if(page == 1) 
    {
        if(pa1_freq > pa7_freq)
        {
            if(uwTick - blink > 100)
            {
                
                chg_led(1,sta);
                sta = !sta;
                blink = uwTick;
            }
        }
        else if(pa1_freq < pa7_freq)
        {
            if(uwTick - blink > 100)
            {
                
                chg_led(2,sta);   
                sta = !sta;                
                blink = uwTick;
            }            
        }        
        else    
        {
            chg_led(1,1);
            chg_led(2,0);
        }
        
        
        
        
     
            if(key[1].short_flag == 1)
            {
                pa1_freq += 1000;
                if(pa1_freq > 10000)    pa1_freq = 1000;
                key[1].short_flag = 0;
            }
            if(key[2].short_flag == 1)
            {
                pa1_duty += 10;
                if(pa1_duty > 90)    pa1_duty = 10;
                key[2].short_flag = 0;
            }  
    }
    if(page == 2)
    {
        if(pa1_freq > pa7_freq)
        {
            if(uwTick - blink > 100)
            {
                
                chg_led(1,sta);   
sta = !sta;                
                blink = uwTick;
            }
        }
        else if(pa1_freq < pa7_freq)
        {
            if(uwTick - blink > 100)
            {
                
                chg_led(2,sta);       
sta = !sta;                
                blink = uwTick;
            }            
        }        
        else    
        {
            chg_led(1,0);
            chg_led(2,1);
        }      
            if(key[1].short_flag == 1)
            {
                pa7_freq += 1000;
                if(pa7_freq > 10000)    pa7_freq = 1000;
                key[1].short_flag = 0;
            }
            if(key[2].short_flag == 1)
            {
                pa7_duty += 10;
                if(pa7_duty > 90)    pa7_duty = 10;
                key[2].short_flag = 0;
            }             
    }    
    
    
    if(key[3].short_flag == 1)
    {
        if(mode == 0)
        {
            page ++;
            if(page >= 3)    page = 1;
            key[3].short_flag = 0;
        }
        if(mode == 1)
        {
            key[3].short_flag = 0;
        }
    }
    if(key[4].short_flag == 1)
    {
        mode = !mode;
        
        key[4].short_flag = 0;
    }    
    
    
    
    
}

/***********************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        scan_key();
    }

}

/**********/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(mode == 1)
    {
        if(strcmp(rx_uart,"@") == 0)
        {
            page = 1;
        }
        else if(strcmp(rx_uart,"#") == 0) 
        {
            page = 2;
        }
        else 
        {
            sprintf(tx_uart,"ERROR");
            HAL_UART_Transmit_IT(&huart1,(uint8_t *)tx_uart,strlen(tx_uart));
        }
    }
    else
    {
            sprintf(tx_uart,"KEY CONTROL");
            HAL_UART_Transmit_IT(&huart1,(uint8_t *)tx_uart,strlen(tx_uart));    
    }
    
    memset(rx_uart,0,sizeof(rx_uart));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,sizeof(rx_uart));    
    
}


