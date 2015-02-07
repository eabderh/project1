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
description: list.c contains the code for implementing a two way linked list
	data structure. this includes all the functions needed to edit and access
	data within the list. in addition, functions for sorting the data have
	been added.
notes: none
assumptions: none
bugs: none
---------------------------------------------------------------------------*/



#include <stdio.h> // CHECK CHECK CHECK
#include <stdlib.h>
#include <assert.h>

#include "list.h"
//#include "datatypes.h"

#define LIST_SORTED   -1234
#define LIST_UNSORTED -4132



// prototypes for private functions used in list.c only
void list_validate(list_t* L);
llist_node_t* findmax( list_t* list_ptr, llist_node_t* startnode, llist_node_t* lastnode);

// sort #1
list_t* sort_insertion(list_t* list_ptr);
// sort #2
list_t* sort_recursiveselection(list_t* list_ptr);
void recursiveselection_r(list_t* list_ptr, llist_node_t* startnode, llist_node_t* lastnode);
// sort #3
list_t* sort_iterativeselection(list_t* list_ptr);
// sort #4
list_t* sort_merge(list_t* list_ptr);
list_t* mergesort_r(list_t* firstlist);
list_t* mergesort_partition(list_t* list_ptr);
list_t* mergesort_merge(list_t* list_a, list_t* list_b);



/*---------------------------------------------------------------------------
function: list_construct
description: allocates and sets up a list structure and the corresponding
	dummy nodes. the list will be initialized with size being zero and marked
	as sorted.
notes: none
input: none
output:	pointer to the initialized list structure
*/

list_t*  list_construct(int (*datacompare_fptr)(data_t data1, data_t data2),
						void (*datadestruct_fptr)(data_t data))
{
list_t* L = NULL;
llist_node_t* insertnode_ptr;

if (datacompare_fptr  == NULL || datadestruct_fptr == NULL)
	{
	exit(1);
	}


// allocating and initializing the list_t structure
L = (list_t*) malloc(sizeof(list_t));
if (L == NULL)
	{
	exit(1);
	}
L->llist_head = NULL;
L->llist_tail = NULL;
L->llist_size = 0;
L->llist_sort = LIST_SORTED;
L->datacompare_fptr = datacompare_fptr;
L->datadestruct_fptr = datadestruct_fptr;


// allocating and initializing the header dummy node structure
insertnode_ptr = (llist_node_t*) malloc(sizeof(llist_node_t));
if (insertnode_ptr == NULL)
	{
	exit(1);
	}
// grounding
insertnode_ptr->data_ptr = NULL;
insertnode_ptr->prev = NULL;
insertnode_ptr->next = NULL;
// setting the list head pointer
L->llist_head = insertnode_ptr;
insertnode_ptr = NULL;

// allocating and initializing the tail dummy node structure
insertnode_ptr = (llist_node_t*) malloc(sizeof(llist_node_t));
if (insertnode_ptr == NULL)
	{
	exit(1);
	}
// grounding
insertnode_ptr->data_ptr = NULL;
insertnode_ptr->prev = NULL;
insertnode_ptr->next = NULL;
// setting the tail head pointer
L->llist_tail = insertnode_ptr;
insertnode_ptr = NULL;

// linking the two dummy nodes
L->llist_head->next = L->llist_tail;
L->llist_tail->prev = L->llist_head;

/* the last line of this function must call validate */
//list_validate(L);
return L;
}



/*---------------------------------------------------------------------------
function: list_destruct
description: deallocates all the memory associated with a given list.
notes: all data is freed and therefore assumed to be allocated
input:	list_ptr - pointer to list to be deallocated
output:	none
*/

