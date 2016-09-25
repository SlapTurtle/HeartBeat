#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 3
#define PEAKC 250

#define LOOPCOUNT 10000

#define DEBUGMODE 2

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RAWC, LOWC, HIGHC, DERC, SQRC, MWIC, PEAKC, 0, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 0, 0}, *ptr = &qrsP;

	// data inits
	qrsP.RR_LOW = 0;
	qrsP.RR_HIGH = 300;

    FILE *file = openFile("ECG.txt");

	for(int i = 0; i < LOOPCOUNT; i++){

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
		if(qrsP.Index_Mwi == qrsP.MWICycle){qrsP.Index_Mwi = 0;};
		if(qrsP.peakcount == qrsP.PeakCyle){qrsP.peakcount = 0;};

		qrsP.DATA_TIMEMS+= 4;	//250 inputs per second = 4 ms / input

		if (DEBUGMODE == 0) {
			// Peak detection
			if (isPeak(ptr, (qrsP.Index_Mwi - 1 + qrsP.MWICycle) % qrsP.MWICycle) == 1) {
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
				if (isPeak(ptr, (qrsP.Index_Mwi - 1 + qrsP.MWICycle) % qrsP.MWICycle)) {
					if (qrsP.DATA_PEAKS[qrsP.peakcount%PEAKC] > qrsP.THRESHOLD1) {
						if (qrsP.peakcount%PEAKC && qrsP.THRESHOLD1) {
							int RRpeak = qrsP.DATA_PEAKSTIME[qrsP.peakcount%PEAKC] - qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK];
							if (RRpeak > qrsP.RR_LOW-1 && RRpeak < qrsP.RR_HIGH+1) {
								result(ptr, qrsP.peakcount%PEAKC);
							} else {
								if (RRpeak > qrsP.RR_MISS) {
									int backwards = qrsP.peakcount%PEAKC;
									while (1) {
										if (qrsP.DATA_PEAKS[backwards] > qrsP.THRESHOLD2) {
											result(ptr, backwards);
											break;
										} else {
											if (backwards > 0) {
												backwards--;
											} else {
												backwards = PEAKC-1;
											}

										}
									}
								}
							}
						} else {
							CalculateRR(ptr, qrsP.peakcount%PEAKC);
						}
						qrsP.peakcount++;
					} else {
						qrsP.NPKF = NPKF(ptr);
						qrsP.THRESHOLD1 = THRESHOLD1(ptr);
						qrsP.THRESHOLD2 = THRESHOLD2(ptr);

					}
				}
			}
		}
		// DEBUG (END)

 	closeFile(file);

    return 1;
}
