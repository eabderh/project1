/*---- FILE HEADER ----------------------------------------------------------
project: project1
file: 
dependencies: none
-----------------------------------------------------------------------------
student/author: elias abderhalden
date: 2014-09-12
-----------------------------------------------------------------------------
class: ece3220 fall 2014
instructor: jacob sorber
assignment: 
purpose: the goal of mp4 is to devise and write a method for dynamic memory
	managment. to some extend, both the malloc() and the free() function
	used for dynamic memory managment in the c language are reimplemented.
	the memory managment code is in mem.c and mem.h and the code for using
	and testing the memory managment method is in lab4.c, list.c, and list.h
-----------------------------------------------------------------------------
description: list.h has function headers for list.c
notes: none
assumptions: none
bugs: none
---------------------------------------------------------------------------*/




typedef void* data_t;

// header structure for the two way linked list
typedef struct llist_node_tag
	{
	// Private members for list.c only
	data_t data_ptr;
	struct llist_node_tag* prev;
	struct llist_node_tag* next;
	} llist_node_t;

// data structure for a node in the two way linked list
typedef struct list_tag
	{
	llist_node_t* llist_head;
	llist_node_t* llist_tail;
	int llist_size;		// number of items in list
	int llist_sort;		// a flag if list is sorted or unsorted
	int (*datacompare_fptr) (data_t data1, data_t data2);
	void (*datadestruct_fptr) (data_t data);
	} list_t;







// public data types

// setting public definitions of pointers to linked list and nodes
typedef llist_node_t* pIterator;
typedef list_t* pList;



// public function headers

// build and cleanup functions
list_t*  list_construct(int (*datacompare_fptr)(data_t data1, data_t data2),
			void (*datadestruct_fptr)(data_t data));
void list_destruct(pList list_ptr);

// iterators into positions in the list
pIterator list_iter_first(pList list_ptr);
pIterator list_iter_tail(pList list_ptr);
pIterator list_iter_next(pIterator idx_ptr);

// access functions
data_t list_access(pList list_ptr, pIterator idx_ptr);
pIterator list_elem_find(pList list_ptr, data_t elem_ptr);

// functions for inserting data
void list_insert(pList list_ptr, data_t elem_ptr, pIterator idx_ptr);
void list_insert_sorted(pList list_ptr, data_t elem_ptr);

// function for removing data
data_t list_remove(pList list_ptr, pIterator idx_ptr);

// other functions
int list_size(pList list_ptr);
int list_sortstatus(pList list_ptr);
//extern int list_compare(data_t a, data_t b);
list_t* list_sort(list_t* list_ptr, int sort_type);


