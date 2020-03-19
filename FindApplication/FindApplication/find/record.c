/*
 * record.c
 *
 *  Created on: 20 feb 2020
 *      Author: matteo
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "record.h"

void freeRecord(record **record) {
	if (*record == NULL) {
		return;
	}
	free((*record)->word);

	list *data = (*record)->recordData;

	listNode *node = data->head;
	listNode *next;
	do {

		free(node->data);
		next = node->next;
		free(node);
	} while ((node = next) != NULL);

	free(*record);
}
