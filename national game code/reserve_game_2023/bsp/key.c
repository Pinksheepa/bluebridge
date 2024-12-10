#include "key.h"

struct keyblock key[5]={};


uint8_t read_key()
{
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) == 0 )     return 1;
    else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == 0 )     return 2;
    else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == 0 )     return 3;
    else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == 0 )     return 4;
    else return 0;
}



void scan_key()
{
    uint8_t sta_key = read_key();
    
    if(sta_key != 0)
    {
        key[sta_key].age ++;
        if(key[sta_key].age == 2)
        {
            key[sta_key].press = 1;
        }
        
    }
    else 
    {
        for(int i = 0; i <5 ; i ++)
        {
            if(key[i].age > 200)
            {
                key[i].long_flag = 1;
            }
            
            if(key[i].press == 1 && key[i].long_flag == 0)
            {   
                key[i].short_flag = 1;
            }
            
            key[i].age = 0;
            key[i].press = 0;
            
//            if(i == 3)
//            {   
//                if(key[i].age > 50)
//                {
//                    key[i].long_flag = 1;
//                }
//                
//                if(key[i].double_ageEN == 1 && key[i].press == 1)
//                {
//                    key[i].double_flag = 1;
//                    
//                    key[i].press = 0;
//                    key[i].double_ageEN = 0;
//                }
//                if(key[i].press == 1 && key[i].long_flag == 0)
//                {
//                    key[i].double_ageEN = 1;
//                }
//                if(key[i].double_ageEN == 1)
//                {
//                    key[i].double_age ++ ;
//                }
//                if(key[i].double_age > 10)  // 1s
//                {
//                    key[i].short_flag = 1;
//                    
//                    key[i].double_age = 0;
//                    key[i].double_ageEN = 0;
//                }
//            }
//            else if(key[i].age > 50)  //0.5s
//            {
//                key[i].long_flag = 1;
//            }
//            else if(key[i].press == 1 && key[i].long_flag == 0)
//            {
//                key[i].short_flag = 1;
//            }
//            
//            key[i].age = 0;
//            key[i].press = 0;
            
        }
    
    }


}


