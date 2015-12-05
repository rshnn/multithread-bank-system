#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"

/* 
	Create new linked list.
	@param	data	Input pointer to data	 
	@return 		Head of list. 
*/
linkedList newLinkedList(void* data)
{
	linkedList l 		= (linkedList) malloc(sizeof(*l));
	l->head = l->tail 	= NULL;
	Node n;

	if(data){
		assert(data);
		n = newLinkedNode(data);
		l->head = l->tail = n;
	}

	return l;
}

/*
	Add new node to list
	@param data 	Pointer to data to append
	@return 		Head of list
*/
Node newLinkedNode(void* data)
{
	assert(data);

	Node n 	= (Node) malloc(sizeof(*n));
	n->data = data;
	n->next = NULL;

	return n;
}

/*
	Append a linked list node to the linked list.  Updates linked 
	list stucture
	@param n 	Node to append
	@param l 	LinkedList structure to append to
*/
void appendLinkedNode(Node n, linkedList l)
{
	assert(l);
	assert(n);

	if(!l->tail)
		l->head = l->tail = n;
	else
		l->tail = l->tail->next = n;
}


/*
	Remove the head of the linked list.
	@param l 	Pointer to linked list
	@return 	New linked list structure with new head
*/
Node removeLinkedListHead(linkedList l)
{
	assert(l);
	assert(l->head);
	Node n;

	if(l->tail == l->head)
		l->tail = NULL;

	n 		= l->head;
	l->head = n->next;
	
	return n;
}

/*
	Delete linked list structure.  Frees dynamically allocated
	data form each node.  Then frees linked list structure.
	@param l 	LinkedList to destroy
	@param *fcn	Function pointer to a delete function applicable to data type.
*/
void deleteLinkedList(linkedList l, void (*deleteFunction)(void*))
{
	if(l){
		if(l->head){
			Node node = l->head;

			while(node){
				Node temp	= node;
				node 		= temp->next;
				deleteFunction(temp->data);
				free(temp);
			}
		}

		free(l);
	}
}

/*
	Remove a node from the list.  Search and remove.
	@param data		Data to remove from list
	@param l 		LinkedList to remove from
	@return 		Updated LinkedList structure
*/
Node removeLinkedNode(void* data, linkedList l)
{
	assert(l);
	assert(l->head);

	if(l->head->data == data){
		if(l->tail == l->head)
			l->tail = NULL;

		Node n = l->head;

		l->head = l->head->next;

		return n;
	}

	Node n = l->head;

	while(n->next){
		if(n->next->data == data){
			Node ret = n->next;

			if(n->next == l->tail)
				l->tail = n;

			n->next = n->next->next;

			return ret;
		}
	}

	return NULL;
}

/*
	Prints LinkedList structure.  Iterates through nodes.
	@param l 		LinkedList to print
	@param *fcn 	Pointer to function that prints data type
*/
void printLinkedList(linkedList l, void (*printFunction)(void*))
{

	if(l) {
		printf("Printing linked list\n");
		Node current = l->head;

		while(current){
			printFunction(current->data);
			current = current->next;
		}
	}

	printf("\n");
}

/*
	Boolean check to see if linkedList is empty.
	@param l 	LinkedList to check
	@return 	Boolean type
*/
bool linkedListIsEmpty(linkedList l)
{
	assert(l);

	return l->head == NULL && l->tail == NULL;
}

