#include "qsr.h"

#ifndef FILTERS_H
#define FILTERS_H

int lowPassFilter(QRS_params *qsrP);
int highPassFilter(QRS_params *qsrP);
int derivativeFilter(QRS_params *qsrP);
int squaringFilter(QRS_params *qsrP);
int MoveWindowsIntegration(QRS_params *qsrP);

#endif //FILTERS_H
