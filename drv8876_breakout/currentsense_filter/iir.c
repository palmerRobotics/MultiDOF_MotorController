#include "iir.h"

void update_previous_io(IIRfilter* filter, float new_output, float new_input);
void print_filter(IIRfilter* fitler);

void initFilter(IIRfilter* filter, char order, float b[], float a[]) {
    printf("Initializing filter...\n");
    filter->order = order;
    //printf("filter->order: %d\n", filter->order);
    int i = 0;
    for (i; i <= order; i++) {
        filter->b[i] = b[i];
        filter->a[i] = a[i];
        filter->x[i] = 0;
        filter->y[i] = 0;
        //printf("%f | %f | %f | %f\n", filter->b[i], filter->a[i], filter->x[i], filter->y[i]);
    }
    for (i; i < MAX_ORDER; i++) {
        filter->b[i] = 0;
        filter->a[i] = 0;
        filter->x[i] = 0;
        filter->y[i] = 0;
        //printf("%f | %f | %f | %f\n", filter->b[i], filter->a[i], filter->x[i], filter->y[i]);
    }

    print_filter(filter);
}

float filterSignal (IIRfilter* filter, float signal) {
   float output = 0;
   for (int i = filter->order; i > 0; i--) {
       output += filter->b[i]*filter->x[i] - filter->a[i]*filter->y[i];
   }
   output += filter->b[0]*filter->x[0];
   update_previous_io(filter, output, signal);
   return output;
}

// these two functions could be one, if the i/o vector is passed in. or maybe a flag. having both is repetitive
void update_previous_io(IIRfilter* filter, float new_output, float new_input) {
    //printf("before updating io, x=%f, y=%f: (x | y)\n", new_input, new_output);
    //printf("new io, x=%f, y=%f: (x | y)\n", new_input, new_output);
    /*
    for (int i = 0; i < filter->order+1; i++) {
        printf("%f | %f\n", filter->x[i], filter->y[i]);
    }
    */
    for (int i = filter->order; i > 0; i--) {
        filter->y[i] = filter->y[i-1];
        filter->x[i] = filter->x[i-1];
        
    }
    filter->x[0] = new_input;
    //y[0] should never be updated not accessed, since the neweest output is always dependent on previous outputs
    filter->y[1] = new_output;
    //printf("after updating io: (x | y)\n");
    /*
    for (int i = 0; i < filter->order+1; i++) {
        printf("%f | %f\n", filter->x[i], filter->y[i]);
    }
    */
}

void print_filter(IIRfilter* filter) {
    printf("filter->order: %d\n", filter->order);
    printf("Printing filter: (b | a | x | y)\n");
    for (int i = 0; i < MAX_ORDER; i++) { //change max_order to filter->order
        float b = filter->b[i];
        float a = filter->a[i];
        float y = filter->y[i];
        float x = filter->x[i];
        printf("%f | %f | %f | %f\n", b, a, x, y);
    }
}