#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWI 9999

#define LOOPCOUNT 30

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, RAWC, LOWC, HIGHC, DERC, SQRC, {0}, {0}, {0}, {0}, {0}};
	QRS_params *ptr = &qrsP;

    FILE *file = openFile("ECG.txt");

 	for(int i = 0; i < LOOPCOUNT; i++){
 		for(int j = 0; j < LOWC; j++){

			qrsP.DATA_RAW[j % qrsP.RAWCycle] = getNextData(file);

			if(i > 0) {
				qrsP.DATA_LOW[j % qrsP.LOWCycle] = lowPassFilter(ptr, j);
			}
			if(i > 1) {
				qrsP.DATA_HIGH[j % qrsP.HIGHCycle] = highPassFilter(ptr, j);
			}
			if(i > 2) {
				qrsP.DATA_DER[j % qrsP.DERCycle] = derivativeFilter(ptr, j);
			}
			if(i > 3){
				qrsP.DATA_SQR[j % qrsP.SQRCycle] = squaringFilter(ptr, j);
			}
			if(i > 4){
				qrsP.DATA_MWI[i*LOWC + j] = MoveWindowsIntegration(ptr, i*LOWC + j);
			}

			if (i > 4 && isPeak(ptr, i*LOWC + j) == 1) {
				qrsP.SPKF = SPKF(ptr);
				printf("[PEAK] [%f]\n", qrsP.SPKF);
			} else {
				printf("\n");
			}

			printf("cycle: %i - ", i);
			printf("index: %i - ", j);
			printf("raw: %i - ", qrsP.DATA_RAW[j % qrsP.RAWCycle]);
			printf("low: %i - ", qrsP.DATA_LOW[j % qrsP.LOWCycle]);
			printf("high: %i - ", qrsP.DATA_HIGH[j % qrsP.HIGHCycle]);
			printf("der: %i - ", qrsP.DATA_DER[j % qrsP.DERCycle]);
			printf("sqr: %i - ", qrsP.DATA_SQR[j % qrsP.SQRCycle]);
			printf("mwi: %i - ", qrsP.DATA_MWI[i*LOWC + j]);
			printf("time: %i ", qrsP.DATA_TIMEMS);

			qrsP.DATA_TIMEMS+= 4;
		}
 	}

 	closeFile(file);

    return 1;
}