void list_destruct(list_t* list_ptr)
{
llist_node_t* delnode_ptr = NULL;
llist_node_t* currentnode_ptr = NULL;

/* the first line must validate the list */
//list_validate(list_ptr);

currentnode_ptr = list_ptr->llist_head;
// algorithm: deletes nodes and associated data pointers by moving along
// delnode_ptr lead by nextnode. includes the removal of the dummy nodes
while (currentnode_ptr != NULL)
	{
	// the data pointer is removed from the current node
	if (currentnode_ptr->data_ptr != NULL)
		{
		(*list_ptr->datadestruct_fptr)(currentnode_ptr->data_ptr);
		currentnode_ptr->data_ptr = NULL;
		}
	delnode_ptr = currentnode_ptr;
	currentnode_ptr = currentnode_ptr->next;
		// the currentnode_ptr is moved to the nextnode
	// delnode_ptr is grounded and freed
	delnode_ptr->prev = NULL;
	delnode_ptr->next = NULL;
	free(delnode_ptr);
	delnode_ptr = NULL;
	}

// grounding the list structure
list_ptr->llist_head = NULL;
list_ptr->llist_tail = NULL;
free(list_ptr);
list_ptr = NULL;

return;
}



/*---------------------------------------------------------------------------
function: list_iter_first
description: returns a pointer to the node that comes right after the dummy
	header node
notes: 	- the returned pointer can point to the dummy tail node if the list
			is empty.
		- the input pointer to a list must be valid, otherwise program stops
input:	list_ptr - pointer to a list
output:	pointer to a node
*/

llist_node_t* list_iter_first(list_t* list_ptr)
{
assert(list_ptr != NULL);
//list_validate(list_ptr);
return list_ptr->llist_head->next;
}



/*---------------------------------------------------------------------------
function: list_iter_tail
description: returns pointer to the dummy tail node
notes: the input pointer to a list must be valid, otherwise program stops
input:	list_ptr - pointer to a list
output: pointer to a node
*/

llist_node_t* list_iter_tail(list_t* list_ptr)
{
assert(list_ptr != NULL);
//list_validate(list_ptr);
return list_ptr->llist_tail;
}



/*---------------------------------------------------------------------------
function: list_iter_next
description: function that returns a pointer to the next node to the given
	node
notes: - program will stop if the input pointer is either NULL or points to
	the dummy tail block
input:	idx_ptr - pointer to a node
output:	pointer to the next node
*/

llist_node_t* list_iter_next(llist_node_t* idx_ptr)
{
assert(idx_ptr != NULL && idx_ptr->next != NULL);
return idx_ptr->next;
}



/*---------------------------------------------------------------------------
function: list_access
description: returns the data pointer associated with the given node. if the
	list is emtpy or the given node is either of the dummy head or tail
	nodes, NULL is returned.
notes: the input node pointer is assumed to be a valid pointer to a node
	structure
input:	list_ptr - pointer to a list
		idx_ptr - pointer to a node within the list
output:	pointer to a data_t structure. returns NULL in above mentioned cases
*/

data_t list_access(list_t* list_ptr, llist_node_t* idx_ptr)
{
assert(list_ptr != NULL);
assert(idx_ptr != NULL);
/* debugging function to verify that the structure of the list is valid */
//list_validate(list_ptr);

if ((list_ptr->llist_size == 0) ||
	(idx_ptr == list_ptr->llist_head) ||
	(idx_ptr == list_ptr->llist_tail))
	{
	return NULL;
	}

return idx_ptr->data_ptr;
}



/*---------------------------------------------------------------------------
function: list_elem_find
description: finds the first node in the list that has data matching the
	input data. the list_compare() function is used for the comparison where a
	return of 0 is considered a match. if there are no matches NULL is
	returned.
notes: function will fail if either input is NULL
input:	list_ptr - pointer to a list
		elem_ptr - pointer to data
output:	pointer to the node found
*/

llist_node_t* list_elem_find(list_t* list_ptr, data_t elem_ptr)
{

llist_node_t* rovingnode_ptr = NULL;

// validation
assert(list_ptr != NULL);
assert(elem_ptr != NULL);
//list_validate(list_ptr);

/* algorithm: the roving node moves down the list starting with the node
after the dummy header. if the roving node's data matches the input data, the
function returns the roving node. if the rovingnode encounters the dummy tail
the function returns a NULL since no matches were found*/
rovingnode_ptr = list_ptr->llist_head->next;
while (1)
	{
	if (rovingnode_ptr == list_ptr->llist_tail)
		{
		return NULL;
		}
	if ((*list_ptr->datacompare_fptr)(rovingnode_ptr->data_ptr, elem_ptr) == 0)
		{
		return rovingnode_ptr;
		}
	rovingnode_ptr = rovingnode_ptr->next;
	}
}



