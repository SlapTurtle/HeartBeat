#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 100

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

    int looprate = RAWC;
	for(int i = 0, j = 0; j < looprate; j++){
		qrsP.DATA_RAW[qrsP.Index_Raw] = getNextData(file);
		qrsP.Index_Raw++;
		if(i == 0 && qrsP.Index_Raw == qrsP.RAWCycle){
			looprate = LOWC;
			j = 0;
			i++;
		}
		qrsP.Index_Raw %= qrsP.RAWCycle;


		if(i > 0) {
			qrsP.DATA_LOW[qrsP.Index_Low] = lowPassFilter(ptr);
			qrsP.Index_Low++;
			if(i == 1 && qrsP.Index_Low == qrsP.LOWCycle){
				looprate = HIGHC;
				j = 0;
				i++;
			}
			qrsP.Index_Low %= qrsP.LOWCycle;

		}
		if(i > 1) {
			qrsP.DATA_HIGH[qrsP.Index_High] = highPassFilter(ptr);
			qrsP.Index_High++;
			if(i == 2 && qrsP.Index_High == qrsP.HIGHCycle){
				looprate = DERC;
				j = 0;
				i++;
			}
			qrsP.Index_High %= qrsP.HIGHCycle;

		}
		if(i > 2) {
			qrsP.DATA_DER[qrsP.Index_Der] = derivativeFilter(ptr);
			qrsP.Index_Der++;
			if(i == 3 && qrsP.Index_Der == qrsP.DERCycle){
				looprate = SQRC;
				j = 0;
				i++;
			}
			qrsP.Index_Der %= qrsP.DERCycle;

		}
		if(i > 3){
			qrsP.DATA_SQR[qrsP.Index_Sqr] = squaringFilter(ptr);
			qrsP.Index_Sqr++;
			if(i == 4 && qrsP.Index_Sqr == qrsP.SQRCycle){
				looprate = MWIC;
				j = 0;
				i++;
			}
			qrsP.Index_Sqr %= qrsP.SQRCycle;


		}
		if(i > 4){
			qrsP.DATA_MWI[qrsP.Index_Mwi] = MoveWindowsIntegration(ptr);
			qrsP.Index_Mwi++;
			//qrsP.Index_Mwi %
		}

		// DEBUG (START)

		if (i > 4) {
			if (DEBUGMODE == 0) {
				// Peak detection
				if (i > 4 && isPeak(ptr, qrsP.Index_Mwi-1) == 1) {
					qrsP.SPKF = SPKF(ptr);
					printf("[PEAK] [%f]\n", qrsP.SPKF);
				} else {
					printf("\n");
				}
				printf("cycle: %i - ", i);
				printf("index: %i - ", j);
				printf("raw: %i - ", qrsP.DATA_RAW[(qrsP.Index_Raw - 1 + qrsP.RAWCycle) % qrsP.RAWCycle]);
				printf("low: %i - ", qrsP.DATA_LOW[(qrsP.Index_Low - 1 + qrsP.LOWCycle) % qrsP.LOWCycle]);
				printf("high: %i - ", qrsP.DATA_HIGH[(qrsP.Index_High - 1 + qrsP.HIGHCycle) % qrsP.HIGHCycle]);
				printf("der: %i - ", qrsP.DATA_DER[(qrsP.Index_Der - 1 + qrsP.DERCycle) % qrsP.DERCycle]);
				printf("sqr: %i - ", qrsP.DATA_SQR[(qrsP.Index_Sqr - 1 + qrsP.SQRCycle) % qrsP.SQRCycle]);
				printf("mwi: %i - ", qrsP.DATA_MWI[qrsP.Index_Mwi - 1]);
				printf("time: %i ", qrsP.DATA_TIMEMS);
			} else {
				int peak = 0;
				if (isPeak(ptr, qrsP.Index_Mwi-1) == 1) {
					qrsP.SPKF = SPKF(ptr);
					peak = 1;
				}
				int height = qrsP.DATA_MWI[qrsP.Index_Mwi-1] / 100;
				if (height > 0) {
					lastheight = height;
					for (int y = 0; y < height; y++) {
						printf(".");
					}
					if (peak == 1) {
						printf("  [%4.2f, %i ms]", qrsP.SPKF, qrsP.DATA_TIMEMS);
					}
					printf("\n");

				} else if (lastheight > 0) {
					lastheight = 0;
					printf("\n[...]\n\n");
				}

			}

		}

		// DEBUG (END)

		qrsP.DATA_TIMEMS+= 4;
	}

 	closeFile(file);

    return 1;
}
