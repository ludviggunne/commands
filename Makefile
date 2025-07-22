override CFLAGS:=
override LDFLAGS:=

sources=$(wildcard *.c)
commands=$(sources:%.c=build/%)

all: $(commands)

build/%: %.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(@) $(<)

build:
	mkdir build

clean:
	rm -rf build

.PHONY: clean
