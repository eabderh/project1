/*---- FILE HEADER ----------------------------------------------------------
project: project1
file: sctracer.c
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
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <errno.h>

#include "list.h"


typedef struct data_entry_tag 
	{
	int call;
	int num;
	int returned_flag;
	} data_entry;

#define a ((data_entry*)input_a)
#define b ((data_entry*)input_b)

int list_compare(void* input_a, void* input_b)
{

if (a->call < b->call) return 1;
else if (a->call > b->call) return -1;
else return 0;
}




void list_datadestruct(void* input_a)
{
free(a);
return;
}



int main(int argc, char* argv[])
{
char* cmd_envp[] = {NULL};
char* cmd_arg[10];
char delims[] = " \0";
int x;

if (argc == 2)
	{

	cmd_arg[0] = strtok(argv[1], delims);
	x = 0;
	while (cmd_arg[x] != NULL)
		{
		x++;
		cmd_arg[x] = strtok(NULL, delims);
		}
	}
else
	{
	printf("Wrong arguments\n");
	return 0;
	}


pid_t pid;

pid = fork();
if (pid < 0)
	{
	perror("fork\n");
	}
else if (pid == 0)
	{
	// child
	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP);
		// stopping process until parent is ready
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
	int pid_status;
	int syscall;

	pList list;
	data_entry comp_entry;
	data_entry* temp_entry;
	pIterator position;

	list = list_construct(&list_compare, &list_datadestruct);


	waitpid(pid, &pid_status, 0);
		// waiting until child is ready
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
		// setting up ptrace options to stop at system calls
	while (1)
		{
		ptrace(PTRACE_SYSCALL, pid, 0, 0);
			// restarting child process
		waitpid(pid, &pid_status, 0);
			// waiting until something has stopped child process
		if (WIFSTOPPED(pid_status) && (WSTOPSIG(pid_status) & 0x80))
			// checking if child stopped because of sys call
			{
			syscall = ptrace(PTRACE_PEEKUSER, pid, sizeof(long)*ORIG_RAX, NULL);
				// get info on sys calll
			if (errno != 0)
				{
				//printf("te");
				perror("peek");
				}

			// storing and updating sys call in list
			comp_entry.call = syscall;
			position = list_elem_find(list, &comp_entry);
			if (position == NULL)
				{
				temp_entry = (data_entry*) malloc(sizeof(data_entry));
				//temp_entry = {.call = syscall; .num = 1; .returned_flag = 1};
				temp_entry->call = syscall;
				temp_entry->num = 1;
				temp_entry->returned_flag = 0;
				list_insert_sorted(list, temp_entry);
				}
			else
				{
				temp_entry = list_access(list, position);
				if (temp_entry->returned_flag == 1)
					{
					temp_entry->returned_flag = 0;
					temp_entry->num++;
					}
				else
					{
					temp_entry->returned_flag = 1;
					}
				}


			}
		else if (WIFEXITED(pid_status))
			// child exited so loop ends
			{
			break;
			}

		}
	
	// printing results
	while (list_size(list) != 0)
		{
		temp_entry = list_remove(list, list_iter_first(list));
		fprintf(stdout, "%d\t%d\n", temp_entry->call, temp_entry->num);
		list_datadestruct(temp_entry);
		}
	list_destruct(list);
	}

return 0;
}