/*---------------------------------------------------------------------------
function: list_insert
description: adds data to a list. the given pointer to the data is included
	in a new node structure, which is then added to the list. the position it
	is added to is right before the given and user defined node.
notes: 	- the input node is assumed to be any node including the dummy tail
	but	excluding the dummy header node.
		- the function will change a sorted list into an unsorted list
input:	list_ptr - pointer to a list
		elem_ptr - pointer to the data
		idx_ptr - pointer to a node
output:	none
*/

void list_insert(list_t* list_ptr,
	data_t elem_ptr,
	llist_node_t* idx_ptr)
{
llist_node_t* leadingnode_ptr = NULL;
llist_node_t* insertnode_ptr = NULL;

// validation
//assert(list_ptr != NULL);
//list_validate(list_ptr);

// allocating and initializing the node to be added
insertnode_ptr = (llist_node_t*) malloc(sizeof(llist_node_t));
if (insertnode_ptr == NULL)
	{
	exit(1);
	}
insertnode_ptr->data_ptr = elem_ptr;
insertnode_ptr->prev = NULL;
insertnode_ptr->next = NULL;

// initializing the node that will come before the insert node
leadingnode_ptr = idx_ptr->prev;
// linking the insert node to the node before it
insertnode_ptr->prev = leadingnode_ptr;
leadingnode_ptr->next = insertnode_ptr;
// linking the insert node to the node following it
insertnode_ptr->next = idx_ptr;
idx_ptr->prev = insertnode_ptr;
// incrementing the list size
list_ptr->llist_size +=1;

/* the last two lines of this function must be the following */
if (list_ptr->llist_sort == LIST_SORTED)
	{
	list_ptr->llist_sort = LIST_UNSORTED;
	}
//list_validate(list_ptr);
return;
}



/*---------------------------------------------------------------------------
function: list_insert_sorted
description: adds data to a sorted list. the data will be inserted in
	accordance to the list_compare() function defined in datatypes. the data
	will be inserted before the node that the list_compare has returned a 1 to.
	therefore if data is considered equal, the last data inserted will be
	last on the group of equal data.
notes: function will fail if the input list is unsorted
input:	list_ptr - pointer to a sorted list
		elem_ptr - pointer to the data
output:	none
*/

void list_insert_sorted(list_t* list_ptr, data_t elem_ptr)
{
llist_node_t* leadingnode_ptr = NULL;
llist_node_t* rovingnode_ptr = NULL;
llist_node_t* insertnode_ptr = NULL;

// validation
assert(list_ptr != NULL);
assert(list_ptr->llist_sort == LIST_SORTED);

// allocating and initializing the node to be added
insertnode_ptr = (llist_node_t*) malloc(sizeof(llist_node_t));
if (insertnode_ptr == NULL)
	{
	exit(1);
	}
insertnode_ptr->data_ptr = elem_ptr;
insertnode_ptr->prev = NULL;
insertnode_ptr->next = NULL;

/*algorithm: the rovingnode will move along the list starting with the first
node after the dummy header node. if the rovingnode equals the dummy tail or
if the comparison function shows that the inserted data should come before
the rovingnode's data, the loop is exited. the data is then inserted before
the rovingnode*/
rovingnode_ptr = list_ptr->llist_head->next;
while (1)
	{
	if (rovingnode_ptr == list_ptr->llist_tail)
		{
		break;
		}
	if ((*list_ptr->datacompare_fptr)(elem_ptr, rovingnode_ptr->data_ptr) == 1)
		{
		break;
		}
	rovingnode_ptr = rovingnode_ptr->next;
	}

// initializing the node that will come before the insert node
leadingnode_ptr = rovingnode_ptr->prev;
// linking the insert node to the node before it
insertnode_ptr->prev = leadingnode_ptr;
leadingnode_ptr->next = insertnode_ptr;
// linking the insert node to the node following it
insertnode_ptr->next = rovingnode_ptr;
rovingnode_ptr->prev = insertnode_ptr;
// incrementing the list size
list_ptr->llist_size +=1;


/* the last line of this function must be the following */
//list_validate(list_ptr);
return;
}



