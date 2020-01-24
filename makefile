ZEPHYR=../zephyr/zephyrSpinalHdl
SHELL=/bin/bash
NETLIST_DEPENDENCIES=$(shell find hardware/scala -type f)
.ONESHELL:
ROOT=$(shell pwd)

all:
	make -C software/standalone/spiDemo clean all BSP=Arty7Linux
	sbt "runMain saxon.board.digilent.Arty7Linux"

saxonUp5kEvn_prog_icecube2:
	iceprog -o 0x00000 hardware/synthesis/icecube2/icecube2_Implmnt/sbt/outputs/bitmap/SaxonUp5kEvn_bitmap.bin

saxonUp5kEvn_prog_icecube2_soft:
	iceprog -S -o 0x00000 hardware/synthesis/icecube2/icecube2_Implmnt/sbt/outputs/bitmap/SaxonUp5kEvn_bitmap.bin


saxonUp5kEvn_prog_demo: software/bootloader
	iceprog -o 0x100000 software/bootloader/up5kEvnDemo.bin


.PHONY: software/bootloader
software/bootloader:
	source ${ZEPHYR}/zephyr-env.sh
	make -C software/bootloader all
