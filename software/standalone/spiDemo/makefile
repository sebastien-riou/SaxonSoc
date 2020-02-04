PROJ_NAME=spiDemo
STANDALONE = ..
DEBUG=yes

SRCS = 	$(wildcard src/*.c) \
		$(wildcard src/*.cpp) \
		$(wildcard src/*.S) \
        ${STANDALONE}/common/start.S

LDSCRIPT ?= ${BSP_PATH}/linker/spidemo.ld

include ${STANDALONE}/common/bsp.mk
include ${STANDALONE}/common/riscv64-unknown-elf.mk
include ${STANDALONE}/common/standalone.mk
