#ifndef _KEY_H_
#define _KEY_H_

#include "my_main.h"

struct keyblock
{
    uint8_t age;
    uint8_t short_flag;
    
    uint8_t press;
    
    uint8_t double_flag;
    uint8_t long_flag;
    uint8_t double_age;
    uint8_t double_ageEN;
};

uint8_t key_scan(void);
void key_serv(void);


#endif

