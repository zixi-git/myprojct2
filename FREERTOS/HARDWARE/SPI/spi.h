#ifndef SPI_H
#define SPI_H
#include <stm32f4xx.h>
#define W25Q128_SS	PBout(14)
void w25q128_read_id(uint8_t *m_id,uint8_t *d_id);
uint8_t SPI1_SendByte(uint8_t byte);
void w25q128_init(void);
void w25q128_read_data(uint8_t *data,uint32_t addr,uint8_t size);
void w25q128_SectorErase(uint32_t addr);
void w25q128_page_write(uint8_t *data,uint32_t addr);
#endif
