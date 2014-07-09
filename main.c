#include "../lctf/src/libctf.h"

#include "conversion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Print the usage information.
 */
static void
usage ()
{
	printf("Usage: bsdctfdump file\n");
}

/**
 * Print all labels.
 * 
 * @param file file containing the labels
 */
static void
dump_labels (ctf_file file)
{
	ctf_label label = NULL;
	int retval; 

	printf("-- Labels ------\n");

	while ((retval = ctf_file_get_next_label(file, label, &label)) == CTF_OK)
	{
		char* name;
		ctf_label_get_name(label, &name);

		ctf_label_index index;
		ctf_label_get_index(label, &index);

		printf(" Name: %s\n", name);	
		printf("Index: %d\n", index);	
	}

	if (retval == CTF_EMPTY)
	{
		printf("No labels.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}
}

/**
 * Print general information about the file.
 *
 * @param file the file in question
 */
static void
dump_general_information (ctf_file file)
{
	ctf_version version;
	ctf_file_get_version(file, &version);

	ctf_bool is_compressed;
	ctf_file_is_compressed(file, &is_compressed);

	ctf_file parent_file;
	ctf_file_get_parent_file(file, &parent_file);

	char* parent_basename;
	ctf_file_get_basename(parent_file, &parent_basename);

	printf("-- General Information ------\n");
	printf("   Version: %d\n", version);
	printf("Compressed: %s\n", is_compressed ? "yes" : "no");
	printf("    Parent: %s\n", parent_file ? parent_basename : "N/A");
}

/**
 * Print all types.
 * 
 * @param file file containing the types
 */
static void
dump_types (ctf_file file)
{
	ctf_type type = NULL;
	int retval; 
	int inner_retval;

	printf("-- Types ------\n");

	while ((retval = ctf_file_get_next_type(file, type, &type)) == CTF_OK)
	{
		ctf_id id;
		ctf_type_get_id(type, &id);

		ctf_kind kind;
		ctf_type_get_kind(type, &kind);

		ctf_bool is_root;
		ctf_type_is_root(type, &is_root);

		printf("       ID: %d\n", id);	
		printf("     Kind: %s\n", kind_to_string(kind));
		printf("     Root: %s\n", is_root ? "yes" : "no");

		switch (kind)
		{
			case CTF_KIND_INT:
			{
				ctf_int _int;
				ctf_int_init(type, &_int);
				
				ctf_int_size size;
				ctf_int_get_size(_int, &size);

				ctf_int_offset offset;
				ctf_int_get_offset(_int, &offset);

				ctf_int_content content;
				ctf_int_get_content(_int, &content);

				ctf_bool is_signed;
				ctf_int_is_signed(_int, &is_signed);

				char* name;
				ctf_int_get_name(_int, &name);

				printf("     Name: %s\n", name);
				printf("     Size: %d\n", size);
				printf("   Offset: %d\n", offset);
				printf("   Signed: %s\n", is_signed ? "yes" : "no");
				printf("  Content: %s\n", int_content_to_string(content));
			}
			break;

			case CTF_KIND_FLOAT:
			{
				ctf_float _float;
				ctf_float_init(type, &_float);

				ctf_float_size size;
				ctf_float_get_size(_float, &size);

				ctf_float_offset offset;
				ctf_float_get_offset(_float, &offset);

				ctf_float_encoding encoding;
				ctf_float_get_encoding(_float, &encoding);

				char* name;
				ctf_float_get_name(_float, &name);

				printf("     Name: %s\n", name);
				printf("     Size: %d\n", size);
				printf("   Offset: %d\n", offset);
				printf("  Content: %s\n", float_encoding_to_string(encoding));
			}
			break;

			case CTF_KIND_ARRAY:
			{
				ctf_array array;
				ctf_array_init(type, &array);

				ctf_type content_type;
				ctf_array_get_type(array, &content_type);

				ctf_array_length length;
				ctf_array_get_length(array, &length);

				char* name;
				ctf_array_get_name(array, &name);

				char* type_string;
				type_string = type_to_string(content_type);

				printf("     Name: %s\n", name);
				printf("  Content: %s\n", type_string);
				printf("   Length: %d\n", length);
			
				free(type_string);
			}
			break;

			case CTF_KIND_ENUM:
			{
				ctf_enum _enum;
				ctf_enum_init(type, &_enum);

				char* name;
				ctf_enum_get_name(_enum, &name);

				printf("     Name: %s\n", name);

				ctf_enum_entry enum_entry = NULL;	
				while ((inner_retval = ctf_enum_get_next_enum_entry(_enum, enum_entry,
				    &enum_entry)) == CTF_OK)
				{
					char* entry_name;
					ctf_enum_entry_get_name(enum_entry, &entry_name);

					ctf_enum_entry_value entry_value;
					ctf_enum_entry_get_value(enum_entry, &entry_value);

					printf("       %s = %d\n", entry_name, entry_value); 
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No enum entries.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}
			}
			break;

			/* FALL THROUGH */
			case CTF_KIND_UNION:
			case CTF_KIND_STRUCT:
			{
				ctf_struct_union struct_union;
				ctf_struct_union_init(type, &struct_union);

				char* name;
				ctf_struct_union_get_name(struct_union, &name);

				printf("     Name: %s\n", name);

				ctf_member member = NULL;
				while ((inner_retval = ctf_struct_union_get_next_member(struct_union, 
				    member, &member)) == CTF_OK)
				{
					char* member_name;
					ctf_member_get_name(member, &member_name);

					ctf_type member_type;
					ctf_member_get_type(member, &member_type);

					char* member_type_string;
					member_type_string = type_to_string(member_type);

					printf("       %s %s", member_type_string, member_name);

					if (kind == CTF_KIND_STRUCT)
					{
						ctf_member_offset member_offset;
						ctf_member_get_offset(member, &member_offset);

						printf(" | %llu", member_offset);
					}
					
					printf("\n");
					free(member_type_string);
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No members.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}
			}
			break;

			case CTF_KIND_TYPEDEF:
			{
				ctf_typedef _typedef;
				ctf_typedef_init(type, &_typedef);
				
				char* name;
				ctf_typedef_get_name(_typedef, &name);

				ctf_type old_type;
				ctf_typedef_get_type(_typedef, &old_type);

				char* type_string;
				type_string = type_to_string(new_type);

				printf(" Old type: %s\n", type_string);
				printf(" New type: %s\n", name);

				free(type_string);
			}
			break;

			case CTF_KIND_FUNC:
			{
				ctf_function function;
				ctf_function_init(type, &function);

				char* name;
				ctf_function_get_name(function, &name);

				ctf_type return_type;
				ctf_function_get_return_type(function, &return_type);

				char* return_type_string;
				return_type_string = type_to_string(return_type);

				printf("     Name: %s\n", name);
				printf("  Returns: %s\n", return_type_string);

				free(return_type_string);

				ctf_argument argument = NULL;
				while ((inner_retval = ctf_function_get_next_argument(function,
				    argument, &argument)) == CTF_OK)
				{
					ctf_type argument_type;
					ctf_argument_get_type(argument, &argument_type);

					char* argument_type_string;
					argument_type_string = type_to_string(argument_type);

					printf("       %s", argument_type_string);
					free(argument_type_string);
				}

				if (inner_retval == CTF_EMPTY)
				{
					printf ("      No arguments.\n");
					break;
				}

				if (inner_retval != CTF_END)
				{
					printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
				}
			}
			break;

			/* FALL THROUGH */
			case CTF_KIND_POINTER:
			case CTF_KIND_VOLATILE:
			case CTF_KIND_CONST:
			case CTF_KIND_RESTRICT:
			{
				ctf_type ref_type;
				ctf_type_init(type, &ref_type);

				char* type_string;
				type_string = type_to_string(ref_type);

				printf("Reference: %s\n", type_string);
				free(type_string);
			}
			break;

			case CTF_KIND_FWD_DECL:
			{
				ctf_fwd_decl fwd_decl;
				ctf_fwd_decl_init(type, &fwd_decl);

				char* name;
				ctf_fwd_decl_get_name(fwd_decl, &name);

				printf("     Name: %s\n", name);
			}
			break;
		}

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No types.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}
}

/**
 * Print all data objects.
 * 
 * @param file file containing the data objects 
 */
static void
dump_data_objects (ctf_file file)
{
	ctf_data_object data_object = NULL;
	int retval;

	printf("-- Data Objects ------\n");

	while ((retval = ctf_file_get_next_data_object(file, data_object, 
	    &data_object)) == CTF_OK)
	{
		char* name;
		ctf_data_object_get_name(data_object, &name);

		ctf_type type;
		ctf_data_object_get_type(data_object, &type);

		char* type_string = type_to_string(type);

		printf(" Name: %s\n", name);	
		printf(" Type: %s\n", type_string);	
		free(type_string);

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No data objects.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}
}

/**
 * Print all functions.
 * 
 * @param file file containing the functions 
 */
static void
dump_functions (ctf_file file)
{
	ctf_function function = NULL;
	int retval; 
	int inner_retval;

	printf("-- Functions ------\n");

	while ((retval = ctf_file_get_next_function(file, function, &function)) 
	    == CTF_OK)
	{
		char* name;
		ctf_function_get_name(function, &name);

		ctf_type return_type;
		ctf_function_get_return_type(function, &return_type);

		char* return_type_string = type_to_string(return_type);

		printf("   Name: %s\n", name);
		printf("Returns: %s\n", return_type_string);

		free(return_type_string);

		ctf_argument argument = NULL;
		while ((inner_retval = ctf_function_get_next_argument(function,
				argument, &argument)) == CTF_OK)
		{
			ctf_type argument_type;
			ctf_argument_get_type(argument, &argument_type);

			char* argument_type_string = type_to_string(argument_type);

			printf("     %s\n", argument_type_string);
			free(argument_type_string);
		}

		if (inner_retval == CTF_EMPTY)
		{
			printf(" No arguments.\n");
			continue;
		}

		if (inner_retval != CTF_END)
		{
			printf("ERROR: %s\n", ctf_get_error_string(inner_retval));
		}

		printf("\n");
	}

	if (retval == CTF_EMPTY)
	{
		printf("No functions.");
		return;
	}

	if (retval != CTF_END)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
	}
}

/**
 * Print all available information stored inside the CTF section.
 * 
 * @param argc argument count
 * @param argv array of string arguments
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int
main (int argc, char* argv[])
{
	int option;
	int general_information_flag;
	int labels_flag;
	int types_flag;
	int data_objects_flag;
	int functions_flag;

	general_information_flag = labels_flag = types_flag = data_objects_flag = 
	    functions_flag = 0;

	while ((option = getopt(argc, argv, "gltdf")) != -1)
	{
		switch(option)
		{
			case 'g': 
				general_information_flag = 1;
			break;

			case 'l': 
				labels_flag = 1;
			break;

			case 't': 
				types_flag = 1;
			break;

			case 'd': 
				data_objects_flag = 1;
			break;

			case 'f': 
				functions_flag = 1;
			break;

			case '?':
				fprintf(stderr, "ERROR: invalid option %c\n", optopt);	
				usage();
				return EXIT_FAILURE;

			default: 
				fprintf(stderr, "ERROR: unknown error during option parsing\n");	
				return EXIT_FAILURE;
		}
	}

	if (argc - optind < 1)
	{
		usage();
		return EXIT_FAILURE;
	}

	ctf_file file;
	int retval;

	if ((retval = ctf_file_read(argv[optind], &file)) != CTF_OK)
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(retval));
		return EXIT_FAILURE;
	}

	if (general_information_flag == 0
	 && labels_flag == 0
	 && types_flag == 0
	 && data_objects_flag == 0
	 && functions_flag == 0)
	{
		general_information_flag = labels_flag = types_flag = data_objects_flag = 
		    functions_flag = 1;
	}

	if (general_information_flag == 1)
		dump_general_information(file);

	if (labels_flag == 1)
		dump_labels(file);

	if (types_flag == 1)
		dump_types(file);

	if (data_objects_flag == 1)
		dump_data_objects(file);

	if (functions_flag == 1)
		dump_functions(file);

	return EXIT_SUCCESS;
}

