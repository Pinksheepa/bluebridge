#include "my_main.h"

char code1 = '@';
char code2 = '@';
char code3 = '@';

char real_code1 = '1';
char real_code2 = '2';
char real_code3 = '3';

uint16_t freq = 1000;
uint16_t duty = 50;

uint8_t time100ms = 0;
uint8_t led_100ms = 0;

uint8_t warn = 0;
uint8_t warn_flag = 0;
uint16_t basetime;

/** uart**/
char uart_rx[50];
char uart_tx[50];

/*** key**/
extern struct keyblock key[];


/*** led **/
uint8_t led_sta = 0x00;

uint8_t i;

void led_disp(uint8_t sta)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,sta << 8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);

}
void led_chg(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1) ;
    led_sta = ( led_sta & ~pos ) | ( pos * sta );
    led_disp(led_sta);
}

/*** lcd ***/
uint8_t page = 0;
char text[50];


void interface()
{
    if(page == 0)
    {
        sprintf(text,"       PSD   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        
        
        sprintf(text,"    B1:%c       ",code1);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"    B2:%c      ",code2);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"    B3:%c       ",code3);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
    
    }
    if(page == 1)
    {
        LCD_ClearLine(Line5);
        
        sprintf(text,"       STA   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);        
        sprintf(text,"    F:%dHz   ",freq);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"    D:%d%%   ",duty);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);    
    }


}













void setup()
{
    
    /** key **/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /** lcd*/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    LCD_Clear(Black);

    /*** uart**/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,sizeof(uart_rx));
    
    /*** pwm **/
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    
    led_disp(led_sta);
    basetime = uwTick;
    
    __HAL_TIM_PRESCALER(&htim17,800-1);
}

void loop()
{
    interface();
    
    if(page == 0)
    {
        if(key[1].short_flag == 1)
        {
            if(code1 == '@')    code1 = '0';
            else if(code1 == '9')     code1 = '0';
            else
            code1 ++ ;
            
            
            key[1].short_flag = 0;
        }
        if(key[2].short_flag == 1)
        {
            if(code2 == '@')    code2 = '0';
            else if(code2 == '9')     code2 = '0';
            else
            code2 ++ ;
            
            
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1)
        {
            if(code3 == '@')    code3 = '0';
            else if(code3 == '9')     code3 = '0';
            else
            code3 ++ ;
            
            
            key[3].short_flag = 0;
        }
        if(key[4].short_flag == 1)
        {
            if(real_code1 == code1 && real_code2 == code2 && real_code3 == code3)
            {
                __HAL_TIM_SetAutoreload(&htim2,500-1);
                __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
                
                led_chg(1,1);
                
                warn = 0;
                page = !page;
                freq = 2000;
                duty = 10;
                
                HAL_TIM_Base_Start_IT(&htim16);                
            }
            else
            {
                warn ++;
                        if(warn >= 3 )
        {
            HAL_TIM_Base_Start_IT(&htim17);
        }   
                
                code1 = '@';
                code2 = '@';
                code3 = '@';
            }           
            key[4].short_flag = 0;
        }  

        
    }
    if(page == 1)
    {
        if(key[1].short_flag == 1)
        {
            
            key[1].short_flag = 0;
        }
        if(key[2].short_flag == 1)
        {
            
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1)
        {
            
            key[3].short_flag = 0;
        }
        if(key[4].short_flag == 1)
        {
            
            key[4].short_flag = 0;
        }     
    }
    
    
    
}
    


















/*********************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        key_serv();
    }
    if(htim->Instance == TIM16)
    {
        time100ms++;
        

        
        
        if(time100ms >= 50)
        {
            led_chg(1,0);
            page = 0;
            
            time100ms = 0;
            __HAL_TIM_SetAutoreload(&htim2,1000-1);
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,500);   
            freq = 1000;
            duty = 50;
                code1 = '@';
                code2 = '@';
                code3 = '@';
            HAL_TIM_Base_Stop_IT(&htim16);
        }
    }
    if(htim->Instance == TIM17)
    {
        led_100ms ++;
        if(warn >= 3)
            {

                i = !i;
                led_chg(2,i);
            }     
                if(led_100ms >= 50)
                {
                    led_100ms = 0;
                    HAL_TIM_Base_Stop_IT(&htim17);
                }            
    
    }

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    
    char old_pswd[10];
    char new_pswd[10];
    char coode[4]={real_code1,real_code2,real_code3};

 
    sscanf(uart_rx,"%3s-%3s",old_pswd,new_pswd);
    
    if(strcmp(old_pswd,coode) == 0)
    {
        real_code1 = new_pswd[0];
        real_code2 = new_pswd[1];
        real_code3 = new_pswd[2];
    }
    
    memset(uart_rx,0,sizeof(uart_rx));
    memset(old_pswd,0,sizeof(old_pswd));
    memset(new_pswd,0,sizeof(new_pswd));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,sizeof(uart_rx));
}



