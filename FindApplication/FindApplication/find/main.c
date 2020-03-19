#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include "executor.h"
#include "find.h"
#include "kmp.h"
#include "report.h"
#include "verbose.h"

struct option long_options[] = {
		{"word", required_argument, 0, 'w'},
		{"input", required_argument, 0, 'i'},
		{"exclude", required_argument, 0, 'e'},
		{"output", required_argument, 0, 'o'},
		{"verbose", no_argument, 0, 'v'},
		{"report", required_argument, 0, 'r'},
		{"show", required_argument, 0, 's'},
		{"file", required_argument, 0, 'f'},
		{"help", no_argument, 0, 'h'},
		{NULL, 0, NULL, 0}
};

void help()
{
    printf("Normal scan: ./find --word|-w <word> -- input|-i <path>\n");
    printf("Report Generation: ./find -- word|-w <word.txt> -- input|-i <path.txt> --output|-o <report.txt>\n");
    printf("File ignore: -- exclude|-e <file_extension> \n");
    printf("Verbose: -- verbose|-v\n");
    printf("Report's Analysis: ./find -- report <reportfile> -- show <word> <n> (if n omitted, n = 1)\n");
    printf("Print all locations where the word <word> occurs in the <file> file:\nfind -- report|-r <reportfile> -- show <word> -- file <path/to/file>\n");
}

int main(int argc, char **argv) {

	// il programma è stato invocato senza alcun argomento, mostro help ed esco
	if (argc == 1) {
		help();
		exit(1);
	}

    char *outputFile = NULL;
    char *report = NULL;
    char *showWord = NULL;
    char *file = NULL;
    char *word = NULL;
    char *input = NULL;
    char *extension = NULL;
    int n = 1;
    int option_index = 0;
    int opt;

    const char *short_options = "w:i:e:o:vr:s:f:h";
    /*
     * Gestione dei parametri dell'applicazione(ogni elemento che inizia con - o -- è considerato
     * parametro dalla funzione getopt_long)
     */

    while ((opt = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
    	switch (opt) {
    	case 'w':
    		word = optarg;
    		break;
    	case 'i':
    		input = optarg;
    		break;
    	case 'e':
    		extension = optarg;
    		break;
    	case 'o':
    		outputFile = optarg;
    		break;
    	case 'v':
    		setFlag();
    		break;
    	case 'r':
    		report = optarg;
    		break;
    	case 's':
    		showWord = optarg;
    		break;
    	case 'f':
    		file = optarg;
    		break;
    	case 'h':
    		help();
    		break;
    	}
    }

    // n, il parametro opzionale dopo show, è un numero, quindi non un parametro
    // che può essere gestito con getopt_long; devo gestirlo tramite indice (optind)
    if (optind < argc) {
    	if (argc - optind > 1) {
    		help();
    		exit(1);
    	}
    	if (argc - optind == 1) {
    		// converto in numero
    		n = atoi(argv[optind]);
    		// n deve essere maggiore o uguale a 1, altrimenti mostro help ed esco
    		if (n < 1) {
    			help();
    			exit(1);
    		}
    	}
    }

    if (report == NULL) {
    	// crea report
    	exec(input, word, outputFile, extension);
    } else {
    	// se è stato specificato un file
    	char *real = NULL;
    	if (file != NULL) {
    		// ottengo il path assoluto
    		real = realpath(file, NULL);
    		if (real == NULL) {
    			printf("Il file %s non esiste", file);
    			exit(1);
    		}
    	}
    	// analizza report
    	analisiReport(report, showWord, n, real);
    }

    return 0;
}
