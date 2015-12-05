#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#undef true
#undef false
#undef NULL
#define true 1
#define false 0
#define NULL 0
typedef char* string;
typedef int bool;


typedef struct linkedList* linkedList;
typedef struct Node* Node;

struct linkedList {
	Node head;
	Node tail;
};

struct Node {
	void* data;
	Node next;
};

linkedList newLinkedList(void* data);

Node newLinkedNode(void* data);

void appendLinkedNode(Node n, linkedList l);

Node removeLinkedListHead(linkedList l);

void deleteLinkedList(linkedList l, void (*deleteFunction)(void*));

Node removeLinkedNode(void* data, linkedList l);

void printLinkedList(linkedList l, void (*printFunction)(void*));

bool linkedListIsEmpty(linkedList l);

#endif