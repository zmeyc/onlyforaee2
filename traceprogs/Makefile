SRCS = simpleloop.c matmul.c blocked.c fibonacci.c
PROGS = simpleloop matmul blocked fibonacci

all : $(PROGS)

$(PROGS) : % : %.c
	gcc -Wall -g -o $@ $<


traces: $(PROGS)
	./runit simpleloop
	./runit matmul 100
	./runit blocked 100 25
	./runit fibonacci 1000

.PHONY: clean
clean :
	rm -f simpleloop matmul blocked fibonacci tr-*.ref *.marker *~
