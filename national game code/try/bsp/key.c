#include "key.h"

struct keyblock key[5]={0};

uint8_t key_scan()
{
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == 0)     return 1;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == 0)     return 2;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 0)     return 3;
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0)     return 4;
    else return 0;
}

void key_serv()
{
    uint8_t key_sta = key_scan();
    
    if(key_sta != 0)
    {
        key[key_sta].age ++;
        if(key[key_sta].age == 2)
        {
            key[key_sta].press = 1;
        }
    
    }
    else 
    {

       
        for(int i = 0; i < 5; i++ )
        {
            if(i == 3)
            {
                    if(key[i].double_ageEN == 1 && key[i].press == 1)
            {
                key[i].double_flag = 1;
                
                key[i].press = 0;
                key[i].double_ageEN = 0;
            }
            
            if(key[i].press == 1 && key[i].long_flag == 0 )
            {
                key[i].double_ageEN = 1;
            }
            if(key[i].double_ageEN == 1)
            {
                key[i].double_age ++ ;
            }
            if(key[i].double_age > 30)
            {
                key[i].short_flag = 1;
                
                key[i].double_age = 0;
                key[i].double_ageEN = 0;
            }
        }
            
            if(key[i].press == 1 && key[i].long_flag == 0)
            {
                key[i].short_flag = 1;
            }
            
            key[i].press = 0;
            key[i].age = 0;
        }
    
    }
    if(key[key_sta].age == 100)  //1000ms
    {
        key[key_sta].long_flag = 1;
    }
    
}
