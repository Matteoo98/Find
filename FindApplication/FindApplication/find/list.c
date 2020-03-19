/*
 * list.c
 *
 *  Created on: 20 feb 2020
 *      Author: matteo
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"

void addToList(list **list, void **data) {
	// creo un nuovo nodo della lista
	listNode *newNode = (listNode *)malloc(sizeof(listNode));
	if(newNode == NULL) {
		perror("Impossibile allocare un nuovo dato alla lista!");
		exit(1);
	}
	newNode->data = *data;
	newNode->next = NULL;

	// aggiungo il nuovo nodo alla lista
	if((*list)->head == NULL) {
		// se la lista è vuota, lo aggiungo all'inizio (e quindi allo stesso tempo alla fine) della lista
		(*list)->head = newNode;
		(*list)->tail = newNode;
	} else {
		// altrimenti lo aggiungo alla fine ed aggiorno la coda
		listNode *last = (*list)->tail;
		last->next = newNode;
		(*list)->tail = newNode;
	}
}

// liberare memoria occupata dalla lista
void freeList(list **list) {
	if (*list == NULL) {
		return;
	}

	// se la lista è vuota
	if ((*list)->head == NULL) {
		free(*list);
		return;
	}

	listNode *node = (*list)->head;
	listNode *next;
	do {
		// elimino gli elementi della lista uno per uno
		free(node->data);
		next = node->next;
		free(node);
	} while ((node = next) != NULL);

	free(*list);
}
