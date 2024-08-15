
#include "stub/stub.h"
#include "hwdebug/hwdebug.h"




/********************
GLOBAL VARIABLES

-results: it stores the results of a GDB query
-ptr_results: an index that indicates the end of the data that has been stored into 'results'
********************/

save_results *results;
int ptr_results = 0;


//Functions and data structures coming from outside (see stub.c)
extern addrmap suma_addrmap[];
extern int create_stub(int ptr_results, save_results *results);
extern char *get_varname(char *command);


void cb_from(const char *str, void *data)
{
 //printf(">> %s\n",str);
}

/********************
while_wait_for_stop

-Waits until a breakpoint is reached
-In case we have no breakpoints, it simply adds extra delay to the execution of the debugger
********************/
int while_wait_for_stop(){
	
	if (!wait_for_stop(h))
	{
		mi_disconnect(h);
		return 1;
	}
}

int wait_for_stop (){

	int res=1;
	mi_stop *sr;

	while (!mi_get_response(h))
	usleep(1000);
	/* The end of the async. */
	sr=mi_res_stop(h);
		if (sr)
		{
			//printf("Stopped, reason: %s\n",mi_reason_enum_to_str(sr->reason));
			mi_free_stop(sr);
		}
		else
		{
			//printf("Error while waiting\n");
			//printf("mi_error: %d\nmi_error_from_gdb: %s\n",mi_error,mi_error_from_gdb);
			res=0;
		}
	return res;
}

/********************
breakpoint_main

-Set a breakpoint in the main function of a program
********************/
int breakpoint_main(){

	bk=gmi_break_insert(h,getenv("SOURCE_PATH"), detect_main());

	if (!bk){
		printf("Error setting breakpoint in main() function\n");
		return;
	}

}

/********************
step_executable

-Go one step forward (one high-level instruction) within the source code
********************/
int step_executable() {

	int res = gmi_exec_next(h);
	if (res == 1){
   	 	while_wait_for_stop();
		printf("Software step performed\n");
	}
	return res;
}

/********************
run_executable

-Runs the desired executable file
********************/
int run_executable(){
	
	if (!gmi_exec_run(h))
	{
		printf("Error when running executable file!\n");
		mi_disconnect(h);
		return 1;
	}
}

/********************
cont_executable

-Resumes a program's execution from the last breakpoint found
********************/
int cont_executable(){

	printf("Continuing program execution..\n");

	if (!gmi_exec_continue(h))
        {
	       printf("Error when continuing program execution!\n");
	       mi_disconnect(h);
	       return 1;
        }
}

/********************
free_memory

-Frees up all the memory that was previously allocated for the debugger's data structures
********************/
void free_memory(){

	int i;

	for (i=0; i < 100; i++){
		free(results[i].name);
		free(results[i].type);
		free(results[i].value);	
		free(results[i].func);	
		free(results[i].file);	
	}

	free(results);	

	ptr_results = 0;

}

/********************
reload_hgdb

-Reload the DEBUGGER
********************/
int reload_hgdb(){

	//Free up the allocated memory
	free_memory();
	//Exit GDB process
	gmi_gdb_exit(h);
	//Close the connection.
	mi_disconnect(h);
	printf("Okay, let's start another GDB session...\n");
   	return 0;

}


/********************
exit_hgdb

-Exit the DEBUGGER
********************/
int exit_hgdb(){

	//Free up the allocated memory
	free_memory();
	//Exit GDB process
	gmi_gdb_exit(h);
	//Close the connection.
	mi_disconnect(h);
	printf("Goodbye!\n");
   	return 0;

}

/*


/********************
set_executable

-Sets the executable file to be run and debugged
********************/
int set_executable(){

	// Set the child name and the command line aguments.
	if (!gmi_set_exec(h,getenv("EXEC_PATH"),""))
	{
		printf("Error setting executable file and args\n");
		mi_disconnect(h);
		return 1;
	}

	breakpoint_main();
}

