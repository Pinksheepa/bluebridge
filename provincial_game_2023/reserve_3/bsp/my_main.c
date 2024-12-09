#include "my_main.h"
    uint8_t led=0;
    uint8_t i=0;
float change_fre = 4000;

uint16_t time_100ms = 0;

uint16_t v_holdtime;

uint8_t key2EN = 1;

uint16_t time_10ms = 0;

char pwm_mode[10] = {'L'};
uint8_t lock;
uint8_t duty;
float pulse;

float velocity;
float freq;

uint8_t R = 1;
uint8_t K = 1;

uint8_t totals = 0;
float vh_max;
float vl_max;

uint8_t para_choose = 1;

        uint8_t R_temp = 1;
        uint8_t K_temp = 1;

uint8_t adc_lock = 0;
float autoload = 249;
/**** adc ***/
float volt;


float adc_trans()
{
    float voltage;
    
    HAL_ADC_Start(&hadc2);
    
    voltage = 3.3f * HAL_ADC_GetValue(&hadc2) / 65535.0f ;
    
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
    if(page == 1)
    {
        sprintf(text,"        DATA   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"     M=%1s",pwm_mode);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     P=%2d%%   ",duty);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
        sprintf(text,"     V=%.1f   ",velocity);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
    }
    if(page == 2)
    {
        LCD_ClearLine(Line5);
        
        sprintf(text,"        PARA   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"     R=%d   ",R_temp);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     K=%d   ",K_temp);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);        
    }
    if(page == 3)
    {     
        sprintf(text,"        RECD   ");
        LCD_DisplayStringLine(Line1,(uint8_t *)text);
        sprintf(text,"     N=%d     ",totals);
        LCD_DisplayStringLine(Line3,(uint8_t *)text);
        sprintf(text,"     MH=%.1f     ",vh_max);
        LCD_DisplayStringLine(Line4,(uint8_t *)text);
        sprintf(text,"     ML=%.1f     ",vl_max);
        LCD_DisplayStringLine(Line5,(uint8_t *)text);        
    }    
}











void setup()
{
    /*** input capture **/
    HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_2);
    HAL_TIM_IC_Start(&htim3,TIM_CHANNEL_1);
    
    
    /**** pwm ***/
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
    
    
    /*** led **/
    led_disp(0x00);
    
    /****lcd***/
    LCD_Init();
    
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);
    LCD_Clear(Black);
    
    /**** key ***/
    HAL_TIM_Base_Start_IT(&htim6);
    
    v_holdtime = uwTick;
}

