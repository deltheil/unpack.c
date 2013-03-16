override CFLAGS := -std=c99 -Wall -Werror $(CFLAGS)
LDLIBS = -lmsgpack

all: unpack

clean:
	rm -f *.o unpack

.PHONY: all clean