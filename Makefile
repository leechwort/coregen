src = $(wildcard src/*.c)
kissfft_obj = $(wildcard ext/kissfft/*.o)

LDFLAGS = -lm -Iext/kissfft/ -I./

all: \
	main
tinydsp.o: libkissfft
	gcc -Wall -fPIC -c *.c $(LDFLAGS)
main: main.c coregen.c tinydsp.o
	gcc -Wall -fPIC -o $@ coregen.c \
		wavedata.c \
		main.c \
		tinydsp.o  $(kissfft_obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f main
	rm -f *.o
	rm -f $(kissfft_obj)

.PHONY: libkissfft
libkissfft:
	$(MAKE) -C ext/kissfft