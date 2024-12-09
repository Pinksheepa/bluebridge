#include "my_main.h"

char uart_rx[50];
char uart_tx[50];
char direction[5]={'S'};

extern struct keyblock key[];

char text[50];
uint8_t page=1;

float adc_voltage;
float distance;

uint8_t led_sta = 0x00;

uint8_t time_1s=0;
uint32_t led_time = 0;
uint8_t i = 1;

/*********** ADC **************/
float adc_trans()
{
    float adc_vol;
    
    HAL_ADC_Start(&hadc2);
    adc_vol = 3.3f * HAL_ADC_GetValue(&hadc2) / 65536.0f ;
    
    return adc_vol;
}





/****************** LED*****************/
void led_disp(uint8_t led)
{
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,led<<8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);    
}

void led_chg(uint8_t num,uint8_t sta)
{
    uint8_t pos = 0x01 << (num - 1);
    
    led_sta = (led_sta & ( ~ pos ) ) | ( pos * sta );
    
    led_disp(led_sta);

}


void interface_1()
{
    /*** adc  **/
    adc_voltage = adc_trans();
    if(adc_voltage < 3.0f)
    {
        distance = 100.0f * adc_voltage;
    }
    else
    {
        distance = 300.0f;
    }
    sprintf(text,"       D:%.1f        ",distance);
    LCD_DisplayStringLine(Line4,(uint8_t *)text);     
}

void interface_2()
{
    sprintf(text,"        WARN        ");
    LCD_DisplayStringLine(Line4,(uint8_t *)text);     

}



void setup()
{
    /*** uasrt **/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);  
/******** key *************/    
    HAL_TIM_Base_Start_IT(&htim6);
    
/******  LCD   *****/
    LCD_Init();    
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);    
    
    sprintf(text,"        DATA        ");
    LCD_DisplayStringLine(Line1,(uint8_t *)text);
    
    sprintf(text,"       N:%1s",direction);
    LCD_DisplayStringLine(Line3,(uint8_t *)text);
    sprintf(text,"       D:        ");
    LCD_DisplayStringLine(Line4,(uint8_t *)text); 

/******* ADC  ****/    
    
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
    

/*********led************/    
    led_disp(0x00);
    
    led_time = uwTick;
}

void loop()
{
    
     
    
    /**************  interface   *****/
    if(page == 1)
    {
            interface_1();
            sprintf(text,"       N:%1s",direction);
            LCD_DisplayStringLine(Line3,(uint8_t *)text);         
        if(strcmp(direction,"L") == 0)
        {
            HAL_TIM_Base_Start_IT(&htim17);      // 5s time start
            HAL_UART_AbortReceive(&huart1);    // ban reverse direction
            
            if( (uwTick - led_time  ) > 100)
            {
                static uint8_t op = 1;
                led_chg(1,op);
                op = !op;
                
                led_time = uwTick;
            }
            
            if(key[3].short_flag == 1)
            {
                HAL_TIM_Base_Stop_IT(&htim17);
                page = 1;
                direction[0] = 'S' ;

                sprintf(uart_tx,"Success\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);  

                HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50); 
                key[3].short_flag = 0;
                time_1s = 0;
            }
            
            sprintf(text,"       N:%1s",direction);
            LCD_DisplayStringLine(Line3,(uint8_t *)text); 
            
        }
        else if(strcmp(direction,"R") == 0)
        {
            HAL_TIM_Base_Start_IT(&htim17);      // 5s time start
            HAL_UART_AbortReceive(&huart1);    // ban reverse direction            
            
            if( (uwTick - led_time ) > 100)
            {
                static uint8_t opp = 1;
                led_chg(2,opp);
                opp = !opp;
                
                
                led_time = uwTick;
            }            
            if(key[4].short_flag == 1)
            {
                HAL_TIM_Base_Stop_IT(&htim17);
                page = 1;
                direction[0] = 'S' ;
                
                sprintf(uart_tx,"Success\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);                
                
                HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);                 
                key[4].short_flag = 0;
                time_1s = 0;
            } 
            sprintf(text,"       N:%1s",direction);
            LCD_DisplayStringLine(Line3,(uint8_t *)text);            
        }
        else if(strcmp(direction,"S") == 0)
        {
            led_disp(0x00);
            if(key[3].short_flag == 1)
            {
                                sprintf(uart_tx,"Warn\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50); 
                    page = 2;         
                    interface_2();                  
                key[3].short_flag = 0;
            }
            if(key[4].short_flag == 1)
            {
                sprintf(uart_tx,"Warn\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50); 
                    page = 2;         
                    interface_2();                   
               
                key[4].short_flag = 0;
            }         
        }       
    }
    else if(page == 2)
    {   
        led_disp(0x80);
        LCD_ClearLine(Line1);
        LCD_ClearLine(Line3);
        interface_2();                       
        HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
        if(key[1].short_flag == 1)      //turn round
        {
            led_disp(0x00);
            direction[0] = 'S';
            time_1s = 0;
            HAL_TIM_Base_Stop_IT(&htim17);
            page = 1;
            
                sprintf(uart_tx,"Success\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);              
            sprintf(text,"        DATA        ");
            LCD_DisplayStringLine(Line1,(uint8_t *)text);            
            sprintf(text,"       N:%s        ",direction);
            LCD_DisplayStringLine(Line3,(uint8_t *)text);
            sprintf(text,"       D:        ");
            LCD_DisplayStringLine(Line4,(uint8_t *)text);                 
            key[1].short_flag = 0;
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

/***** USART *****/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(page == 1)
    {
        
        if((strcmp(uart_rx,"R") != 0) && (strcmp(uart_rx,"L") != 0) )
        {
            sprintf(uart_tx,"ERROR\r\n");        
            HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);
            memset(uart_rx,0,sizeof(uart_rx));
        }    
        else
        {
            sscanf(uart_rx,"%1s",direction);
            
        }

        
    }
    else if(page == 2)
    {
        
        sprintf(uart_tx,"WAIT\r\n");        
        HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);         
    }
    memset(uart_rx,0,sizeof(uart_rx));
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)uart_rx,50);
}


/*********** Key scan *********/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        key_serv();
    }
    if(htim->Instance == TIM17)
    {        
        time_1s++;

        if(time_1s == 5)
        {
            if(page == 1)
            {
                sprintf(uart_tx,"Warn\r\n");            
                HAL_UART_Transmit(&huart1,(uint8_t *)uart_tx,strlen(uart_tx),50);                 
                
                page = 2;
                LCD_Clear(Black);
                time_1s = 0;
            }
         
        }
    }
    
}
