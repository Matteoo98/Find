//
// Created by matteo on 19/02/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "executor.h"
#include "find.h"
#include "kmp.h"
#include "list.h"
#include "report.h"

// lista delle parole, ottenute dal file che le contiene
list *words = NULL;
// lista dei, ottenute dal file che le contiene e visitando le eventuali directory
list *paths = NULL;
// lista dei record (parola - occorrenze totali parola - recordData) per ogni parola
list *records = NULL;
char *extToExclude = NULL;
// booleano per valutare se ho già visitato una directory oppure no
// utilizzato per evitare di visitare sottodirectory se la ricerca non
// è ricorsiva
int isFirstTimeVisitingDir = 1;
// condizione usata per mostrare errore più adeguato
int wordExistsInReport = 0;

// elimino eventuali \r \n presenti a fine stringa
void cleanString(char **str) {
	while ((*str)[strlen((*str)) - 1] == '\r' || (*str)[strlen((*str)) - 1] == '\n') {
		(*str)[strlen((*str)) - 1] = '\0';
	}
}

// leggo una singola directory
void readSingleDir(char *singlePath, int isRecursive) {
	// se non è ricorsiva e mi trovo già dentro una directory mi fermo
	if (!isRecursive && (isFirstTimeVisitingDir == 0)) {
		return;
	}

	// apro una directory
	DIR *d = opendir(singlePath);

	if (!d) {
		perror(strcat("Impossibile aprire directory ", singlePath));
		exit (-1);
	}

	struct dirent * entry;
	char * d_name;

	isFirstTimeVisitingDir = 0;

	// ciclo gli elementi della directory
	while (1) {
		entry = readdir(d);
		if (!entry) {
			break;
		}

		d_name = entry->d_name;

		int len = strlen(singlePath) + 1 + strlen(d_name) + 1;
		char newPath[len];
		strcpy(newPath, singlePath);
		strcat(newPath, "/");
		strcat(newPath, d_name);
		strcat(newPath, "\0");

		// se è un file regolare lo aggiungo alla lista
		if (DT_REG & entry->d_type) {
			// se è stata specificata un'estensione da escludere, faccio il controllo
			char *ext = strrchr(newPath, '.');
			if (ext != NULL) {
				ext += 1;
			}
			// se non bisogna escludere alcuna estensione oppure l'estensione
			// da esclcudere è diversa da quella del file in questione
			if (extToExclude == NULL || strcmp(extToExclude, ext) != 0) {
				char *copy = strdup(newPath);
				// lo aggiungo alla lista dei file
				addToList(&paths, (void **)(&copy));
			}
			continue;
		}

		// se non è una directory, passo al prossimo elemento
		if (!(DT_DIR & entry->d_type)) {
			continue;
		}

		// se è una directory, la visito
		if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
			readSingleDir(newPath, isRecursive);
		}
	}

	// chiudo la directory
	if (closedir(d)) {
		perror(strcat("Errore nella chiusura della directory ", singlePath));
		exit(-1);
	}
}

