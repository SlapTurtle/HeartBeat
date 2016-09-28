#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main function for the QRS algorithm
void peakDetection(QRS_params *qrsP) {
	if (isPeak(qrsP, qrsP->Index_Mwi)) {
		if (qrsP->DATA_PEAKS[qrsP->peakcount%PEAKC] > qrsP->THRESHOLD1) {
			if (qrsP->THRESHOLD1) {
				int RRpeak = qrsP->DATA_PEAKSTIME[qrsP->peakcount%PEAKC] - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK];
				if (RRpeak > qrsP->RR_LOW && RRpeak < qrsP->RR_HIGH) {
					qrsP->RRmissed = 0;

					qrsP->DATA_RPEAKS[qrsP->Index_RPeak++] = RRpeak;
					if(qrsP->Index_RPeak == qrsP->RPEAKCycle){ qrsP->Index_RPeak = 0; }
					qrsP->DATA_RPEAKS_OK[qrsP->Index_RPeak_OK++] = RRpeak;
					if(qrsP->Index_RPeak_OK == qrsP->RPEAKCycle){ qrsP->Index_RPeak_OK = 0; }

					setValues1(qrsP, qrsP->peakcount);

				} else {
					if (RRpeak > qrsP->RR_MISS) {

						int backwards = qrsP->peakcount%PEAKC;
						while (qrsP->DATA_PEAKS[backwards] < qrsP->THRESHOLD2) {
							// cannot run indefinitely - will at most run until previous peak was found,
							// and at least one previous peak is present when this block of code is reached.
							if (backwards > 0) {
								backwards--;
							} else {
								backwards = PEAKC-1;
							}
						}
						qrsP->DATA_RPEAKS[qrsP->Index_RPeak++] = RRpeak;
						if(qrsP->Index_RPeak == qrsP->RPEAKCycle){ qrsP->Index_RPeak = 0; }

						setValues2(qrsP, backwards);
					}
				}
			} else {
				setValues1(qrsP, qrsP->peakcount);
			}
			qrsP->peakcount++;
			if(qrsP->peakcount == qrsP->PeakCyle){qrsP->peakcount = 0;};
		} else {
			qrsP->NPKF = NPKF(qrsP, qrsP->peakcount%PEAKC);
			qrsP->THRESHOLD1 = THRESHOLD1(qrsP);
			qrsP->THRESHOLD2 = THRESHOLD2(qrsP);

			//Output missed 5 consecutive rr-intervals
			if ((qrsP->DATA_TIMEMS - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK]) > qrsP->DATA_RPEAKS[(qrsP->Index_RPeak - 1 + PEAKC) % PEAKC]*5){
				if(qrsP->DATA_TIMEMS - qrsP->dead_timems > qrsP->DATA_RPEAKS[(qrsP->Index_RPeak - 1 + PEAKC) % PEAKC]){
					qrsP->dead_timems = qrsP->DATA_TIMEMS;
					printf("[Warning: No Pulse Detected At %.2f s!]\n", qrsP->DATA_TIMEMS/1000.0);
				}
			}
		}
	}
}

// Checks whether a point is a peak or not (4 ms delay)
int isPeak(QRS_params *params, int index) {
	int p = (index - 1 + params->MWICycle) % params->MWICycle;
	if (params->DATA_MWI[(p-1 + params->MWICycle) % params->MWICycle] < params->DATA_MWI[p] && params->DATA_MWI[(p+1 + params->MWICycle) % params->MWICycle] < params->DATA_MWI[p]) {
		params->DATA_PEAKS[params->peakcount%PEAKC] = params->DATA_MWI[p];
		params->DATA_PEAKSTIME[params->peakcount%PEAKC] = params->DATA_TIMEMS;
		return 1;
	} else {
		return 0;
	}
}

void setValues1(QRS_params *params, int c) {
	params->SPKF = SPKF(params, c);

	double rr2accum = 0;
	double rr2count = 0;
	double accum = 0;
	double count = 0;

	for(int i = 0; i < PEAKC; i++){
		if(params->DATA_RPEAKS[i]){
			accum += params->DATA_RPEAKS[i];
			count++;
		}
	}
	for(int i = 0; i < PEAKC; i++){
		if(params->DATA_RPEAKS_OK[i]){
			rr2accum += params->DATA_RPEAKS_OK[i];
			rr2count++;
		}
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

	params->THRESHOLD1 = THRESHOLD1(params);
	params->THRESHOLD2 = THRESHOLD2(params);

	output(params, c);
}

void setValues2(QRS_params *params, int c) {
	params->SPKF = SPKF2(params, c);

	double accum = 0;
	double count = 0;

	for(int i = 0; i < PEAKC; i++){
		if(params->DATA_RPEAKS[i]){
			accum += params->DATA_RPEAKS[i];
			count++;
		}
	}

	if (count > 0) {
		params->RR_AVERAGE1 = accum/count;
	}

	params->RR_LOW = 0.92 * params->RR_AVERAGE1;
	params->RR_HIGH = 1.16 * params->RR_AVERAGE1;
	params->RR_MISS = 1.66 * params->RR_AVERAGE1;

	params->THRESHOLD1 = THRESHOLD1(params);
	params->THRESHOLD2 = THRESHOLD2(params);

	output(params, c);
}

// Processes the data outputs of the QRS algorithm
// and translates it to the user of the device
void output(QRS_params *params, int c) {

	int peakValue = params->DATA_PEAKS[c];
	double time = params->DATA_PEAKSTIME[c];
	double last = params->DATA_PEAKSTIME[params->LAST_RPEAK];
	double dur = time - last;

	double pulse = 60/(dur/1000);

	char* PVwarning = "";
	if(peakValue < 2000){
		PVwarning = "[Warning: Low R-Peak!]";
	}

	if (pulse < 300 && pulse > 0) { // The decision to disregard certain peaks is clarified in the report
		char pval[5], tim[5], hrat[5];
		sprintf(pval, "%i", peakValue);
		sprintf(tim, "%.2f", (time / 1000.0));
		sprintf(hrat, "%.1f", pulse);
		printf("R-Peak Value: %s | Time: %s s | Heartrate: %s bpm | %s \n", appendSpaces(pval, 4), appendSpaces(tim, 5), appendSpaces(hrat, 5), PVwarning);
	}

	params->LAST_RPEAK = c;
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


// Below four functions are mathematical expressions
// used to calculate new values for SPKF, NPKF and thresholds

double SPKF(QRS_params *params, int c) {
	double val = params->DATA_PEAKS[c];
	return 0.125 * val + 0.875 * params->SPKF;
}

double SPKF2(QRS_params *params, int c) {
	double val = params->DATA_PEAKS[c];
	return 0.25 * val + 0.75 * params->SPKF;
}

double THRESHOLD1(QRS_params *params) {
	return params->NPKF + 0.25 * (params->SPKF - params->NPKF);
}

double THRESHOLD2(QRS_params *params) {
	return 0.5 * params->THRESHOLD1;
}

double NPKF(QRS_params *params, int c) {
	double val = params->DATA_PEAKS[c];
	return 0.125 * val + 0.875 * params->NPKF;
}
