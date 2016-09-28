#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Main function for the QRS algorithm
void peakDetection(QRS_params *qrsP) {

	//Find Peak
	if(qrsP->DATA_MWI[(qrsP->Index_Mwi-2 + MWIC) % MWIC] < qrsP->DATA_MWI[(qrsP->Index_Mwi-1 + MWIC) % MWIC] && qrsP->DATA_MWI[(qrsP->Index_Mwi-1 + MWIC) % MWIC] > qrsP->DATA_MWI[qrsP->Index_Mwi]){

		//peak found
		int peak = qrsP->DATA_MWI[(qrsP->Index_Mwi-1 + MWIC) % MWIC];

		//Store peak and time
		qrsP->DATA_PEAKS[qrsP->peakcount] = peak;
		qrsP->DATA_PEAKSTIME[qrsP->peakcount] = qrsP->DATA_TIMEMS;

		//Threshold1 check
		if(peak <= qrsP->THRESHOLD1){

			//peak less than or equal to threshold 1
			qrsP->NPKF = 0.125*peak + 0.875*qrsP->NPKF;
			qrsP->THRESHOLD1 = qrsP->NPKF + 0.25 * (qrsP->SPKF - qrsP->NPKF);
			qrsP->THRESHOLD2 = 0.5 * qrsP->THRESHOLD1;

		}
		//Peak higher than threshold 1
		else {

			//CalculateRR
			double rr = qrsP->DATA_PEAKSTIME[qrsP->peakcount] - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK];

			//RR_LOW < RR < RR_HIGH
			if((qrsP->RR_LOW < rr && rr < qrsP->RR_HIGH)){

				//reset miss-counter
				qrsP->RRmissed = 0;

				//algorithm values
				qrsP->LAST_RPEAK = qrsP->peakcount; 		//store index of last Rpeak
				qrsP->SPKF = 0.125*peak + 0.875*qrsP->SPKF;
				qrsP->DATA_RECENTRR_OK[qrsP->Index_RROK] = rr;
				qrsP->DATA_RECENTRR[qrsP->Index_RR] = rr;
				qrsP->RR_AVERAGE2 = avg2(qrsP);
				qrsP->RR_AVERAGE1 = avg1(qrsP);
				qrsP->RR_LOW = 0.92*qrsP->RR_AVERAGE2;
				qrsP->RR_HIGH = 1.16*qrsP->RR_AVERAGE2;
				qrsP->RR_MISS = 1.66*qrsP->RR_AVERAGE2;
				qrsP->THRESHOLD1 = qrsP->NPKF + 0.25 * (qrsP->SPKF - qrsP->NPKF);
				qrsP->THRESHOLD2 = 0.5 * qrsP->THRESHOLD1;

				//updates index
				qrsP->Index_RROK++;
				if(qrsP->Index_RROK == RRC) { qrsP->Index_RROK = 0; }
				qrsP->Index_RR++;
				if(qrsP->Index_RR == RRC) { qrsP->Index_RR = 0; }

			}
			//RR > RR_MISS
			else if (rr > qrsP->RR_MISS){

				//increment miss-counter
				qrsP->RRmissed++;

				//Searchback for new peak above threshold2
				int peak2 = 0, back;
				for(back = 1; peak2 <= qrsP->THRESHOLD2; back++){
					peak2 = qrsP->DATA_PEAKS[(qrsP->peakcount - back + PEAKC) % PEAKC];
				}

				//calculate new RR
				//rr = qrsP->DATA_PEAKSTIME[(qrsP->peakcount - back + PEAKC) % PEAKC] - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK];

				//algorithm values
				qrsP->LAST_RPEAK = qrsP->peakcount; 		//store index of last Rpeak
				qrsP->SPKF = 0.25*peak2 + 0.75*qrsP->SPKF;
				qrsP->DATA_RECENTRR[qrsP->Index_RR] = rr;
				qrsP->RR_AVERAGE1 = avg1(qrsP);
				qrsP->RR_LOW = 0.92*qrsP->RR_AVERAGE1;
				qrsP->RR_HIGH = 1.16*qrsP->RR_AVERAGE1;
				qrsP->RR_MISS = 1.66*qrsP->RR_AVERAGE1;
				qrsP->THRESHOLD1 = qrsP->NPKF + 0.25 * (qrsP->SPKF - qrsP->NPKF);
				qrsP->THRESHOLD2 = 0.5 * qrsP->THRESHOLD1;

				//update indexes
				qrsP->Index_RR++;
				if(qrsP->Index_RR == RRC) { qrsP->Index_RR = 0; }
			}
			else{
				//qrsP->LAST_RPEAK = qrsP->peakcount;
				//qrsP->DATA_RECENTRR[qrsP->Index_RR] = rr;
			}

			//output every r-peak found.
			output(qrsP);

			/*
			printf("spkf: %.1f - ", qrsP->SPKF);
			printf("npkf: %.1f - ", qrsP->NPKF);
			printf("RR: %.1f - ", rr);
			printf("peakval: %i - ", peak);
			printf("time: %i - ", qrsP->DATA_TIMEMS);
			printf("low: %.1f - ", qrsP->RR_LOW);
			printf("high: %.1f - ", qrsP->RR_HIGH);
			printf("miss: %.1f - ", qrsP->RR_MISS);
			printf("av1: %.1f - ", qrsP->RR_AVERAGE1);
			printf("av2: %.1f - ", qrsP->RR_AVERAGE2);
			printf("t1: %.1f -  ", qrsP->THRESHOLD1);
			printf("t2: %.1f \n", qrsP->THRESHOLD2);
			*/

		}
		//updates peakcount
		qrsP->peakcount++;
		if(qrsP->peakcount == PEAKC) { qrsP->peakcount = 0; }
	}

}

