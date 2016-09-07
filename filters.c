#include "filters.h"
#include "qsr.h"

static int dataBreach = 0;
static int pX = 0;
static int pY = 0;

int lowPassFilter(int input, QRS_params qrsP) {

	/* qrsP.f1_x[pX] = input;
	int temp = 0;

	temp = 2 * qrsP.f1_y[(pY) % 2] - qrsP.f1_y[(pY-1) % 2] + (1/32) * (qrsP.f1_x[pX % 12]-2*qrsP.f1_x[(pX-5) % 12]+qrsP.f1_x[(pX-11) % 12]);
	qrsP.f1_y[pY % 2] = temp;

	pY++;
	if(pY == 2){
		pY = 0;
	}

	pX++;
	if(pX == 12){
		pX = 0;
	}



	return temp; */

}
