#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWI 9999

#define LOOPCOUNT 100

#define DEBUGMODE 2

#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>

int main() {
	QRS_params qrsP = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RAWC, LOWC, HIGHC, DERC, SQRC, {0}, {0}, {0}, {0}, {0}};
	QRS_params *ptr = &qrsP;

	// data inits
	qrsP.RR_LOW = 0;
	qrsP.RR_HIGH = 2000;

	// debug
	int lastheight = 0;

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

			// DEBUG (START)

			if (i > 4) {

				if (DEBUGMODE == 0) {
					// Peak detection
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
				} else if (DEBUGMODE == 1){
					int peak = 0;
					if (isPeak(ptr, i*LOWC + j) == 1) {
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
					int height = qrsP.DATA_MWI[i*LOWC + j] / 50;
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
					if (isPeak(ptr, i*LOWC + j) == 1) {
						if (qrsP.DATA_PEAKS[qrsP.peakcount] > qrsP.THRESHOLD1) {
							if (qrsP.peakcount > 0) {
								int RRpeak = qrsP.DATA_PEAKSTIME[qrsP.peakcount] - qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK];
								if (RRpeak > qrsP.RR_LOW-1 && RRpeak < qrsP.RR_HIGH+1) {
									CalculateRR(ptr);
									printf("Value: %i [%i ms since last peak]\n", qrsP.DATA_PEAKS[qrsP.peakcount], qrsP.DATA_PEAKSTIME[qrsP.peakcount]-qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK]);
									qrsP.LAST_RPEAK = qrsP.peakcount;
								} else {
									if (RRpeak >= qrsP.RR_MISS) {
										int backwards = qrsP.peakcount;
										while (backwards >= 0) {
											if (qrsP.DATA_PEAKS[backwards] > qrsP.THRESHOLD2) {
												CalculateRR(ptr);
												printf("Value: %i [%i ms since last peak]\n", qrsP.DATA_PEAKS[qrsP.peakcount], qrsP.DATA_PEAKSTIME[qrsP.peakcount]-qrsP.DATA_PEAKSTIME[qrsP.LAST_RPEAK]);
												qrsP.LAST_RPEAK = qrsP.peakcount;
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
				}

			}

			// DEBUG (END)

			qrsP.DATA_TIMEMS+= 4;
		}
 	}

 	closeFile(file);

    return 1;
}