/*---------------------------------------------------------------------------
function: list_remove
description: removes a given node from a list. the data pointer is returned
notes:	- the input node pointer is assumed to be valid for the given list
		- the function will fail if the input node is either the dummy header
			or tail
		- warning: the input node will be freed. therefore the node pointer/
			llist_node_t* of the calling function will be invalid and dangling
input:	list_ptr - pointer to a list
		idx_ptr - pointer to a node in the list
output:	pointer to data of node that was removed. if the node pointer is
	NULL then NULL is returned
*/

data_t list_remove(list_t* list_ptr, llist_node_t* idx_ptr)
{
llist_node_t* prevnode_ptr = NULL;
llist_node_t* nextnode_ptr = NULL;
data_t ret_val;

// validation
assert(list_ptr != NULL);
if (idx_ptr == NULL)
	{
	return NULL;
	}

assert(idx_ptr != list_ptr->llist_head);
assert(idx_ptr != list_ptr->llist_tail);

// saving the data before the node is freed
ret_val = idx_ptr->data_ptr;
// initializing adjacent nodes
prevnode_ptr = idx_ptr->prev;
nextnode_ptr = idx_ptr->next;
// linking the adjacent with each other, excluding the node to be removed
prevnode_ptr->next = nextnode_ptr;
nextnode_ptr->prev = prevnode_ptr;
// grounding any pointers in the node structure and deallocating it
idx_ptr->prev = NULL;
idx_ptr->next = NULL;
idx_ptr->data_ptr = NULL;
free(idx_ptr);
idx_ptr = NULL;
// decrementing size
list_ptr->llist_size -=1;

/* the last line should verify the list is valid after the remove */
//list_validate(list_ptr);
return ret_val;
}



/*---------------------------------------------------------------------------
function: list_size
description: function returns the number of nodes in the given list,
	excluding the dummy header and tail nodes
notes: function will fail if the input list pointer is NULL
input:	list_ptr - pointer to a list
output:	size of the list
*/

int list_size(list_t* list_ptr)
{
assert(list_ptr != NULL);
assert(list_ptr->llist_size >= 0);
return list_ptr->llist_size;
}



/*---------------------------------------------------------------------------
function: list_validate
description: given below
notes: not written by the files author
input:	L - pointer to a list
output:	none
*/

/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY
 * THAT YOUR LIST IS CONSISTENT.
 */

void list_validate(list_t* L)
{
llist_node_t* N;
int count = 0;
assert(L != NULL && L->llist_head != NULL && L->llist_tail != NULL);
assert(L->llist_head != L->llist_tail);
assert(L->llist_head->prev == NULL && L->llist_tail->next == NULL);
assert(L->llist_head->data_ptr == NULL && L->llist_tail->data_ptr == NULL);
assert(L->llist_head->next != NULL && L->llist_tail->prev != NULL);
assert(L->llist_sort == LIST_SORTED || L->llist_sort == LIST_UNSORTED);

N = L->llist_head->next;
assert(N->prev == L->llist_head);
while (N != L->llist_tail)
	{
	assert(N->data_ptr != NULL);
	assert(N->next->prev == N);
	count++;
	N = N->next;
	}
assert(count == L->llist_size);
if (L->llist_sort == LIST_SORTED && L->llist_size > 0)
	{
	N = L->llist_head->next;
	while (N->next != L->llist_tail)
		{
		assert((*L->datacompare_fptr)(N->data_ptr, N->next->data_ptr) != -1);
		N = N->next;
		}
	}
}



/*---------------------------------------------------------------------------
function: list_sortstatus
description: checks if a given list is sorted or not
notes: none
input:	list_ptr - pointer to a list
output:	returns 1 if list is sorted, otherwise a 0 is returned
*/
int list_sortstatus(list_t* list_ptr)
{
if (list_ptr->llist_sort == LIST_SORTED)
	{
	return 1;
	}
return 0;
}



