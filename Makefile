CFLAGS ?= -Wall -Werror
LDFLAGS ?= -lmsgpack

all: unpack

.PHONY: clean
clean:
	rm -f unpack.o unpack