#include <stdint.h>
#include <ysyxsoc.h>

#define REG32(addr) (*(volatile uint32_t *)(addr))

#define SPI_BASE    0x10001000u
#define SPI_TX0     REG32(SPI_BASE + 0x00)
#define SPI_TX1     REG32(SPI_BASE + 0x04)
#define SPI_CTRL    REG32(SPI_BASE + 0x10)
#define SPI_DIVIDER REG32(SPI_BASE + 0x14)
#define SPI_SS      REG32(SPI_BASE + 0x18)

#define SPI_CTRL_GO     (1u << 8)
#define SPI_CTRL_TX_NEG (1u << 10)
#define SPI_CTRL_ASS    (1u << 13)

uint32_t ysyxsoc_spi_transfer(uint32_t slave_select, uint32_t tx0,
                              uint32_t tx1, uint32_t control) {
  SPI_DIVIDER = 1;
  SPI_SS = slave_select;
  SPI_TX1 = tx1;
  SPI_TX0 = tx0;

  // Program the mode before GO so the first MOSI bit is preloaded.
  SPI_CTRL = control & ~SPI_CTRL_GO;
  SPI_CTRL = control | SPI_CTRL_GO;
  while (SPI_CTRL & SPI_CTRL_GO) {
  }

  return SPI_TX0;
}

uint32_t ysyxsoc_flash_read(uint32_t offset) {
  uint32_t value = ysyxsoc_spi_transfer(
      1u << 0, 0, 0x03000000u | (offset & 0x00ffffffu),
      SPI_CTRL_ASS | SPI_CTRL_TX_NEG | 64u);

  // The flash shifts bytes in flash order; return a CPU-endian word.
  return ((value & 0x000000ffu) << 24) |
         ((value & 0x0000ff00u) << 8) |
         ((value & 0x00ff0000u) >> 8) |
         ((value & 0xff000000u) >> 24);
}

uint8_t ysyxsoc_bitrev(uint8_t value) {
  return ysyxsoc_spi_transfer(
      1u << 7, (uint32_t)value << 8, 0,
      SPI_CTRL_ASS | SPI_CTRL_TX_NEG | 16u);
}
