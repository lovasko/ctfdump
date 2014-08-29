#ifndef CTF_DUMP_CONVERSION_H
#define CTF_DUMP_CONVERSION_H

#include "../../libctf/src/libctf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char*
kind_to_string (ctf_kind kind);

char*
type_to_string (ctf_type type);

const char*
float_encoding_to_string (ctf_float_encoding float_encoding);

const char*
int_content_to_string (ctf_int_content int_content);

#endif

