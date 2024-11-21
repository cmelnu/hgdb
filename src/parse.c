#include "parse.h"


extern save_results *results;
extern int ptr_results;


/********************
parse_local_vars_list

-Parses the data structure containing the results in response to a GDB query
********************/
int parse_local_vars_list(mi_results *r){
		
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
  	    return -1;
  	}

	//In case the debugger jumps to a source line not belonging to our source file, skip it
	if (strcmp(fr->file, getenv("SOURCE_PATH")) != 0){
		return -1;
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
parse_break_list

-Parses the data structure containing a list of breakpoints in response to a GDB query
********************/
int parse_break_list(mi_results *r){
		
	mi_results * a = NULL;
	mi_results * aux_bk = NULL;
	mi_results * body_node = NULL;

	int i = 0;
	int j = 0;
	int k = 0;


	printf("First node = %s\n", r->var);

	r = r->v.rs;

	while(r->next != NULL){
		printf("Node before hdr = %s \n", r->var);
		r = r->next;
	}

	//This aux stores the body node. The body contains a chain of breakpoints 
	body_node = r;

	//The first breakpoint node to be analysed
	aux_bk = r->v.rs;

	while (r != NULL && aux_bk != NULL){

		//First field of the first breakpoint, 'number'
		r = aux_bk->v.rs;

		printf("First field of the current breakpoint =	%s %s \n", r->var, r->v.cstr);

		while(r->next != NULL){

			printf("Breakpoint field = %s and value = %s\n", r->var, r->v.cstr);
			r = r->next;
		}

		//Now r points to the next breakpoint
		r = aux_bk->next;

		//Aux_bk points to the next breakpoint
		aux_bk = aux_bk->next;
		
	}
}