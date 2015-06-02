#include <stdint.h>
#include <stdio.h>

#include "dump.h"

void
dump_general_information(ctf_file file)
{
	ctf_version version;
	ctf_file parent_file;
	uint8_t is_compressed;
	char* parent_basename;

	ctf_file_get_version(file, &version);
	ctf_file_get_is_compressed(file, &is_compressed);
	ctf_file_get_parent_file(file, &parent_file);
	ctf_file_get_basename(parent_file, &parent_basename);

	printf("   Version: %d\n", version);
	printf("Compressed: %s\n", is_compressed ? "yes" : "no");
	printf("    Parent: %s\n", parent_file ? parent_basename : "N/A");
	printf("\n");
}

