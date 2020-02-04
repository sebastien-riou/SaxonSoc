# Mainly from https://github.com/Digilent/Arty/blob/master/Projects/GPIO/src/constraints/Arty_Master.xdc

set_property CFGBVS VCCO [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]

create_clock -name GCLK100 -period 10.0 [get_nets GCLK100]
#TODO
#set_clock_groups -asynchronous -group pll_CLKOUT0 -group pll_CLKOUT1

# JTAG
#set_property -dict { PACKAGE_PIN T18   IOSTANDARD LVCMOS33 } [get_ports { system_cpu_tms }]; #IO_L7N_T1_D10_14 Sch=ck_io[38]
#set_property -dict { PACKAGE_PIN R18   IOSTANDARD LVCMOS33 } [get_ports { system_cpu_tdo }]; #IO_L7P_T1_D09_14 Sch=ck_io[39]
#set_property -dict { PACKAGE_PIN P18   IOSTANDARD LVCMOS33 } [get_ports { system_cpu_tdi }]; #IO_L9N_T1_DQS_D13_14 Sch=ck_io[40]
#set_property -dict { PACKAGE_PIN N17   IOSTANDARD LVCMOS33 } [get_ports { system_cpu_tck }]; #IO_L9P_T1_DQS_14 Sch=ck_io[41]
# JTAG, same as in Murax project
#set_property PACKAGE_PIN	F4	 [get_ports system_cpu_tck]
#set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tck]
#set_property PACKAGE_PIN	D2	 [get_ports system_cpu_tms]
#set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tms]
#set_property PACKAGE_PIN	D4	 [get_ports system_cpu_tdo]
#set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tdo]
#set_property PULLUP		true	 [get_ports system_cpu_tdo]
#set_property PACKAGE_PIN	E2	 [get_ports system_cpu_tdi]
#set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tdi]

# JTAG, pinout compatible straight connection to digilent-hs2 on PMOD D upper row
set_property PACKAGE_PIN	D4	 [get_ports system_cpu_tms]
set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tms]
set_property PACKAGE_PIN	D3	 [get_ports system_cpu_tdi]
set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tdi]
set_property PACKAGE_PIN	F4	 [get_ports system_cpu_tdo]
set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tdo]
set_property PACKAGE_PIN	F3	 [get_ports system_cpu_tck]
set_property IOSTANDARD LVCMOS33 [get_ports system_cpu_tck]
set_property PULLUP		true	 [get_ports system_cpu_tdo]


set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets system_cpu_tck_IBUF]

## cpu_reset:0
set_property PACKAGE_PIN C2 [get_ports cpu_reset]
set_property IOSTANDARD LVCMOS33 [get_ports cpu_reset]


# UART
set_property -dict { PACKAGE_PIN D10   IOSTANDARD LVCMOS33 } [get_ports { system_uartA_uart_txd }]; #IO_L19N_T3_VREF_16 Sch=uart_rxd_out
set_property -dict { PACKAGE_PIN A9    IOSTANDARD LVCMOS33 } [get_ports { system_uartA_uart_rxd }]; #IO_L14N_T2_SRCC_16 Sch=uart_txd_in

