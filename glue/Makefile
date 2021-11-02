
REFLIB_ROOT = ../
IGNORE_TARGET = true
include $(REFLIB_ROOT)Makefile.env

SUBDIRS = generic MSP430F5xx_6xx MSP430FR2xx_4xx MSP430FR57xx MSP430FR5xx_6xx

all: $(SUBDIRS)

MSP430F5xx_6xx: force_look
	@$(MAKE) -C MSP430F5xx_6xx -j1 $(MFLAGS)

MSP430FR2xx_4xx: force_look
	@$(MAKE) -C MSP430FR2xx_4xx -j1 $(MFLAGS)

MSP430FR57xx: force_look
	@$(MAKE) -C MSP430FR57xx -j1 $(MFLAGS)

MSP430FR5xx_6xx: force_look
	@$(MAKE) -C MSP430FR5xx_6xx -j1 $(MFLAGS)

generic: force_look
	@$(MAKE) -C generic -j1 $(MFLAGS)

force_look:
	@true