double avg1(QRS_params *qrsP){
	double accum = 0.0;
	int count = 0;
	for(int i = 0; i < RRC; i++){
		if(qrsP->DATA_RECENTRR[i]){
			accum += qrsP->DATA_RECENTRR[i];
			count++;
		}
	}

	return accum/count;
}

double avg2(QRS_params *qrsP){
	double accum = 0.0;
	int count = 0;
	for(int i = 0; i < RRC; i++){
		if(qrsP->DATA_RECENTRR_OK[i]){
			accum += qrsP->DATA_RECENTRR_OK[i];
			count++;
		}
	}
	return accum/count;
}

void output(QRS_params *qrsP) {

	int peakValue = qrsP->DATA_PEAKS[qrsP->peakcount];
	double peaktime = qrsP->DATA_PEAKSTIME[qrsP->peakcount];
	double dur = qrsP->DATA_PEAKSTIME[qrsP->peakcount] - qrsP->DATA_PEAKSTIME[qrsP->LAST_RPEAK];

	double pulse = 60.0 * 1000.0 / ( (dur) ? dur : (qrsP->DATA_RECENTRR[(qrsP->Index_RR + RRC) % RRC]));

	char* PVwarning = "";
	char* RRwarning = "";
	if(peakValue < 2000){
		PVwarning = "[Warning: Low R-Peak!]";
	}
	if(qrsP->RRmissed >= 5){
		RRwarning = "[Warning: Missed RR-Intervals!]";
	}

	if (0 < pulse && pulse < 300) { // The decision to disregard certain peaks is clarified in the report
		char pval[4], time[5], hrat[5];
		sprintf(pval, "%i", peakValue);
		sprintf(time, "%.2f", (peaktime/1000.0));
		sprintf(hrat, "%.1f", pulse);
		printf("R-Peak Value: %s | Time: %s s | Heartrate: %s bpm | %s%s \n", appendSpaces(pval, 4), appendSpaces(time, 5), appendSpaces(hrat, 5), PVwarning, RRwarning);
	}
}

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
