FLAGS= -g

OBJECTS= hwdebug/hwdebug.o stub/stub.o


all: ${OBJECTS}
	gcc -w $(FLAGS) -o hgdb hgdb.c ${OBJECTS} -lmigdb -fcommon

hwdebug/hwdebug.o:hwdebug/hwdebug.c
	gcc $(FLAGS) hwdebug/hwdebug.c -c -o hwdebug/hwdebug.o

stub/stub.o:stub/stub.c
	gcc $(FLAGS) stub/stub.c -c -o stub/stub.o

clean:
	rm -rf hgdb *~ examples/*~ hwdebug/*.o stub/*.o
