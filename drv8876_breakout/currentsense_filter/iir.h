#ifndef IIR_H
#include <stdio.h>

#define MAX_ORDER 3 //really tempted to use malloc() instead...

typedef volatile struct iirfilter { //not sure if stuct should be defined as volatile... it made a warning go away though
    char order;
    float b[MAX_ORDER+1];
    float a[MAX_ORDER+1]; //what if I just completely ignore a[0]. as in leave it out entirely. since the coefficients are already normalized.
    float x[MAX_ORDER+1];
    float y[MAX_ORDER];
} IIRfilter;

void initFilter(IIRfilter* filter, char order, float b[], float a[]);
float filterSignal(IIRfilter* filter, float signal);

#endif