# GPIO A
set_property -dict { PACKAGE_PIN V15   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[0] }]; #IO_L16P_T2_CSI_B_14 Sch=ck_io[0]
set_property -dict { PACKAGE_PIN U16   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[1] }]; #IO_L18P_T2_A12_D28_14 Sch=ck_io[1]
set_property -dict { PACKAGE_PIN P14   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[2] }]; #IO_L8N_T1_D12_14 Sch=ck_io[2]
set_property -dict { PACKAGE_PIN T11   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[3] }]; #IO_L19P_T3_A10_D26_14 Sch=ck_io[3]
set_property -dict { PACKAGE_PIN R12   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[4] }]; #IO_L5P_T0_D06_14 Sch=ck_io[4]
set_property -dict { PACKAGE_PIN T14   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[5] }]; #IO_L14P_T2_SRCC_14 Sch=ck_io[5]
set_property -dict { PACKAGE_PIN T15   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[6] }]; #IO_L14N_T2_SRCC_14 Sch=ck_io[6]
set_property -dict { PACKAGE_PIN T16   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[7] }]; #IO_L15N_T2_DQS_DOUT_CSO_B_14 Sch=ck_io[7]
set_property -dict { PACKAGE_PIN N15   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[8] }]; #IO_L11P_T1_SRCC_14 Sch=ck_io[8]
set_property -dict { PACKAGE_PIN M16   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[9] }]; #IO_L10P_T1_D14_14 Sch=ck_io[9]
set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[10] }]; #IO_L18N_T2_A11_D27_14 Sch=ck_io[10]
set_property -dict { PACKAGE_PIN U18   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[11] }]; #IO_L17N_T2_A13_D29_14 Sch=ck_io[11]
set_property -dict { PACKAGE_PIN R17   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[12] }]; #IO_L12N_T1_MRCC_14 Sch=ck_io[12]
set_property -dict { PACKAGE_PIN P17   IOSTANDARD LVCMOS33 } [get_ports { system_gpioA_gpio[13] }]; #IO_L12P_T1_MRCC_14 Sch=ck_io[13]
# GPIO B
## user_led:0
set_property LOC H5 [get_ports system_gpioB_gpio[0]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[0]]
## user_led:1
set_property LOC J5 [get_ports system_gpioB_gpio[1]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[1]]
## user_led:2
set_property LOC T9 [get_ports system_gpioB_gpio[2]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[2]]
## user_led:3
set_property LOC T10 [get_ports system_gpioB_gpio[3]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[3]]
## user_sw:0
set_property LOC A8 [get_ports system_gpioB_gpio[4]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[4]]
## user_sw:1
set_property LOC C11 [get_ports system_gpioB_gpio[5]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[5]]
## user_sw:2
set_property LOC C10 [get_ports system_gpioB_gpio[6]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[6]]
## user_sw:3
set_property LOC A10 [get_ports system_gpioB_gpio[7]]
set_property IOSTANDARD LVCMOS33 [get_ports system_gpioB_gpio[7]]

# serial flash
set_property -dict { PACKAGE_PIN L13   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_flash_ss[0] }]; #IO_L6P_T0_FCS_B_14 Sch=qspi_cs
set_property -dict { PACKAGE_PIN K17   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_flash_data[0] }]; #IO_L1P_T0_D00_MOSI_14 Sch=qspi_dq[0]
set_property -dict { PACKAGE_PIN K18   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_flash_data[1] }]; #IO_L1N_T0_D01_DIN_14 Sch=qspi_dq[1]
# ## spiflash_1x:0.wp
#set_property LOC L14 [get_ports system_spiA_flash_data[2]]
#set_property IOSTANDARD LVCMOS33 [get_ports system_spiA_flash_data[2]]
# ## spiflash_1x:0.hold
#set_property LOC M14 [get_ports system_spiA_flash_data[3]]
#set_property IOSTANDARD LVCMOS33 [get_ports system_spiA_flash_data[3]]

