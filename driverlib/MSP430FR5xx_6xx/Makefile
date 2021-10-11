
DEVICE:=MSP430FR5994

###################### Windows OS ######################
ifeq ($(OS),Windows_NT)
	################## GCC Root Variable ###################
	GCC_DIR     ?= $(HOMEDRIVE)$(HOMEPATH)/ti/gcc
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/include
	DRIVERLIB   := msp430_driverlib_2_91_08_00/driverlib/MSP430FR5xx_6xx
	GLUELIB     := ../../glue
	LDDIR       := $(GCC_MSP_INC_DIR)/$(DEVICE)
	RM          := rd /s /q
################### Linux or Mac OS ####################
else
	################## GCC Root Variable ###################
	GCC_DIR     ?= /opt/msp430
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/msp430-elf/lib
	DRIVERLIB   := ./
	GLUELIB     := ../../glue
	LDDIR       := $(GCC_MSP_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z)
	RM          := rm -f
endif

######################################
GCC_INC_DIR      ?= /opt/msp430-gcc-support-files/include

all: tags

tags: 
	@echo "Generating tags .."
	echo $(deps)
	@exuberant-ctags --recurse=yes $(DRIVERLIB) $(GLUELIB) $(GCC_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z).h $(GCC_INC_DIR)/msp430.h $(GCC_INC_DIR)/msp430fr5xx_6xxgeneric.h

clean:
	@$(RM) *.o tags
