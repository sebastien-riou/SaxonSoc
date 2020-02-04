#ifndef __S25FL_H__
#define __S25FL_H__

#include <stdint.h>
#include <stdbool.h>

//functions for S25FL128SAGMF100 and compatible devices


//expect the following functions:
//void assert_true(bool predicate){}
//void flash_spi_select(bool enable){}
//void flash_spi_config(const spi_config_t *const spi_config){}
//void flash_spi_write(const void*const src, uint32_t size){}
//void flash_spi_read(void*const dst, uint32_t size){}

#define S25FL_WRR                0x01
#define S25FL_WRITE              0x02
#define S25FL_READ               0x03
#define S25FL_WRDIS              0x04
#define S25FL_READ_SR1           0x05
#define S25FL_WREN               0x06
#define S25FL_FAST_READ1X_SDR    0x0B
#define S25FL_READ_CR1           0x35
#define S25FL_FAST_READ2x_SDR    0x3B
#define S25FL_READ_ID            0x9F
#define S25FL_FAST_READ2x_SDR_IO 0xBB
#define S25FL_FAST_READ2x_DDR_IO 0xBD
#define S25FL_BULK_ERASE         0xC7
#define S25FL_SECTOR_ERASE       0xD8
#define S25FL_FAST_READ4x_DDR_IO 0xED

#define S25FL_SR1_WIP 1
#define S25FL_SR1_WEL 2

#define S25FL_CR1_QUAD 2

#define S25FL_SFDP_HDR_BASE 0
#define S25FL_SFDP_HDR_SIZE 0x38
#define S25FL_IDCFI_BASE 0x1000
#define S25FL_IDCFI_SIZE 0x120

static unsigned int s25fl_read_reg(uint8_t reg){
    flash_spi_select(1);
    flash_spi_write(&reg,1);
    uint8_t out;
    flash_spi_read(&out,1);
    flash_spi_select(0);
    return out;
}

static unsigned int s25fl_read_sr1(void){return s25fl_read_reg(S25FL_READ_SR1);}
static unsigned int s25fl_read_cr1(void){return s25fl_read_reg(S25FL_READ_CR1);}
static unsigned int s25fl_write_enabled(void){return (s25fl_read_sr1() & S25FL_SR1_WEL) ? 1 : 0;}
static unsigned int s25fl_quad_enabled(void){return (s25fl_read_cr1() & S25FL_CR1_QUAD) ? 1 : 0;}

static void s25fl_write_sr1cr1(uint8_t sr1,uint8_t cr1){
    flash_spi_select(1);
    flash_spi_write8(S25FL_WREN);
    flash_spi_select(0);
    assert_true(s25fl_write_enabled());
    flash_spi_select(1);
    flash_spi_write8(S25FL_WRR);
    flash_spi_write8(sr1);
    flash_spi_write8(cr1);
    flash_spi_select(0);
    while(s25fl_write_enabled());
}

static void s25fl_init(void){
    uint8_t cr1=s25fl_read_cr1();//read Configuration Register 1

    if(0==(cr1 & S25FL_CR1_QUAD)){//set CR1.QUAD
        uint8_t sr1=s25fl_read_reg(S25FL_READ_SR1);//read Status Register 1
        s25fl_write_sr1cr1(sr1,cr1|S25FL_CR1_QUAD);
    }
}

static void s25fl_bulk_erase(void) {
    flash_spi_select(1);
    flash_spi_write8(S25FL_BULK_ERASE);
    flash_spi_select(0);
}

static void s25fl_wait() {
    flash_spi_select(1);
    uint8_t status;
    do {
        status = s25fl_read_sr1();
    } while (status & S25FL_SR1_WEL);
    flash_spi_select(0);
}

static void _s25fl_write_addr(uint32_t addr){
    uint8_t*addr8 = (uint8_t*)&addr;
    flash_spi_write(addr8+2,1);
    flash_spi_write(addr8+1,1);
    flash_spi_write(addr8+0,1);
}

static void s25fl_erase_sector(uint32_t addr) {
  flash_spi_select(1);
  flash_spi_write8(S25FL_SECTOR_ERASE);
  _s25fl_write_addr(addr);
  flash_spi_select(0);
}

