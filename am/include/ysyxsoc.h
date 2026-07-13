#ifndef __AM_YSYXSOC_H__
#define __AM_YSYXSOC_H__

#include <stdint.h>

uint32_t ysyxsoc_spi_transfer(uint32_t slave_select, uint32_t tx0,
                              uint32_t tx1, uint32_t control);
uint32_t ysyxsoc_flash_read(uint32_t offset);
uint8_t ysyxsoc_bitrev(uint8_t value);

#endif
