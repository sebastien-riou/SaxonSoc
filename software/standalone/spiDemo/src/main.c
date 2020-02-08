#include <stdint.h>

#include "saxon.h"
#include "spi.h"

#define SPI ((Spi_Reg*)(SYSTEM_SPI_B_APB))
#define SPI_CS 0

void print_hex_digit(uint8_t digit){
	uart_write(UART_A, digit < 10 ? '0' + digit : 'A' + digit - 10);
}


void print_hex_byte(uint8_t byte){
	print_hex_digit(byte >> 4);
	print_hex_digit(byte & 0x0F);
}

void assert_true(int a){
	while(!a){
		GPIO_USER->OUTPUT = 0xE;
		while(1);
	}
}
void assert_eq32(uint32_t a,uint32_t b){
	assert_true(a==b);
}

void print_hex(uint32_t val, uint32_t digits)
{
	for (int i = (4*digits)-4; i >= 0; i -= 4)
		uart_write(UART_A, "0123456789ABCDEF"[(val >> i) % 16]);
}

uint32_t readUInt() {
  uint32_t r = 0;
  for(int i=0;i<4;i++) {
    while(!(UART_A->STATUS >> 24));
    uint8_t c = UART_A->DATA;
    r <<= 8;
    r |= c;
  }
  return r;
}

void print(uint8_t * data) {
  uart_writeStr(UART_A, data);
}


#define N25Q

#include "spi_b.h"
void flash_spi_select(bool enable){spib_select(enable,SPI_CS,SPI);}
void flash_spi_config(const spi_config_t *const spi_config){spib_config(spi_config,SPI);}
void flash_spi_write(const void*const src, uint32_t size){spib_write(src,size,SPI);}
void flash_spi_read(void*const dst, uint32_t size){spib_read(dst,size,SPI);}
static void flash_spi_write8(uint8_t b){flash_spi_write(&b,1);}
#ifdef S25FL
#include "s25fl.h"
static void     flash_init(void){s25fl_init();}
static void     flash_bulk_erase(void) {s25fl_bulk_erase();}
static void     flash_wait(void) {s25fl_wait();}
static void     flash_erase_sector(uint32_t addr) {s25fl_erase_sector(addr);}
static void     flash_write(const void*const src, uint32_t size, uint32_t addr){s25fl_write(src, size, addr);}
static void     flash_read_with_config(void*const dst, uint32_t size, uint32_t addr, const spi_config_t*const config){s25fl_read_with_config(dst, size, addr, config);}
static void     flash_basic_read(void*const dst, uint32_t size, uint32_t addr) {s25fl_basic_read(dst, size, addr);}
static void     flash_read_id(void*const dst, uint32_t size, uint32_t addr) {s25fl_read_id(dst, size, addr);}
static void     flash_read(void*const dst, uint32_t size, uint32_t addr) {s25fl_read(dst, size, addr);}
static void     flash_read_last(void*const dst, uint32_t size, uint32_t addr) {s25fl_read_last(dst, size, addr);}
static uint32_t flash_read32(uint32_t addr){s25fl_read32(addr);}
static uint32_t flash_read32_last(uint32_t addr){s25fl_read32_last(addr);}
#define SFDP_HDR_BASE S25FL_SFDP_HDR_BASE
#define SFDP_HDR_SIZE S25FL_SFDP_HDR_SIZE
#define IDCFI_BASE    S25FL_IDCFI_BASE
#define IDCFI_SIZE    S25FL_IDCFI_SIZE
#endif
#ifdef N25Q
#include "n25q.h"
static void     flash_init(void){n25q_init();}
static void     flash_bulk_erase(void) {n25q_bulk_erase();}
static void     flash_wait(void) {n25q_wait();}
static void     flash_erase_sector(uint32_t addr) {n25q_erase_sector(addr);}
static void     flash_write(const void*const src, uint32_t size, uint32_t addr){n25q_write(src, size, addr);}
static void     flash_read_with_config(void*const dst, uint32_t size, uint32_t addr, const spi_config_t*const config){n25q_read_with_config(dst, size, addr, config);}
static void     flash_basic_read(void*const dst, uint32_t size, uint32_t addr) {n25q_basic_read(dst, size, addr);}
static void     flash_read_id(void*const dst, uint32_t size, uint32_t addr) {n25q_read_id(dst, size, addr);}
//static void     flash_read(void*const dst, uint32_t size, uint32_t addr) {n25q_read(dst, size, addr);}
//static void     flash_read_last(void*const dst, uint32_t size, uint32_t addr) {n25q_read_last(dst, size, addr);}
//static uint32_t flash_read32(uint32_t addr){n25q_read32(addr);}
//static uint32_t flash_read32_last(uint32_t addr){n25q_read32_last(addr);}
#define SFDP_HDR_BASE N25Q_SFDP_HDR_BASE
#define SFDP_HDR_SIZE N25Q_SFDP_HDR_SIZE
#define IDCFI_BASE    N25Q_IDCFI_BASE
#define IDCFI_SIZE    N25Q_IDCFI_SIZE
#endif



