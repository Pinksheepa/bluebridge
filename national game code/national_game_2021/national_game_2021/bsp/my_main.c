#include "my_main.h"
extern struct keyblock key[];

float a_angle;
float b_angle;
float freq;

float pa6_duty;
float pa7_duty;


float ax[100];
float bx[100];
char mode = 'A';

int ax_para = 20; 
int bx_para = 20;
int freq_para = 1000;

int time10ms;

uint8_t j = 2;

float angle;


/****** led ********/
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
    sta_led = ( sta_led & ~pos ) | ( pos * sta);
    disp_led(sta_led);
}

void function_led()
{
    if( (int)(ax[j-1]-ax[j-2])*((ax[j-1]-ax[j-2]) >= 0) > ax_para)
    {
        chg_led(1,1);
    }
    else    chg_led(1,0);
    if( (int)(bx[j-1]-bx[j-2])*((bx[j-1]-bx[j-2]) >= 0) > bx_para )
    {
        chg_led(2,1);
    }
    else    chg_led(2,0);
    if( freq > freq_para)
    {
        chg_led(3,1);
    }
    else    chg_led(3,0);
    
    if( mode == 'A')
    {
        chg_led(4,1);
    }
    else chg_led(4,0);
    if( angle < 10 )
    {
        chg_led(5,1);
    }
    else chg_led(5,0);
}




/****** lcd****/
uint8_t page = 0;
char text[50];

void interface()
{   
    if(page == 0)
    {
        sprintf(text,"        DATA         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"   a:%.1f         ",ax[j-1]);
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"   b:%.1f         ",bx[j-1]);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"   f:%.fHz       ",freq);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        
        
        sprintf(text,"   ax:%.f        ", ((ax[j-1]-ax[j-2]) * ( 2.0f* ( (ax[j-1]-ax[j-2]) >= 0)  - 1 )) );
        LCD_DisplayStringLine(Line6,(uint8_t *)text);
        sprintf(text,"   bx:%.f         ", ((bx[j-1]-bx[j-2]) * ( 2.0f *((bx[j-1]-bx[j-2]) >= 0)  - 1)) );
        LCD_DisplayStringLine(Line7,(uint8_t *)text);
        sprintf(text,"   mode:%c         ",mode);
        LCD_DisplayStringLine(Line8,(uint8_t *)text);        
        
    }
    else if(page == 1)
    {
        sprintf(text,"        PARA         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"   Pax:%d         ",ax_para);
        LCD_DisplayStringLine(Line2,(uint8_t *)text);
        sprintf(text,"   Pbx:%d       ",bx_para);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"   Pf:%d             ",freq_para);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
    
    }
}

/***************** adc********/


float adc_trans()
{
    float vol;
    vol = 3.3f * HAL_ADC_GetValue(&hadc2) / 65536 ;
    return vol;
}

/************ duty ****/
void duty_calcu()
{
    static uint8_t reuse = 0;
    if(uwTick - time10ms > 10)
    {
        if(reuse == 0)
        {
            pa6_duty = (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2) + 1) * 100.0f / (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1)+1) ;
            MX_TIM3_Init_PA7();
            HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
            HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_2);             
        }
        else
        {
            pa7_duty = (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1) + 1) * 100.0f / (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2)+1) ;
            MX_TIM3_Init();  
                HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_2); 
        }
        
        reuse = !reuse;
        time10ms = uwTick;
    }    
/***************** pa6 ******************/    
    if(pa6_duty <= 10)
    {
        a_angle = 0;
    }
    else if(pa6_duty >= 10 && pa6_duty <= 90)
    {
        a_angle = 2.25 * (pa6_duty - 10);
    
    }
    else if(pa6_duty > 90 && pa6_duty < 100)
    {
        a_angle = 180;
    }
    else if(pa6_duty > 100)
    {
        a_angle = 0;
    }
/***************** pa7 ******************/
    if(pa7_duty <= 10)
    {
        b_angle = 0;
    }
    else if(pa7_duty >= 10 && pa7_duty <= 90)
    {
        b_angle = 2.25 * (pa7_duty - 10);
    
    }
    else if(pa7_duty > 90 && pa7_duty < 100)
    {
        b_angle = 180;
    }
    else if(pa7_duty > 100)
    {
        b_angle = 0;
    }    
/*********************************************/
}

/***** uart *****/
char rx_uart[50];
char tx_uart[50];


float a_rank[5];
float b_rank[5];

