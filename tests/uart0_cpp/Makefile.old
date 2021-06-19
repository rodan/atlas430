#==============================================================================
#
# Makefile for building MSP430 DriverLib Code examples from command line
# environment using the GCC Open Source Compiler for MSP430
#
#==============================================================================

DEVICE:=MSP430FR5994
PROJ:=main
SUBDIRS:= 
OBJ_DIR:=build

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
	DRIVERLIB   := ../../driverlib
	GLUELIB     := ../../glue
	LDDIR       := $(GCC_MSP_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z)
	RM          := rm -f
endif

######################################
GCC_BIN_DIR      ?= $(GCC_DIR)/bin
GCC_INC_DIR      ?= $(GCC_DIR)/msp430-elf/include
######################################
CC               := $(GCC_BIN_DIR)/msp430-elf-gcc
GDB	             := $(GCC_BIN_DIR)/msp430-elf-gdb
######################################

# -fdata-sections, -ffunction-sections and -Wl,--gc-sections -Wl,-s
# are used for dead code elimination, see:
# http://gcc.gnu.org/ml/gcc-help/2003-08/msg00128.html

CFLAGS           := -D__$(DEVICE)__ -mmcu=$(DEVICE) # -g -DDEPRECATED
CFLAGS           += -Wall -Wno-switch
CFLAGS		     += -fno-force-addr -finline-limit=1 -fno-schedule-insns
CFLAGS		     += -fshort-enums -Wl,-Map=output.map
LDFLAGS          := -T $(LDDIR).ld -L $(GCC_MSP_INC_DIR) -mmcu=$(DEVICE) -mhwmult=f5series -g -Wl,--gc-sections
CFLAGS_REL	     += -Os -fdata-sections -ffunction-sections -fomit-frame-pointer
LDFLAGS_REL	     += -Wl,-s
CFLAGS_DBG	     += -O1 -ggdb
LDFLAGS_DBG	     += 
INCLUDES         := -I $(GCC_MSP_INC_DIR) -I $(GCC_INC_DIR) -I $(DRIVERLIB) -I $(GLUELIB) -I ./
######################################
DRIVERLIB_SRC    := $(wildcard $(DRIVERLIB)/*.c)
GLUELIB_SRC      := $(wildcard $(GLUELIB)/*.c)
LOCAL_SRC        := $(wildcard *.cpp)
PROJ_SRC         := $(PROJ).cpp
SRC              := $(PROJ_SRC) $(DRIVERLIB_SRC) $(GLUELIB_SRC) $(LOCAL_SRC)

DRIVERLIB_OBJECTS = $(DRIVERLIB_SRC:$(DRIVERLIB)/%.c=$(OBJ_DIR)/%.o)
GLUELIB_OBJECTS   = $(GLUELIB_SRC:$(GLUELIB)/%.c=$(OBJ_DIR)/%.o)
LOCAL_OBJECTS     = $(LOCAL_SRC:%.cpp=$(OBJ_DIR)/%.o)

#DRIVERLIB_OBJECTS:= $(patsubst %.c,%.o,$(DRIVERLIB_SRC))
#GLUELIB_OBJECTS  := $(patsubst %.c,%.o,$(GLUELIB_SRC))
#LOCAL_OBJECTS    := $(patsubst %.c,%.o,$(LOCAL_SRC))
EXOBJECT         := $(OBJ_DIR)/$(PROJ).o
EXOUTPUT         := $(OBJ_DIR)/$(PROJ).elf
######################################

# Append specific CFLAGS/LDFLAGS
DEBUG := $(shell grep "^\#define CONFIG_DEBUG" config.h)
ifeq ($(DEBUG),)
TARGET	:= RELEASE
CFLAGS	+= $(CFLAGS_REL)
LDFLAGS	+= $(LDFLAGS_REL)
else
TARGET	:= DEBUG
CFLAGS	+= $(CFLAGS_DBG)
LDFLAGS	+= $(LDFLAGS_DBG)
endif

.PHONY: all
.PHONY: clean
.PHONY: install
.PHONY: config
.PHONY: depend
.PHONY: driverlib
.PHONY: doc
.PHONY: cflags_update

all: depend config.h tags driverlib $(EXOUTPUT)

depend:	$(OBJ_DIR)/$(PROJ).dep

$(OBJ_DIR)/$(PROJ).dep: $(SRC)
	@echo "Generating dependencies.."
	@mkdir -p $(OBJ_DIR)
	@touch $@
	@makedepend $(INCLUDES) -Y -f $@ $^ &> /dev/null
	@grep -v ^\# $(OBJ_DIR)/$(PROJ).dep | cut -d" " -f2 | sort -u | xargs > $(OBJ_DIR)/$(PROJ).hdep
	@echo "tags: `cat $(OBJ_DIR)/$(PROJ).hdep`" >> $(OBJ_DIR)/$(PROJ).dep
	@rm -f $@.bak

config.h:
	@echo "config.h file is missing" && false

tags: $(PROJ_SRC) $(DRIVERLIB_SRC) $(GLUELIB_SRC) $(LOCAL_SRC) $(OBJ_DIR)/$(PROJ).dep
	@echo "Generating tags .."
	echo $(deps)
	@exuberant-ctags --recurse=yes ./ $(DRIVERLIB) $(GLUELIB) $(GCC_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z).h $(shell cat $(OBJ_DIR)/$(PROJ).hdep)

$(OBJ_DIR)/%.o: $(DRIVERLIB)/%.c
	@echo -e ">> build driverlib /$<"
	mkdir -p $(OBJ_DIR)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@
	#$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(BUILDDIR)/$(@F)

$(OBJ_DIR)/%.o: $(GLUELIB)/%.c
	@echo -e ">> build gluelib /$<"
	mkdir -p $(OBJ_DIR)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo -e "\n>> local/$<"
	mkdir -p $(OBJ_DIR)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOBJECT): $(PROJ_SRC) $(OBJ_DIR)/$(PROJ).cflags
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOUTPUT): $(DRIVERLIB_OBJECTS) $(GLUELIB_OBJECTS) $(LOCAL_OBJECTS) $(EXOBJECT)
	@echo -e "\n>> Building $@ as target $(TARGET)"
	$(CC) $(LDFLAGS) $(DRIVERLIB_OBJECTS) $(GLUELIB_OBJECTS) $(LOCAL_OBJECTS) -o $@ && size $@
#	$(CC) $(LDFLAGS) $(EXOBJECT) $(DRIVERLIB_OBJECTS) $(GLUELIB_OBJECTS) $(LOCAL_OBJECTS) -o $@ && size $@

debug: all
	$(GDB) $(OBJ_DIR)/$(PROJ).elf

clean:
	$(RM) build/*.o build/$(PROJ).{elf,cflags,dep,hdep}
	$(RM) tags

cleanlib:
	$(RM) $(DRIVERLIB_OBJECTS) $(GLUELIB_OBJECTS)

install: $(OBJ_DIR)/$(PROJ).elf
	@bash ../../tools/burn.sh $<

# rebuild if CFLAGS changed, as suggested in:
# http://stackoverflow.com/questions/3236145/force-gnu-make-to-rebuild-objects-affected-by-compiler-definition/3237349#3237349
$(OBJ_DIR)/$(PROJ).cflags: cflags_update
	@echo "$(CFLAGS)" | cmp -s - $@ || echo "$(CFLAGS)" > $@

-include $(OBJ_DIR)/$(PROJ).dep
