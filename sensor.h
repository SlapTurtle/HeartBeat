#include <stdio.h>

#ifndef SENSOR_H
#define SENSOR_H

int getNextData(FILE *file);
FILE* openFile(const char* filename);
void closeFile(FILE *file);

#endif //SENSOR_H
