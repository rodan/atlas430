#==============================================================================
#
# Makefile for building MSP430 DriverLib Code examples from command line
# environment using the GCC Open Source Compiler for MSP430
#
#==============================================================================

PROJ:=main
REFLIB_ROOT:=../../
#MEMORY_MODEL=LARGE

include $(REFLIB_ROOT)/Makefile.env

SUBDIRS:= 
OBJ_DIR:=build/$(TARGET)

######################################
GCC_BIN_DIR     ?= $(GCC_DIR)/bin
GCC_INC_DIR     ?= $(GCC_DIR)/msp430-elf/include
######################################
CC               = $(GCC_BIN_DIR)/msp430-elf-gcc
GDB              = $(GCC_BIN_DIR)/msp430-elf-gdb
######################################

# -fdata-sections, -ffunction-sections and -Wl,--gc-sections -Wl,-s
# are used for dead code elimination, see:
# http://gcc.gnu.org/ml/gcc-help/2003-08/msg00128.html

CFLAGS       := -D__$(DEVICE)__ -mmcu=$(DEVICE) # -g -DDEPRECATED
CFLAGS       += -Wall -Wno-switch -Wno-int-to-pointer-cast
CFLAGS       += $(CFLAGS_EXTRA)
#CFLAGS       += -Werror
#CFLAGS      += -S -fverbose-asm
#CFLAGS		 += -fno-force-addr -finline-limit=1 -fno-schedule-insns
#CFLAGS		 += -fshort-enums -Wl,-Map=output.map
LDFLAGS          := -T $(LDDIR).ld -L $(LIB_DIR) -L $(GCC_EXTRA_INC_DIR) -mmcu=$(DEVICE) $(HWMULT) -g -Wl,--gc-sections
CFLAGS_REL	 += -Os -fdata-sections -ffunction-sections -fomit-frame-pointer
LDFLAGS_REL	 += -Wl,-s
CFLAGS_DBG	 += -O1 -ggdb
LDFLAGS_DBG	 += 
INCLUDES         := -I $(GCC_EXTRA_INC_DIR) -I $(GCC_INC_DIR) -I $(DRIVERLIB) -I $(ATLAS430) -I $(ATLAS430_SYS) -I $(GCC_CCS_INC_GCC_DIR) -I ./ -I .
######################################
LOCAL_SRC        := $(wildcard *.c)
PROJ_SRC         := $(PROJ).c
SRC              := $(PROJ_SRC) $(DRIVERLIB_SRC) $(ATLAS430_SRC) $(ATLAS430_SYS_SRC) $(LOCAL_SRC)

LOCAL_OBJECTS     = $(LOCAL_SRC:%.c=$(OBJ_DIR)/%.o)

EXOBJECT         := $(OBJ_DIR)/$(PROJ).o
EXOUTPUT         := $(OBJ_DIR)/$(PROJ).elf
######################################

.PHONY: all
.PHONY: clean
.PHONY: install
.PHONY: config
.PHONY: config_macros
.PHONY: mkdir_od
.PHONY: new_timestamps
.PHONY: depend
.PHONY: doc
.PHONY: cflags_update
.PHONY: size

ifeq ($(OS),Windows_NT)
CFLAGS	+= $(CFLAGS_REL)
LDFLAGS	+= $(LDFLAGS_REL)
all: mkdir_od config.h $(EXOUTPUT)
else
# Append specific CFLAGS/LDFLAGS
DEBUG := $(shell grep "^#define CONFIG_DEBUG" config.h)
MACROS := $(shell $(REFLIB_ROOT)/tools/extract_macros.sh -t $(TARGET) -f config.h)
CFLAGS += $(MACROS)
ifeq ($(DEBUG),)
CTARGET	:= RELEASE
CFLAGS	+= $(CFLAGS_REL)
LDFLAGS	+= $(LDFLAGS_REL)
else
CTARGET	:= DEBUG
CFLAGS	+= $(CFLAGS_DBG)
LDFLAGS	+= $(LDFLAGS_DBG)
endif
all: depend version.h config.h config_macros tags $(EXOUTPUT) size
endif

ifeq ($(MEMORY_MODEL),LARGE)
CFLAGS	+= -mlarge -mcode-region=lower -mdata-region=lower -D__LARGE_DATA_MODEL__ -D__LARGE_CODE_MODEL__
LDFLAGS	+= -mlarge -mcode-region=lower
endif

