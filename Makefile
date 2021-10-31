
SUBDIRS = driverlib glue

TESTS := ad7789 clock_system cypress_fm24 gt9xx honeywell_hsc_ssc maxim_ds3231 maxim_ds3234 sensirion_sht1x ti_tca6408 timer_a0 uart0 vishay_tsop  # uart0_cpp

all: driverlib glue
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var);)

driverlib: force_look
	@$(MAKE) -C driverlib -j1 $(MFLAGS)

glue: force_look
	@$(MAKE) -C glue -j1 $(MFLAGS)

clean:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) clean ;)

cppcheck:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) cppcheck ;)

scan-build:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) scan-build ;)

force_look:
	@true
