#include "key.h"

struct keyblock key[5];

uint8_t key_read()
{
    if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == 0 )
    {
        return 1;
    }
    else if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == 0 )
    {
        return 2;
    }
    else if( HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 0 )
    {
        return 3;
    }
    else if( HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0 )
    {
        return 4;
    }
    else return 0;

}


void key_serv()
{
    uint8_t key_sta = key_read();
    
    if(key_sta != 0)
    {
        key[key_sta].age ++ ;
        if(key[key_sta].age == 2)
        {
            key[key_sta].short_flag = 1;
        }   
        
    }
    else
    {
        for(int i =0 ; i<5; i++)
        {
            
            
            
//            if(key[i].press == 1 && key[i].double_ageEN == 1)  // the second press
//            {
//                key[i].double_flag = 1;
//                key[i].double_age = 0;
//                key[i].double_ageEN = 0;
//            }            
//            
//            
//            if(key[i].press == 1 && key[i].long_flag == 0)
//            {
//                key[i].double_ageEN = 1;
//            }
//            if(key[i].double_ageEN == 1)
//            {
//                key[i].double_age++;
//            }
//            if(key[i].double_age > 19)      // the second press > 200ms
//            {
//                key[i].short_flag = 1;
//                
//                key[i].double_age = 0;
//                key[i].double_ageEN = 0;
//            }

            
            key[i].press = 0;
            key[i].age =0;
            key[i].long_flag = 0;
        
        }    
    }
    if(key[key_sta].age > 49 ) //press time > 500ms
    {
        key[key_sta].long_flag = 1; 
    }    

}