/********************
set_breakpoint

-First, it parses the command 'break' and separates both the command and the argument [line number]
-Secondly, the breakpoint is set after the argument [line number] has been converted to integer
********************/
int set_breakpoint(char *command){

	int lineno = 0;
	char *linestr = NULL;
	
	//In these two lines, we obtain the argument [line number]
	strtok(command, " ");
	linestr = strtok(NULL, " ");

	//In case no line number has been provided, no breakpoint is set
	if (linestr == NULL){
		printf("Error: no lines provided, breakpoint not set\n");
		return(-1);
	}
	
	//Convert the argument to integer
	lineno = strtol(linestr, NULL, 10);

	//Insert the breakpoint
	bk=gmi_break_insert(h,getenv("SOURCE_PATH"),lineno);
	if (!bk){
		printf("Error setting breakpoint\n");
		return 1;
	}
}

/********************
print_frame

-Prints out information about the current stack frame
********************/
void print_frame()
{

 	//First, we obtain the current frame
 	mi_frames *f = gmi_stack_info_frame(h);

 	if(!f){
  	    printf("Error! empty frames info. The executable/source file has not been set yet\n");
  	    return;
  	}

	printf("\n \n Frame info (level, address, function, file, line, args): \n \n");

 	//After obtaining the current frame, we simply print out its information.
 	//This frame comes as a linked list, so we only have to go to the next node until we reach the end
 	while(f){
		printf("	Level %d, addr %p, func %s, where: %s:%d args? %c\n\n",f->level,f->addr,
		       f->func,f->file,f->line,f->args ? 'y' : 'n');
		f=f->next;
   	}

}

/********************
show_trace

-Shows the execution trace of the chosen program
********************/
void show_trace(){
	
	int i;

	for (i = 0; i < ptr_results; i++)
		printf("%s  %s  %s  %s  %s  %d \n", results[i].name, results[i].type, 
			   results[i].value, results[i].func, results[i].file, results[i].line);

}

/********************
init_data_structures

-Allocates memory for all the necessary data structures that are used when parsing GDB's certain outputs
********************/
void init_data_structures(){

	int j;

	//We allocate memory for the data structure that will store the results of a GDB query
	results = malloc (sizeof(save_results) * 100);

	//Next, we allocate memory for each of its fields
	for(j = 0; j < 100; j++){
		results[j].name  = (char*)malloc(100);
		results[j].type  = (char*)malloc(100);
		results[j].value = (char*)malloc(100);	
		results[j].func  = (char*)malloc(100);	
		results[j].file  = (char*)malloc(100);	
	}
}

/********************
clear_data_structures

-Reallocates memory for all the necessary data structures
********************/
void clear_data_structures(){

	free_memory();
	init_data_structures();
}

/********************
parse

-Parses the data structure containing the results in response to a GDB query
********************/
void parse(mi_results *r){
		
	mi_results * a = NULL;
	mi_results * aux = NULL;
	mi_frames *fr = NULL;

	int i = 0;
	int j = 0;
	int k = 0;

	//Before navigating through the tree, we get its first node
	r = r->v.rs;

	fr = gmi_stack_info_frame(h);

 	if(!fr){
  	    printf("Error! empty frames info. The executable/source file has not been set yet\n");
  	    return;
  	}

	//In case the debugger jumps to a source line not belonging to our source file, skip it
	if (strcmp(fr->file, getenv("SOURCE_PATH")) != 0){
		return;
	}

	//We start navigating through the tree.. until we find a NULL node
	while( r != NULL ){
			//We save the root node, because we will need it later on
			aux = r;
			//For the current root node, we obtain its first child node
			a = r->v.rs;
			//For each child node, there is a list which contains the data we want
			while ( a != NULL ){
				if (strcmp(a->var,"name") == 0){ printf("	%s	", a->v.cstr);
						strcpy(results[ptr_results].name, a->v.cstr);}
				else if (strcmp(a->var,"type") == 0) { printf("%s	", a->v.cstr);
						strcpy(results[ptr_results].type, a->v.cstr);}
				else if (strcmp(a->var,"value") == 0) { printf("%s\n", a->v.cstr);
						strcpy(results[ptr_results].value, a->v.cstr);}

				k++;

				if (k % 3 == 0 && k != 0){
					strcpy(results[ptr_results].func, fr->func);
					strcpy(results[ptr_results].file, fr->file);
					results[ptr_results].line = (int)fr->line;
					ptr_results++;
					k = 0;
				}
				a = a->next;
			}
			//After navigating through all the current root node's child nodes, we get back to the root node and go to the next root node
			r = aux;
			r = r->next;
	}
}



