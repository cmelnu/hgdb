
#include "stub.h"


int size_FSM_states = 0;
FSM_states *inst_states;
mi_frames *fr = NULL;
char *in_addr = NULL;
char *out_addr = NULL;
char *var_id_addr = NULL;
char *exec_path = NULL;


/* ########################################################

	ADDRESS MAPS

   ######################################################## */

addrmap suma_addrmap[] = {

	{ .id = 1,   .var_name = "in",  .address = "0x40000000"},
	{ .id = 2,   .var_name = "out", .address = "0x42000000"},
	{ .id = 3,   .var_name = "v",   .address = "0x4A000000"},
	{ .id = 97,  .var_name = "a",   .address = "0x44000000"},
	{ .id = 98,  .var_name = "b",   .address = "0x46000000"},
	{ .id = 99,  .var_name = "c",   .address = "0x48000000"}
	

};


/* ########################################################

	END OF ADDRESS MAPS

   ######################################################## */


void hardware_info(){

	int i;

	printf("Hardware variables in the system: \n\n");

	if(strstr(exec_path,"suma") != NULL){
		
		for(i = 0 ; i < 6; i++){
			printf("Varname = %s \nAddress = %s \nId = %d \n\n", suma_addrmap[i].var_name, suma_addrmap[i].address, suma_addrmap[i].id);
		}
	}

	printf("\n\n");
}

char *get_var_addr(char *command){

	int i;
	char *varname = NULL;
	
	//In these two lines, we obtain the argument [line number]
	strtok(command, " ");
	varname = strtok(NULL, " ");

	char *pos;
	if ((pos=strchr(varname, '\n')) != NULL)
		*pos = '\0';

	if(strstr(exec_path,"suma") != NULL){
		
		for(i = 0 ; i < 5; i++){
			if (strcmp(suma_addrmap[i].var_name, varname) == 0){
				break;
			}
		}
		return suma_addrmap[i].address;
	}

}

int get_var_id(char *command){

	int i;
	char *varname = NULL;
	
	//In these two lines, we obtain the argument [variable name]
	strtok(command, " ");
	varname = strtok(NULL, " ");

	//Since the varname comes from STDIN, a newline character might have been added inadvertently
	//We just remove it, in case it is found
	char *pos;
	if ((pos=strchr(varname, '\n')) != NULL)
		*pos = '\0';

	if(strstr(exec_path,"suma") != NULL){
		
		for(i = 0 ; i < 5; i++){
			if (strcmp(suma_addrmap[i].var_name, varname) == 0){
				break;
			}
		}
		return suma_addrmap[i].id;
	}

}


void print_lines(){

	int i;
	for(i = 0; i < size_FSM_states; i++)
		printf("Line = %d   States = %d\n", inst_states[i].inst_line, inst_states[i].num_states);
				
}

/********************
manage_lines

-Creates a data structure that will store the line_of_code - amount_of_states connection
-Its size will be equal to the number of source lines
********************/
int manage_lines(int ptr_results,save_results *results,int option){

	int i;
	int j = 0;
	int aux = 0;
	int lines = 0;

	if (option == 0){
	
		for(i = 0; i < ptr_results; i++){
			if(aux != results[i].line){
				aux = results[i].line;
				lines++;				
			}	
		}
			
		return lines;
	}
	else{

		for(i = 0; i < ptr_results; i++){
			if(aux != results[i].line){
				aux = results[i].line;
				inst_states[j].inst_line = results[i].line;	
				inst_states[j].num_states = 1;	
				j++;	
			}	
		}
	
		return lines;
	}
}

char *get_in_addr(){
	return in_addr;
}

char *get_out_addr(){
	return out_addr;
}

char *get_var_id_addr(){
	return var_id_addr;
}


/********************
set_env

-Set certain variables that are used to store the in,out and v ports of a hardware module
********************/
void set_env(){

	int i;

	exec_path = getenv("EXEC_PATH");

	if(strstr(exec_path,"suma") != NULL){
		for(i = 0 ; i < 5; i++){
			if (strcmp(suma_addrmap[i].var_name, "in") == 0){
				break;
			}
		}
		in_addr = suma_addrmap[i].address;
	}
	if(strstr(exec_path,"suma") != NULL){
		for(i = 0 ; i < 5; i++){
			if (strcmp(suma_addrmap[i].var_name, "out") == 0){
				break;
			}
		}
		out_addr = suma_addrmap[i].address;
	}

	if(strstr(exec_path,"suma") != NULL){
		for(i = 0 ; i < 5; i++){
			if (strcmp(suma_addrmap[i].var_name, "v") == 0){
				break;
			}
		}
		var_id_addr = suma_addrmap[i].address;
	}
}


/********************
create_stub

-Create all the necessary data structures to simulate a certain hardware
********************/
int create_stub(int ptr_results, save_results *results){


	int lines = manage_lines(ptr_results, results, 0);

	size_FSM_states = lines;
	inst_states = malloc(sizeof(FSM_states) * lines);

	int nums =  manage_lines(ptr_results, results, 1);
	//print_lines();

	set_env();	

	return 0;

}

/********************
how_many_states

-Returns the amount of hardware states to skip in order to go from one instruction to the next one in sequence
********************/
int how_many_states(){

	int i = 0;
	int lineno = 0;
	fr = gmi_stack_info_frame(h);

	if(!fr){
  	    printf("Error! empty frames info\n");
  		return;
  	}

	lineno = (int)fr->line;
	for(i = 0; i < size_FSM_states; i++){
		if(lineno == inst_states[i].inst_line)
			break;
	}

	return inst_states[i].num_states;

}
