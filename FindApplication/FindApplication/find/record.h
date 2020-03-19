/*
 * record.h
 *
 *  Created on: 20 feb 2020
 *      Author: matteo
 */

#ifndef RECORD_H_
#define RECORD_H_

#include "list.h"

// struttura per memorizzare una singola coppia linea-carattere
typedef struct recordDataLine {
	int line;
	int lineChar;
} recordDataLine;

// struttura per memorizzare un singolo file path, il numero di
// occorrenze della parola in quel file, e una lista di coppie
// linea-carattere del testo in cui è possibile trovare la parola
typedef struct recordData {
	char *file;
	int occurrences;
	list *lines;
} recordData;

// un record contiene una parola, il numero di occorrenze totali
// e una lista di file in cui è possibile trovare la parola
typedef struct record {
	char *word;
	int total;
	list *recordData;
} record;

void freeRecord(record **record);

#endif /* RECORD_H_ */
