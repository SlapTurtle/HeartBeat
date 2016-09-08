#include "sensor.c"
#include "filters.c"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

// Main function for organising the program execution.
// The functions and object predefined are just for inspiration.
// Please change order,names arguments to fit your solution.

int main() {
    QRS_params qrsP;
    FILE *file;
 	file = openfile("ECG.txt");
 	int *arr = qrsP.DATA_RAW;
 	int *arr2 = qrsP.DATA_LOW;
 	int *arr3 = qrsP.DATA_HIGH;
 	int *arr4 = qrsP.DATA_DER;
 	int *arr5 = qrsP.DATA_SQR;

 	/*
 	for(int i = 0; i < 32; i++){
 		qrsP.DATA_RAW[i] = 0;
 	}
 	for(int i = 0; i < 32; i++){
 		qrsP.DATA_LOW[i] = 0;
 	}*/

 	//datagetters
 	for(int i = 0; i < 6; i++){
 		printf("CYCLE: %i %s \n ", i, "--------------------");	//print
 		for(int j = 0; j < 32; j++){
			printf("ID: %i - ", j);				//print

			qrsP.DATA_RAW[j] = getNextData(file);
			printf("raw: %i - ", arr[j]);	//print

			if(i > 0){
				qrsP.DATA_LOW[j] = lowPassFilter(arr, arr2, j);
			}
			else{
				qrsP.DATA_LOW[j] = qrsP.DATA_RAW[j];
			}
			printf("low: %i - ", arr2[j]);	//print

			/*if(i > 1){
				qrsP.DATA_HIGH[j % 32] = highPassFilter(arr2, arr3, j);
			}
			else{
				qrsP.DATA_HIGH[j % 32] = qrsP.DATA_LOW[j % 32];
			}
			printf("high: %i - ", arr3[j % 32]); //print

			if(i > 2){
				qrsP.DATA_DER[j % 32] = derivativeFilter(arr3, arr4, j);
			}
			else{
				qrsP.DATA_DER[j % 32] = qrsP.DATA_HIGH[j % 32];
			}
			printf("der: %i - ", arr4[j % 32]); //print

			if(i > 3){
				qrsP.DATA_SQR[j % 32] = squaringFilter(arr4, arr5, j);
			}
			else{
				qrsP.DATA_SQR[j % 32] = qrsP.DATA_DER[j % 32];
			}
			printf("sqr: %i", arr5[j % 32]); //print */
			printf("\n");
		}
 	}

 	//for(int i = 0; i < 32; i++){
 	//	printf("%i \n", arr[(i )% 32]);
 	//}

    return 0;
}