void exec(char *path, char *word , char *outputFile, char *exclude) {

    // CREAZIONE/APERTURA FILE REPORT
    FILE *fp_report = fopen(outputFile, "w");
    if (fp_report == NULL) {
    	perror("Errore apertura/creazione file report");
    }

    // FILE DI INPUT
    FILE *fp_path = fopen(path, "r");
    if (fp_path == NULL) {
        perror("Errore in apertura del file");
        exit(1);
    }

    // FILE WORDS
    FILE *fp_words = fopen(word, "r");
    if (fp_words == NULL) {
        perror("Errore in apertura del file");
        exit(1);
    }

    extToExclude = exclude;

    size_t size = 0;

    paths = (list *)calloc(1, sizeof(list));
    char *singleLine = NULL;

    // estraggo i path dal file
    while (getline(&singleLine, &size, fp_path) != -1) {
    	// prendo riga per riga, la pulisco, controllo se è stata specificata
    	// la ricorsività (r)
    	char *singlePath = strtok(singleLine, " ");
    	cleanString(&singlePath);
    	if (strlen(singlePath) == 0) {
    		break;
    	}
    	char *recursiveness = strtok(NULL, " ");
    	if (recursiveness != NULL) {
    		cleanString(&recursiveness);
    	}
    	int isRecursive = (recursiveness != NULL && strcmp(recursiveness, "r") == 0) ? 1 : 0;

    	// ottengo informazioni sul path
    	struct stat pathStat;
    	stat(singlePath, &pathStat);

    	// se è un file regolare, controllo eventuale estensione e lo aggiungo alla lista
    	if (S_ISREG(pathStat.st_mode)) {
    		char *ext = strrchr(singlePath, '.');
    		if (ext != NULL) {
    			// mi sposto dopo il punto
    			ext += 1;
    		}
    		// controllo sull'estensione
    		if (extToExclude == NULL || strcmp(extToExclude, ext) != 0) {
    			char *copy = strdup(singlePath);
    			addToList(&paths, (void **)(&copy));
    		}
    		continue;
    	}

    	// se non è una directory, passo al prossimo elemento
    	if (!S_ISDIR(pathStat.st_mode)) {
    		continue;
    	}

    	// resetto booleano, perchè almeno una volta devo visitare directory
    	isFirstTimeVisitingDir = 1;

    	// se è una directory la visito, eventualmente in modo ricorsivo
    	if (S_ISDIR(pathStat.st_mode)) {
    		readSingleDir(singlePath, isRecursive);
    	}
    }
    free(singleLine);

    // estraggo le words dal file
    words = (list *)calloc(1, sizeof(list));
    char *singleWord = NULL;
    while (getline(&singleWord, &size, fp_words) != -1) {
    	// prendo riga per riga, pulisco e aggiungo alla lista
    	cleanString(&singleWord);
    	char *copy = strdup(singleWord);
    	addToList(&words, (void **)(&copy));
    }
    free(singleWord);

    // prendo una parola alla volta
    listNode *parola = words->head;

    records = (list *)calloc(1, sizeof(list));

    if (paths->head == NULL) {
    	perror("Nessun file adeguato in cui effettuare la ricerca");
    	exit(1);
    }

    // ciclo ogni parola
    while (parola != NULL) {
    	record *record = calloc(1, sizeof(record));

    	// per ogni parola creo un tipo di dato record;
    	// uso &record, in questo modo potrò popolarlo
    	// da un'altra parte
        find(parola->data, paths, &record);

        // aggiungo il record alla lista
        addToList(&records, (void **)(&record));

        // mi sposto alla parola successiva
        parola = parola->next;
    }

    // scrivo report
    scriviReport(records, fp_report);

    //chiudo i vari file
    if(fp_report!=NULL)
        if (fclose(fp_report) == EOF) {
            perror("Errore chiusura file report");
            exit(1);
        }


    if (fclose(fp_path) == EOF) {
    	perror("Errore chiusura file");
    	exit(1);
    }

    if (fclose(fp_words) == EOF) {
        perror("Errore chiusura file");
        exit(1);
    }

    printf("\n\n\t** fine programma **\n\n");
}