Spi_Config spi_cfg;
void init(){
    //SPI init
    spi_cfg.cpol = 1;
    spi_cfg.cpha = 1;
    spi_cfg.mode = 0; //Assume full duplex (standard SPI)
    spi_cfg.clkDivider = 20;
    spi_cfg.ssSetup = 20;
    spi_cfg.ssHold = 20;
    spi_cfg.ssDisable = 20;
    spi_applyConfig(SPI, &spi_cfg);
}

void init_xip(){
    //SPI init
    spi_cfg.cpol = 1;
    spi_cfg.cpha = 1;
    spi_cfg.mode = 0; //Assume full duplex (standard SPI)
    spi_cfg.clkDivider = 0;
    spi_cfg.ssSetup = 0;
    spi_cfg.ssHold = 0;
    spi_cfg.ssDisable = 0;
    spi_applyConfig(SPI, &spi_cfg);
}

void demo_id(void){
    uart_writeStr(UART_A, "Hello world\n");

	spi_select(SPI, 0);
	spi_write(SPI, 0xAB);
	spi_write(SPI, 0x00);
	spi_write(SPI, 0x00);
	spi_write(SPI, 0x00);
	uint8_t id = spi_read(SPI);
	spi_diselect(SPI, 0);


	uart_writeStr(UART_A, "Device ID : ");
	print_hex_byte(id);
	uart_writeStr(UART_A, "\n");

    while(1){
    	uint8_t data[3];
		spi_select(SPI, 0);
		spi_write(SPI, 0x9F);
		data[0] = spi_read(SPI);
		data[1] = spi_read(SPI);
		data[2] = spi_read(SPI);
		spi_diselect(SPI, 0);

		uart_writeStr(UART_A, "CMD 0x9F : ");
		print_hex_byte(data[0]);
		print_hex_byte(data[1]);
		print_hex_byte(data[2]);
		uart_writeStr(UART_A, "\n");
    }
}


void dump_spi_regs(void){
    uint32_t*regs = (uint32_t*)SPI;
    print("SPI regs at ");print_hex(regs,8);print("\n");
    for(int i=0;i<12;i++){
        print_hex(i,2);print(": ");print_hex(regs[i],8);print("\n");
    }
    regs +=0x10;
    print("SPI regs at ");print_hex(regs,8);print("\n");
    for(int i=0;i<16;i++){
        print_hex(i,2);print(": ");print_hex(regs[i],8);print("\n");
    }
}


#define TEST_BASE 0x123440
#define TEST_DW0  0xEFCDAB89
#define TEST_DW1  0x03020100
#define TEST_DW2  0x07060504

