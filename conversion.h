#ifndef CTF_DUMP_CONVERSION_H
#define CTF_DUMP_CONVERSION_H

#include "../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char*
kind_to_string (uint8_t kind);

char*
type_to_string (struct ctf_type* type);

const char*
float_encoding_to_string (uint8_t float_encoding);

const char*
int_content_to_string (uint8_t int_content);

#endif

