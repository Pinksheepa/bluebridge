#include "my_main.h"
uint8_t flip = 0;
char uart_rx[50];
char uart_tx[50];
uint8_t piece =4;
uint8_t totals;
uint8_t X = 1;
uint8_t Y = 1;
uint8_t fuckyou = 1;
uint8_t pa4_totals = 0;
uint8_t pa5_totals = 0;
float vol_pa5_max = 0;
float vol_pa5_min = 0;
float vol_pa4_max = 0;
float vol_pa4_min = 0;

float pa5_mean = 0;
float pa4_mean = 0;

/**** adc ***/
float vol_pa5;
float vol_pa4;
float temp_vol_pa5;
float temp_vol_pa4;
float freq_pa1;

float freq_pa7;
uint8_t mode = 0;

uint16_t adc_buf[2];


float adc_trans_PA4()
{
    float voltage;

    voltage = 3.3f * adc_buf[0] / 65535.0f ; 
    
    return voltage;
}

float adc_trans_PA5()
{
    float voltage;
    
    voltage = 3.3f * adc_buf[1] / 65535.0f ;
    
    return voltage;
}


/**** key **/
extern struct keyblock key[];


/*** led ***/

uint8_t led_sta = 0x00;

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
    
    led_sta = ( led_sta & (~pos) ) | ( pos * sta );

    led_disp(led_sta);
}


/*** lcd ***/
uint8_t page = 1;
char text[50];

void interface()
{
    if(flip == 1)
    {
        led_chg(4,0);
        LCD_WriteReg(R1, 0x0100);
        LCD_WriteReg(R96, 0xA700);         
    }
    else if(flip == 0)
    {
        led_chg(4,1);
        LCD_WriteReg(R1, 0x0000);
        LCD_WriteReg(R96, 0x2700);
    }
    
    if(page == 1)
    {
        LCD_ClearLine(Line7);
        LCD_ClearLine(Line3);
        sprintf(text,"        DATA   ");
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"     PA4=%.2f",vol_pa4);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     PA5=%.2f  ",vol_pa5);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
        sprintf(text,"     PA1=%4.0f        ",freq_pa1);
        LCD_DisplayStringLine(Line6,(uint8_t *)text);        
    }
    if(page == 2)
    {
        LCD_ClearLine(Line6);
        LCD_ClearLine(Line7);
        LCD_ClearLine(Line3);
        sprintf(text,"        PARA   ");
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"     X=%d            ",X);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     Y=%d           ",Y);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
    }
    if(page == 3)
    {    
        if(piece == 4)
        {
        LCD_ClearLine(Line3);
        sprintf(text,"        REC-PA4   ");
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"     N=%d     ",pa4_totals);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     A=%.2f     ",vol_pa4_max);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);
        sprintf(text,"     T=%.2f     ",vol_pa4_min);
        LCD_DisplayStringLine(Line6,(uint8_t *)text);  
        sprintf(text,"     H=%.2f     ",pa4_mean);
        LCD_DisplayStringLine(Line7,(uint8_t *)text); 
        }
        else
        {
        LCD_ClearLine(Line3);
        sprintf(text,"        REC-PA5   ");
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"     N=%d     ",pa5_totals);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     A=%.2f     ",vol_pa5_max);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);
        sprintf(text,"     T=%.2f     ",vol_pa5_min);
        LCD_DisplayStringLine(Line6,(uint8_t *)text);  
        sprintf(text,"     H=%.2f     ",pa5_mean);
        LCD_DisplayStringLine(Line7,(uint8_t *)text);         
        }
    }    
}











void setup()
{  
    /*** i2c ***/
    I2CInit();

    X = eeprom_read(1);  // freq
    Y = eeprom_read(0);  // volt    
    
    /*** input capture **/
    HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);
    
    /**** uart ***/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
    
    /*** adc**/
    HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adc_buf,2);    
    
    /*** pwm**/
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
    
    
    /*** led **/
    led_disp(0x00);
    
    /****lcd***/
    LCD_Init();
    
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /**** key ***/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /*** time ***/
    __HAL_TIM_SET_PRESCALER(&htim16,8000-1);
    __HAL_TIM_SetAutoreload(&htim16,30000-1);
    HAL_TIM_Base_Start_IT(&htim16);
    HAL_TIM_Base_Start_IT(&htim17);
}