# SDCARD
#set_property -dict { PACKAGE_PIN D4    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_ss[0] }]; #IO_L11N_T1_SRCC_35 Sch=jd[1]
#set_property -dict { PACKAGE_PIN D3    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[0] }]; #IO_L12N_T1_MRCC_35 Sch=jd[2]
#set_property -dict { PACKAGE_PIN F4    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[1] }]; #IO_L13P_T2_MRCC_35 Sch=jd[3]
#set_property -dict { PACKAGE_PIN F3    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_sclk }]; #IO_L13N_T2_MRCC_35 Sch=jd[4]
##dummy assignement to PMOD JD just because I don't know how to remove those while keep 4 pins for the spiA_flash
#set_property -dict { PACKAGE_PIN E2    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[2] }];
#set_property -dict { PACKAGE_PIN D2    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[3] }];
#assigned to Pmod A
set_property -dict { PACKAGE_PIN G13   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_ss[0]   }]; #IO_0_15 Sch=ja[1]
set_property -dict { PACKAGE_PIN B11   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[0] }]; #IO_L4P_T0_15 Sch=ja[2]
set_property -dict { PACKAGE_PIN A11   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[1] }]; #IO_L4N_T0_15 Sch=ja[3]
set_property -dict { PACKAGE_PIN D12   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_sclk    }]; #IO_L6P_T0_15 Sch=ja[4]
#dummy assignement to PMOD JA just because I don't know how to remove those while keep 4 pins for the spiA_flash
#set_property -dict { PACKAGE_PIN D13   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[2] }]; #IO_L6N_T0_VREF_15 Sch=ja[7]
#set_property -dict { PACKAGE_PIN B18   IOSTANDARD LVCMOS33 } [get_ports { system_spiA_sdcard_data[3] }]; #IO_L10P_T1_AD11P_15 Sch=ja[8]
#set_property -dict { PACKAGE_PIN A18   IOSTANDARD LVCMOS33 } [get_ports { ja[9] }]; #IO_L10N_T1_AD11N_15 Sch=ja[9]
#set_property -dict { PACKAGE_PIN E15   IOSTANDARD LVCMOS33 } [get_ports { ja[10] }]; #IO_L11P_T1_SRCC_15 Sch=ja[10]

# User SPI
set_property -dict { PACKAGE_PIN G1    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_user_data[1] }]; #IO_L17N_T2_35 Sch=ck_miso
set_property -dict { PACKAGE_PIN H1    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_user_data[0] }]; #IO_L17P_T2_35 Sch=ck_mosi
set_property -dict { PACKAGE_PIN F1    IOSTANDARD LVCMOS33 } [get_ports { system_spiA_user_sclk }]; #IO_L18P_T2_35 Sch=ck_sck
#set_property -dict { PACKAGE_PIN C1    IOSTANDARD LVCMOS33 } [get_ports { ck_ss }]; #IO_L16N_T2_35 Sch=ck_ss
#dummy assignement to PMOD JC just because I don't know how to remove those while keep 4 pins for the spiA_flash
#set_property LOC U12 [get_ports system_spiA_user_data[2]]
#set_property IOSTANDARD LVCMOS33 [get_ports system_spiA_user_data[2]]
#set_property LOC V12 [get_ports system_spiA_user_data[3]]
#set_property IOSTANDARD LVCMOS33 [get_ports system_spiA_user_data[3]]

# serial flash on PMOD JC
set_property -dict { PACKAGE_PIN U12   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_ss[0] }];
set_property -dict { PACKAGE_PIN V12   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_data[0] }];
set_property -dict { PACKAGE_PIN V10   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_data[1] }];
set_property -dict { PACKAGE_PIN V11   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_sclk }];
set_property -dict { PACKAGE_PIN T13   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_data[2] }];
set_property -dict { PACKAGE_PIN U13   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_flash_data[3] }];

# serial user on PMOD JB
##set_property -dict { PACKAGE_PIN E15   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_ss[0] }];
#set_property -dict { PACKAGE_PIN E16   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_data[0] }];
#set_property -dict { PACKAGE_PIN D15   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_data[1] }];
#set_property -dict { PACKAGE_PIN C15   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_sclk }];
#set_property -dict { PACKAGE_PIN K15   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_data[2] }];
#set_property -dict { PACKAGE_PIN J15   IOSTANDARD LVCMOS33 } [get_ports { system_spiB_user_data[3] }];



set_property -dict { PACKAGE_PIN U11   IOSTANDARD LVCMOS33 } [get_ports { inWfi }]; #IO_L19N_T3_A09_D25_VREF_14 Sch=ck_io[26]


set_property -dict { PACKAGE_PIN E3    IOSTANDARD LVCMOS33 } [get_ports GCLK100]

