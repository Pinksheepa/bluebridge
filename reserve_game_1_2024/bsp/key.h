#ifndef _KEY_H_
#define _KEY_H_

#include "my_main.h"

struct keyblock
{
    uint8_t age;
    
    uint8_t short_flag;
};

void key_serv(void);

#endif