static void s25fl_write(const void*const src, uint32_t size, uint32_t addr){
    flash_spi_select(1);
    flash_spi_write8(S25FL_WREN);
    flash_spi_select(0);
    assert_true(s25fl_write_enabled());
    flash_spi_select(1);
    flash_spi_write8(S25FL_WRITE);
    _s25fl_write_addr(addr);
    flash_spi_write(src,size);
    flash_spi_select(0);
}

static void s25fl_read_with_config(void*const dst, uint32_t size, uint32_t addr, const spi_config_t*const config){
    flash_spi_select(1);
    const uint8_t default_op = 0x0B;
    uint8_t op;
    switch(config->data_width){
        case 1: op =  default_op;assert_true(false==config->ddr);break;
        case 2: op = config->ddr ? 0xBD : 0xBB; break;
        case 4: op = 0xED; assert_true(true==config->ddr); break;
        default: assert_true(false);
    }
	flash_spi_write8(op);
    if(default_op!=op){
        flash_spi_config(config);
    }
    _s25fl_write_addr(addr);
    uint8_t dummy=0;
    switch(op){
        case S25FL_FAST_READ1X_SDR:
        case S25FL_FAST_READ2x_SDR:
        case S25FL_FAST_READ2x_SDR_IO:
            flash_spi_read(&dummy,1);//dummy cycles
            break;
        case S25FL_FAST_READ2x_DDR_IO://0 mode, 3 dummies
            for(int i=0;i<3;i++) flash_spi_read(&dummy,1);//dummy cycles
            break;
        case S25FL_FAST_READ4x_DDR_IO://1 mode, 6 dummies
            flash_spi_write(&dummy,1);//mode cycles
            for(int i=0;i<6;i++) flash_spi_read(&dummy,1);//dummy cycles
            break;
    }
	flash_spi_read(dst,size);
	flash_spi_select(0);
    flash_spi_config(0);
}

static void _s25fl_basic_read(void*const dst, uint32_t size, uint32_t addr, uint8_t instruction) {
    flash_spi_select(1);
    flash_spi_write8(instruction);
    _s25fl_write_addr(addr);
    flash_spi_read(dst,size);
    flash_spi_select(0);
}

static void s25fl_basic_read(void*const dst, uint32_t size, uint32_t addr) {
    _s25fl_basic_read(dst,size,addr,S25FL_READ);
}

static void s25fl_read_id(void*const dst, uint32_t size,uint32_t addr) {
  _s25fl_basic_read(dst,size,addr,S25FL_READ_ID);
}

static bool s25fl_selected=false;
static uint32_t s25fl_last_addr;

static void _s25fl_read(void*const dst, uint32_t size, uint32_t addr, const uint8_t mode){
    bool set_address = false;
    if(!s25fl_selected){
        flash_spi_select(1);
        s25fl_selected = true;
        flash_spi_write8(S25FL_FAST_READ4x_DDR_IO);
        spi_config_t config;
        config.data_width = 4;
        config.ddr = true;
        flash_spi_config(&config);
        set_address=true;
    }else{
        if(addr!=s25fl_last_addr+4){
            flash_spi_select(0);
            flash_spi_select(1);
            set_address=true;
        }
    }
    if(set_address){
        _s25fl_write_addr(addr);
        flash_spi_write(&mode,1);
        uint8_t dummy;
        for(int i=0;i<6;i++) flash_spi_read(&dummy,1);//dummy cycles
    }
    s25fl_last_addr=addr+size;
    flash_spi_read(dst,size);
}

static void s25fl_read(void*const dst, uint32_t size, uint32_t addr) {
    _s25fl_read(dst,size,addr,0x5A);
}

static void s25fl_read_last(void*const dst, uint32_t size, uint32_t addr) {
    _s25fl_read(dst,size,addr,0x00);
    flash_spi_select(0);
    s25fl_selected = false;
}

static uint32_t s25fl_read32(uint32_t addr){
    uint32_t out;
    _s25fl_read(&out,4,addr,0x5A);
    return out;
}

static uint32_t s25fl_read32_last(uint32_t addr){
    uint32_t out;
    _s25fl_read(&out,4,addr,0x00);
    flash_spi_select(0);
    s25fl_selected = false;
    return out;
}

#endif //__S25FL_H__