void loop()
{
    interface();
    volt = adc_trans();
    pulse = duty * autoload / 100.0f ;
    
    
    if(adc_lock == 0)
    {
        if(volt < 1)
        {
            duty = 10;        
        }
        else if(volt >= 1 && volt <= 3)
        {
            duty = 37.5f * volt - 27.5;
        }
        else if(volt > 3)
        {
            duty = 85;
        }
        __HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,pulse);
    }
    
    freq = 1000000.0f / (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2) ) ;
    //duty = 1 + 100.0f * (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1) ) / (HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2) );
    
    velocity = freq * 2 * 3.14 * R / 100.0f / K;  
    
    if(strcmp(pwm_mode,"H") == 0)
    {
            if(vh_max < velocity)
            {
                if(lock == 0)
                {
                    lock = 1;
                    HAL_TIM_Base_Start_IT(&htim16);                
                }
            }
        
        
    }
    else if(strcmp(pwm_mode,"L") == 0)
    {
            if(vl_max < velocity)
            {
                if(lock == 0)
                {
                    lock = 1;
                HAL_TIM_Base_Start_IT(&htim16);
                    }
            }
    }
    
    
    if(key[1].short_flag == 1)
    {
        if(page == 3)
        {
            page = 1;
        }
        else
        {
            page ++ ;
        }
        
        key[1].short_flag = 0;
    }
    
    if(page == 1)
    {
        led_chg(1,1);
        
        
        if(key[2].short_flag == 1)
        {    
            if(key2EN == 1)
            {
            totals ++;
            HAL_TIM_Base_Start_IT(&htim17);
            key2EN = 0;
            }
            key[2].short_flag = 0;
        }
    
        if(key[4].long_flag == 1)
        {
            led_chg(3,1);
            adc_lock = 1;
            
            key[4].long_flag = 0;
        }
        else if(key[4].short_flag == 1)
        {
            led_chg(3,0);
            adc_lock = 0;
            
            key[4].short_flag = 0;
        }
        
        if(key[3].short_flag == 1)
        {
            key[3].short_flag = 0;
        }
    
    }
    if(page == 2)
    {

        led_chg(1,0);

        if(key[2].short_flag == 1)
        {
            para_choose = !para_choose;    
            key[2].short_flag = 0;
        }
        if(para_choose == 1)
        {
            if(key[3].short_flag == 1)
            {
                R_temp ++ ;
                if(R_temp == 11)
                {
                    R_temp = 1;
                }
                key[3].short_flag = 0;
            }
            if(key[4].short_flag == 1)
            {
                R_temp --;
                if(R_temp == 0)
                {
                    R_temp = 10;
                }
                key[4].short_flag = 0;
            }            
        }
        if(para_choose == 0)
        {
            if(key[3].short_flag == 1)
            {
                K_temp ++ ;
                if(K_temp == 11)
                {
                    K_temp = 1;
                }
                key[3].short_flag = 0;
            }
            if(key[4].short_flag == 1)
            {
                K_temp --;
                if(K_temp == 0)
                {
                    K_temp = 10;
                }
                key[4].short_flag = 0;
            }            
        }

        if(key[4].long_flag == 1)
        {
            key[4].long_flag = 0;
        }
        
    }
    
    if(page == 3)
    {
        para_choose = 1;
        R = R_temp;
        K = K_temp;
        led_chg(1,0);
    
        for(int i=0; i<5 ; i++)
        {
            key[i].short_flag = 0;
            key[i].long_flag = 0;
        }
        
        
    }
    
    
    
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
            if(time_10ms == ( i * 10) )
            {
                if(led == 0)
                {
                    led = 1;
                    led_chg(2,1);
                            
                }
                else
                {
                    led = 0;
                    led_chg(2,0);
                           
                }
                         if(strcmp(pwm_mode,"H") == 0)
        {
            change_fre -= 80;
            autoload = 1000000 / change_fre ;
            __HAL_TIM_SetAutoreload(&htim2,autoload-1);
        } 
        else if(strcmp(pwm_mode,"L") == 0)
        {
            change_fre += 80;
            autoload = 1000000 / change_fre ;
            __HAL_TIM_SetAutoreload(&htim2,autoload-1);
        } 
                i++;
            } 
            
        time_10ms ++;
        


        if(time_10ms == 500)
        {
            key2EN = 1;
            led_chg(2,0);
            
            i=0;
            led=0;
            time_10ms = 0;
            HAL_TIM_Base_Stop_IT(&htim17);
            
            if(strcmp(pwm_mode,"H") == 0)   
            {
                change_fre = 4000;
                pwm_mode[0] = 'L';
            }
            else if(strcmp(pwm_mode,"L") == 0)   
            {
                change_fre = 8000;
                pwm_mode[0] = 'H';
            }
        }
      
    }
    if(htim->Instance == TIM16)
    {
        time_100ms ++;
        
        if(time_100ms > 20)
        {
            if(strcmp(pwm_mode,"H") == 0 && vh_max < velocity)
            {
                vh_max = velocity;
            }
            if(strcmp(pwm_mode,"L") == 0 && vl_max < velocity)
            {
                vl_max = velocity;
            }            
            
            lock = 0;
            HAL_TIM_Base_Stop_IT(&htim16);
            time_100ms = 0;
        }
    }
        
}


