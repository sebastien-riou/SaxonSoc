#ifndef SPI_H_
#define SPI_H_

#include "type.h"

typedef struct
{
  volatile u32 DATA;
  volatile u32 BUFFER;
  volatile u32 CONFIG;
  volatile u32 INTERRUPT;

  volatile u32 _a[4];

  volatile u32 CLK_DIVIDER;
  volatile u32 SS_SETUP;
  volatile u32 SS_HOLD;
  volatile u32 SS_DISABLE;

  volatile u32 _rfu[4];

  volatile u32 XIPCFG[4];
} Spi_Reg;

typedef struct {
	u32 cpol;
	u32 cpha;
	u32 mode;
	u32 clkDivider;
	u32 ssSetup;
	u32 ssHold;
	u32 ssDisable;
} Spi_Config;

#define SPI_CMD_WRITE (1 << 8)
#define SPI_CMD_READ (1 << 9)
#define SPI_CMD_SS (1 << 11)

#define SPI_RSP_VALID (1 << 31)

#define SPI_STATUS_CMD_INT_ENABLE = (1 << 0)
#define SPI_STATUS_RSP_INT_ENABLE = (1 << 1)
#define SPI_STATUS_CMD_INT_FLAG = (1 << 8)
#define SPI_STATUS_RSP_INT_FLAG = (1 << 9)


#define SPI_MODE_CPOL (1 << 0)
#define SPI_MODE_CPHA (1 << 1)

static u32 spi_idle(Spi_Reg *reg){
    return 3==(reg->INTERRUPT>>30);
}
static u32 spi_cmdAvailability(Spi_Reg *reg){
	return reg->BUFFER & 0xFFFF;
}
static u32 spi_rspOccupancy(Spi_Reg *reg){
	return reg->BUFFER >> 16;
}

static void spi_write(Spi_Reg *reg, u8 data){
	while(spi_cmdAvailability(reg) == 0);
	reg->DATA = data | SPI_CMD_WRITE;
}

static u8 spi_read(Spi_Reg *reg){
	while(spi_cmdAvailability(reg) == 0);
	reg->DATA = SPI_CMD_READ;
	while(spi_rspOccupancy(reg) == 0);
	return reg->DATA;
}

static void spi_select(Spi_Reg *reg, u32 slaveId){
	while(spi_cmdAvailability(reg) == 0);
	reg->DATA = slaveId | 0x80 | SPI_CMD_SS;
}

static void spi_diselect(Spi_Reg *reg, u32 slaveId){
	while(spi_cmdAvailability(reg) == 0);
	reg->DATA = slaveId | 0x00 | SPI_CMD_SS;
}

static void spi_applyConfig(Spi_Reg *reg, Spi_Config *config){
	reg->CONFIG = (config->cpol << 0) | (config->cpha << 1) | (config->mode << 4);
	reg->CLK_DIVIDER = config->clkDivider;
	reg->SS_SETUP = config->ssSetup;
	reg->SS_HOLD = config->ssHold;
	reg->SS_DISABLE = config->ssDisable;
}

#endif /* SPI_H_ */
