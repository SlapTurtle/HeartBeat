#include "qsr.h"

void peakDetection(QRS_params *params) {

}

int isPeak(QRS_params *params, int index) {
	int p = index - 1;
	if (params->DATA_MWI[p-1] <= params->DATA_MWI[p] && params->DATA_MWI[p+1] < params->DATA_MWI[p]) {
		params->DATA_PEAKS[params->peakcount] = params->DATA_MWI[p];
		params->DATA_PEAKSTIME[params->peakcount] = params->DATA_TIMEMS;

		params->SPKF = SPKF(params);

		params->peakcount++;
		return 1;
	} else {
		return 0;
	}
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