#define XIPCFG0_DUMMY_DATA_POS 16
#define XIPCFG0_DUMMY_DATA_WIDTH 8
#define XIPCFG0_DUMMY_DATA_MASK (((1<<XIPCFG0_DUMMY_DATA_WIDTH)-1)<<XIPCFG0_DUMMY_DATA_POS)
#define XIPCFG0_DUMMY_CNT_POS 24
#define XIPCFG0_DUMMY_CNT_WIDTH 8
#define XIPCFG0_DUMMY_CNT_MASK (((1<<XIPCFG0_DUMMY_CNT_WIDTH)-1)<<XIPCFG0_DUMMY_CNT_POS)

void test_xip(void){

    //SPI->XIPCFG[0] = (SPI->XIPCFG[0] & ~XIPCFG0_DUMMY_DATA_MASK) | (0<<XIPCFG0_DUMMY_DATA_POS);
    SPI->XIPCFG[0] = (SPI->XIPCFG[0] & ~XIPCFG0_DUMMY_CNT_MASK) | (4<<XIPCFG0_DUMMY_CNT_POS);

    //SPI->XIPCFG[0] = 0x10B;
    //SPI->XIPCFG[1] = 0;
    dump_spi_regs();

    volatile const uint32_t * const test_dat = (volatile const uint32_t*const)(SYSTEM_SPI_B_BMB+TEST_BASE);
    //dummy read to erase the test area from the cache
    uint32_t tmp=test_dat[4096/4];

    for(int i=0;i<3;i++){
        print_hex(test_dat+i, 8);print(": ");print_hex(test_dat[i], 8);print("\n");
    }
    assert_eq32(TEST_DW0,test_dat[0]);
    GPIO_USER->OUTPUT = 0xB;
    assert_eq32(TEST_DW1,test_dat[1]);
    GPIO_USER->OUTPUT = 0xC;
    assert_eq32(TEST_DW2,test_dat[2]);
    GPIO_USER->OUTPUT = 0xD;
    assert_eq32(TEST_DW0,test_dat[0]);
    GPIO_USER->OUTPUT = 0x0;
    //dummy read to erase the test area from the cache
    tmp=test_dat[4096/4];
}

uint8_t data_read[0x200];

void print_info(void){
    print("SFDP header\n");
    flash_read_id(data_read, SFDP_HDR_SIZE, SFDP_HDR_BASE);
    for(int i=0;i<SFDP_HDR_SIZE;i++){
        print_hex(i,4);print(": ");print_hex(data_read[i],2);print("\n");
    }
    print("ID-CFI area\n");
    flash_read_id(data_read, IDCFI_SIZE, IDCFI_BASE);
    for(int i=0;i<IDCFI_SIZE;i++){
        print_hex(i,4);print(": ");print_hex(data_read[i],2);print("\n");
    }
    uint8_t sr1=s25fl_read_reg(S25FL_READ_SR1);
    print("SR1: ");print_hex(sr1, 2);print("\n");

    #ifdef S25FL
    uint8_t cr1=s25fl_read_cr1();
    print("CR1: ");print_hex(cr1, 2);print("\n");
    #endif

    #ifdef N25Q
    uint16_t nv_reg=n25q_read_nv_reg();
    print("NV REG: ");print_hex(nv_reg, 4);print("\n");
    #endif
}

uint32_t flash_basic_read32(uint32_t addr){
    uint32_t out;
    flash_basic_read(&out,4,addr);
    return out;
}

int test_data_valid(void){
    if(TEST_DW0!=flash_basic_read32(TEST_BASE)) return 0;
    if(TEST_DW1!=flash_basic_read32(TEST_BASE+4)) return 0;
    if(TEST_DW2!=flash_basic_read32(TEST_BASE+8)) return 0;
    return 1;
}

void exit_quad(void){
    spi_config_t config4x = {4,true,20 };
    flash_spi_config(&config4x);
    flash_spi_select(1);
    flash_spi_write8(0xF5);
    flash_spi_select(0);
    spi_config_t config1x = {1,false,20 };
    flash_spi_config(&config1x);
    n25q_write_nv_reg(0xFFFF);
}

