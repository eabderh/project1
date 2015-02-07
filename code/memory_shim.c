/*---- FILE HEADER ----------------------------------------------------------
project: project1
file: memory_shim.c
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
#include <dlfcn.h>


#define TEMP_FILE "lc_temp~"

FILE* fdata;
//char TEMP_FILE[] = "lc_temp~";


//void __attribute__((constructor)) setup(void);


// function prototypes
void free(void* ptr);
void* malloc(size_t size);

void setup();
void free_setup (void* ptr);
void* malloc_setup (size_t size);

void free_data (void* ptr);
void* malloc_data (size_t size);

// function pointers
void (*free_var) (void* ptr) = &free_setup;
void* (*malloc_var) (size_t size) = &malloc_setup;
	// initialized to setup for first time use

void (*free_normal) (void* ptr);
void* (*malloc_normal) (size_t size);






void free(void* ptr)
{
(*free_var)(ptr);
return;
}


void* malloc(size_t size)
{
void* ptr = (*malloc_var)(size);
return ptr;
}



void setup(void)
{

free_normal = dlsym(RTLD_NEXT, "free");
malloc_normal = dlsym(RTLD_NEXT, "malloc");


free_var = free_normal; 
malloc_var = malloc_normal;

fdata = fopen(TEMP_FILE, "w+");
if (fdata == NULL)
	{
	perror("fopen");
	return;
	}

free_var = free_data;
malloc_var = malloc_data;

return;
}



void free_setup (void* ptr)
{
setup();
free_data(ptr);
return;
}



void* malloc_setup (size_t size)
{
setup();
void* ptr = malloc_data(size);
return ptr;
}



void free_data (void* ptr)
{
free_var = free_normal; 
malloc_var = malloc_normal;

fprintf(fdata, "free %p\n", ptr);
fflush(fdata);
free(ptr);

free_var = free_data;
malloc_var = malloc_data;

return;
}



void* malloc_data (size_t size)
{
void* ptr;

free_var = free_normal; 
malloc_var = malloc_normal;

ptr = malloc(size);
fprintf(fdata, "malloc %p %d\n", ptr, (int)size);
fflush(fdata);

free_var = free_data;
malloc_var = malloc_data;

return ptr;
}



