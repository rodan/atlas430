
SUBDIRS = driverlib drivers

TESTS := clock_jitter clock_system crc exf fexec i2c_master scheduler spi_master uart vishay_tsop

all: driverlib drivers
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var);)

driverlib: force_look
	@$(MAKE) -C driverlib -j1 $(MFLAGS)

drivers: force_look
	@$(MAKE) -C drivers -j1 $(MFLAGS)

clean:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) clean ;)

cppcheck:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) cppcheck ;)

scan-build:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) scan-build ;)

force_look:
	@true
