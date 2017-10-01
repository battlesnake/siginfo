.PHONY: all
all: siginfo

.PHONY: clean
clean:
	rm -f -- siginfo *.o

siginfo: siginfo.o terminal.o
	gcc -o $@ $^

%.o: %.c $(wildcard *.h)
	gcc -c -o $@ $<
