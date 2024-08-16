FLAGS = -g

all: suma vars

suma: suma.c
	gcc $(FLAGS) -o suma suma.c

vars: vars.c
	gcc $(FLAGS) -o vars vars.c

clean:
	rm -rf suma vars *~

.PHONY: all clean