/*---------------------------------------------------------------------------
function: list_sort
description: runs a certain sort function depending on the input
notes: none
input:	list_ptr - pointer to a list to be sorted
		sort_type - integer to indicate which sort algorithm should be used
output:	returns pointer to a sorted list. if the input list pointer is NULL
	or if the sort type is incorrect, NULL is returned.
*/

list_t* list_sort(list_t* list_ptr, int sort_type)
{

list_t* list_sorted = NULL;

if ((list_ptr == NULL) || (sort_type < 1) || (sort_type > 4))
	{
	return NULL;
	}

if (sort_type == 1)
	{
	list_sorted = sort_insertion(list_ptr);
	}
if (sort_type == 2)
	{
	list_sorted = sort_recursiveselection(list_ptr);
	}
if (sort_type == 3)
	{
	list_sorted = sort_iterativeselection(list_ptr);
	}
if (sort_type == 4)
	{
	list_sorted = sort_merge(list_ptr);
	}

list_validate(list_sorted);
printf("List sorted\n");
return list_sorted;
}



/*---------------------------------------------------------------------------
function: sort_insertion
description: uses insertion sort to sort the given list. all nodes of the
	input list are deleted and the data is reinserted into a sorted list.
notes: function will fail if the input is NULL or if the input list is empty
input:	list_ptr - pointer to a list to be sorted
output:	a pointer to a sorted list is returned
*/

list_t* sort_insertion(list_t* list_ptr)
{
list_t* list_sorted;
llist_node_t* rovingnode;
llist_node_t* currentnode;
llist_node_t* lastnode;
data_t temprec_ptr;

// validation
//assert(list_ptr != NULL);
//assert(list_ptr->llist_size >= 0);
	// assertion that the list is not empty

// constructing the new list that will be sorted
list_sorted = list_construct(	list_ptr->datacompare_fptr, 
								list_ptr->datadestruct_fptr);

// initializing the node pointers
rovingnode = list_ptr->llist_head->next;
lastnode = list_ptr->llist_tail;

/* algorithm: rovingnode starts with the first node after the header node
and moves down the list until it reaches the tail node. the currentnode
points to the node one previous to the rovingnode. it can therefore be
deleted and the rovingnode will still be valid. every currentnode is
removed using list_remove. the data is then reinserted into the new sorted
list using list_insert_sorted */
while(rovingnode != lastnode)
	{
	currentnode = rovingnode;
	rovingnode = currentnode->next;

	temprec_ptr = list_remove(list_ptr, currentnode);
	//currentnode = NULL;
	list_insert_sorted(list_sorted, temprec_ptr);
	//temprec_ptr = NULL;
	}

// deleting the old unsorted list.
list_destruct(list_ptr);
list_ptr = NULL;

// setting the new list as sorted. not really necessary since all new lists
// are initialized as sorted
list_sorted->llist_sort = LIST_SORTED;

return list_sorted;
}


/*---------------------------------------------------------------------------
function: sort_recursiveselection
description: initializes a recursive function
notes: function returns and does not change anything on the list if the list
	is empty
input: 	list_ptr - pointer to list to be sorted
output: pointer to the sorted list
*/

list_t* sort_recursiveselection(list_t* list_ptr)
{
if (list_ptr->llist_size == 0)
	{
	return list_ptr;
	}

recursiveselection_r( list_ptr, list_ptr->llist_head->next, list_ptr->llist_tail->prev);
list_ptr->llist_sort = LIST_SORTED;

return list_ptr;
}



/*---------------------------------------------------------------------------
function: recursiveselection_r
description: recursive sorting function. finds the node that should be in
	the last place on the list and switches it with node currently in last
	place. calls itself to repeat the process but without the last node
	being included since it is already in the right spot.
notes: function depends on findmax function
input: 	startnode - pointer to starting node
		lastnode - pointer to ending node
output: none
*/