/********************
print_locals

-Obtain the local variables and other relevant information related to them
********************/
int print_locals(){

	mi_results *r = NULL;
	//We obtain the list of local variables
	r = gmi_stack_list_locals(h, 2);

	if (!r){
		printf("Could not list local variables. The executable/source file has not been set yet \n");
		return -1;
	}

	printf("\n \n Local variables (name, type, value): \n \n");
	//After obtaining the list, we perform some kind of parsing which allows us to obtain some info about the variables (names,values,types)
	parse(r);
	free(r);
}

/********************
current_state_info

-Shows information about the current execution state (both local variables and the current stack frame)
********************/
void current_state_info(){
	
	print_locals();
	print_frame();
	
}

/********************
detect_main

-Detects the line of source code where the 'main' function is located
********************/
int detect_main(){

	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[512];
	
	//Here, we try to open our program's source file
	if((fp = fopen(getenv("SOURCE_PATH"), "r")) == NULL) {
		printf("The source file could not be open \n");
		return (-1);
	}

	//We start reading the source file and we keep doing it while we find characters on it
	while(fgets(temp, 512, fp) != NULL) {
		//In case a match is found, we get out of the loop
		if((strstr(temp, "main")) != NULL) {
			find_result = 1;
			break;
		}
		//This will count the amount of lines already parsed
		line_num++;
	}
	//If we haven't found any matches, we do nothing..
	if(find_result == 0) {
		return (-1);
	}

	//We close the file if still open.
	if(fp) {
		fclose(fp);
	}

	//The return value is the line number where the 'main' function is located
	return line_num;
}

/********************
trace_executable

-Traces the execution of the desired program
********************/
void trace_executable(){

	/*In order to avoid printf's while obtaining the first execution trace ( when creating a STUB. That is, when switching to HWDEBUG mode), we temporarily disable STDOUT
	  and re-open the output channel at the end of this function */
	int save_out = dup(STDOUT_FILENO);
	close(STDOUT_FILENO);

	//This variable is used to stop the below loop
	int step = 1;
	//We insert a breakpoint at the beginning of the 'main' function (by detecting its line number)
	breakpoint_main();
	//We clear the data structures that are going to be re-used
	clear_data_structures();
	//We start running the desired program and..
	run_executable();
	//we wait for it to reach the breakpoint at the 'main' function
	while_wait_for_stop();

	//While it's possible to step through the program's source code..
	while(1){

		print_locals();
		//we go one step forward
		step = step_executable();
		//was it possible? if not, then let's get out of the loop
		if (step != 1) break;
		//if so, then we obtain the values of the local variables and store them
		
	}

	dup2(save_out, STDOUT_FILENO);
	
}

int set_hw_debug(){

	run_executable();
	while_wait_for_stop();
	trace_executable();
	run_executable();
	while_wait_for_stop();
	create_stub(ptr_results, results);

}

