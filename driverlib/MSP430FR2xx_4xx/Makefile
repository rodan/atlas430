
REFLIB_ROOT = ../../
IGNORE_TARGET = true
include $(REFLIB_ROOT)Makefile.env
FAMILY_HEADER = $(GCC_EXTRA_INC_DIR)/msp430fr2xx_4xxgeneric.h

SRC_C = $(wildcard *.c)
SRC_H = $(wildcard *.h) $(wildcard inc/*.h) $(wildcard rom_headers/*.h)
SRC	= $(SRC_C) $(SRC_H) $(FAMILY_HEADER)

tags: $(SRC) 
	@echo "Generating tags .."
	@exuberant-ctags --recurse=yes ./ $(FAMILY_HEADER)

clean:
	@rm -f tags
