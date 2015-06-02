#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "dump.h"

static int 
dump_int(ctf_type type, void* arg)
{
	ctf_int _int;
	ctf_int_size size;
	ctf_int_offset offset;
	ctf_int_content content;
	uint8_t is_signed;
	char* name;

	(void) arg;

	ctf_int_init(type, &_int);
	ctf_int_get_name(_int, &name);
	ctf_int_get_size(_int, &size);
	ctf_int_get_offset(_int, &offset);
	ctf_int_get_content(_int, &content);
	ctf_int_get_is_signed(_int, &is_signed);

	printf("        Name: %s\n", name);
	printf("        Size: %d\n", size);
	printf("      Offset: %d\n", offset);
	printf("      Signed: %s\n", is_signed ? "yes" : "no");
	printf("     Content: %s\n", ctf_int_content_to_string(content));

	return CTF_OK;
}

static int
dump_float(ctf_type type, void* arg)
{
	ctf_float _float;
	char* name;
	ctf_float_size size;
	ctf_float_offset offset;
	ctf_float_encoding encoding;

	(void) arg;

	ctf_float_init(type, &_float);
	ctf_float_get_name(_float, &name);
	ctf_float_get_size(_float, &size);
	ctf_float_get_offset(_float, &offset);
	ctf_float_get_encoding(_float, &encoding);

	printf("        Name: %s\n", name);
	printf("        Size: %d\n", size);
	printf("      Offset: %d\n", offset);
	printf("     Content: %s\n", ctf_float_encoding_to_string(encoding));

	return CTF_OK;
}

static int 
dump_array(ctf_type type, void* arg)
{
	ctf_array array;
	ctf_type content_type;
	ctf_id content_id;
	ctf_array_length length;

	(void) arg;

	ctf_array_init(type, &array);
	ctf_array_get_content_type(array, &content_type);
	ctf_type_get_id(content_type, &content_id);
	ctf_array_get_length(array, &length);

	printf(" Content ID: %d\n", content_id);
	printf("     Length: %d\n", length);

	return CTF_OK;
}

static int 
dump_typedef(ctf_type type, void* arg)
{
	ctf_typedef _typedef;
	char* name;
	ctf_type ref_type;
	ctf_id ref_id;

	(void) arg;

	ctf_typedef_init(type, &_typedef);
	ctf_typedef_get_name(_typedef, &name);
	ctf_typedef_get_type(_typedef, &ref_type);
	ctf_type_get_id(ref_type, &ref_id);

	printf("Reference ID: %d\n", ref_id);
	printf("    New name: %s\n", name);
	return CTF_OK;
}

static void 
dump_member(void* member, void* arg)
{
	char* member_name;
	ctf_member_offset member_offset;
	ctf_type member_type;
	ctf_id member_id;

	(void) arg;

	ctf_member_get_name(member, &member_name);
	ctf_member_get_offset(member, &member_offset);
	ctf_member_get_type(member, &member_type);
	ctf_type_get_id(member_type, &member_id);

	printf("          %d | %s | %llu\n", member_id, member_name, member_offset);
}

static int 
dump_struct_union(ctf_type type, void* arg)
{
	char* name;
	ctf_struct_union struct_union;
	ctf_count member_count;

	(void) arg;

	ctf_struct_union_init(type, &struct_union);
	ctf_struct_union_get_name(struct_union, &name);

	printf("        Name: %s\n", name);

	ctf_struct_union_get_member_count(struct_union, &member_count);
	if (member_count == 0)
		printf("       No members.");
	else
		ctf_struct_union_foreach_member(struct_union, NULL, dump_member);

	return CTF_OK;
}

static void
dump_enum_entry(void* enum_entry, void* arg)
{
	char* entry_name;
	ctf_enum_entry_value entry_value;

	(void) arg;

	ctf_enum_entry_get_name(enum_entry, &entry_name);
	ctf_enum_entry_get_value(enum_entry, &entry_value);

	printf("          %s = %d\n", entry_name, entry_value); 
}

static int 
dump_enum(ctf_type type, void* arg)
{
	ctf_enum _enum;
	ctf_count entry_count;
	char* name;

	(void) arg;

	ctf_enum_init(type, &_enum);
	ctf_enum_get_name(_enum, &name);

	printf("        Name: %s\n", name);

	ctf_enum_get_entry_count(_enum, &entry_count);
	if (entry_count == 0)
		printf("       No entries.");
	else
		ctf_enum_foreach_enum_entry(_enum, NULL, dump_enum_entry);

	return CTF_OK;
}

static int 
dump_fwd_decl(ctf_type type, void* arg)
{
	ctf_fwd_decl fwd_decl;
	char* name;

	(void) arg;

	ctf_fwd_decl_init(type, &fwd_decl);
	ctf_fwd_decl_get_name(fwd_decl, &name);

	printf("        Name: %s\n", name);
	return CTF_OK;
}

static int 
dump_pointer(ctf_type type, void* arg)
{
	ctf_pointer pointer;
	ctf_type ref_type;
	ctf_id ref_id;

	(void) arg;

	ctf_pointer_init(type, &pointer);
	ctf_pointer_get_type(pointer, &ref_type);
	ctf_type_get_id(ref_type, &ref_id);

	printf("Reference ID: %d\n", ref_id);
	return CTF_OK;
}

static int 
dump_none(ctf_type type, void* arg)
{
	(void) type;
	(void) arg;

	return CTF_OK;
}

static int 
dump_qualifier(ctf_type type, void* arg)
{
	ctf_type ref_type;
	ctf_id ref_id;

	(void) arg;

	ctf_type_init(type, &ref_type);
	ctf_type_get_id(ref_type, &ref_id);

	printf("Reference ID: %d\n", ref_id);

	return CTF_OK;
}

static void
dump_argument(void* argument, void* arg)
{
	ctf_type type;
	ctf_id id;

	(void) arg;

	ctf_argument_get_type(argument, &type);
	ctf_type_get_id(type, &id);

	printf("%d ", id);
}

static int 
dump_function(ctf_type type, void* arg)
{
	ctf_type return_type;
	ctf_id return_id;
	ctf_function function;

	(void) arg;

	ctf_function_init(type, &function);
	ctf_function_get_return_type(function, &return_type);
	ctf_type_get_id(return_type, &return_id);

	printf("      Return: %d\n", return_id);
	printf("   Arguments: ");
	ctf_function_foreach_argument(function, NULL, dump_argument);
	printf("\n");

	return CTF_OK;
}

void
dump_type(void* type, void* data)
{
	ctf_id id;
	ctf_kind kind;
	uint8_t is_root;
	ctf_polycall dump_fns[] = {
		dump_none,
		dump_int,
		dump_float,
		dump_pointer,
		dump_array,
		dump_function,
		dump_struct_union,
		dump_struct_union,
		dump_enum,
		dump_fwd_decl,
		dump_typedef,
		dump_qualifier,
		dump_qualifier,
		dump_qualifier
	};

	(void) data;

	ctf_type_get_id(type, &id);
	ctf_type_get_kind(type, &kind);
	ctf_type_get_is_root(type, &is_root);

	printf("          ID: %d\n", id);	
	printf("        Kind: %s\n", ctf_kind_to_string(kind));
	printf("        Root: %s\n", is_root ? "yes" : "no");
		
	ctf_type_polycall(type, NULL, dump_fns);
	printf("\n");
}

void
dump_types(ctf_file file)
{
	ctf_count type_count;

	ctf_file_get_type_count(file, &type_count);
	if (type_count == 0)
		printf("No types.");
	else
		ctf_file_foreach_type(file, NULL, dump_type);
}

