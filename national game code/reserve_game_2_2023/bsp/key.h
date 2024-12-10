#ifndef _KEY_H
#define _KEY_H

#include "my_main.h"

struct keyblock
{
    uint8_t age;
    uint8_t press;
    uint8_t short_flag;
    uint8_t long_flag;

};

uint8_t read_key(void);
void scan_key(void);


#endif
