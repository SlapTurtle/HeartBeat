#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 9999

#define LOOPCOUNT 100

#define DEBUGMODE 1

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RAWC, LOWC, HIGHC, DERC, SQRC, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, 0, 0}, *ptr = &qrsP;

	// data inits
	qrsP.RR_LOW = 0;
	qrsP.RR_HIGH = 2000;

	// debug
	int lastheight = 0;

    FILE *file = openFile("ECG.txt");

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
		} else if (DEBUGMODE == 1){
			int peak = 0;
			if (isPeak(ptr, qrsP.Index_Mwi - 1) == 1) {
				// datapoint is a peak
				if (qrsP.DATA_PEAKS[qrsP.peakcount] > qrsP.THRESHOLD1) {
					// peak is greater than THRESHOLD1
					printf("PEAK %i GREATER THAN THRESHOLD1 %.1f \n", qrsP.DATA_PEAKS[qrsP.peakcount], qrsP.THRESHOLD1);

					if (qrsP.peakcount > 0) {
						// not first peak
						int RRpeak = qrsP.DATA_PEAKSTIME[qrsP.peakcount] - qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK];
						if (RRpeak > qrsP.RR_LOW-1 && RRpeak < qrsP.RR_HIGH+1) {
							printf("RRpeak %i BETWEEN BOUNDS %i AND %i \n", RRpeak, qrsP.RR_LOW, qrsP.RR_HIGH);
							// peak interval is within bounds
							CalculateRR(ptr);
						} else {
							printf("RRpeak %i NOT BETWEEN BOUNDS %i AND %i \n", RRpeak, qrsP.RR_LOW, qrsP.RR_HIGH);
							if (RRpeak >= qrsP.RR_MISS) {
								// peak interval is greater than miss bound
								int backwards = qrsP.peakcount;
								printf("SEARCHBACK [RRpeak %i >= RR_MISS %i]\n", RRpeak, qrsP.RR_MISS);
								while (backwards >= 0) {
									if (qrsP.DATA_PEAKS[backwards] > qrsP.THRESHOLD2) {
										CalculateRR(ptr);
										break;
									} else {
										backwards--;
									}
								}
							}
						}
					} else {
						printf("FIRST PEAK DETECTED AND DISMISSED\n");
						CalculateRR(ptr);
					}

					//CalculateRR(ptr);

					qrsP.peakcount++;
				} else {
					// peak is not greater than THRESHOLD1
					printf("PEAK %i NOT GREATER THAN THRESHOLD1 %.1f \n", qrsP.DATA_PEAKS[qrsP.peakcount], qrsP.THRESHOLD1);
					qrsP.NPKF = NPKF(ptr);
					qrsP.THRESHOLD1 = THRESHOLD1(ptr);
					qrsP.THRESHOLD2 = THRESHOLD2(ptr);

				}

				//qrsP.SPKF = SPKF(ptr);
				peak = 1;
			}

			// DATA PLOT
			int height = qrsP.DATA_MWI[qrsP.Index_Mwi - 1] / 50;
			if (height > 0) {
				lastheight = height;
				for (int y = 0; y < height; y++) {
					printf(".");
				}
				if (peak == 1) {
					printf("  [%4.2f, %i ms, avg: %i - %i]", qrsP.SPKF, qrsP.DATA_TIMEMS, qrsP.RR_AVERAGE1, qrsP.RR_AVERAGE2);
				}
				printf("\n");

			} else if (lastheight > 0) {
				lastheight = 0;
				printf("\n[...]\n\n");
			}

		} else {
			if (isPeak(ptr, qrsP.Index_Mwi - 1) == 1) {
				if (qrsP.DATA_PEAKS[qrsP.peakcount] > qrsP.THRESHOLD1) {
					if (qrsP.peakcount > 0) {
						int RRpeak = qrsP.DATA_PEAKSTIME[qrsP.peakcount] - qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK];
						if (RRpeak > qrsP.RR_LOW-1 && RRpeak < qrsP.RR_HIGH+1) {
							result(ptr);
						} else {
							if (RRpeak >= qrsP.RR_MISS) {
								int backwards = qrsP.peakcount;
								while (backwards >= 0) {
									if (qrsP.DATA_PEAKS[backwards] > qrsP.THRESHOLD2) {
										result(ptr);
										break;
									} else {
										backwards--;
									}
								}
							}
						}
					} else {
						CalculateRR(ptr);
					}
					qrsP.peakcount++;
				} else {
					qrsP.NPKF = NPKF(ptr);
					qrsP.THRESHOLD1 = THRESHOLD1(ptr);
					qrsP.THRESHOLD2 = THRESHOLD2(ptr);

				}
			}

		}	// DEBUG (END)
	}

 	closeFile(file);

    return 1;
}
