#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 800

#define LOOPCOUNT 10

#define DEBUGMODE 1

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, {0}, {0}, RAWC, LOWC, HIGHC, DERC, SQRC, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, {0}, {0}}, *ptr = &qrsP;

	// debug
	int lastheight = 0;

    FILE *file = openFile("ECG.txt");
    int temp;

	for(int i = 0; i < MWIC; i++){

		qrsP.DATA_RAW[qrsP.Index_Raw] = getNextData(file);
		qrsP.DATA_LOW[qrsP.Index_Low] = lowPassFilter(ptr);
		qrsP.DATA_HIGH[qrsP.Index_High] = highPassFilter(ptr);
		qrsP.DATA_DER[qrsP.Index_Der] = derivativeFilter(ptr);
		qrsP.DATA_SQR[qrsP.Index_Sqr] = squaringFilter(ptr);
		qrsP.DATA_MWI[qrsP.Index_Mwi] = MoveWindowsIntegration(ptr);

		//Index increment-cycles.
		qrsP.Index_Raw++;
		if(qrsP.Index_Raw == qrsP.RAWCycle){qrsP.Index_Raw = 0;};
		qrsP.Index_Low++;
		if(qrsP.Index_Low == qrsP.LOWCycle){qrsP.Index_Low = 0;};
		qrsP.Index_High++;
		if(qrsP.Index_High == qrsP.HIGHCycle){qrsP.Index_High = 0;};
		qrsP.Index_Der++;
		if(qrsP.Index_Der == qrsP.DERCycle){qrsP.Index_Der = 0;};
		qrsP.Index_Sqr++;
		if(qrsP.Index_Sqr == qrsP.SQRCycle){qrsP.Index_Sqr = 0;};
		qrsP.Index_Mwi++;

		qrsP.DATA_TIMEMS+= 4;	//250 inputs per second = 4 ms / input


		if (DEBUGMODE == 0) {
			// Peak detection
			if (isPeak(ptr, qrsP.Index_Mwi - 1) == 1) {
				qrsP.SPKF = SPKF(ptr);
				printf("[PEAK] [%f]\n", qrsP.SPKF);
			} else {
				printf("\n");
			}
			printf("step: %i - ", i);
			printf("raw: %i - ", qrsP.DATA_RAW[(qrsP.Index_Raw - 1 + qrsP.RAWCycle) % qrsP.RAWCycle]);
			printf("low: %i - ", qrsP.DATA_LOW[(qrsP.Index_Low  - 1 + qrsP.LOWCycle) % qrsP.LOWCycle]);
			printf("high: %i - ", qrsP.DATA_HIGH[(qrsP.Index_High  - 1 + qrsP.HIGHCycle) % qrsP.HIGHCycle]);
			printf("der: %i - ", qrsP.DATA_DER[(qrsP.Index_Der  - 1 + qrsP.DERCycle) % qrsP.DERCycle]);
			printf("sqr: %i - ", qrsP.DATA_SQR[(qrsP.Index_Sqr  - 1 + qrsP.SQRCycle) % qrsP.SQRCycle]);
			printf("mwi: %i - ", qrsP.DATA_MWI[qrsP.Index_Mwi - 1]);
			printf("time: %i ", qrsP.DATA_TIMEMS - 4);
		} else {
			int peak = 0;
			if (isPeak(ptr, qrsP.Index_Mwi - 1) == 1) {
				qrsP.SPKF = SPKF(ptr);
				peak = 1;
			}
			int height = qrsP.DATA_MWI[qrsP.Index_Mwi - 1] / 100;
			if (height > 0) {
				lastheight = height;
				for (int y = 0; y < height; y++) {
					printf(".");
				}
				if (peak == 1) {
					printf("  [%4.2f, %i ms]", qrsP.SPKF, qrsP.DATA_TIMEMS - 4);
				}
				printf("\n");

			} else if (lastheight > 0) {
				lastheight = 0;
				printf("\n[...]\n\n");
			}

		}	// DEBUG (END)
	}

 	closeFile(file);

    return 1;
}