void rank()
{
    for(int i = 0 ;i<5;i++)
    {
        a_rank[i] =ax[j-1 - i];
        b_rank[i] =bx[j-1 - i];
    }
    int i,j,temp;
    for(i=0;i<5-1;i++)
    {
        for(j=0;j<5-1-i;j++)
        {
            if(a_rank[j] > a_rank[j+1])
            {
                a_rank[j] = temp;
                a_rank[j] = a_rank[j+1];
                a_rank[j+1] = temp;
            }
            if(b_rank[j] > b_rank[j+1])
            {
                b_rank[j] = temp;
                b_rank[j] = b_rank[j+1];
                b_rank[j+1] = temp;
            }            
        }
    
    }

}






/*****************************************************************************************************************/

void setup()
{
    /**** lcd ***/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /***** led ****/
    disp_led(0x00);
    
    /***** key ****/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /***** freq ******/
    HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);
    HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_2);    
    
    /***** adc ***/
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
    
    time10ms = uwTick;
    /***** huart ***/
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,sizeof(rx_uart));

}
/******************************************************************************************************************************/
void loop()
{
    function_led();
    
    interface();
    
    duty_calcu();
    
    rank();
    
    angle = 90 - ax[j-1] + bx[j-1];
    
    if(key[1].short_flag == 1)
    {
        page = !page;
        LCD_Clear(Black);
        
        key[1].short_flag = 0;
    }
    
    if(mode == 'A')
    {
        if(key[4].short_flag == 1)
        {
            freq = 1000000.0f / (HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2) + 1);
            
            ax[j] = a_angle;
            bx[j] = b_angle;
            j++;
            
            key[4].short_flag = 0;
        }
    }
    else if(mode == 'B')
    {
        key[4].short_flag = 0;
    }
    
    if(page == 0)
    {
        if(key[3].short_flag == 1)
        {
            if(mode == 'A') mode = 'B';
            else mode = 'A';
            key[3].short_flag = 0;
        }
        
        if(key[2].short_flag == 1)  key[2].short_flag = 0;
    }
    if(page == 1)
    {
        if(key[2].short_flag == 1)
        {
            ax_para += 10;
            bx_para += 10;
            if(ax_para > 60)
            {
                ax_para = bx_para = 10;
            }
            key[2].short_flag = 0;
        }
        if(key[3].short_flag == 1)
        {
            freq_para += 1000;
            if(freq_para > 10000)
            {
                freq_para = 1000;
            }
            key[3].short_flag = 0;
        }
    }
    
    
}



/************* TIM IT ***/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        scan_key();
    }

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(strcmp(rx_uart,"a?") == 0)
    {
        sprintf(tx_uart,"a:%.1f",ax[j-1]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);
    }
    else if(strcmp(rx_uart,"b?") == 0)
    {
        sprintf(tx_uart,"b:%.1f",bx[j-1]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);    
    }
    else if( strcmp(rx_uart,"aa?") == 0 )
    {
        sprintf(tx_uart,"aa:%.1f-%.1f-%.1f-%.1f-%.1f",ax[j-5],ax[j-4],ax[j-3],ax[j-2],ax[j-1]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);      
    }
    else if(strcmp(rx_uart,"bb?") == 0)
    {
        sprintf(tx_uart,"bb:%.1f-%.1f-%.1f-%.1f-%.1f",bx[j-5],bx[j-4],bx[j-3],bx[j-2],bx[j-1]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);      
    }
    else if(strcmp(rx_uart,"qa?") == 0)
    {
        sprintf(tx_uart,"qa:%.1f-%.1f-%.1f-%.1f-%.1f",a_rank[0],a_rank[1],a_rank[2],a_rank[3],a_rank[4]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);          
        
    }
    else if(strcmp(rx_uart,"qb?") == 0)
    {
        sprintf(tx_uart,"qb:%.1f-%.1f-%.1f-%.1f-%.1f",b_rank[0],b_rank[1],b_rank[2],b_rank[3],b_rank[4]);
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);          
        
    }
    else
    {
        sprintf(tx_uart,"error");
        HAL_UART_Transmit(&huart1,(uint8_t *)tx_uart,strlen(tx_uart),50);
    }
    
    memset(rx_uart,0,sizeof(rx_uart));
    
    HAL_UARTEx_ReceiveToIdle_IT(&huart1,(uint8_t *)rx_uart,sizeof(rx_uart));
}


