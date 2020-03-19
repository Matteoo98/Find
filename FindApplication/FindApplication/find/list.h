/*
 * list.h
 *
 *  Created on: 20 feb 2020
 *      Author: matteo
 */

#ifndef LIST_H_
#define LIST_H_

// un singolo elemento della lista
typedef struct listNode {
	// usato puntatore a void per memorizzare dati di qualsiasi tipo
	void *data;
	struct listNode *next;
} listNode;

// lista concatenata con guardie (ha cioè riferimento a primo e ultimo elemento)
typedef struct list {
	listNode *head;
	listNode *tail;
} list;

// per poter aggiungere elementi alla lista, devo lavorare con un livello di indirizzamento (*) in più
void addToList(list **list, void **data);

void freeList(list **list);

#endif /* LIST_H_ */
