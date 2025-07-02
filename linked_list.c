#include "linked_list.h"
#include <stdlib.h>
#include <string.h>

void* malloc_impl(size_t size)
{
	return malloc(size);
}

void free_impl(void* addr)
{
	free(addr);
}

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

//(void)(malloc_fptr);
//(void)(free_fptr);

struct linked_list * linked_list_create(void)
{
	struct linked_list *ll = malloc_fptr(sizeof(struct linked_list));
	if(!ll) return false;

	ll->head = ll->tail = NULL;
	ll->size = 0;

	return ll;
}

bool linked_list_delete(struct linked_list * ll)
{
	if(!ll) return false;

	while(ll->head)
	{
		struct node* nodeptr = ll->head;
		ll->head = nodeptr->next;
		free_fptr(nodeptr);
	}

	free_fptr(ll);
	
	return true;
}

size_t linked_list_size(struct linked_list* ll)
{
	if(!ll) return SIZE_MAX;
	return ll->size;
}

bool linked_list_insert_end(struct linked_list* ll, unsigned int data)
{
	if(!ll) return false;

	struct node* newnode = malloc_fptr(sizeof(struct node));
	if(!newnode) return false;
	
	newnode->next = NULL;
	newnode->data = data;
	if(!ll->tail) ll->head = newnode;
	else ll->tail->next = newnode;

	ll->tail = newnode;
	ll->size++;
	return true;
}

bool linked_list_insert_front(struct linked_list* ll, unsigned int data)
{
	if(!ll) return false;
	
	struct node* newnode = malloc_fptr(sizeof(struct node));
	if(!newnode) return false;

	newnode->data = data;
	newnode->next = ll->head;
	ll->head = newnode;
	if(ll->size == 0) ll->tail = newnode;
	ll->size++;

	return true;
}

bool linked_list_insert(struct linked_list* ll, size_t index, unsigned int data)
{
	if(!ll) return false;

	struct node* p = NULL;
	struct node* n = ll->head;

	while(index)
	{
		if(!n) return false;
		p = n;
		n = n->next;
		index--;
	}

	struct node* newnode = malloc_fptr(sizeof(struct node));
	if(!newnode) return false;

	newnode->next = n;
	newnode->data = data;
	if(p) p->next = newnode;
	else ll->head = ll->tail = newnode;

	ll->size++;

	return true;
}

size_t linked_list_find(struct linked_list* ll, unsigned int data)
{
	if(!ll) return SIZE_MAX;

	struct node *n = ll->head;
	size_t i = 0;
	while(n)
	{
		if(n->data == data) return i;
		i++;
		n = n->next;
	}

	return SIZE_MAX;
}

bool linked_list_remove(struct linked_list* ll, size_t index)
{
	if(!ll) return false;

	struct node* p = NULL;
	struct node* n = ll->head;

	while(index)
	{
		if(!n) return false;
		p = n;
		n = n->next;
		index--;
	}
	
	if(p) p->next = n->next;
	else ll->head = n->next;

	if(n == ll->tail) ll->tail = p;

	free_fptr(n);
	ll->size--;

	return true;
}

struct iterator * linked_list_create_iterator(struct linked_list* ll, size_t index)
{
	if(!ll) return NULL;
	struct node * curnode = ll->head;
	size_t i = index;
	i++;
	while(curnode && (--i > 0))
	{
		curnode = curnode->next;
	}

	if(i)
	{
		return NULL;
	}
		
	struct iterator * itr = malloc_fptr(sizeof(struct iterator));
	if(!itr) return NULL;

	itr->ll = ll;
	itr->current_node = curnode;
	itr->current_index = index;
	itr->data = curnode->data;
	return itr;
}

bool linked_list_delete_iterator(struct iterator * iter)
{
	if(!iter) return false;
	free_fptr(iter);
	return true;
}

bool linked_list_iterate(struct iterator * iter)
{
	if(!iter) return false;

	struct node* next_node;
	if((next_node = iter->current_node->next))
	{
		iter->current_node = next_node;
		iter->current_index++;
		iter->data = next_node->data;
		return true;
	}

	return false;
}

bool linked_list_register_malloc(void * (*malloc)(size_t))
{
	malloc_fptr = malloc;
	return true;
}

bool linked_list_register_free(void (*free)(void*))
{
	free_fptr = free;
	return true;
}
