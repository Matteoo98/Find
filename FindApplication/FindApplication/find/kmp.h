//
//  KMP.h
//
//
//  Created by Mattia Romagnoli on 18/02/2020.
//  Copyright © 2020 Mattia Romagnoli. All rights reserved.
//

#ifndef KMP_h
#define KMP_h

#include "record.h"

#endif /* KMP_h */

void computeLPSArray(char* pat, int M, int* lps);
void KMPSearch(char* pat, char* txt, int lineCount, recordData **recordData);
