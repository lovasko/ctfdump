#ifndef CTFDUMP_LABELS_H
#define CTFDUMP_LABELS_H

#include <ctf/ctf.h>

/**
 * Print all labels.
 * 
 * @param file file containing the labels
 */
void
dump_labels(ctf_file file);

#endif