void loop()
{ 
    interface();
    
             temp_vol_pa4 = adc_trans_PA4();
            temp_vol_pa5 = adc_trans_PA5();   
    
    if(mode == 0)   freq_pa7 = freq_pa1 * X;
    if(mode == 1)   freq_pa7 = freq_pa1 / X;
    
    __HAL_TIM_SetAutoreload(&htim3,( 1000000.0f / freq_pa7 + 1));
    __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,(( 1000000.0f / freq_pa7 + 1) / 2));
    
    if(key[1].short_flag == 1)
    {
        page ++ ;
        if(page == 4)
        {
            page = 1;
        }
        
        key[1].short_flag = 0;
    }    
   
    if(page == 1)
    {
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
            totals ++ ;
            pa4_totals ++;
            pa5_totals ++;
            
            if(pa4_totals == 1)
            {
                vol_pa4_max = vol_pa4;
                vol_pa4_min = vol_pa4;
            }
                        if(pa5_totals == 1)
            {
                vol_pa5_max = vol_pa4;
                vol_pa5_min = vol_pa4;
            }
            
            vol_pa4 = temp_vol_pa4;
            vol_pa5 = temp_vol_pa5;
            
            pa4_mean = ((pa4_totals-1)*pa4_mean) / pa4_totals;
            pa5_mean = ((pa5_totals-1)*pa5_mean) / pa5_totals;
            
            if(vol_pa4_max <= vol_pa4) vol_pa4_max = vol_pa4;
            if(vol_pa5_max <= vol_pa5) vol_pa5_max = vol_pa5;
            
            if(vol_pa4 <= vol_pa4_min) vol_pa4_min = vol_pa4;
            if(vol_pa5 <= vol_pa5_min) vol_pa5_min = vol_pa5;
            
            freq_pa1 = 10000000.0f / (HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2)  ) ;
            
            key[4].short_flag = 0;
        }
        
        
    }
    if(page == 2)
    {
        if(key[2].short_flag == 1)
        {
            X++;
            if(X == 5)
            {
                X = 1;
            }
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1)
        {
            Y++;
            if(Y == 5)
            {
                Y = 1;
            }
            key[3].short_flag = 0;
        }
        if(key[4].short_flag == 1)
        {
            if(mode == 1)   
            {
                led_chg(1,0);
                led_chg(2,1);
                mode = 0;
            }
            else if(mode == 0) 
            {
                led_chg(2,0);
                led_chg(1,1);
                mode = 1;
            }
            
            key[4].short_flag = 0;
        }         
    }
    if(page == 3)
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
            if(piece == 4)piece = 5;
            else piece =4;
            
            
            key[4].short_flag = 0;
        } 
            
        if(key[4].long_flag == 1)
        {
            if(piece == 4)
            {
             
                pa4_totals = 0;


vol_pa4_max = 0;
vol_pa4_min = 0;


pa4_mean = 0;
            
            }
            else
            {
                vol_pa5_max = 0; pa5_totals = 0;
vol_pa5_min = 0;pa5_mean = 0;
            }
            key[4].long_flag =0;
        }
    
    }
    
}

/***uart*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(strcmp(uart_rx,"X") == 0)
    {
        sprintf(uart_tx,"X:%d",X);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    if(strcmp(uart_rx,"Y") == 0)
    {
        sprintf(uart_tx,"Y:%d",Y);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    if(strcmp(uart_rx,"PA1") == 0)
    {
        sprintf(uart_tx,"PA1:%4f",freq_pa1);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    if(strcmp(uart_rx,"PA4") == 0)
    {
        sprintf(uart_tx,"PA4:%.2f",vol_pa4);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    if(strcmp(uart_rx,"PA5") == 0)
    {
        sprintf(uart_tx,"PA5:%.2f",vol_pa5);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }    
    if(strcmp(uart_rx,"#") == 0)
    {
        LCD_Clear(Black);
        if(flip == 0)   flip = 1;
        else if(flip == 1) flip = 0;
    }    
    
    
    memset(uart_rx,0,sizeof(uart_rx));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
}



/**** TIM  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if(htim->Instance == TIM6)
    {
        key_serv(); 
    }
    if(htim->Instance == TIM17)
    {
        if(vol_pa4 > (vol_pa5*Y))
        {
        if(fuckyou == 1)
        {
            led_chg(3,1);
            fuckyou = 0;
        }
        else if(fuckyou == 0)
        {
            led_chg(3,0);
            fuckyou = 1;
        }
    }
      
    }
    if(htim->Instance == TIM16)
    {
        eeprom_write(1,X);
        HAL_Delay(10);
        eeprom_write(0,Y);
    }
        
}


