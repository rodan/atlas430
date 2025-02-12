
REFLIB_ROOT = ../../
IGNORE_TARGET = true
include $(REFLIB_ROOT)Makefile.env

SRC_C = $(wildcard *.c)
SRC_H = $(wildcard *.h)
SRC	= $(SRC_C) $(SRC_H)

tags: $(SRC)
	@echo "Generating tags .."
	@exuberant-ctags --recurse=yes ./

clean:
	@rm -f tags
