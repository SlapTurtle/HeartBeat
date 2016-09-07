#include "sensor.h"
#include "filters.h"
#include "qsr.h"
#include <stdio.h>
#include <stdlib.h>

// Main function for organising the program execution.
// The functions and object predefined are just for inspiration.
// Please change order,names arguments to fit your solution.

int main()
{
    QRS_params qsr_params;       // Instance of the made available through: #include "qsr.h"
 FILE *file;                  // Pointer to a file object
 file = openfile("ECG.txt");

 int i,j;

    i = getNextData(file);          // Read Data from Sensor
    j = getNextData(file);

    printf("%i\n", i);
    printf("%i\n", j);

    /*
    lowPassFilter();            // Filter Data

    peakDetection(&qsr_params); // Perform Peak Detection

    closefile(file);
    */

 return 0;
}
