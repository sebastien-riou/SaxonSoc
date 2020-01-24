#include <stdint.h>

#include "saxon.h"
#include "spi.h"
//#include "spiDemo.h"
#include "spiFlash.h"

#define SPI ((Spi_Reg*)(SYSTEM_SPI_A_APB))
#define SPI_CS 0
Spi_Config spiA;
void init(){
    //SPI init
    spiA.cpol = 1;
    spiA.cpha = 1;
    spiA.mode = 0; //Assume full duplex (standard SPI)
    spiA.clkDivider = 24;
    spiA.ssSetup = 24;
    spiA.ssHold = 24;
    spiA.ssDisable = 24;
    spi_applyConfig(SPI, &spiA);
}

void print_hex_digit(uint8_t digit){
	uart_write(UART_A, digit < 10 ? '0' + digit : 'A' + digit - 10);
}


void print_hex_byte(uint8_t byte){
	print_hex_digit(byte >> 4);
	print_hex_digit(byte & 0x0F);
}

void assert_true(int a){
	while(!a){
		GPIO_A->OUTPUT = 0xE;
		while(1);
	}
}
void assert_eq32(uint32_t a,uint32_t b){
	assert_true(a==b);
}


#define FAST_READ1X_SDR 0x0B
#define FAST_READ2x_SDR 0x3B
#define FAST_READ2x_SDR_IO 0xBB
#define FAST_READ2x_DDR_IO 0xBD
#define FAST_READ4x_DDR_IO 0xED
const uint8_t mode2op[] = {FAST_READ1X_SDR,FAST_READ2x_SDR_IO,FAST_READ2x_DDR_IO,FAST_READ4x_DDR_IO};

uint32_t fast_read32(uint8_t mode, uint32_t addr){
    spi_select(SPI, 0);
	spi_write(SPI, mode2op[mode]);
    if(mode>0){
        spiA.mode = mode;
        while(spi_cmdAvailability(SPI) != 0x100);
        spi_applyConfig(SPI, &spiA);
    }
    spi_write(SPI, addr >> 16);
	spi_write(SPI, addr >>  8);
	spi_write(SPI, addr >>  0);
    switch(mode2op[mode]){
        case FAST_READ1X_SDR:
        case FAST_READ2x_SDR:
        case FAST_READ2x_SDR_IO:
            spi_write(SPI, 0);//dummy cycles
            break;
        case FAST_READ2x_DDR_IO://0 mode, 6 dummies
            spi_write(SPI, 0);//dummy cycles
            spi_write(SPI, 0);//dummy cycles
            spi_write(SPI, 0);//dummy cycles
            break;
        case FAST_READ4x_DDR_IO://1 mode, 6 dummies
            spi_write(SPI, 0);//mode cycles
            for(int i=0;i<6;i++) spi_read(SPI);//dummy cycles
            break;
    }
    /*if(mode==1){
        spiA.mode = mode;
        while(spi_cmdAvailability(SPI) != 0x100);
        spi_applyConfig(SPI, &spiA);
    }*/
	uint32_t out = spi_read(SPI);
    out = (out<<8) | spi_read(SPI);
    out = (out<<8) | spi_read(SPI);
    out = (out<<8) | spi_read(SPI);
	spi_diselect(SPI, 0);
    spiA.mode = 0;//revert to standard SPI
    spi_applyConfig(SPI, &spiA);
    return out;
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

#define SPI_FLASH_READ_SR1 0x05
#define SR1_WIP 1
#define SR1_WEL 2

#define SPI_FLASH_READ_CR1 0x35
#define CR1_QUAD 2

uint8_t spi_read_reg(uint8_t reg){
    spi_select(SPI, 0);
    spi_write(SPI, reg);
    uint8_t out=spi_read(SPI);
    spi_diselect(SPI, 0);
    return out;
}
uint8_t spi_read_sr1(void){return spi_read_reg(SPI_FLASH_READ_SR1);}
int spi_write_enabled(void){return (spi_read_sr1() & SR1_WEL) ? 1 : 0;}
#define SPI_FLASH_WREN  0x06
#define SPI_FLASH_WRDIS 0x04
#define SPI_FLASH_WRR   0x01
void spi_write_sr1cr1(uint8_t sr1,uint8_t cr1){
    spi_select(SPI, 0);
    spi_write(SPI,SPI_FLASH_WREN);
    spi_diselect(SPI, 0);
    assert_true(spi_write_enabled());
    spi_select(SPI, 0);
    spi_write(SPI, 0x01);//Write Registers (WRR 01h)
    spi_write(SPI,sr1);
    spi_write(SPI,cr1);
    spi_diselect(SPI, 0);
    while(spi_write_enabled());
    //spi_select(SPI, 0);
    //spi_write(SPI,SPI_FLASH_WRDIS);
    //spi_diselect(SPI, 0);
}
void main() {
    GPIO_A->OUTPUT = 0xA;
    GPIO_A->OUTPUT_ENABLE = 0xF;
	init();

    uint8_t cr1=spi_read_reg(SPI_FLASH_READ_CR1);//read Configuration Register 1

    if(0==(cr1 & CR1_QUAD)){//set CR1.QUAD
        uint8_t sr1=spi_read_reg(SPI_FLASH_READ_SR1);//read Status Register 1
        spi_write_sr1cr1(sr1,cr1|CR1_QUAD);
    }

    //uint8_t test[] = {0,1,0,2,0,3,0};
    uint8_t test[] = {2,3};
    for(int i=0;i<sizeof(test);i++){
        int mode=test[i];
        assert_eq32(0x89ABCDEF,fast_read32(mode,0x123456));
        GPIO_A->OUTPUT = mode;
    }

    while(1){
        GPIO_A->OUTPUT = 0xF & (GPIO_A->OUTPUT+1);
    }
}
