/*---- FILE HEADER ----------------------------------------------------------
project: project1
file: leakcount.c
author: elias abderhalden
date: 2014-10-03
-----------------------------------------------------------------------------
class: ece3220 fall 2014
instructor: jacob sorber
assignment: project 1: tracing, system calls, and processes
---------------------------------------------------------------------------*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>
#include <errno.h>

#include "list.h"
#define BUFFERSIZE 80


char TEMP_FILE[] = "lc_temp~";


// variables and functions for the list datastructure
typedef struct data_entry_tag 
	{
	char* pointer_string;
	int size;
	} data_entry;


#define a ((data_entry*)input_a)
#define b ((data_entry*)input_b)

int list_compare(void* input_a, void* input_b)
{
return (strcmp(a->pointer_string, b->pointer_string));
}


void list_datadestruct(void* input_a)
{
free(a->pointer_string);
free(a);
}





int main(int argc, char* argv[])
{
int x;
int pid;
int waitpid_status;


char delimiter[] = " \0";
char* cmd_envp[] = {"LD_PRELOAD=./memory_shim.so", NULL };
char* cmd_arg[10];

// arguments
if (argc == 2)
	{
	// using strtok to contrive arguments
	cmd_arg[0] = strtok(argv[1], delimiter);
	x = 0;
	while (cmd_arg[x] != NULL)
		{
		x++;
		cmd_arg[x] = strtok(NULL, delimiter);
		}
	}
else
	{
	fprintf(stderr, "Error: program arguments\n");
	return 0;
	}


// fork into two seperate processes
pid = fork();
if (pid < 0)
	{
	perror("fork");
	return -1;
	}
else if (pid == 0)
	{
	// child
	execvpe(cmd_arg[0], cmd_arg, cmd_envp );
	if (errno != 0)
		{
		perror("execvpe");
		}
	exit(1);
	}
else
	{
	// parent
	waitpid(pid, &waitpid_status, WCONTINUED);

	char input_buffer[BUFFERSIZE];
	char type_string[BUFFERSIZE];
	char* pointer_string;
	int bytes;
	FILE* fdata;
	pList list;
	pIterator position;
	data_entry* data;

	fdata = fopen(TEMP_FILE, "r");
	if (errno != 0)
		{
		perror("fopen");
		exit(0);
		}
	fseek(fdata, 0, SEEK_SET);
		// seeking head of file

	// using a list to save and organize data in data file
	list = list_construct( &list_compare, &list_datadestruct);
	data_entry* data_comp = (data_entry*) malloc(sizeof(data_entry));
	while (!feof(fdata))
		{
		// inputting a line from file
		input_buffer[0] = '\0';
		fgets(input_buffer, sizeof(input_buffer), fdata);
		if (feof(fdata))
			{
			break;
			}

		// contrive type and pointer strings from input
		pointer_string = (char*) malloc(BUFFERSIZE * sizeof(char));
		sscanf(input_buffer, "%s %s", type_string, pointer_string);

		if (strcmp(type_string, "malloc") == 0)
			{
			// inserting data into list
			sscanf(input_buffer, "%*s %*s %d ", &bytes);
			data = (data_entry*) malloc(sizeof(data_entry));
			data->pointer_string = pointer_string;
			data->size = bytes;
			list_insert(list, data, list_iter_tail(list));

			}
		else if (strcmp(type_string, "free") == 0)
			{
			// checking if pointer already exists
			data_comp->pointer_string = pointer_string;
			position = list_elem_find(list, data_comp);
			if (position != NULL)
				{
				data = list_remove(list, position);
				list_datadestruct(data);
				}
			free(pointer_string);
			}
		}

	// outputting all memory leaks left in list
	int total = 0;
	int x = 0;
	while (list_size(list) != 0)
		{
		data = list_remove(list, list_iter_first(list)); 
		fprintf(stderr, "LEAK\t%d\n", data->size);
		total += data->size;
		list_datadestruct(data);
		x++;
		}

	fprintf(stderr, "TOTAL\t%d\t%d\n", x, total);

	list_destruct(list);
	fclose(fdata);
	remove(TEMP_FILE);

	}

return 0;

}






