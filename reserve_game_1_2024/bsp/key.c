#include "key.h"

struct keyblock key[5]={0};


uint8_t key_scan()
{
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == 0)    return 1;
    else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == 0)    return 2;
    else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 0)    return 3;
    else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0)    return 4;    
    
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
            key[key_sta].short_flag = 1;
        }
    }
    else
    {
        for(int i = 0; i < 5; i++)
        {
            key[i].age = 0;
        }
    }
}
