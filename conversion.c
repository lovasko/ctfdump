#include "conversion.h"

const char*
kind_to_string (ctf_kind kind)
{
	static const char *translation_table[] = 
	{
		"kind:none", 
		"int", 
		"float", 
		"pointer",
		"array",
		"function", 
		"struct", 
		"union", 
		"enum", 
		"forward declaration", 
		"typedef",
		"volatile",
		"const", 
		"restrict"
	};

	if (kind <= CTF_KIND_MAX && kind >= CTF_KIND_MIN)
		return translation_table[kind];
	else
		return "unresolvable";
}

char*
type_to_string (ctf_type type)
{
	ctf_kind kind;
	ctf_type_get_kind(type, &kind);

	switch (kind)
	{
		case CTF_KIND_INT:
		{
			ctf_int _int;
			ctf_int_init(type, &_int);

			char* name;
			ctf_int_get_name(_int, &name);

			return strdup(name);
		}

		case CTF_KIND_FLOAT:
		{
			ctf_float _float;
			ctf_float_init(type, &_float);

			char* name;
			ctf_float_get_name(_float, &name);

			return strdup(name);
		}

		case CTF_KIND_POINTER:
		{
			ctf_pointer pointer;
			ctf_pointer_init(type, &pointer);

			ctf_type ref_type;
			ctf_pointer_get_type(pointer, &ref_type);

			char result[1024];
			char* type_string = type_to_string(ref_type);

			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s*", type_string);
			free(type_string);

			return strdup(result);
		}

		/* FALL THROUGH */
		case CTF_KIND_STRUCT:
		case CTF_KIND_UNION:
		{
			ctf_struct_union struct_union;
			ctf_struct_union_init(type, &struct_union);

			char* name;
			ctf_struct_union_get_name(struct_union, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s %s", 
			    (kind == CTF_KIND_STRUCT ? "struct" : "union"), name);

			return strdup(result);
		}

		case CTF_KIND_TYPEDEF:
		{
			ctf_typedef _typedef;
			ctf_typedef_init(type, &_typedef);

			char* name;
			ctf_typedef_get_name(_typedef, &name);

			return strdup(name);
		}

		case CTF_KIND_CONST:
		{
			/* TODO special case const pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "const %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_RESTRICT:
		{
			/* TODO special case restrict pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "restrict %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_VOLATILE:
		{
			/* TODO special case volatile pointer! */
			ctf_type ref_type;
			ctf_type_init(type, &ref_type);

			char* ref_type_string = type_to_string(ref_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "volatile %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_FWD_DECL:
		{
			ctf_fwd_decl fwd_decl;
			ctf_fwd_decl_init(type, &fwd_decl);

			char* name;
			ctf_fwd_decl_get_name(fwd_decl, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "forward declaration of %s", name);

			return strdup(result);
		}

		case CTF_KIND_ARRAY:
		{
			ctf_array array;
			ctf_array_init(type, &array);
			
			ctf_array_length length;
			ctf_array_get_length(array, &length);

			ctf_type content_type;
			ctf_array_get_content_type(array, &content_type);

			char* type_string = type_to_string(content_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s [%d]", type_string, length);
			free(type_string);

			return strdup(result);
		}

		case CTF_KIND_ENUM:
		{
			ctf_enum _enum;
			ctf_enum_init(type, &_enum);

			char* name;
			ctf_enum_get_name(_enum, &name);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "enum %s", name);
			
			return strdup(result);
		}

		case CTF_KIND_FUNC:
		{
			ctf_function function;
			ctf_function_init(type, &function);

			ctf_type return_type;
			ctf_function_get_return_type(function, &return_type);

			char* return_type_string;
			return_type_string = type_to_string(return_type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s ()", return_type_string);
			free(return_type_string);

			return strdup(result);
		}

		case CTF_KIND_NONE:
			return strdup("type_to_string:none");
	}

	char result[1024];
	memset(result, '\0', 1024);

	ctf_id id;
	ctf_type_get_id(type, &id);

	snprintf(result, 1024, "%d %d unresolvable", id, kind);
	return strdup(result);
}

const char*
float_encoding_to_string (ctf_float_encoding float_encoding)
{
	static const char* translation_table[] = 
	{
		"float:none", 
		"single", 
		"double", 
		"complex",
		"double complex",
		"long double complex", 
		"long double", 
		"interval", 
		"double interval", 
		"long double interval", 
		"imaginary",
		"double imaginary",
		"long double imaginary"
	};

	if (float_encoding <= CTF_FLOAT_ENCODING_MAX 
	 && float_encoding >= CTF_FLOAT_ENCODING_MIN)
		return translation_table[float_encoding];
	else
		return "unresolvable";
}

const char*
int_content_to_string (ctf_int_content int_content)
{
	if (int_content == CTF_INT_CONTENT_NUMBER)
		return "number";

	if (int_content == CTF_INT_CONTENT_CHAR)
		return "char";

	if (int_content == CTF_INT_CONTENT_BOOLEAN)
		return "boolean";

	if (int_content == CTF_INT_CONTENT_VARARGS)
		return "varargs";

	return "unknown";
}

