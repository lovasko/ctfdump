#ifndef CTFDUMP_DATA_H
#define CTFDUMP_DATA_H

#include <ctf/ctf.h>

/**
 * Print all data objects.
 * 
 * @param file file containing the data objects 
 */
void
dump_data_objects(ctf_file file);

#endif

