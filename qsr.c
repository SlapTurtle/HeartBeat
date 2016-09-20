#include "qsr.h"
#include <stdio.h>

void peakDetection(QRS_params *params) {

}

int isPeak(QRS_params *params, int index) {
	int p = index - 1;
	if (params->DATA_MWI[p-1] < params->DATA_MWI[p] && params->DATA_MWI[p+1] < params->DATA_MWI[p]) {
		params->DATA_PEAKS[params->peakcount] = params->DATA_MWI[p];
		params->DATA_PEAKSTIME[params->peakcount] = params->DATA_TIMEMS;

		params->SPKF = SPKF(params);
		return 1;
	} else {
		return 0;
	}
}

void CalculateRR(QRS_params *params) {
	if (params->peakcount < 1) {
		return;
	}

	int i = 0;
	int rr2accum = 0;
	int rr2count = 0;
	int accum = 0;
	int count = 0;

	while (i <= params->peakcount - 1) {
		if (i <= 7) {
		accum+= params->DATA_PEAKSTIME[params->peakcount-i] - params->DATA_PEAKSTIME[params->peakcount-i-1];
		count = i+1;
		}

		if (rr2count <= 7) {
			if (params->DATA_PEAKS[params->peakcount-i] > params->THRESHOLD1) {
				rr2accum += params->DATA_PEAKSTIME[params->peakcount-i] - params->DATA_PEAKSTIME[params->peakcount-i-1];
				rr2count++;
			}
		} else {
			break;
		}

		i++;
	}

	if (count > 0) {
		//printf("SETTING AVG1 TO %i \n", accum/count);
		params->RR_AVERAGE1 = accum/count;
	}

	if (rr2count > 0) {
		//printf("SETTING AVG2 TO %i \n", rr2accum/rr2count);
		params->RR_AVERAGE2 = rr2accum/rr2count;
	}

	params->RR_LOW = 0.92 * params->RR_AVERAGE2;
	params->RR_HIGH = 1.16 * params->RR_AVERAGE2;
	params->RR_MISS = 1.66 * params->RR_AVERAGE2;

	params->SPKF = SPKF(params);

	params->THRESHOLD1 = params->NPKF + 0.25 * (params->SPKF - params->NPKF);
	params->THRESHOLD2 = 0.5 * params->THRESHOLD1;
	//printf("SETTING THRESHOLDS TO %.2f and %.2f", params->THRESHOLD1, params->THRESHOLD2);
	//printf("TRESHOLD1 IS NOW %i \n", params->THRESHOLD1);
	//printf("TRESHOLD2 IS NOW %i \n", params->THRESHOLD2);

}


double SPKF(QRS_params *params) {
	return 0.125 * params->DATA_PEAKS[params->peakcount] + 0.875 * params->SPKF;
}

double THRESHOLD1(QRS_params *params) {
	return params->NPKF + 0.25 * (params->SPKF - params->NPKF);
}

double THRESHOLD2(QRS_params *params) {
	return 0.5 * params->THRESHOLD1;
}

double NPKF(QRS_params *params) {
	return 0.125 * params->DATA_PEAKS[params->peakcount] + 0.875 * params->NPKF;
}

