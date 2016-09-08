#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>

static int getNextData(FILE *file);
static FILE* openfile(const char* filename);
static void closefile(FILE *file);

int getNextData(FILE *file) {
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
