//
//  KMP.c
//
//
//  Created by Mattia Romagnoli on 18/02/2020.
//  Copyright © 2020 Mattia Romagnoli. All rights reserved.
//

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "kmp.h"

// effettua la ricerca di una parola in una riga di un file
// se la trovo, aggiungo un'occorrenza (specificata tramite recordDataLine)
// alla lista del recordData relativo al file in cui si sta facendo la ricerca
void KMPSearch(char* pat, char* txt, int lineCount, recordData **recordData) {

    int M = strlen(pat);
    int N = strlen(txt);
  
    // create lps[] that will hold the longest prefix suffix
    // values for pattern
    int lps[M];
  
    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pat, M, lps);
  
    int i = 0; // index for txt[]
    int j = 0; // index for pat[]
    while (i < N) {
        if (pat[j] == txt[i]) {
            j++;
            i++;
        }
  
        if (j == M) {
        	// se trovo un'occorrenza, creo un linea per salvarne le informazioni
        	// cioè dove si trova (linea-carattere)
        	struct recordDataLine *line = (recordDataLine *)calloc(1, sizeof(recordDataLine));
        	line->line = lineCount;
        	line->lineChar = i - j + 1;
        	// aggiungo l'occorrenza alla lista delle occorrenze trovate
        	(*recordData)->occurrences++;
        	addToList(&((*recordData)->lines), (void **)(&line));
            j = lps[j - 1];
        }
  
        // mismatch after j matches
        else if (i < N && pat[j] != txt[i]) {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
}
  
// Fills lps[] for given patttern pat[0..M-1]
void computeLPSArray(char* pat, int M, int* lps) {
    // length of the previous longest prefix suffix
    int len = 0;
  
    lps[0] = 0; // lps[0] is always 0
  
    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
            // This is tricky. Consider the example.
            // AAACAAAA and i = 7. The idea is similar
            // to search step.
            if (len != 0) {
                len = lps[len - 1];
  
                // Also, note that we do not increment
                // i here
            }
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}
