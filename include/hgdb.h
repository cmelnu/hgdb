// hgdb.h
#ifndef HGDB_H
#define HGDB_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "stub.h"
#include "error.h"
#include "mi_gdb.h"


struct save_results_struct{

	char *name;
	char *type;
	char *value;
	char *func;
	char *file;
	int  *line;

};

typedef struct save_results_struct save_results;

//These variables hold all the state of a gdb "connection"
mi_bkpt *bk;
mi_wp *wp;
mi_h *h;
mi_frames *fr;

#endif // HGDB_H