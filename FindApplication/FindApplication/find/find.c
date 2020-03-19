//
//  Find.c
//
//
//  Created by Mattia Romagnoli on 18/02/2020.
//  Copyright © 2020 Mattia Romagnoli. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include "find.h"
#include "kmp.h"
#include "report.h"
#include "verbose.h"

// esegue la ricerca di una parola all'interno di tutti i file
// ottenuti dalla lettura del file contenente i path
void find(char *parola, list *paths, record **record) {
    start_timer();
    verboseStartWord(parola);

	(*record)->word = strdup(parola);
	list *recordList = (*record)->recordData = calloc(1, sizeof(list));

	// parola tutta minuscola per semplificare ricerca
	for(int i = 0; i < strlen(parola); i++) {
		parola[i] = tolower(parola[i]);
	}

	int total = 0;
	listNode *node = paths->head;

	do {
		char *path = node->data;
		FILE *fp = fopen(path, "r");
        verboseStartPath(path);
		size_t size = 0;
		char *line = NULL;
		// variabile usata per sapere in che riga del file mi trovo
		int lineCount = 1;

		recordData *data = (recordData *)calloc(1, sizeof(recordData));

		data->lines = (list *)calloc(1, sizeof(list));

		// leggo ogni file riga per riga, e per ogni riga ricerco la parola
		// tramite algoritmo KMP
		while (getline(&line, &size, fp) != -1) {
			// linea tutta minuscola per semplificare ricerca
			for(int i = 0; i < strlen(line); i++) {
				line[i] = tolower(line[i]);
			}

			// ricerco la parola all'interno di una singola riga di un file tramite algoritmo KMP
			KMPSearch(parola, line, lineCount, &data);

			lineCount++;
		}
		addToList(&recordList, (void **)(&data));
		// aumento total del numero di occorrenze in un singolo file
		total += data->occurrences;
		data->file = strdup(path);
        verboseEndPath(path);
        stop_timer();
		fclose(fp);

	} while((node = node->next) != NULL);

	// aggiorno occorrenze totali
	(*record)->total = total;

    verboseEndWord(parola);

}
