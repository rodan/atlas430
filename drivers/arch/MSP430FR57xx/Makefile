
REFLIB_ROOT = ../../../
IGNORE_TARGET = true
include $(REFLIB_ROOT)Makefile.env
FAMILY_HEADER = $(GCC_EXTRA_INC_DIR)/msp430fr57xxgeneric.h

SRC_C = $(wildcard *.c)
SRC_H = $(wildcard *.h)
SRC	= $(SRC_C) $(SRC_H) $(FAMILY_HEADER)

tags: $(SRC) 
	@echo "Generating tags .."
	@exuberant-ctags --recurse=yes ./ ../../../driverlib/MSP430FR57xx $(FAMILY_HEADER)

clean:
	@rm -f tags
