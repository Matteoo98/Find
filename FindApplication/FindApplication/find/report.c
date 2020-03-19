//
//  report.c
//
//  Created by Mattia Romagnoli on 18/02/2020.
//  Copyright © 2020 Mattia Romagnoli. All rights reserved.
//

#include "report.h"

// usato per avere ordine alfabetico delle parole
int compareWord(const void *rec1, const void *rec2) {
	record *r1 = *(record **)rec1;
	record *r2 = *(record **)rec2;
	return strcmp(r1->word, r2->word);
}

// usato per avere ordine decrescente per occorrennza e ordine alfabetico
// dei path assoluti nel caso di più file con stesso numero di occorrenze
int compareOccurrencesAndPath(const void *rec1, const void *rec2) {
	recordData *r1 = *(recordData **)rec1;
	recordData *r2 = *(recordData **)rec2;
	// comparazione occorrenze
	int occCmp = (r2->occurrences - r1->occurrences);
	if (occCmp != 0) {
		return occCmp;
	} else {
		// se stesso numero di occorrenze, comparazione path assoluti
		return strcmp(r1->file, r2->file);
	}
}

// Le parole vengono elencate nel report in ordine alfabetico mentre per ogni parola
// i file vengono presentati in ordine decrescente per occorrenza.
// Nel caso di più file con il medesimo numero di occorrenze i file verranno
// presentati in ordine alfabetico (usando il path assoluto).
void scriviReport(list *records, FILE *output) {
	// il report viene scritto tramite la navigazione della struttura dati
	// in questo caso una lista di record

	// conto quanti record ho, lo faccio per avere dimensione array
	int count = 0;
	listNode *recNodeSize = records->head;
	do {
		count++;
	} while ((recNodeSize = recNodeSize->next) != NULL);

	// creo un array e lo riempio con i singoli record
	record **recs = (record **)calloc(count, sizeof(record *));
	recNodeSize = records->head;
	int counter = 0;
	do {
		recs[counter++] = recNodeSize->data;
	} while ((recNodeSize = recNodeSize->next) != NULL);

	// ordino i record (un record per parola) in base all'ordine alfabetico delle parole
	qsort(recs, count, sizeof(record *), compareWord);

	if(output==NULL) {
	    printf("REPORT : \n\n");
        // per ogni record stampo il report
        for(int i = 0; i < count; i++) {
	    stampaParola(recs[i]->word);
	    stampaTotal(recs[i]->total);
	    stampaDettagli(recs[i]);
        }
	} else {

        // per ogni record scrivo il report
        for(int i = 0; i < count; i++) {
            scriviParola(recs[i]->word, output);
            scriviTotal(recs[i]->total, output);
            // scrivo i dettagli di un singolo record
            scriviDettagli(recs[i], output);
        }

        fprintf(output, "\r\n");

	}

}

void scriviParola(char* parola, FILE* fp_report) {
    fprintf(fp_report, "WORD %s\r\n", parola);
}

void scriviTotal(int total, FILE* fp_report) {
    fprintf(fp_report, "TOTAL %d\r\n", total);
}

// per ogni record, avrò una lista di recordData (che contengono informazioni
// su file e numero di occorrenze per file)
void scriviDettagli(record *record, FILE* fp_report) {

	// conto numero di recordData
	int count = 0;
	listNode *recDataNodeSize = record->recordData->head;
	do {
		count++;
	} while ((recDataNodeSize = recDataNodeSize->next) != NULL);

	// creo un array di recordData
	recordData **recs = (recordData **)calloc(count, sizeof(recordData *));
	recDataNodeSize = record->recordData->head;
	int counter = 0;
	do {
		recs[counter++] = recDataNodeSize->data;
	} while ((recDataNodeSize = recDataNodeSize->next) != NULL);

	// lo ordino in base ad ordine decrescente per occorrennza e ordine alfabetico
	// dei path assoluti nel caso di più file con stesso numero di occorrenze
	qsort(recs, count, sizeof(recordData *), compareOccurrencesAndPath);

	// per ogni recordData
	for (int i = 0; i < count; i++) {
		// ottengo path assoluto e scrivo path assoluto del file e numero di occorrenze
		// della parola nel file
		char *filePath = realpath(recs[i]->file, NULL);
		fprintf(fp_report,"FILE %s\r\n", filePath);
		fprintf(fp_report,"OCCURRENCES %d\r\n", recs[i]->occurrences);

		// scrivo ogni singola occorrenza a che linea-carattere si trova
		if (recs[i]->occurrences > 0) {
			listNode *recordDataLineNode = recs[i]->lines->head;
			do {
				recordDataLine *recordDataLine = recordDataLineNode->data;
				fprintf(fp_report,"%d %d\r\n", recordDataLine->line, recordDataLine->lineChar);
			} while((recordDataLineNode = recordDataLineNode->next) != NULL);
		}
	}
}

void stampaParola(char* parola) {
    printf( "WORD %s\r\n", parola);
}

void stampaTotal(int total) {
    printf("TOTAL %d\r\n", total);
}

// per ogni record, avrò una lista di recordData (che contengono informazioni
// su file e numero di occorrenze per file)
void stampaDettagli(record *record) {

    // conto numero di recordData
    int count = 0;
    listNode *recDataNodeSize = record->recordData->head;
    do {
        count++;
    } while ((recDataNodeSize = recDataNodeSize->next) != NULL);

    // creo un array di recordData
    recordData **recs = (recordData **)calloc(count, sizeof(recordData *));
    recDataNodeSize = record->recordData->head;
    int counter = 0;
    do {
        recs[counter++] = recDataNodeSize->data;
    } while ((recDataNodeSize = recDataNodeSize->next) != NULL);

    // lo ordino in base ad ordine decrescente per occorrennza e ordine alfabetico
    // dei path assoluti nel caso di più file con stesso numero di occorrenze
    qsort(recs, count, sizeof(recordData *), compareOccurrencesAndPath);

    // per ogni recordData
    for (int i = 0; i < count; i++) {
        // ottengo path assoluto e scrivo path assoluto del file e numero di occorrenze
        // della parola nel file
        char *filePath = realpath(recs[i]->file, NULL);
         printf("FILE %s\r\n", filePath);
         printf("OCCURRENCES %d\r\n", recs[i]->occurrences);

        // scrivo ogni singola occorrenza a che linea-carattere si trova
        if (recs[i]->occurrences > 0) {
            listNode *recordDataLineNode = recs[i]->lines->head;
            do {
                recordDataLine *recordDataLine = recordDataLineNode->data;
                printf("%d %d\r\n", recordDataLine->line, recordDataLine->lineChar);
            } while((recordDataLineNode = recordDataLineNode->next) != NULL);
        }
    }
}

