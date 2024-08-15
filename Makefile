
dbg:	
	gcc -w -g -o hgdb hgdb.c stub/stub.c hwdebug/hwdebug.c -lmigdb -fcommon

tests:
	gcc -g -o examples/vars.out examples/vars.c
	gcc -g -o examples/suma.out examples/suma.c

suma:
	gcc -g -O0 -o examples/suma.out examples/suma.c

vars:
	gcc -g -o examples/vars.out examples/vars.c

exec:
	make
	./hgdb

val:
	make
	valgrind --leak-check=full --show-leak-kinds=all ./hgdb

clean:
	rm -rf hgdb *~ examples/*.out examples/*~

targets:	
	@echo "all exec val clean"
