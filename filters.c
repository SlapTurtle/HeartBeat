#include "filters.h"
#include "qsr.h"

#define N 30

int lowPassFilter(QRS_params *qsrP) {
	return 2 * qsrP->DATA_LOW[(qsrP->Index_Low - 1 + qsrP->LOWCycle) % qsrP->LOWCycle] - qsrP->DATA_LOW[(qsrP->Index_Low - 2 + qsrP->LOWCycle) % qsrP->LOWCycle] + (qsrP->DATA_RAW[qsrP->Index_Raw] - 2 * qsrP->DATA_RAW[(qsrP->Index_Raw - 6 + qsrP->RAWCycle) % qsrP->RAWCycle] +  qsrP->DATA_RAW[(qsrP->Index_Raw - 12 + qsrP->RAWCycle) % qsrP->RAWCycle] ) / 32;
}

int highPassFilter(QRS_params *qsrP) {
	return qsrP->DATA_HIGH[(qsrP->Index_High - 1 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - (qsrP->DATA_LOW[qsrP->Index_Low] / 32) + qsrP->DATA_LOW[(qsrP->Index_Low - 16 + qsrP->LOWCycle) % qsrP->LOWCycle] - qsrP->DATA_LOW[(qsrP->Index_Low - 17 + qsrP->LOWCycle) % qsrP->LOWCycle] + ( qsrP->DATA_LOW[(qsrP->Index_Low - 32 + qsrP->LOWCycle) % qsrP->LOWCycle] / 32 );
}

int derivativeFilter(QRS_params *qsrP) {
	return ( 2 * qsrP->DATA_HIGH[qsrP->Index_High] + qsrP->DATA_HIGH[(qsrP->Index_High - 1 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - qsrP->DATA_HIGH[(qsrP->Index_High - 3 + qsrP->HIGHCycle) % qsrP->HIGHCycle] - 2 * qsrP->DATA_HIGH[(qsrP->Index_High - 4 + qsrP->HIGHCycle) % qsrP->HIGHCycle] ) / 8;
}

int squaringFilter(QRS_params *qsrP) {
	return qsrP->DATA_DER[qsrP->Index_Der] * qsrP->DATA_DER[qsrP->Index_Der];
}

int MoveWindowsIntegration(QRS_params *qsrP){
	int out = 0;
	for(int i = 0; i < N; i++){
		out += qsrP->DATA_SQR[(qsrP->Index_Sqr - i + qsrP->SQRCycle) % qsrP->SQRCycle];
	}
	return out / N;
}
