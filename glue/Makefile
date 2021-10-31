
REFLIB_ROOT = ../
IGNORE_TARGET = true
include $(REFLIB_ROOT)Makefile.env

SUBDIRS = MSP430F5xx_6xx MSP430FR2xx_4xx MSP430FR57xx MSP430FR5xx_6xx

SRC_C = $(wildcard *.c)
SRC_H = $(wildcard *.h)
SRC	= $(SRC_C) $(SRC_H)

all: tags $(SUBDIRS)

tags: $(SRC) 
	@echo "Generating tags .."
	@exuberant-ctags --recurse=no ./*

MSP430F5xx_6xx: force_look
	@$(MAKE) -C MSP430F5xx_6xx -j1 $(MFLAGS)

MSP430FR2xx_4xx: force_look
	@$(MAKE) -C MSP430FR2xx_4xx -j1 $(MFLAGS)

MSP430FR57xx: force_look
	@$(MAKE) -C MSP430FR57xx -j1 $(MFLAGS)

MSP430FR5xx_6xx: force_look
	@$(MAKE) -C MSP430FR5xx_6xx -j1 $(MFLAGS)

clean:
	@rm -f tags
	@rm -f MSP430F5xx_6xx/tags
	@rm -f MSP430FR2xx_4xx/tags
	@rm -f MSP430FR57xx/tags
	@rm -f MSP430FR5xx_6xx/tags

force_look:
	@true
