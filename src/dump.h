#ifndef CTFDUMP_DUMP_H
#define CTFDUMP_DUMP_H

#include <ctf/ctf.h>

/**
 * Print general information about the file.
 *
 * @param file the file in question
 */
void
dump_general_information(ctf_file file);

/**
 * Print all labels.
 * 
 * @param file file containing the labels
 */
void
dump_labels(ctf_file file);

/**
 * Print all types.
 * 
 * @param file file containing the types
 */
void
dump_types(ctf_file file);

/**
 * Print all data objects.
 * 
 * @param file file containing the data objects 
 */
void
dump_data_objects(ctf_file file);

/**
 * Print all functions.
 * 
 * @param file file containing the functions 
 */
void
dump_functions(ctf_file file);

#endif

