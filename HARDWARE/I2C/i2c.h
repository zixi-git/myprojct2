#ifndef I2C_H
#define I2C_H
#include <stm32f4xx.h>
void eeprom_init(void);
void sda_pin_mode(GPIOMode_TypeDef pin_mode);
void i2c_start(void);
void i2c_stop(void);
void i2c_send_byte(uint8_t txd);
uint8_t i2c_read_byte(void);
uint8_t i2c_wait_ack(void);
void i2c_send_noack(void);
void i2c_send_ack(void);
int32_t eeprom_write(uint8_t addr,uint8_t *buf,uint32_t len);
int32_t eeprom_read(uint8_t addr,uint8_t *buf,uint32_t len);
#endif
