#include "my_main.h"

int blink;
uint8_t i = 1;
float volt = 0;
float freq = 1000;
float para_volt = 0.3;
float duty = 10 ;

/**********************************************************************/
/**** key *****/
extern struct keyblock key[5];

/***** led****/
uint8_t sta_led = 0x00;

void disp_led(uint8_t num)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,num << 8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);

}

void chg_led(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1);
    sta_led = ( sta_led & ~pos ) | ( sta * pos );
    disp_led(sta_led);
}

/**** lcd **/
uint8_t page = 1;

char text[50];

void interface()
{
    if(page == 1)
    {
        sprintf(text,"        DATA         ");
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        
        sprintf(text,"     VR37:%.2fV        ",volt);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     PA7:%.fHz         ",freq);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
        
    }
    else if(page == 2)
    {
        sprintf(text,"        PARA         ");
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"      VP1:%.1fV         ",para_volt);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
    
    }
}

/****** adc***/

float trans_adc(ADC_HandleTypeDef *hadc)
{
    float v ;
    HAL_ADC_Start(hadc);
    v = ( 3.3f * HAL_ADC_GetValue(hadc) / 65536.0f);

    return v;
}

/**** uart ****/
char tx_uart[50];
char rx_uart[50];





/**********************************************************************/

void setup()
{
    /**** key **/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /*** lcd****/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    
    LCD_Clear(Black);
    
    /****led***/
    disp_led(0x00);
    
    /*** adc **/
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
    
    
    /**** uart ***/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,50);
    
    /**** pwm ***/
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    /*** systick***/
    blink = uwTick;
    
}






void loop()
{
    interface();
    volt = trans_adc(&hadc2);
    __HAL_TIM_SetAutoreload(&htim17,1000000.0f / freq - 1);
    
    __HAL_TIM_SetCompare(&htim17,TIM_CHANNEL_1,(uint16_t)(duty/100.0f * (1000000.0f / freq)));
    
    if(volt > para_volt)
    {
        if(uwTick - blink > 100)
        {
            chg_led(3,i);
            i = !i;
            blink = uwTick;
        }
        
        
    }
    else
    {
        chg_led(3,0);
    }
    
    
    
    if(page == 1)
    {
        chg_led(1,1);
        chg_led(2,0);
        
        if(key[3].short_flag == 1)
        {
            freq += 1000;
            if(freq > 10000)
            {
                freq = 1000;
            }
            
            key[3].short_flag = 0;
        }        
        
        
        if(key[2].short_flag == 1) key[2].short_flag = 0;
    }
    if(page == 2)
    {
        chg_led(2,1);
        chg_led(1,0);
        
        if(key[2].short_flag == 1)
        {
            para_volt += 0.3;
            if(para_volt > 3.3)
            {
                para_volt = 0;
            }
            
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1) key[2].short_flag = 0;
    
    }
    
    
    
    
    if(key[1].short_flag == 1)
    {
        LCD_Clear(Black);
        page ++;
        if(page == 3)
        {
            page = 1;
        }
        key[1].short_flag = 0;
    }    
    


}





/***** TIM ELLAPSEPERIOD**/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        scan_key();
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    int num;
    sscanf(rx_uart,"%d",&num);
    
    if( strlen(rx_uart) == 1 && 1<= num && num <=9)
    {
        duty = num*10;
    }
    else 
    {
        sprintf(tx_uart,"Error");
        HAL_UART_Transmit_IT(&huart1,(uint8_t *)tx_uart,strlen(tx_uart));
    }
    
    num = 0;
    memset(rx_uart,0,sizeof(rx_uart));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,50);
}

