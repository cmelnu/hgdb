#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>



/*

	MESSAGES TO BE SENT TO THE FPGA

*/

#define D_INIT 		0
#define D_FINISH   -2
#define D_STEP	    1
#define D_F_STEP	2
#define D_READVAR 	3
#define D_RESTART   4


unsigned receive(char *address);
int send (int message, char *address);
int hw_debug_init();
unsigned hw_debug_read(int var_id, char *var_addr, char *var_id_addr, char *in_addr, char *out_addr);
int hw_debug_step(char *in_addr);
int hw_debug_restart(char *in_addr);



