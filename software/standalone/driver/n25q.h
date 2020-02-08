#ifndef __N25Q_H__
#define __N25Q_H__

#include <stdint.h>
#include <stdbool.h>

//functions for N25Q256A13ESF40G and compatible devices


//expect the following functions:
//void assert_true(bool predicate){}
//void flash_spi_select(bool enable){}
//void flash_spi_config(const spi_config_t *const spi_config){}
//void flash_spi_write(const void*const src, uint32_t size){}
//void flash_spi_read(void*const dst, uint32_t size){}

#include "s25fl.h"

#define N25Q_SFDP_HDR_BASE S25FL_SFDP_HDR_BASE
#define N25Q_SFDP_HDR_SIZE 0x11
#define N25Q_IDCFI_BASE S25FL_IDCFI_BASE
#define N25Q_IDCFI_SIZE 0x11


#define N25Q_READ_NV_REG         0xB5
#define N25Q_WRITE_NV_REG        0xB1

#define N25Q_NV_REG_QUAD 8

static unsigned int n25q_read_nv_reg(void){
    uint8_t reg = N25Q_READ_NV_REG;
    flash_spi_select(1);
    flash_spi_write(&reg,1);
    uint16_t out;
    uint8_t*out8 = (uint8_t*)&out;
    flash_spi_read(out8+1,1);
    flash_spi_read(out8,1);
    flash_spi_select(0);
    return out;
}

static void n25q_write_nv_reg(uint16_t nv_reg){
    //skip it to avoid misconfiguring devices
/*    flash_spi_select(1);
    flash_spi_write8(S25FL_WREN);
    flash_spi_select(0);
    assert_true(s25fl_write_enabled());
    flash_spi_select(1);
    flash_spi_write8(N25Q_WRITE_NV_REG);
    flash_spi_write8(nv_reg>>8);
    flash_spi_write8(nv_reg & 0xFF);
    flash_spi_select(0);
    while(s25fl_write_enabled());*/
}

static void n25q_init(void){
    print("STATUS REG=");print_hex(s25fl_read_sr1(),2);print("\n");
    print("STATUS REG=");print_hex(s25fl_read_sr1(),2);print("\n");
    uint16_t r=n25q_read_nv_reg();
    print("NV REG=");print_hex(r,4);print("\n");
    if(r & N25Q_NV_REG_QUAD){
        print("Set Quad\n");
        r = r & ~N25Q_NV_REG_QUAD;
        n25q_write_nv_reg(r);
        r=n25q_read_nv_reg();
        print("NV REG=");print_hex(r,4);print("\n");

    }
}

static void n25q_read_with_config(void*const dst, uint32_t size, uint32_t addr, const spi_config_t*const config){
    flash_spi_select(1);
    const uint8_t default_op = 0x0B;
    uint8_t op;
    int dummy_cycles=1;
    switch(config->data_width){
        case 1: op = default_op;assert_true(false==config->ddr);break;
        case 2: op = config->ddr ? 0x3D : 0xBB; dummy_cycles = 2;break;
        case 4: op = config->ddr ? 0xED : 0xEB; dummy_cycles = 5; break;
        default: assert_true(false);
    }
	flash_spi_write8(op);
    if((default_op!=op) && (0==config->ddr)){
        flash_spi_config(config);
    }
    _s25fl_write_addr(addr);
    if((default_op!=op) && (config->ddr)){//for DDR we use the output direction only, input is tricky to get to work, depends on PCB tracks length
        assert_true(0);//cannot get DDR to work for now
        dummy_cycles = 16;
        flash_spi_config(config);
    }
    uint8_t dummy=0;
    for(int i=0;i<dummy_cycles;i++) flash_spi_read(&dummy,1);//dummy cycles
	flash_spi_read(dst,size);
    flash_spi_config(0);
	flash_spi_select(0);
}

static void     n25q_bulk_erase(void) {s25fl_bulk_erase();}
static void     n25q_wait(void) {s25fl_wait();}
static void     n25q_erase_sector(uint32_t addr) {s25fl_erase_sector(addr);}
static void     n25q_write(const void*const src, uint32_t size, uint32_t addr){s25fl_write(src, size, addr);}
//static void     n25q_read_with_config(void*const dst, uint32_t size, uint32_t addr, const spi_config_t*const config){s25fl_read_with_config(dst, size, addr, config);}
static void     n25q_basic_read(void*const dst, uint32_t size, uint32_t addr) {s25fl_basic_read(dst, size, addr);}
static void     n25q_read_id(void*const dst, uint32_t size, uint32_t addr) {s25fl_read_id(dst, size, addr);}
//static void     n25q_read(void*const dst, uint32_t size, uint32_t addr) {s25fl_read(dst, size, addr);}
//static void     n25q_read_last(void*const dst, uint32_t size, uint32_t addr) {s25fl_read_last(dst, size, addr);}
//static uint32_t n25q_read32(uint32_t addr){s25fl_read32(addr);}
//static uint32_t n25q_read32_last(uint32_t addr){s25fl_read32_last(addr);}


#endif //__N25Q_H__