config.h:
	@echo "config.h file is missing" && false

config_macros: config.h
	@echo "active macros: $(MACROS)"

include $(REFLIB_ROOT)/Makefile.libatlas430

$(OBJ_DIR)/%.o: %.c config.h
	@echo -e " * $(@F)"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOUTPUT): $(LOCAL_OBJECTS) $(EXOBJECT) $(OBJ_DIR)/libatlas430.a
	@echo -e "\n>> Building $@ as target $(CTARGET)"
	@$(CC) $(LDFLAGS) $(LOCAL_OBJECTS) -L$(OBJ_DIR) -latlas430 -o $@

mkdir_od:
	@mkdir -p $(OBJ_DIR) || echo ""

clean:
	$(RM) $(OBJ_DIR)/*.o $(OBJ_DIR)/$(PROJ).{elf,cflags,dep,hdep}
	$(RM) -r $(OBJ_DIR)
	$(RM) tags

install: $(OBJ_DIR)/$(PROJ).elf
	@bash $(REFLIB_ROOT)/tools/burn.sh $<

envcheck:
	@bash $(REFLIB_ROOT)/tools/check_setup.sh

# rebuild if CFLAGS changed, as suggested in:
# http://stackoverflow.com/questions/3236145/force-gnu-make-to-rebuild-objects-affected-by-compiler-definition/3237349#3237349
$(OBJ_DIR)/$(PROJ).cflags: cflags_update
	@echo "$(CFLAGS)" | cmp -s - $@ || echo "$(CFLAGS)" > $@

ifneq ($(OS),Windows_NT)

version.h: $(SRC)
	@$(REFLIB_ROOT)/tools/setver.sh

depend: $(OBJ_DIR)/$(PROJ).dep

$(OBJ_DIR)/$(PROJ).dep: $(SRC)
	@echo "Generating dependencies.."
	@mkdir -p $(OBJ_DIR)
	@touch $@
	@makedepend $(INCLUDES) -p "$(OBJ_DIR)/" -Y -f $@ $^ 1>/dev/null 2>&1
	@gsed -i "s|$(OBJ_DIR)/$(ATLAS430_SYS)/|$(OBJ_DIR)/|;s|$(OBJ_DIR)/$(ATLAS430)/|$(OBJ_DIR)/|;s|$(OBJ_DIR)/$(DRIVERLIB)/|$(OBJ_DIR)/|" $(OBJ_DIR)/main.dep
	@grep -v '^#' $(OBJ_DIR)/$(PROJ).dep | cut -d" " -f2 | sort -u | xargs > $(OBJ_DIR)/$(PROJ).hdep
	@echo "tags: `cat $(OBJ_DIR)/$(PROJ).hdep`" >> $(OBJ_DIR)/$(PROJ).dep
	@rm -f $@.bak

tags: $(PROJ_SRC) $(DRIVERLIB_SRC) $(ATLAS430_SRC) $(ATLAS430_SYS_SRC) $(LOCAL_SRC) $(OBJ_DIR)/$(PROJ).dep
	@echo "Generating tags .."
	@$(CTAGS) --recurse=yes ./ $(DRIVERLIB) $(ATLAS430) $(ATLAS430_SYS) $(LDDIR).h $(shell cat $(OBJ_DIR)/$(PROJ).hdep)

new_timestamps:
	@$(BASH) $(REFLIB_ROOT)/tools/update_rtca_now.sh

size: $(OBJ_DIR)/$(PROJ).elf
	@size $(OBJ_DIR)/$(PROJ).elf

debug: all
	$(GDB) $(OBJ_DIR)/$(PROJ).elf

scan-build: clean
	@scan-build --use-cc=$(CC) --analyzer-target=msp430-elf --exclude ./driverlib/MSP430FR5xx_6xx make PARSER=scan-build

cppcheck: clean
	@mkdir -p build/cppcheck
	@cppcheck -j8 --enable=warning --enable=performance --enable=information --enable=missingInclude --cppcheck-build-dir=build/cppcheck -D__GNUC__ -D__$(DEVICE)__ $(MACROS) -I $(DRIVERLIB) -I $(ATLAS430) -I $(ATLAS430_SYS) -I $(GCC_EXTRA_INC_DIR) -I ./ ./ $(SRC)

$(EXOBJECT): $(PROJ_SRC) $(OBJ_DIR)/$(PROJ).cflags
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

-include $(OBJ_DIR)/$(PROJ).dep
endif

