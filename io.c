#include <stdio.h>

void dbg_print_int(int n) {
    fprintf(stdout, "%d\n", n);
}

void dbg_print_bool(char b) {
    fprintf(stdout, "%s\n", b == 1 ? "true" : "false");
}
