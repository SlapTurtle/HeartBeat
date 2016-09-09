#include "filters.h"
#include "qsr.h"

#define N 30

int lowPassFilter(QRS_params *qsrP, int index) {
	return 2 * qsrP->DATA_LOW[(index - 1 + qsrP->LOWCycle) % qsrP->LOWCycle] - qsrP->DATA_LOW[(index - 2 + qsrP->LOWCycle) % qsrP->LOWCycle] + ( qsrP->DATA_RAW[index % qsrP->RAWCycle] - 2 * qsrP->DATA_RAW[(index - 6 + qsrP->RAWCycle) % qsrP->RAWCycle] +  qsrP->DATA_RAW[(index - 12 + qsrP->RAWCycle) % qsrP->RAWCycle] ) / 32;
}

int highPassFilter(QRS_params *qsrP, int index) {
	return qsrP->DATA_HIGH[(index - 1 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - (qsrP->DATA_LOW[index % qsrP->LOWCycle] / 32) + qsrP->DATA_LOW[(index - 16 + qsrP->LOWCycle) % qsrP->LOWCycle] - qsrP->DATA_LOW[(index - 17 + qsrP->LOWCycle) % qsrP->LOWCycle] + ( qsrP->DATA_LOW[(index - 32 + qsrP->LOWCycle) % qsrP->LOWCycle] / 32 );
}

int derivativeFilter(QRS_params *qsrP, int index) {
	return ( 2 * qsrP->DATA_HIGH[index % qsrP->HIGHCycle] + qsrP->DATA_HIGH[(index - 1 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - qsrP->DATA_HIGH[(index - 3 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - 2 * qsrP->DATA_HIGH[(index - 4 + qsrP->HIGHCycle) % qsrP->HIGHCycle] ) / 8;
}

int squaringFilter(QRS_params *qsrP, int index) {
	return qsrP->DATA_DER[index % qsrP->DERCycle] * qsrP->DATA_DER[index % qsrP->DERCycle];
}

int MoveWindowsIntegration(QRS_params *qsrP, int index){
	int out = 0;
	for(int i = 0; i < N; i++){
		out += qsrP->DATA_SQR[(index - i + qsrP->SQRCycle) % qsrP->SQRCycle];
	}
	return out / N;
}
