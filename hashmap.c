/*
	This hashmap implementation is a product of following tutorials 
	online for creating hashmaps using C.  The result is an implementation
	that is not completely of my original design.  But, creating a hashmap 
	is not the purpose of pa5, so whatever.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linkedlist.h"
#include "hashmap.h"

typedef struct var* var;

struct var {
	string name;
	void* data;
};

var newVar(string name, void* data)
{
	var v 	= malloc(sizeof(*v));
	v->name = name;
	v->data = data;
	return v;
}


map newMap(size_t size, double load)
{
	map m 		= malloc(sizeof(*m));
	m->size 	= size;
	m->count 	= 0;
	m->load 	= load;
	m->values 	= malloc(sizeof(linkedList) * size);

	fillEmpty(m);

	return m;
}

void deleteVar(void* va)
{
	var v = (var) va;
	free(v->data);
	free(v);
}

void deleteMap(map m)
{
	int i;
	for (i = 0; i < m->size; i++)
		deleteLinkedList(m->values[i], deleteVar);
	free(m->values);
	free(m);
}


void insertMapValue(string key, void* value, map m)
{
	if(m->count >= m->size * m->load)
		resizeMap(m, 2);

	size_t index 	= hashKey(key) % m->size;

	var v 			= newVar(key, value);
	Node node 		= newLinkedNode(v);
	linkedList list = m->values[index];

	if(!list)
		list = m->values[index] = newLinkedList(NULL);

	if(((var) (node->data))->data != value)
		abort();

	appendLinkedNode(node, list);
	m->count++;
}

void resizeMap(map m, size_t scale)
{
	linkedList* old = m->values;
	size_t size = scale * m->size;
	size_t oldSize = m->size;

	m->size = size;
	m->values = malloc(sizeof(linkedList) * size);

	fillEmpty(m);

	size_t index;

	for(index = 0; index < oldSize; index++){
		linkedList list = old[index];

		if(list){
			while(!linkedListIsEmpty(list)){
				Node node = removeLinkedListHead(list);
				var v = node->data;
				m->count--;
				insertMapValue(v->name, v->data, m);
				free(v);
				free(node);
			}

			free(list);
		}
	}

	free(old);
}

int hashKey(string key)
{
	int hash = 140210887;
	char* c;

	for(c = key; *c; c++){
		hash *= 84567211;
		hash ^= (int) *c;
		hash %= 2147483647;
	}

	return hash;
}

void* getKeyValue(string key, map m)
{
	linkedList list = m->values[hashKey(key) % m->size];

	if(!list || !list->head)
		return NULL;

	Node node = list->head;
	var v = (var) node->data;

	while(node && strcmp(key, v->name)){
		node = node->next;
		v = node ? (var) node->data : NULL;
	}

	return node ? v->data : NULL;
}

void fillEmpty(map m)
{
	size_t index;

	for(index = 0; index < m->size; index++)
		m->values[index] = NULL;
}

void printValue(void* value)
{
	var v = (var) value;

	int val = *((int*) v->data);

	printf("%s at index %d ", v->name, val);
}

void printMap(map m)
{
	int i;

	printf("Map Size: %zu\n", m->size);

	if (m) {
		for (i = 0; i < m->size; i++) {
			if(m->values[i]){
				printLinkedList(m->values[i], printValue);
			}
		}
	}
}