int hw_debug(){

	int i;
	char aux[40];
	char aux_d[40];
	char *line = NULL;
	ssize_t bufsize = 0;
	int size = 0;
	int end = 0;
	int states = 0;
	mi_frames *fr = NULL;

	//At the beginning, we print out the help 
	print_hw_help();
	
	//Here, we run the program for the first time in order to obtain an execution trace (used when creating the stub)
	//run_executable();
	//while_wait_for_stop();
	
	//We obtain an execution trace
	//trace_executable();

	//We reload the program in order to start at the beginning of the main() function
	//run_executable();
	//while_wait_for_stop();

	//In order to align the program's execution with the hardware's execution, we perform a single step.
	//step_executable();
		
	//We create the stub and all necessary data structures
        //create_stub(ptr_results, results);

	//hw_debug_step(get_in_addr());

	set_hw_debug();
	//step_executable();
	//hw_debug_step(get_in_addr());

	while(end == 0){

		printf ("hwdebug > ");
		size = getline(&line, &bufsize, stdin);

		if (strcmp(line,"\n") != 0){
			if (strcmp(line,"help\n") == 0){
		 		print_hw_help();
			}
			//In case we perform a step...
			else if (strcmp(line,"step\n") == 0){
				 //We obtain the number of current source line
				 fr = gmi_stack_info_frame(h);
			 	 if(!fr){
			  	        printf("Error! empty frames info. The executable/source file has not been set yet\n");
			  	        return;
			  	 }
				//Since we want to avoid stepping to other source files (such as libraries), we make sure the current line belongs to our current source file
				 if (strcmp(fr->file, getenv("SOURCE_PATH")) == 0){
					 //We perform a step within the source file
					 step_executable();
					 //Next, we check out how many times we have to step into the hardware's execution
					 states = how_many_states();
					 //Finally, we perform as many steps as necessary (within the hardware)
					 for(i = 0; i < states; i++)
			 		 	hw_debug_step(get_in_addr());

					 printf("Hardware step performed\n");
				 }
			}
			else if (strstr(line,"read") != NULL){

				fr = gmi_stack_info_frame(h);
				if(!fr){
					printf("Error! empty frames info. The executable/source file has not been set yet\n");
					return;
				}
				 /*We want to check out that the hardware's version of a variable has the same value 
				     as the source code's variable at a certain point of execution */
				 strcpy(aux, line);
				 strcpy(aux_d, line);
		 		 unsigned res = hw_debug_read(get_var_id(line), get_var_addr(aux), get_var_id_addr(), get_in_addr(), get_out_addr());
                                
				strtok(aux_d, " ");
				char *varname = strtok(NULL, " ");

				char *pos;
				if ((pos = strchr(varname, '\n')) != NULL)
					*pos = '\0';

 
				//printf("Variable to read = %s \n", varname);

				int line = (int)fr->line;

				for(i = 0; i < ptr_results; i++){
					if(strcmp(results[i].name,varname) == 0 && results[i].line == line ){
						printf("Ok, var name %s and line %d state found, the var has value %s \n", varname, line, results[i].value);
						if ((int)res != atoi(results[i].value))
							printf("DISCREPANCY DETECTED!\n");
					}
				}


			}
			else if (strcmp(line, "restart\n") == 0){
				/* Restarting consists of restarting the program, clearing all data structures and restarting the harware */
				
				//run_executable();
				//while_wait_for_stop();
				//clear_data_structures();
				//trace_executable();
				//step_executable();
				hw_debug_restart(get_in_addr());
				set_hw_debug();
				//step_executable();
				//hw_debug_step(get_in_addr());
				
				
			}
			else if (strcmp(line, "curstate\n") == 0){
				current_state_info();
			}
			else if (strcmp(line, "hwinfo\n") == 0){
				hardware_info();
			}
			else if (strcmp(line,"exit\n") == 0){
				return 0;
			}
		}//end if
	}//end while

}

/********************
check_env

-Checks whether EXEC_PATH and SOURCE_PATH environmental variables already exist. 
-If not, an example to be run must be chosen before running the debugger
********************/
void check_env(){

	char *exec_path = getenv("EXEC_PATH");
	char *source_path = getenv("SOURCE_PATH");

	if( exec_path == NULL || source_path == NULL ){
		printf("Either one of these variables has not been specified:  exec_path, SOURCE_PATH\nPlease run '. ./config.sh' without arguments for further details\n");
		exit(1);
	}
}


/********************
print_hw_ help

-Prints out the hardware debugger's help
********************/
void print_hw_help(){

	printf("\n");
	printf("LIST OF COMMANDS FOR HWDEBUG MODE: \n");
	printf("\n");
	printf("-step		:	perform a step both within the source code and the hardware \n");
	printf("-read		:	read a hardware variable and automatically compare its value to the one in the source code (discrepancy detection) \n");
	printf("-restart	:	restart both the software and the hardware versions \n");
	printf("-hwinfo		:	show information about the harware variables in our system.\n");
	printf("-curstate	:	show info about the current state of the program\n");
	printf("\n");

}


