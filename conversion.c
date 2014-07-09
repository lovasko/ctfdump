#include "conversion.h"

const char*
kind_to_string (ctf_kind kind)
{
	const char *translation_table[] = 
	{
		"none", 
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
	uint8_t kind;
	(void) ctf_type_get_kind(type, &kind);

	void* data;
	(void) ctf_type_get_data(type, &data);

	switch (kind)
	{
		case CTF_KIND_INT:
		{
			struct ctf_int* _int = data;
			char* name;
			
			(void) ctf_int_get_name(_int, &name);
			return strdup(name);
		}

		case CTF_KIND_FLOAT:
		{
			struct ctf_float* _float = data;
			char* name;

			(void) ctf_float_get_name(_float, &name);
			return strdup(name);
		}

		case CTF_KIND_POINTER:
		{
			char result[1024];
			struct ctf_type* type = data;
			char* type_string = type_to_string(type);

			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s*", type_string);
			free(type_string);

			return strdup(result);
		}

		/* FALL THROUGH */
		case CTF_KIND_STRUCT:
		case CTF_KIND_UNION:
		{
			struct ctf_struct_union* struct_union = data;
			char result[1024];
			char* name;

			(void) ctf_struct_union_get_name(struct_union, &name);
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s %s", 
			    (kind == CTF_KIND_STRUCT ? "struct" : "union"), name);

			return strdup(result);
		}

		case CTF_KIND_TYPEDEF:
		{
			struct ctf_typedef* _typedef = data;
			char* name;

			(void) ctf_typedef_get_name(_typedef, &name);

			return strdup(name);
		}

		case CTF_KIND_CONST:
		{
			/* TODO special case const pointer! */
			struct ctf_type* type = data;
			char* ref_type_string = type_to_string(type);
			char result[1024];

			memset(result, '\0', 1024);
			snprintf(result, 1024, "const %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_RESTRICT:
		{
			/* TODO special case restrict pointer! */
			struct ctf_type* type = data;
			char* ref_type_string = type_to_string(type);
			char result[1024];

			memset(result, '\0', 1024);
			snprintf(result, 1024, "restrict %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_VOLATILE:
		{
			/* TODO special case volatile pointer! */
			struct ctf_type* type = data;
			char* ref_type_string = type_to_string(type);
			char result[1024];

			memset(result, '\0', 1024);
			snprintf(result, 1024, "volatile %s", ref_type_string);
			free(ref_type_string);

			return strdup(result);
		}

		case CTF_KIND_FWD_DECL:
		{
			struct ctf_fwd_decl* fwd_decl = data;
			char result[1024];
			char* name;

			(void) ctf_fwd_decl_get_name(fwd_decl, &name);
			memset(result, '\0', 1024);
			snprintf(result, 1024, "forward declaration of %s", name);

			return strdup(result);
		}

		case CTF_KIND_ARRAY:
		{
			struct ctf_array* array = data;
			
			char* name;
			(void) ctf_array_get_name(array, &name);

			uint32_t element_count;
			(void) ctf_array_get_element_count(array, &element_count);

			struct ctf_type* type;
			(void) ctf_array_get_type(array, &type);
			char* type_string = type_to_string(type);

			char result[1024];
			memset(result, '\0', 1024);
			snprintf(result, 1024, "%s %s[%d]", type_string, name, element_count);
			free(type_string);

			return strdup(result);
		}

		case CTF_KIND_NONE:
			return strdup("none");
	}

	return strdup("unresolvable");
}

const char*
float_encoding_to_string (ctf_float_encoding float_encoding)
{
	static const char* translation_table[] = 
	{
		"none", 
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

