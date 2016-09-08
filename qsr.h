#ifndef QSR_H
#define QSR_H

// Header file for QRS functionality 
// it is recommended to use the structure "QRS_parameters"
// to organize all variables and parameters

typedef struct QRS_params
{ // Structure for QRS parameters
   double SPKF;
   double NPKF; 
   double THRESHOLD1;
   double THRESHOLD2;
   const int LOWCycle;
   const int HIGHCycle;
   const int DERCycle;
   int DATA_RAW[32];
   int DATA_LOW[32];
   int DATA_HIGH[32];
   int DATA_DER[32];
   int DATA_SQR[32];
   
} QRS_params;

// Feel free to change and add methods
void peakDetection(QRS_params *params);

#endif // QSR_H
