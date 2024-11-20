#ifndef DH11_H
#define DH11_H
#include <stm32f4xx.h>
void dht11_init(void);
int32_t dht11_read(uint8_t *buf);
#endif
