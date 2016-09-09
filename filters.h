#include "qsr.h"

#ifndef FILTERS_H
#define FILTERS_H

int lowPassFilter(QRS_params *qsrP, int index);
int highPassFilter(QRS_params *qsrP, int index);
int derivativeFilter(QRS_params *qsrP, int index);
int squaringFilter(QRS_params *qsrP, int index);

#endif //FILTERS_H
