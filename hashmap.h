/*
	This hashmap implementation is a product of following tutorials 
	online for creating hashmaps using C.  The result is an implementation
	that is not completely of my original design.  But, creating a hashmap 
	is not the purpose of pa5, so whatever.
*/

#include <stdlib.h>

#ifndef _HASHMAP_H_
#define _HASHMAP_H_
typedef struct map* map;

struct map {
	linkedList* values;
	size_t count;
	size_t size;
	double load;
};

map newMap(size_t size, double load);

void insertMapValue(string key, void* value, map m);

void resizeMap(map m, size_t scale);

int hashKey(string key);

void* getKeyValue(string key, map m);

void fillEmpty(map m);

#endif
