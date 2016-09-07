#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>

int getNextData(FILE *file)
{
 int input;
 fscanf(file, "%i", &input);
 return input;
}

FILE* openfile(const char* filename) {
   FILE *file = fopen("ECG.txt","r");

   return file;
}

void closefile(FILE *file){
 fclose(file);
}
