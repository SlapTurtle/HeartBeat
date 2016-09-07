#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

// Main function for organising the program execution.
// The functions and object predefined are just for inspiration.
// Please change order,names arguments to fit your solution.

int main() {
    QRS_params qrsP;       		// Instance of the made available through: #include "qsr.h"
    FILE *file;                 // Pointer to a file object
 	file = openfile("ECG.txt");

 	int i;
 	for(int j = 0; j<11; j++) {
 		qrsP.f1_x[j] = 0;
 	}
 	for(int j = 0; j<2; j++) {
 	 		qrsP.f1_y[j] = 0;
 	}


 	for(int j = 0; j < 24; j++){
 		printf("ID: %i - ", j);
 		i = getNextData(file);          // Read Data from Sensor
 		printf("in: %i - ", i);
 		i = lowPassFilter(i, qrsP);
 		printf("low: %i \n", i);
 	}
             // Filter Data

    /*peakDetection(&qsr_params); // Perform Peak Detection

    closefile(file);
    */

    return 0;
}
