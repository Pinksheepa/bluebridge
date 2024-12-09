#include "my_main.h"

uint8_t time_100ms = 0;
uint8_t limit_choose = 1;

float standard_1_up = 2.2;
float standard_1_dw = 1.2;

float standard_2_up = 3.0;
float standard_2_dw = 1.4;

float pass1;
float pass2;

uint8_t totals = 0;

uint8_t totals_1 = 0;
uint8_t totals_2 = 0;

uint8_t pass_pieces_1 = 0;
uint8_t pass_pieces_2 = 0;

/**** adc ***/
float volt1;
float volt2;

float adc1_trans()
{
    float voltage;    
    
    HAL_ADC_Start(&hadc1);
    voltage = 3.3f * HAL_ADC_GetValue(&hadc1) / 65535.0f;   
    
    return voltage;
}

float adc2_trans()
{
    float voltage;    
    
    HAL_ADC_Start(&hadc2);
    voltage = 3.3f * HAL_ADC_GetValue(&hadc2) / 65535.0f;   
    
    return voltage;
}

/*** uart**/
char uart_rx[50];
char uart_tx[50];

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
    if(page == 1)
    {
        sprintf(text,"       GOODS   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"     R37:%.2fV   ",volt1);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     R38:%.2fV   ",volt2);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
    }
    if(page == 2)
    {
        sprintf(text,"      STANDARD   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"    SR37:%.1f-%.1f   ",standard_1_dw,standard_1_up);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"    SR38:%.1f-%.1f   ",standard_2_dw,standard_2_up);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);         
    }
    if(page == 3)
    {     
        sprintf(text,"        PASS   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        char text1[50];
        sprintf(text1,"     PR37:%.1f%%   ",pass1);
        LCD_DisplayStringLine(Line3,(uint8_t *)text1);
        sprintf(text,"     PR38:%.1f%%   ",pass2);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);       
    }    
}








void setup()
{
    /*** uart ***/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
    
    /*** led **/
    led_disp(0x00);
    
    /****lcd***/
    LCD_Init();
    
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /**** key ***/
    __HAL_TIM_SetAutoreload(&htim6,5000-1);
    HAL_TIM_Base_Start_IT(&htim6);
}

void loop()
{
    interface();
    volt2 = adc1_trans();
    volt1 = adc2_trans();
    if(totals_1 == 0 && totals_2 == 0)
    {
        pass1 = pass2 = 0;
    }
    else
    {
    pass1 = 100.0f * pass_pieces_1 / totals_1;
    pass2 = 100.0f * pass_pieces_2 / totals_2;       
    }
    if(key[1].short_flag == 1)
    {
        page ++;
        if(page == 4)
        {
            page = 1;
            
        }
        interface();
        key[1].short_flag = 0;
    }
    
    if(page == 1)
    {
        led_chg(3,1);
        led_chg(4,0);
        led_chg(5,0);
        if(key[2].short_flag == 1)
        {
            if(volt1 > standard_1_up || volt1 < standard_1_dw)
            {
                totals_1 ++ ;
            }
            else
            {
                HAL_TIM_Base_Start_IT(&htim17);
                led_chg(1,1);
                totals_1 ++ ;
                pass_pieces_1 ++ ;
            }            
            totals += totals_1;
            
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1)
        {
            if(volt2 > standard_2_up || volt2 < standard_2_dw)
            {
                totals_2 ++;
            }
            else
            {
                HAL_TIM_Base_Start_IT(&htim17);
                led_chg(2,1);                
                totals_2 ++ ;
                pass_pieces_2 ++ ;            
            }       
            totals += totals_2;
            
            key[3].short_flag = 0;            
        }
    }
    else if(page == 2)
    {
                led_chg(3,0);
        led_chg(4,1);
        led_chg(5,0);
        if(key[2].short_flag == 1)
        {
            limit_choose ++;
            if( limit_choose == 5 )
            {
                limit_choose = 1;
            } 
            key[2].short_flag = 0;
        }
        
        if(limit_choose == 1)
        {
            if(key[3].short_flag == 1)
            {
                if( ( standard_1_up + 0.2 ) > 3.05 )
                {
                    standard_1_up = 2.2;
                }
                else
                standard_1_up += 0.2                                                        ;
                key[3].short_flag = 0;            
            }        
            if(key[4].short_flag == 1)
            {
                if( ( standard_1_up - 0.2 ) < 2.15 )
                {
                    standard_1_up = 3.0;
                }
                else                
                standard_1_up -= 0.2;
                key[4].short_flag = 0;            
            }        
        }
        if(limit_choose == 2)
        {
            if(key[3].short_flag == 1)
            {
                if( ( standard_1_dw + 0.2 ) > 2.05 )
                {
                    standard_1_dw = 1.2;
                }
                else                
                standard_1_dw += 0.2;
                key[3].short_flag = 0;            
            }        
            if(key[4].short_flag == 1)
            {
                if( ( standard_1_dw - 0.2 ) < 1.15 )
                {
                    standard_1_dw = 2.0;
                }
                else                 
                standard_1_dw -= 0.2;
                key[4].short_flag = 0;            
            }        
        }
        if(limit_choose == 3)
        {
            if(key[3].short_flag == 1)
            {
                if( ( standard_2_up + 0.2 ) > 3.05 )
                {
                    standard_2_up = 2.2;
                }
                else                
                standard_2_up += 0.2;
                key[3].short_flag = 0;            
            }        
            if(key[4].short_flag == 1)
            {
                if( ( standard_2_up - 0.2 ) < 2.15 )
                {
                    standard_2_up = 3.0;
                }
                else                 
                standard_2_up -= 0.2;
                key[4].short_flag = 0;            
            }        
        }
        if(limit_choose == 4)
        {
            if(key[3].short_flag == 1)
            {
                if( ( standard_2_dw + 0.2 ) > 2.05 )
                {
                    standard_2_dw = 1.2;
                }
                else                
                standard_2_dw += 0.2;
                key[3].short_flag = 0;            
            }        
            if(key[4].short_flag == 1)
            {
                if( ( standard_2_dw - 0.2 ) < 1.15 )
                {
                    standard_2_dw = 2.0;
                }
                else                                
                standard_2_dw -= 0.2;
                key[4].short_flag = 0;            
            }        
        }        
        
    }
    else if(page == 3)
    {
        led_chg(3,0);
        led_chg(4,0);
        led_chg(5,1);
        if(key[4].short_flag == 1)
        {
             totals = 0;

             totals_1 = 0;
             totals_2 = 0;

             pass_pieces_1 = 0;
             pass_pieces_2 = 0;            
            
            key[4].short_flag = 0;            
        }        
    }
    
    
    
    

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(strcmp(uart_rx,"R37") == 0)
    {
        sprintf(uart_tx,"R37:%d,%d,%.1f%%",totals_1,pass_pieces_1,pass1);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
    }
    if(strcmp(uart_rx,"R38") == 0)
    {
        sprintf(uart_tx,"R38:%d,%d,%.1f%%",totals_2,pass_pieces_2,pass2);
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
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
        time_100ms++;
        if(time_100ms == 10)
        {
            if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8) == 0)
            {
                led_chg(1,0);
            }
            if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9) == 0)
                {
                    led_chg(2,0);
                }
             time_100ms = 0;
             HAL_TIM_Base_Stop_IT(&htim17);
            
        
        }
        
        }

}

