#ifndef CTFDUMP_FUNCTIONS_H
#define CTFDUMP_FUNCTIONS_H

#include <ctf/ctf.h>

/**
 * Print all functions.
 * 
 * @param file file containing the functions 
 */
void
dump_functions(ctf_file file);

#endif