void recursiveselection_r(list_t* list_ptr, llist_node_t* startnode, llist_node_t* lastnode)
{
llist_node_t* tempnode;
data_t tempdata;
if (startnode != lastnode)
	{
	tempnode = findmax(list_ptr ,startnode, lastnode);

	// switching the data of the two nodes
	tempdata = tempnode->data_ptr;
	tempnode->data_ptr = lastnode->data_ptr;
	lastnode->data_ptr = tempdata;

	recursiveselection_r(list_ptr, startnode, lastnode->prev);
	}
return;
}



/*---------------------------------------------------------------------------
function: findmax
description: finds the node with the data that should go on the bottom of
	the list. the function compares all nodes between and including the two
	input nodes.
notes: none
input:	startnode - pointer to first node
		lastnode - pointer to last node
output: returns the node considered the max in the list
*/

llist_node_t* findmax( list_t* list_ptr, llist_node_t* startnode, llist_node_t* lastnode)
{
// initializing the maxnode to the lastnode
llist_node_t* maxnode = lastnode;
while (startnode != lastnode)
	{
	// comparison
	if ((*list_ptr->datacompare_fptr)(startnode->data_ptr, maxnode->data_ptr) == -1)
		{
		maxnode = startnode;
		}
	// moving node up the list
	startnode = startnode->next;
	}
return maxnode;
}




/*---------------------------------------------------------------------------
function: sort_iterativeselection
description: iterative sorting function. finds the node that should be in
	the last place on the list and switches it with node currently in last
	place. repeats the process but without the last node being included
	since it is already in the right spot.
notes:	- function depends on findmax function
		- if the input list is empty, it is returned as is
input: 	list_ptr - pointer to list to be sorted
output: none
*/

list_t* sort_iterativeselection(list_t* list_ptr)
{
llist_node_t* startnode;
llist_node_t* lastnode;
llist_node_t* tempnode;
data_t tempdata;

if (list_ptr->llist_size == 0)
	{
	return list_ptr;
	}

startnode = list_ptr->llist_head->next;
lastnode = list_ptr->llist_tail->prev;

while (startnode != lastnode)
	{
	tempnode = findmax( list_ptr, startnode, lastnode);

	// switching the data of the two nodes
	tempdata = tempnode->data_ptr;
	tempnode->data_ptr = lastnode->data_ptr;
	lastnode->data_ptr = tempdata;

	lastnode = lastnode->prev;
	}
list_ptr->llist_sort = LIST_SORTED;
return list_ptr;
}



/*---------------------------------------------------------------------------
function: sort_merge
description: initializes the mergesort algorithm of sorting a list. uses
	mergesort_r to sort the list
notes:	- the merge sort algorithm splits the list in half, sorts both half
			lists using a recursive call to itself, and then combines the
			two half lists so that the combined list is also sorted. Only
			once the lists are split until they each have 1 element does the
			function return to form the completed list.
		- if the input list is empty, it is returned as is
input: 	list_ptr - pointer to list to be sorted
output: pointer to sorted list
*/

list_t* sort_merge(list_t* list_ptr)
{
if (list_ptr->llist_size == 0)
	{
	return list_ptr;
	}
// starting the recursive function
list_ptr = mergesort_r(list_ptr);
list_ptr->llist_sort = LIST_SORTED;
return list_ptr;
}


/*---------------------------------------------------------------------------
function: mergesort_r
description: recursive function of the mergesort algorithm.
notes:	- the merge sort algorithm splits the list in half, sorts both half
			lists using a recursive call to itself, and then combines the
			two half lists so that the combined list is also sorted. Only
			once the lists are split until they each have 1 element does the
			function return to form the completed list.
		- depends on the functions mergesort_partition and mergesort_merge
input: 	firstlist - pointer to list to be sorted
output: pointer to the sorted list
*/

list_t* mergesort_r(list_t* firstlist)
{
list_t* secondlist;
list_t* list_sorted;
// checking if list has only one node
if (firstlist->llist_size == 1)
	{
	return firstlist;
	}
else
	{
	// splitting the first list to get a second list
	secondlist = mergesort_partition(firstlist);
	// sorting both lists
	firstlist = mergesort_r(firstlist);
	secondlist = mergesort_r(secondlist);
	// combining the two sorted lists
	list_sorted = mergesort_merge(firstlist, secondlist);
	return list_sorted;
	}


}