create_clock -period 3.333 -name {sdramDomain_phyA_sdram_DQS[0]} -waveform {0.000 1.667} [get_ports {sdramDomain_phyA_sdram_DQS[0]}]
create_clock -period 3.333 -name {sdramDomain_phyA_sdram_DQS[1]} -waveform {0.000 1.667} [get_ports {sdramDomain_phyA_sdram_DQS[1]}]

set_property INTERNAL_VREF 0.675 [get_iobanks 34]

set_property -dict { PACKAGE_PIN R2 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[0]]
set_property -dict { PACKAGE_PIN M6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[1]]
set_property -dict { PACKAGE_PIN N4 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[2]]
set_property -dict { PACKAGE_PIN T1 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[3]]
set_property -dict { PACKAGE_PIN N6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[4]]
set_property -dict { PACKAGE_PIN R7 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[5]]
set_property -dict { PACKAGE_PIN V6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[6]]
set_property -dict { PACKAGE_PIN U7 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[7]]
set_property -dict { PACKAGE_PIN R8 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[8]]
set_property -dict { PACKAGE_PIN V7 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[9]]
set_property -dict { PACKAGE_PIN R6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[10]]
set_property -dict { PACKAGE_PIN U6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[11]]
set_property -dict { PACKAGE_PIN T6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[12]]
set_property -dict { PACKAGE_PIN T8 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ADDR[13]]


set_property -dict { PACKAGE_PIN R1 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_BA[0]]
set_property -dict { PACKAGE_PIN P4 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_BA[1]]
set_property -dict { PACKAGE_PIN P2 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_BA[2]]
set_property -dict { PACKAGE_PIN P3 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_RASn]
set_property -dict { PACKAGE_PIN M4 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_CASn]
set_property -dict { PACKAGE_PIN P5 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_WEn]
set_property -dict { PACKAGE_PIN U8 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_CSn]
set_property -dict { PACKAGE_PIN L1 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_DM[0]]
set_property -dict { PACKAGE_PIN U1 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_DM[1]]

set_property -dict { PACKAGE_PIN K5 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 0]]
set_property -dict { PACKAGE_PIN L3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 1]]
set_property -dict { PACKAGE_PIN K3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 2]]
set_property -dict { PACKAGE_PIN L6 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 3]]
set_property -dict { PACKAGE_PIN M3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 4]]
set_property -dict { PACKAGE_PIN M1 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 5]]
set_property -dict { PACKAGE_PIN L4 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 6]]
set_property -dict { PACKAGE_PIN M2 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 7]]
set_property -dict { PACKAGE_PIN V4 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 8]]
set_property -dict { PACKAGE_PIN T5 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[ 9]]
set_property -dict { PACKAGE_PIN U4 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[10]]
set_property -dict { PACKAGE_PIN V5 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[11]]
set_property -dict { PACKAGE_PIN V1 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[12]]
set_property -dict { PACKAGE_PIN T3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[13]]
set_property -dict { PACKAGE_PIN U3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[14]]
set_property -dict { PACKAGE_PIN R3 IOSTANDARD SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQ[15]]


set_property -dict { PACKAGE_PIN N2 IOSTANDARD DIFF_SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQS[0]]
set_property -dict { PACKAGE_PIN U2 IOSTANDARD DIFF_SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQS[1]]
set_property -dict { PACKAGE_PIN N1 IOSTANDARD DIFF_SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQSn[0]]
set_property -dict { PACKAGE_PIN V2 IOSTANDARD DIFF_SSTL135 SLEW FAST IN_TERM UNTUNED_SPLIT_50 } [get_ports sdramDomain_phyA_sdram_DQSn[1]]

set_property -dict { PACKAGE_PIN U9 IOSTANDARD DIFF_SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_CK]
set_property -dict { PACKAGE_PIN V9 IOSTANDARD DIFF_SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_CKn]
set_property -dict { PACKAGE_PIN N5 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_CKE]
set_property -dict { PACKAGE_PIN R5 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_ODT]
set_property -dict { PACKAGE_PIN K6 IOSTANDARD SSTL135 SLEW FAST } [get_ports sdramDomain_phyA_sdram_RESETn]
