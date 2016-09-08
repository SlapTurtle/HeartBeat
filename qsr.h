#ifndef QSR_H
#define QSR_H

typedef struct QRS_params
{
   double SPKF;
   double NPKF; 
   double THRESHOLD1;
   double THRESHOLD2;
   const int RAWCycle;
   const int LOWCycle;
   const int HIGHCycle;
   const int DERCycle;
   const int SQRCycle;
   int DATA_RAW[13];
   int DATA_LOW[33];
   int DATA_HIGH[5];
   int DATA_DER[1];
   int DATA_SQR[101];
   
} QRS_params;

void peakDetection(QRS_params *params);

#endif // QSR_H
