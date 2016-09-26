#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main function for the QRS algorithm
void peakDetection(QRS_params *qrsP) {
	int missedWarning = 0;
	if (isPeak(qrsP, qrsP->Index_Mwi)) {
		if (qrsP->DATA_PEAKS[qrsP->peakcount%PEAKC] > qrsP->THRESHOLD1) {
			if (qrsP->peakcount%PEAKC && qrsP->THRESHOLD1) {
				int RRpeak = qrsP->DATA_PEAKSTIME[qrsP->peakcount%PEAKC] - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK];
				printf("RRpeak: %i, RR_LOW: %f, RR_HIGH: %f \n", RRpeak, qrsP->RR_LOW, qrsP->RR_HIGH);
				if (RRpeak > qrsP->RR_LOW-1 && RRpeak < qrsP->RR_HIGH+1) {
					qrsP->RRmissed = 0;
					result(qrsP, qrsP->peakcount%PEAKC, 0);
				} else {
					//printf("%i \n", qrsP->RRmissed);
					if (qrsP->RRmissed >= 5) {
						missedWarning = 1;
					}
					qrsP->RRmissed++;
					if (RRpeak > qrsP->RR_MISS) {
						int backwards = qrsP->peakcount%PEAKC;
						while (1) {
							// cannot run indefinitely - will at most run until previous peak was found,
							// and at least one previous peak is present when this block of code is reached.
							if (qrsP->DATA_PEAKS[backwards] > qrsP->THRESHOLD2) {
								result(qrsP, backwards, missedWarning);
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
				CalculateRR(qrsP, qrsP->peakcount%PEAKC);
			}
			qrsP->peakcount++;
			if(qrsP->peakcount == qrsP->PeakCyle){qrsP->peakcount = 0;};

		} else {
			qrsP->NPKF = NPKF(qrsP);
			qrsP->THRESHOLD1 = THRESHOLD1(qrsP);
			qrsP->THRESHOLD2 = THRESHOLD2(qrsP);
		}
	}
}

// Checks whether a point is a peak or not (4 ms delay)
int isPeak(QRS_params *params, int index) {
	int p = (index - 1 + params->MWICycle) % params->MWICycle;
	if (params->DATA_MWI[(p-1 + params->MWICycle) % params->MWICycle] < params->DATA_MWI[p] && params->DATA_MWI[(p+1 + params->MWICycle) % params->MWICycle] <= params->DATA_MWI[p]) {
		params->DATA_PEAKS[params->peakcount%PEAKC] = params->DATA_MWI[p];
		params->DATA_PEAKSTIME[params->peakcount%PEAKC] = params->DATA_TIMEMS;

		params->SPKF = SPKF(params);
		return 1;
	} else {
		return 0;
	}
}

// Part of the result function scheme
// Handles the change in values when an R peak is measured
void CalculateRR(QRS_params *params, int c) {
	if (c < 1) {
		return;
	}

	int i = 0;
	int rr2accum = 0;
	int rr2count = 0;
	int accum = 0;
	int count = 0;

	while ((c - i) - i >= 0) {
		if (i <= 7) {
		accum+= params->DATA_PEAKSTIME[c-i] - params->DATA_PEAKSTIME[c-i-1];
		count = i+1;
		}

		if (rr2count <= 7) {
			if (params->DATA_PEAKS[c-i] > params->THRESHOLD1) {
				rr2accum += params->DATA_PEAKSTIME[c-i] - params->DATA_PEAKSTIME[c-i-1];
				rr2count++;
			}
		} else {
			break;
		}

		i++;
	}

	if (count > 0) {
		params->RR_AVERAGE1 = accum/count;
	}

	if (rr2count > 0) {
		params->RR_AVERAGE2 = rr2accum/rr2count;
	}

	params->RR_LOW = 0.92 * params->RR_AVERAGE2;
	params->RR_HIGH = 1.16 * params->RR_AVERAGE2;
	params->RR_MISS = 1.66 * params->RR_AVERAGE2;

	params->SPKF = SPKF(params);

	params->THRESHOLD1 = params->NPKF + 0.25 * (params->SPKF - params->NPKF);
	params->THRESHOLD2 = 0.5 * params->THRESHOLD1;

}

// Helper function for data output
char *appendSpaces(char in[], int max){
    size_t n = strlen(in);
    int diff = max - (int)n;
    char *out = malloc(n + diff + 1);
    strcpy(out, in);
    for(int i = 0; i < diff; i++){
    	out[n + i] = ' ';
    }
    out[n + diff] = '\0';
    return out;
}

// Main function in the result function scheme
// Processes the raw data outputs of the QRS algorithm
// and outputs it to the user of the device
void result(QRS_params *params, int c, int missedWarning) {
	CalculateRR(params, c);
	int peakValue = params->DATA_PEAKS[c];
	double time = params->DATA_PEAKSTIME[c];
	double dur = time - params->DATA_PEAKSTIME[params->LAST_RPEAK];

	if (dur < 0) {
		return;
	}

	double pulse = 60/(dur/1000);
	char* PVwarning = "";
	char* RRwarning = "";
	if(peakValue < 2000){
		PVwarning = "[Warning: Low R-Peak!]";
	}
	if(missedWarning){
		RRwarning = "[Warning: Pulse Stutter]";
	}

	if (pulse < 300) { // The decision to disregard certain peaks is clarified in the report
		char pval[5], tim[5], hrat[5];
		sprintf(pval, "%i", peakValue);
		sprintf(tim, "%.2f", (time / 1000.0));
		sprintf(hrat, "%.1f", pulse);
		printf("Peak Value: %s | Time: %s s | Heartrate: %s bpm | %s %s \n", appendSpaces(pval, 4), appendSpaces(tim, 5), appendSpaces(hrat, 5), PVwarning, RRwarning);
		params->LAST_RPEAK = params->peakcount%PEAKC;
	}
}

// Below four functions are mathematical expressions
// used to calculate new values for SPKF, NPKF and thresholds

double SPKF(QRS_params *params) {
	return 0.125 * params->DATA_PEAKS[params->peakcount%PEAKC] + 0.875 * params->SPKF;
}

double THRESHOLD1(QRS_params *params) {
	return params->NPKF + 0.25 * (params->SPKF - params->NPKF);
}

double THRESHOLD2(QRS_params *params) {
	return 0.5 * params->THRESHOLD1;
}

double NPKF(QRS_params *params) {
	return 0.125 * params->DATA_PEAKS[params->peakcount%PEAKC] + 0.875 * params->NPKF;
}
