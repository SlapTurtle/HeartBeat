#include "sensor.c"
#include "filters.c"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

static QRS_params qrsP = {0, 0, 0, 0, 13, 33, 5, 1, 101, {0}, {0}, {0}, {0}, {0}};

int main() {
    FILE *file = openfile("ECG.txt");
 	int *data[5] = {qrsP.DATA_RAW, qrsP.DATA_LOW, qrsP.DATA_HIGH, qrsP.DATA_DER, qrsP.DATA_SQR};

 	for(int i = 0; i < 10; i++){
 		for(int j = 0; j < 33; j++){

			qrsP.DATA_RAW[j % 13] = getNextData(file);

			if(i > 0) {
				qrsP.DATA_LOW[j % 33] = lowPassFilter(data[0], data[1], j);
			}
			if(i > 1) {
				qrsP.DATA_HIGH[j % 5] = highPassFilter(data[1], data[2], j);
			}
			if(i > 2) {
				qrsP.DATA_DER[j % 1] = derivativeFilter(data[2], data[3], j);
			}
			if(i > 3){
				qrsP.DATA_SQR[j % 101] = squaringFilter(data[3], data[4], j);
			}

			printf("cycle: %i - ", i);				//print
			printf("index: %i - ", j);				//print
			printf("raw: %i - ", data[0][j % 13]);	//print
			printf("low: %i - ", data[1][j % 33]);	//print
			printf("high: %i - ", data[2][j % 5]);	//print
			printf("der: %i - ", data[3][j % 1]); 	//print
			printf("sqr: %i", data[4][j % 101]);	//print
			printf("\n");							//print
		}
 	}

    return 0;
}
