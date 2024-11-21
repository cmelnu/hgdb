
#include "hgdb.h"


struct FSM_states_struct{

	int inst_line;
	int num_states;

};

struct accel_funcs{

	char *func_name;

};

typedef struct FSM_states_struct FSM_states;


struct addrmap_struct{

	int id;
	char *var_name;
	char *address;
	
};

typedef struct addrmap_struct addrmap;