/*---------------------------------------------------------------------------
function: mergesort_partition
description: takes a list and splits it into half.
notes: initializes a new list using list_construct()
input: 	firstlist - pointer to list to be partitioned
output: pointer to the second half list
*/

list_t* mergesort_partition(list_t* firstlist)
{
list_t* secondlist;
llist_node_t* rovingnode;
llist_node_t* lastnode;
int size_first;
int size_second;
int x;

secondlist = list_construct(	firstlist->datacompare_fptr, 
								firstlist->datadestruct_fptr);


// finding new list sizes
size_first = firstlist->llist_size / 2;
size_second = firstlist->llist_size - size_first;

// loop to run until rovingnode has reached half-way node
rovingnode = firstlist->llist_head->next;
x = 0;
while (x < size_first)
	{
	rovingnode = list_iter_next(rovingnode);
	x++;
	}

// splitting the list in half

// setting the tails of both lists to point to the last node in the list
lastnode = firstlist->llist_tail->prev;
lastnode->next = secondlist->llist_tail;
secondlist->llist_tail->prev = lastnode;

// resetting lastnode to point to the new last node of the first list
lastnode = rovingnode->prev;

// linking the tail of firstlist with the last node in the first half list
lastnode->next = firstlist->llist_tail;
firstlist->llist_tail->prev = lastnode;

// linking the head of secondlist with the first node in the second half list
rovingnode->prev = secondlist->llist_head;
secondlist->llist_head->next = rovingnode;

// resetting sizes
firstlist->llist_size = size_first;
secondlist->llist_size = size_second;

return secondlist;
}



/*---------------------------------------------------------------------------
function: mergesort_merge
description: merges two sorted lists so that the resulting list is also
	sorted.
notes:	- initializes a new list using list_construct()
		- depends on list_compare for comparison
input: 	list_a - pointer to a sorted list
		list_b - pointer to a sorted list
output: pointer to a sorted list
*/

list_t* mergesort_merge(list_t* list_a, list_t* list_b)
{
list_t* templist;
list_t* list_sorted;
llist_node_t* node_a;
llist_node_t* node_b;
llist_node_t* node_a_tail;
llist_node_t* node_b_tail;
llist_node_t* node_remove;
llist_node_t** tempnode_ptr;
llist_node_t* nodetail;
data_t tempdata;

list_sorted = list_construct(	list_a->datacompare_fptr, 
								list_a->datadestruct_fptr);

// loop initializations
node_a = list_a->llist_head->next;
node_b = list_b->llist_head->next;
node_a_tail = list_a->llist_tail;
node_b_tail = list_b->llist_tail;
nodetail = list_sorted->llist_tail;

/* algorithm: loop runs until both lists have reached their end. if neither
list has ended, then a comparison is done between the two current nodes.
tempnode_ptr contains a pointer to the current node that is removed and
inserted into the sorted list*/
while ((node_a != node_a_tail) || (node_b != node_b_tail))
	{

	if (node_a == node_a_tail)
		{
		templist = list_b;
		tempnode_ptr = &node_b;
		}
	else if (node_b == node_b_tail)
		{
		templist = list_a;
		tempnode_ptr = &node_a;
		}
	else if ((*list_a->datacompare_fptr)(node_a->data_ptr, node_b->data_ptr) == -1)
		{
		templist = list_b;
		tempnode_ptr = &node_b;
		}
	else
		{
		templist = list_a;
		tempnode_ptr = &node_a;
		}

	node_remove = *tempnode_ptr;
	*tempnode_ptr = (*tempnode_ptr)->next;
	tempdata = list_remove(templist, node_remove);
	node_remove = NULL;
	list_insert(list_sorted, tempdata, nodetail);
	}

list_sorted->llist_sort = LIST_SORTED;

// deallocating the two input lists
list_destruct(list_a);
list_destruct(list_b);

return list_sorted;
}
