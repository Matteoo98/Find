//
// Created by matteo on 22/02/20.
//

#include "verbose.h"

int flag = 0;
time_t start, end;
double execTime;
void setFlag()
{
    flag = 1;
}
void start_timer()
{
    start = clock();
}

void stop_timer()
{
    end = clock();
    execTime = (double)(end - start) / CLOCKS_PER_SEC;

}

void verboseStartWord(char* toPrint) {
    if(flag==1){
        printf("Inizio elaborazione parola : %s\n",toPrint);

    }


}
void verboseEndWord(char* toPrint) {
    if(flag==1){
        printf("Fine elaborazione parola : %s\n\n",toPrint);

    }


}
void verboseStartPath(char* toPrint) {
    if(flag==1){
        printf("Inizio elaborazione file : %s\n",toPrint);

    }


}
void verboseEndPath(char* toPrint) {
    if(flag==1){
        printf("Fine elaborazione file : %s in %lf \n\n",toPrint,execTime);

    }


}