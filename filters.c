#include "filters.h"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

static int lowPassFilter(int *data, int *ouput, int index);
static int highPassFilter(int *data, int *ouput, int index);
static int derivativeFilter(int *data, int *output, int index);
static int squaringFilter(int *data, int *output, int index);

int lowPassFilter(int *data, int *output, int index) {
	return 2 * output[(index - 1 + 33) % 33] - output[(index - 2 + 33) % 33] + ( data[index % 13] - 2 * data[(index - 6 + 13) % 13] +  data[(index - 12 + 13) % 13] ) / 32 ;
}

int highPassFilter(int *data, int *output, int index) {
	return output[(index - 1 + 5) % 5] - (data[index % 33] / 32) + data[(index - 16 + 33) % 33] - data[(index - 17 + 33) % 33] + ( data[(index - 32 + 33) % 33] / 32 );
}

int derivativeFilter(int *data, int *output, int index) {
	return ( 2 * data[index % 5] + data[(index - 1 + 5) % 5] - data[(index - 3 + 5) % 5] - 2 * data[(index - 4 + 5) % 5] ) / 8;
}

int squaringFilter(int *data, int *output, int index) {
	return data[index % 1] * data[index % 1];
}
