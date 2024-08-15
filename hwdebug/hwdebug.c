

#include "hwdebug.h"


/*

Steps when sending a message:

-For a certain message, send the appropriate code to the FPGA
-


*/


int hw_debug_init(){




}

/********************
send

-Writes into a memory location
********************/
int send(int message, char * address){


	int fd;
	void *ptr;
	unsigned val;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	fd=open("/dev/mem",O_RDWR);
	if(fd<1) {
		perror("Error trying to open /dev/mem. Accessing memory was not possible");
		exit(-1);
	}

	addr=strtoul(address,NULL,0);
	//val=strtoul(argv[2],NULL,0);

	page_addr=(addr & ~(page_size-1));
	page_offset=addr-page_addr;

	ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(addr & ~(page_size-1)));
	if((int)ptr==-1) {
		perror("Error writing to main memory");
		exit(-1);
	}

	*((unsigned *)(ptr+page_offset))=(unsigned)message;
	return 0;

}

/********************
receive

-Reads a memory location
********************/
unsigned receive(char *address){

	int fd;
	void *ptr;
	unsigned addr, page_addr, page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	fd=open("/dev/mem",O_RDONLY);
	if(fd<1) {
		perror("Error trying to open /dev/mem. Accessing memory was not possible.");
		exit(-1);
	}

	addr=strtoul(address,NULL,0);
	page_addr=(addr & ~(page_size-1));
	page_offset=addr-page_addr;

	ptr=mmap(NULL,page_size,PROT_READ,MAP_SHARED,fd,(addr & ~(page_size-1)));
	if((int)ptr==-1) {
		perror("Error reading from main memory.");
		exit(-1);
	}

	printf("0x%08x\n",*((unsigned *)(ptr+page_offset)));
	return *((unsigned *)(ptr+page_offset));

}

/********************
hw_debug_step

-Performs a hardware step:
--Sends the step request
--Sends the finish step request. Otherwise, the hardware will finish its execution before debugging a single line of code
********************/
int hw_debug_step(char *in_addr){

	//Step into the source code
	send(D_STEP,   in_addr);
	send(D_F_STEP, in_addr);

}

/********************
hw_debug_read

-Reads the value of a certain hardware variable:
--Sends the ID of the variable we want to read
--Sends a read request
--Receives the variable's current value
********************/
unsigned hw_debug_read(int var_id, char *var_addr, char *var_id_addr, char *in_addr, char *out_addr){

	send(var_id, var_id_addr);
	send(D_READVAR, in_addr);
	return receive(out_addr);

}

/********************
hw_debug_restart

-Restarts the hardware, setting it to its initial state (FSM's initial state and all variable BRAMs set to 0)
********************/
int hw_debug_restart(char *in_addr){

	send(D_RESTART, in_addr);
	
}