void main() {
    dump_spi_regs();
    init();
    //exit_quad();
    //n25q_write_nv_reg(0xFFFF);
    //const spi_config_t config = {4,true,20 };
    //uint32_t val=0x55555555;
    //flash_read_with_config(&val, 4, TEST_BASE, &config);
    //print_info();
    flash_init();
    print_info();
    const uint32_t addr = TEST_BASE;
    uint32_t data_read[3];
    if(GPIO_USER->INPUT & (1<<4)){//program the expected pattern in flash
        GPIO_USER->OUTPUT = 0x0;
        GPIO_USER->OUTPUT_ENABLE = 0x0F;
        init();
        print("\nReading at ");
        print_hex(addr, 8);print("\n");
        flash_basic_read(data_read, sizeof(data_read), addr);
        print_hex(data_read[0], 8);print("\n");
        print_hex(data_read[1], 8);print("\n");
        print_hex(data_read[2], 8);print("\n");

        if(!test_data_valid()){
            print("\nErasing sector at ");
            print_hex(addr, 8);
            print("\n\n");

            flash_bulk_erase();
            flash_wait();
            assert_eq32(0xFFFFFFFF,flash_basic_read32(TEST_BASE));
            GPIO_USER->OUTPUT = 0x2;

            uint32_t data[] = {TEST_DW0,TEST_DW1,TEST_DW2};
            print("Writing flash at ");
            print_hex(addr, 8);
            print("\n");
            flash_write(data, sizeof(data), addr);
            flash_wait();
            GPIO_USER->OUTPUT = 0x3;
            print("\nReading at ");
            print_hex(addr, 8);print("\n");
            flash_basic_read(data_read, sizeof(data_read),addr);
            print_hex(data_read[0], 8);print("\n");
            print_hex(data_read[1], 8);print("\n");
            print_hex(data_read[2], 8);print("\n");
        }
        GPIO_USER->OUTPUT = 0x4;
        assert_eq32(TEST_DW0,data_read[0]);
        GPIO_USER->OUTPUT = 0x5;
        assert_eq32(TEST_DW1,data_read[1]);
        GPIO_USER->OUTPUT = 0x6;
        assert_eq32(TEST_DW2,data_read[2]);
        GPIO_USER->OUTPUT = 0x7;
        while(1);
    }
    GPIO_USER->OUTPUT = 0xA;
    GPIO_USER->OUTPUT_ENABLE = 0x0F;

    //test_xip();//no init by software, use hardwired init values
    GPIO_USER->OUTPUT = 0xA;

    flash_basic_read(data_read, sizeof(data_read),addr);
    print_hex(data_read[0], 8);print("\n");
    print_hex(data_read[1], 8);print("\n");
    print_hex(data_read[2], 8);print("\n");


    assert_eq32(TEST_DW0,flash_basic_read32(TEST_BASE));
    GPIO_USER->OUTPUT = 0x0;
    //init_xip();
    //test_xip();
    GPIO_USER->OUTPUT = 0xA;

    const spi_config_t configs[] = {
        {1,false,20},
        {2,false,20},
        {4,false,20},
    };
    for(int i=0;i<3;i++){
        uint32_t val=0x55555555;
        flash_read_with_config(&val, 4, TEST_BASE, &(configs[i]));
        print("i ");print_hex(i, 1);print(": read val=");print_hex(val, 8);print("\n");
        assert_eq32(TEST_DW0,val);
        GPIO_USER->OUTPUT = i;
    }
    /*assert_eq32(TEST_DW0,flash_read32(TEST_BASE));
    GPIO_USER->OUTPUT = 0xB;
    assert_eq32(TEST_DW1,flash_read32(TEST_BASE+4));
    GPIO_USER->OUTPUT = 0xC;
    assert_eq32(TEST_DW2,flash_read32(TEST_BASE+8));
    GPIO_USER->OUTPUT = 0xD;
    assert_eq32(TEST_DW0,flash_read32(TEST_BASE));
    GPIO_USER->OUTPUT = 0x0;*/
    test_xip();
    int cnt=0;
    while(1){
        GPIO_USER->OUTPUT = 0xF & cnt++;
    }
}
