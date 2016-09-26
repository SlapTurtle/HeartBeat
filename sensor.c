#include "sensor.h"
#include "limits.h"

int getNextData(FILE *file) {
	int input;
	if(fscanf(file, "%i", &input) != EOF){
		return input;
	}
	return INT_MAX; //potential failure due to input being equal to INT_MAX
}

FILE* openFile(const char* filename) {
   return fopen(filename,"r");
}

void closeFile(FILE *file){
	fclose(file);
}
