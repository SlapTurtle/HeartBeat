#include "filters.h"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

static int lowPassFilter(int *data, int *ouput, int index);
static int highPassFilter(int *data, int *ouput, int index);
static int derivativeFilter(int *data, int *output, int index);
static int squaringFilter(int *data, int *output, int index);

int lowPassFilter(int *data, int *output, int index) {
	return output[(index - 1 + 32 ) % 32];
	return 2*data[index];
	return (2 * output[(index - 1 + 32) % 32] - output[(index - 2 + 32) % 32] + (1/32) * (data[index % 32] - 2 * data[(index - 6 + 32) % 32] + data[(index - 12 + 32) % 32]));
}

int highPassFilter(int *data, int *output, int index) {
	return 0;
}

int derivativeFilter(int *data, int *output, int index) {
	return 0;
}

int squaringFilter(int *data, int *output, int index) {
	return data[index] * data[index];
}
