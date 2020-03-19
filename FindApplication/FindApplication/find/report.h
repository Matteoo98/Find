//
//  report.h
//
//  Created by Mattia Romagnoli on 18/02/2020.
//  Copyright © 2020 Mattia Romagnoli. All rights reserved.
//

#ifndef report_h
#define report_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "record.h"

#endif /* report_h */

void scriviParola(char* parola, FILE* fp_report);
void scriviDettagli(record *record, FILE* fp_report);
void scriviTotal(int total, FILE* fp_report);
void scriviReport(list *records, FILE *output);


void stampaParola(char* parola);
void stampaDettagli(record *record);
void stampaTotal(int total);


