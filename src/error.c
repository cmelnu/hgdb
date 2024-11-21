#include <stdio.h>

void hgdb_error(const char *func_name, const char *message) {
    printf("Error in function %s: %s\n", func_name, message);
}
