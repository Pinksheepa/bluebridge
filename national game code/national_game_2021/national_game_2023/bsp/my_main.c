#include "my_main.h"
extern struct keyblock key[];

float volt_pa4,volt_pa5,freq_pa1;

float group_volt_pa4[120];
float group_volt_pa5[120];
int i = 1;

int N_time = 0;
float pa4_max_A, pa5_max_A;
float pa4_min_T, pa5_min_T;
float pa4_mean_H, pa5_mean_H;


uint8_t x_freq;
uint8_t y_volt;

uint8_t channel = 0;
uint8_t mode = 0;
uint8_t blink = 0;
int time1s;
uint8_t qq;
uint8_t screen;
/***************** led **********************************/
uint8_t led_sta = 0x00;

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
    led_sta = (led_sta & ~pos) | (sta * pos);
    disp_led(led_sta);
}

/***************** lcd **********************************/
uint8_t page = 1;
char text[50];

void interface()
{
    if(page == 1)
    {
        sprintf(text,"        DATA         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        
        sprintf(text,"     PA4=%.2f        ",group_volt_pa4[i-1]);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     PA5=%.2f         ",group_volt_pa5[i-1]);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     PA1=%.f         ",freq_pa1
        );
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
        
    }
    else if(page == 2)
    {
        sprintf(text,"        PARA         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);    
 
        sprintf(text,"     X=%d         ",x_freq);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);  
        sprintf(text,"     Y=%d         ",y_volt);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);          
    }
    else if(page == 3 && channel == 0)
    {
        sprintf(text,"        REC-PA4         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);       
    
        sprintf(text,"     N=%d        ",N_time);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     A=%.2f         ",pa4_max_A);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     T=%.2f         ",pa4_min_T);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);
        sprintf(text,"     H=%.2f         ",pa4_mean_H);
        LCD_DisplayStringLine(Line6,(uint8_t *)text);        
    }
    else if(page == 3 && channel == 1)
    {
        sprintf(text,"        REC-PA5         ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);       
    
        sprintf(text,"     N=%d        ",N_time);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     A=%.2f         ",pa5_max_A);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     T=%.2f         ",pa5_min_T);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);
        sprintf(text,"     H=%.2f         ",pa5_mean_H);
        LCD_DisplayStringLine(Line6,(uint8_t *)text);        
    }    
    if(key[1].short_flag == 1)
    {
        LCD_Clear(Black);
        page++;
        if(page == 4)   page = 1;
        
        key[1].short_flag = 0;
    }
}

/***************** adc **********************************/
uint16_t adc_temp[2];

void adc_trans()
{
    volt_pa4 = (3.3f * adc_temp[0]) / (4096) ;
    volt_pa5 = (3.3f * adc_temp[1]) / (4096) ;
}

/***************** uart **********************************/
char rx_uart[50];
char tx_uart[50];



/******************************************************************************************************************************/

void setup()
{
    /***** led  ****/
    disp_led(0x00);
    
    /***** lcd ****/
    LCD_Init();
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /**** i2c ***/
    I2CInit();
    
    /*** key ****/
    HAL_TIM_Base_Start_IT(&htim6);
    
    /**** adc dma****/
    HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adc_temp,2);
    
    /***** freq ***/
    HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_2);
    
    /***** pwm  ***/
    HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);
    
    /***** eeprom ****/
    x_freq =   eeprom_read(1);
    y_volt =   eeprom_read(0);
    time1s = uwTick;

}

void loop()
{
    interface();
    adc_trans();
    
    freq_pa1 = 1000000.0f / (HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2) + 1);
 
    static uint16_t reload_counter;
    if(mode == 0)
    {
        chg_led(1,1);
        chg_led(2,0);
        reload_counter = 80000 / ( freq_pa1 * x_freq );
        __HAL_TIM_SET_PRESCALER(&htim17,reload_counter - 1);
        
    }
    else
    {
        chg_led(1,0);
        chg_led(2,1);
        reload_counter = 80000 * x_freq / ( freq_pa1  );
        __HAL_TIM_SET_PRESCALER(&htim17,reload_counter - 1);
    } 
    
    if( group_volt_pa4[i-1] > (group_volt_pa5[i-1] * y_volt) )
    {
        HAL_TIM_Base_Start_IT(&htim16);
    }
    else
    {
        HAL_TIM_Base_Stop_IT(&htim16);
        chg_led(3,0);
    }
    
    if(screen == 0)
    {
        chg_led(4,1);
    }
    else    chg_led(4,0);
    
    if(page == 1)
    {
        if(key[4].short_flag == 1)
        {
            pa5_mean_H = pa4_mean_H = 0;
            N_time++;
            group_volt_pa4[i] = volt_pa4;
            group_volt_pa5[i] = volt_pa5;
            
            if(i == 1)
            {
                pa4_max_A = pa4_min_T = group_volt_pa4[i];
                pa5_max_A = pa5_min_T = group_volt_pa5[i];
            }
            else
            {
                if(group_volt_pa4[i] >= pa4_max_A)   pa4_max_A = group_volt_pa4[i];
                else if(group_volt_pa4[i] < pa4_min_T)  pa4_min_T = group_volt_pa4[i];
            
                if(group_volt_pa5[i] >= pa5_max_A)   pa5_max_A = group_volt_pa5[i];
                else if(group_volt_pa5[i] < pa5_min_T)  pa5_min_T = group_volt_pa5[i];                
            }
            i++;
            
            for(int j = i - 1; j > 0; j --)
            {
                pa4_mean_H = pa4_mean_H + group_volt_pa4[j];
                
                pa5_mean_H = pa5_mean_H + group_volt_pa5[j];
            }
            
            pa4_mean_H = pa4_mean_H / (i - 1);
            pa5_mean_H = pa5_mean_H / (i - 1);   
            
            key[4].short_flag = 0; 
        }
        
        if(key[2].short_flag == 1)  key[2].short_flag = 0;
        if(key[3].short_flag == 1)  key[3].short_flag = 0;
    }
    
    if(page == 2)
    {
        
       if(key[2].short_flag == 1)
       {    
            x_freq ++;
            if(x_freq >= 5) x_freq = 1;
            eeprom_write(1,x_freq);           
            key[2].short_flag = 0;
       }
       if(key[3].short_flag == 1)
       {
            y_volt ++;
            if(y_volt >= 5) y_volt = 1;             
            eeprom_write(0,y_volt);
            key[3].short_flag = 0;
       }  
        if(key[4].short_flag == 1)
        {
            mode = !mode;
       
            key[4].short_flag = 0;
        }
        
        channel = 0;
    }
    if(page == 3)
    {
        
        if(key[4].short_flag == 1)
        {
            channel = !channel;
            key[4].short_flag = 0;
        }
        if(key[4].long_flag == 1)
        {
            if(channel == 0)
            {
                N_time = pa4_max_A = pa4_mean_H = pa4_min_T = 0;
            }
            else if(channel == 1)
            {
                N_time = pa5_max_A = pa5_mean_H = pa5_min_T = 0;
            }
        
            key[4].long_flag = 0;
        }
        
    
    }

    

    
    

}

/************************************************************************************************************************************************/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        scan_key();
    }
    if(htim->Instance == TIM16)
    {
        
        if(blink == 0)   chg_led(3,1);
        else    chg_led(3,0);
        blink = !blink;
    }

}


