#ifndef __SPI_B_H__
#define __SPI_B_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct spi_config_struct_t {
    uint8_t data_width;
    bool ddr;
    unsigned int divider;
} spi_config_t;

static void spib_select(bool enable, unsigned int cs, Spi_Reg *spi){
    if(enable) spi_select(spi,cs);
    else spi_diselect(spi,cs);
}

static void spib_config(const spi_config_t *const spi_config,Spi_Reg *spi){
    Spi_Config cfg;
    cfg.cpol = 1;
    cfg.cpha = 1;
    cfg.mode = 0; //Assume full duplex (standard SPI)
    cfg.clkDivider = 20;
    cfg.ssSetup = 20;
    cfg.ssHold = 20;
    cfg.ssDisable = 20;
    if(spi_config){
        switch(spi_config->data_width){
            case 1: assert_true(false==spi_config->ddr);break;
            case 2: cfg.mode = spi_config->ddr ? 2:1; break;
            case 4: cfg.mode = spi_config->ddr ? 4:3; break;
            default: assert_true(false);
        }
        cfg.clkDivider = spi_config->divider;
        cfg.ssSetup = spi_config->divider;
        cfg.ssHold = spi_config->divider;
        cfg.ssDisable = spi_config->divider;
    }
    //while(spi_cmdAvailability(spi) != 0x100);
    while(!spi_idle(spi));
    spi_applyConfig(spi, &cfg);
}
static void spib_write(const void*const src, uint32_t size,Spi_Reg *spi){
    const uint8_t*const src8 = (const uint8_t*const) src;
    for(uint32_t i=0;i<size;i++){
        spi_write(spi, src8[i]);
    }
}
static void spib_read(void*const dst, uint32_t size,Spi_Reg *spi){
    uint8_t*const dst8 = (uint8_t*const) dst;
    for(uint32_t i=0;i<size;i++){
        dst8[i] = spi_read(spi);
    }
}

#endif //__SPI_B_H__
