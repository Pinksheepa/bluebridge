#include "my_main.h"

float temp;
uint8_t gear=1;
char mode[10]={"Auto"};
uint16_t time_1s = 0;
uint16_t time_1s_ = 0;
uint16_t sleep_time=0;

float adc_voltage;

/* uart*/ 
char uart_rx[50];
char uart_tx[50];

/** lcd **/
uint8_t page = 0;
char text[50];

uint8_t led_sta = 0x00;

void interface()
{
    if(page == 0)
    {
        sprintf(text,"        DATA    ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);

        sprintf(text,"     TEMP:%.1f    ",temp);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     MODE:%4s",mode);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     GEAR:%d    ",gear);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
    }
    if(page == 1)
    {
        LCD_ClearLine(Line1);
        LCD_ClearLine(Line2);
        LCD_ClearLine(Line3);
        
        sprintf(text,"     SLEEPING    ");
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
        sprintf(text,"     TEMP:%.1f    ",temp);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);    
    }
}

/*** key**/
extern struct keyblock key[5];

void led_disp(uint8_t led)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,led << 8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void led_chg(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1);
    
    led_sta = ( led_sta & (~pos) ) | (pos * sta);
    
    led_disp(led_sta);
    
}

/** adc */
float adc_trans_to_temp()
{
    HAL_ADC_Start(&hadc2);
    
    adc_voltage = 3.3f * HAL_ADC_GetValue(&hadc2) / 65536.0f ;
    
    if(adc_voltage < 1.0f)
    {
        temp = 0;
    }
    else if(adc_voltage > 1.0f && adc_voltage < 3.0f)
    {
        temp = 10.0f * adc_voltage + 10.0f ;
    }
    else if(adc_voltage > 3.0f)
    {
        temp = 40.0;
    }
    
    return temp;
}







void setup()
{
    /*** pwm ***/
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    /*** adc **/
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
    
    /*** uart ***/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
    
    
    /*** lcd ***/
    LCD_Init();
    
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    /*** key tim ***/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /** led*/
    led_disp(0x00);
    
    HAL_TIM_Base_Start_IT(&htim16);

}


void loop()
{
    interface();
    adc_trans_to_temp();

    /**** key***/
    if(page == 0)
    {
        if(key[1].short_flag == 1)
        {
            
            if(strcmp(mode,"Auto") == 0)
            {
                memset(mode,0,10);
                strcpy(mode,"Manu");
            }
            else
            {
                memset(mode,0,10);
                strcpy(mode,"Auto");
            }
            
            time_1s_ = 0;
            
            key[1].short_flag = 0;
        }
        
        if(strcmp(mode,"Auto") == 0)
        {
            led_chg(8,1);
            
            if(temp < 25)
            {
                gear = 1;
            }
            if(temp >= 25 && temp <= 30)
            {
                gear = 2;
            }
            if(temp > 30)
            {
                gear = 3;
            }
        
        }
 
        
        if(strcmp(mode,"Manu") == 0)
        {
            led_chg(8,0);
            
            if(key[2].short_flag == 1)
            {
                if(gear < 3)
                {
                    gear ++;
                }
                
                key[2].short_flag = 0;
            time_1s_ = 0;                  
            }
            if(key[3].short_flag == 1)
            {
                if(gear > 1)
                {
                    gear --;
                }                
                            time_1s_ = 0;
                key[3].short_flag = 0;
            }
        }
        
        if(gear == 1)
        {
            led_chg(2,0);
            led_chg(3,0);
            led_chg(1,1);
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
        }
        if(gear == 2)
        {
            led_chg(2,1);
            led_chg(3,0);
            led_chg(1,0);            
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,200);
        }
        if(gear == 3)
        {
            led_chg(2,0);
            led_chg(3,1);
            led_chg(1,0);            
            __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,400);
        }        
        
    }
    
    if(page == 1)
    {
        for(int i=1; i<4; i++)
        {
            if(key[i].short_flag == 1)
            {
                page = 0;
                
                key[i].short_flag = 0;
                HAL_TIM_Base_Start_IT(&htim16);
            }
        }
    }
}








/********************** IT *********/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        key_serv();
    }
    if(htim->Instance == TIM17)
    {
        time_1s++;
        uint16_t led_uart = time_1s;
        sleep_time = time_1s;
        
        if(led_uart >= 300)
        {
            led_chg(4,0);
            
            time_1s = 0;
            
            HAL_TIM_Base_Stop_IT(&htim17);
        }    
    }
    if(htim->Instance == TIM16)
    {
        time_1s_++;
        sleep_time = time_1s_;
        
        if(sleep_time >= 500)
        {           
            page = 1;
            time_1s_ = 0;
            
            HAL_TIM_Base_Stop_IT(&htim16);
        }    
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    char data[10];
                time_1s_ = 0;
            HAL_TIM_Base_Start_IT(&htim16);
    if(strcmp(uart_rx,"B1") == 0)
    {
        key[1].short_flag = 1;
        led_chg(4,1);
        HAL_TIM_Base_Start_IT(&htim17);
    }
    else if(strcmp(uart_rx,"B2") == 0)
    {
        if(strcmp(mode,"Manu") == 0)
        {
        key[2].short_flag = 1;
        led_chg(4,1);
        HAL_TIM_Base_Start_IT(&htim17);  
        }            
    }
    else if(strcmp(uart_rx,"B3") == 0)
    {
        if(strcmp(mode,"Manu") == 0)
        {
        key[3].short_flag = 1;
        led_chg(4,1);
        HAL_TIM_Base_Start_IT(&htim17);  
        }            
    }    
    else
    {
        sprintf(uart_tx,"NULL");
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    
    memset(uart_rx,0,sizeof(uart_rx));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
}

