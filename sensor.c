#include "sensor.h"

int getNextData(FILE *file) {
	int input;
	fscanf(file, "%i", &input);
	return input;
}

FILE* openFile(const char* filename) {
   return fopen(filename,"r");
}

void closeFile(FILE *file){
	fclose(file);
}
