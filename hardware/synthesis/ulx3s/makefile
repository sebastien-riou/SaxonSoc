VERILOG = ../../netlist/Ulx3sLinuxUboot.v pll_linux.v

FPGA_SIZE ?= 12

SDRAM_SIZE ?= 32

FPGA_KS ?= $(FPGA_SIZE)k

ifeq ($(FPGA_SIZE), 12)
	CHIP_ID=0x21111043
	FPGA_KS = 25k
endif
ifeq ($(FPGA_SIZE), 25)
	CHIP_ID=0x41111043
endif
ifeq ($(FPGA_SIZE), 45)
	CHIP_ID=0x41112043
endif
ifeq ($(FPGA_SIZE), 85)
	CHIP_ID=0x41113043
endif

IDCODE ?= $(CHIP_ID)

compile : bin/toplevel.bit

prog: bin/toplevel.bit
	ujprog $<

generate:
	(cd ../../..; sbt "runMain saxon.board.ulx3s.Ulx3sLinuxUboot $(SDRAM_SIZE)")

sim:
	(cd ../../..; sbt "runMain saxon.board.ulx3s.Ulx3sLinuxUbootSystemSim")

bin/toplevel.json: ${VERILOG}
	mkdir -p bin
	rm -f Ulx3sLinuxUboot.v*.bin
	cp ../../netlist/Ulx3sLinuxUboot.v*.bin . | true
	yosys \
		-p "synth_ecp5 -json $@" \
		${VERILOG}

bin/toplevel.config: bin/toplevel.json
	nextpnr-ecp5 \
		--json $< \
		--textcfg $@ \
		--lpf ulx3s_v20_linux_uboot.lpf \
		--$(FPGA_KS) \
		--freq 50 \
		--package CABGA381 \
		--lpf-allow-unconstrained

bin/toplevel.bit: bin/toplevel.config
	ecppack --compress --idcode $(IDCODE) $< $@

pll:
	ecppll \
                -i 25 \
                -o 50 \
		--clkout1 50 \
		--phase1 90 \
                -n  pll_linux\
                -f pll_linux.v
clean:
	$(RM) -rf bin *.bin