/********************
print_help

-Prints out the help
********************/
void print_help(){

	printf("\n");
	printf("LIST OF COMMANDS FOR EACH EXECUTION MODE: \n");
	printf("\n");
	printf("HGDB: GDB's 'built-in' functions (hgdb mode)\n");
	printf("\n");
	printf("-break [line]	:	set a breakpoint at a certain line of the source code\n");
	printf("-step		:	execute a line of source code\n");
	printf("-run		:	execute the whole program until the end or until a breakpoint is found\n");
	printf("-frame		:	get info about the current stack frame\n");
	printf("-locals		:	show local variables at a certain point of execution (a breakpoint)\n");
	printf("-curstate	:	show info about the current state of the program\n");
	printf("-exit		:	exit the debugger \n");
	printf("-reload		:	restart the chosen program's execution \n");
	printf("\n");
	printf("HWDEBUG: hardware debugging functions\n");
	printf("\n");
	printf("-trace		:	get the execution trace of a program \n");
	printf("-strace		:	print the execution trace of a program \n");
	printf("-hwdebug	:	perform in-system debug between a source code file and a VHDL file. \n \n");
	printf(" NOTICE		:	HWDEBUG mode commands' help will be available by typing 'help' once you have switched to this mode.\n");
	printf("\n");

}


/********************

DEBUGGER

-This debugger works as a command-line interpreter
-It uses GDB's 'built-in' functions, together with functionalities provided by this debugger itself

********************/
int hgdb_execution(){

    char *line = NULL;
    ssize_t bufsize = 0;
    int size = 0;
    int end = 0;

	printf("\n\nWELCOME TO HGDB!\n");
	printf("Author: Christian Meléndez Núñez\n");
	printf("Director: Daniel Jimenez-Gonzalez\n");
	printf("2018, Universitat Politècnica de Catalunya\n\n");

	print_help();

	while(end == 0){

		printf ("hgdb > ");
		size = getline(&line, &bufsize, stdin);

		if (strcmp(line,"\n") != 0){
			if (strstr(line,"help") != NULL){
		 		print_help();
			}
			else if (strstr(line,"break") != NULL){
		 		set_breakpoint(line);
			}
			else if (strcmp(line,"step\n") == 0){
				step_executable();
			}
			else if (strcmp(line,"run\n") == 0){
				run_executable();
				while_wait_for_stop();
			}
			else if (strcmp(line, "cont\n") == 0){
				cont_executable();
				while_wait_for_stop();
			}
			else if (strcmp(line,"frame\n") == 0){
			 	print_frame();
			}
			else if (strcmp(line,"locals\n") == 0){
			 	print_locals();
			}
			else if (strcmp(line,"curstate\n") == 0){
			 	current_state_info();
			}
			else if (strcmp(line,"strace\n") == 0){
			 	show_trace();
			}
			else if (strcmp(line,"trace\n") == 0){
			 	trace_executable();
			}
			else if (strcmp(line,"hwdebug\n") == 0){
			 	hw_debug();
			}
			else if (strcmp(line,"reload\n") == 0){
			 	reload_hgdb();
				return;
			}
			else if (strcmp(line,"exit\n") == 0){
			 	exit_hgdb();
				exit(0);
			}
			else{
				printf("Unrecognized command: %s", line);
			}
		}//end if
	}//end while
}

/********************

MAIN FUNCTION THAT STARTS OUR DEBUGGER'S EXECUTION

********************/
int main(){

	//This infinite loop allows the user to reload the debugger 
	while(1){

		//First, we check that certain environmental variables have been created before running the debugger
		//Otherwise, they must be specified
		check_env();

		//Connect to a gdb child process
		h=mi_connect_local();
		if (!h)
		{
			printf("Connect failed\n");
			return 1;
		}
		printf("Connected to gdb!\n");

		//Set all callbacks
		mi_set_from_gdb_cb(h,cb_from,NULL);
		//Init all the necessary data structures for the debugger
		init_data_structures();	
		//Set the executable file to run and debug
		set_executable();
		//At this point, the debugger starts its execution
		hgdb_execution();

	}

	return 0;

}



