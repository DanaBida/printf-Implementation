all: 2c

2c: main.o toy_printf.o
	gcc -g -Wall -o 2c main.o toy_printf.o

main.o: main.c
	gcc -g -Wall -c -o main.o main.c

toy_printf.o: toy_printf.c toy_stdio.h
	gcc -g -Wall -c -o toy_printf.o toy_printf.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o 2c