// per effettuare l'analisi ricostruisco la struttura dati dal file di report
void analisiReport(char *pathReport, char *word, int n, char *file) {

    FILE *fp = fopen(pathReport, "r");

    if (fp == NULL) {
        perror("Errore in apertura del file di report");
        exit(1);
    }

    // calcolo lunghezza file
    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // se file di report è vuoto, lancio errore ed esco
    if (fileSize == 0) {
    	perror("Il file di report è vuoto");
    	exit(1);
    }

    records = (list *)calloc(1, sizeof(list));
    size_t size = 0;
    char *line = NULL;

    // finchè non sono arrivato alla fine del file (\r\n)
    while (getline(&line, &size, fp) != -1 && strcmp(line, "\r\n") != 0) {
    	// creo un nuovo record e lo popolo con parola e occorrenze totali
    	record *record = calloc(1, sizeof(record));
    	list *recordDataList = (list *)calloc(1, sizeof(list));

    	strtok(line, " ");
    	char *parola = strtok(NULL, " ");
    	cleanString(&parola);
    	record->word = strdup(parola);

    	if (word != NULL && strcmp(record->word, word) == 0) {
    		wordExistsInReport = 1;
    	}

    	getline(&line, &size, fp);
    	strtok(line, " ");
    	char *totale = strtok(NULL, " ");
    	cleanString(&totale);
    	record->total = atoi(totale);

    	// finchè leggo la scritta FILE
    	while (1) {
    		// recordData contiene dati relativi all'analisi di un singolo file
    		recordData *recordData = calloc(1, sizeof(recordData));

    		// controllo se sono arrivato alla fine del file o alla parola successiva
    		int pathCharRead = getline(&line, &size, fp);
    		if (strstr(line, "WORD") != NULL || strcmp(line, "\r\n") == 0) {
    			// in caso torno indietro nella lettura del file di quanto ho letto (pathCharRead)
    			fseek(fp, -pathCharRead, SEEK_CUR);
    			break;
    		}

    		// popolo recordData con path del file e occorrenze delle parola in quel file

    		strtok(line, " ");
    		char *path = strtok(NULL, " ");
    		cleanString(&path);
    		recordData->file = strdup(path);

    		getline(&line, &size, fp);
    		strtok(line, " ");
    		char *occurrences = strtok(NULL, " ");
    		cleanString(&occurrences);
    		recordData->occurrences = atoi(occurrences);

    		// controlli per analisi report
    		if (file == NULL && word != NULL && strcmp(record->word, word) == 0 && recordData->occurrences >= n) {
    			printf("Il file %s contiene almeno %d volta/e la parola %s\r\n", recordData->file, n, word);
    		}

    		// controlli per analisi report
    		if (word != NULL && file != NULL && strcmp(record->word, word) == 0 && strcmp(recordData->file, file) == 0) {
    			if (recordData->occurrences == 0) {
    				printf("Il file %s non contiene la parola %s\r\n", recordData->file, word);
    			}
    		}

    		// se la parola occorre almeno una volta
    		if (recordData->occurrences != 0) {
    			recordData->lines = (list *)calloc(1, sizeof(list));
    			// recordDataLine contiene informazioni relative ad una singola
    			// coppia linea-carattere in cui si trova la parola nel file
    			recordDataLine *recordDataLine = calloc(1, sizeof(recordDataLine));

    			// finchè leggo queste coppie
    			while (1) {
    				int lineCharRead = getline(&line, &size, fp);
    				// controllo se sono arrivato alla fine del file o al file successivo
    				if (strstr(line, "FILE") != NULL || strcmp(line, "\r\n") == 0) {
    					// in caso torno indietro nella lettura del file di quanto ho letto (lineCharRead)
    					fseek(fp, -lineCharRead, SEEK_CUR);
    					break;
    				}
    				int lineNumber = atoi(strtok(line, " "));
    				char *lineChar = strtok(NULL, " ");
    				cleanString(&lineChar);
    				// popolo recordDataLine con linea e carattere in cui si trova la parola
    				recordDataLine->line = lineNumber;
    				recordDataLine->lineChar = atoi(lineChar);

    				// controllo per analisi report
    				if (word != NULL && file != NULL && strcmp(word, record->word) == 0 && strcmp(recordData->file, file) == 0) {
    					printf("Il file %s contiene la parola %s alla linea %d carattere %d\r\n",
    							recordData->file, word, recordDataLine->line, recordDataLine->lineChar);
    				}

    				// aggiungo alla lista
    				addToList(&(recordData->lines), (void **)(&recordDataLine));
    			}
    		}
    		// aggiungo alla lista
    		addToList(&recordDataList, (void **)(&recordData));
    	}
    	record->recordData = recordDataList;
    	// aggiungo record (relativo ad una singola parola), adesso popolato e completo,
    	// alla lista dei record (relativa a tutte le parole)
    	addToList(&records, (void **)(&record));
    }

    // controlli per analisi report
    if (word != NULL && file != NULL && !wordExistsInReport) {
    	printf("La parola %s non esiste nel report %s\r\n", word, pathReport);
    }

    // chiudo file di report
    fclose(fp);
}
