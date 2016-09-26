#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 3
#define PEAKC 250

#define DEBUGMODE 1

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include "limits.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RAWC, LOWC, HIGHC, DERC, SQRC, MWIC, PEAKC, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 0, 0}, *ptr = &qrsP;

	// data inits
	qrsP.RR_LOW = 0;
	qrsP.RR_HIGH = 300;

    FILE *file = openFile("ECG.txt");

	for(int newData = getNextData(file); newData != INT_MAX; newData = getNextData(file)){

		qrsP.DATA_TIMEMS += 4;	//250 inputs per second = 4 ms / input

		qrsP.DATA_RAW[qrsP.Index_Raw] = newData;
		qrsP.DATA_LOW[qrsP.Index_Low] = lowPassFilter(ptr);
		qrsP.DATA_HIGH[qrsP.Index_High] = highPassFilter(ptr);
		qrsP.DATA_DER[qrsP.Index_Der] = derivativeFilter(ptr);
		qrsP.DATA_SQR[qrsP.Index_Sqr] = squaringFilter(ptr);
		qrsP.DATA_MWI[qrsP.Index_Mwi] = MoveWindowsIntegration(ptr);

		if (DEBUGMODE == 0) {
			// Peak detection
			if (isPeak(ptr, qrsP.Index_Mwi) == 1) {
				qrsP.SPKF = SPKF(ptr);
				printf("[PEAK] [%f]\n", qrsP.SPKF);
			} else {
				printf("\n");
			}
			printf("raw: %i - ", qrsP.DATA_RAW[qrsP.Index_Raw]);
			printf("low: %i - ", qrsP.DATA_LOW[qrsP.Index_Low]);
			printf("high: %i - ", qrsP.DATA_HIGH[qrsP.Index_High]);
			printf("der: %i - ", qrsP.DATA_DER[qrsP.Index_Der]);
			printf("sqr: %i - ", qrsP.DATA_SQR[qrsP.Index_Sqr]);
			printf("mwi: %i - ", qrsP.DATA_MWI[qrsP.Index_Mwi]);
			printf("time: %i ", qrsP.DATA_TIMEMS);

		} else if (DEBUGMODE == 1){
			peakDetection(ptr);
		}

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
		if(qrsP.Index_Mwi == qrsP.MWICycle){qrsP.Index_Mwi = 0;};
	}

 	closeFile(file);

    return 1;
}

