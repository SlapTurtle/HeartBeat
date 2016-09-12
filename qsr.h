#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWI 9999
#define RR 8

#ifndef QSR_H
#define QSR_H

typedef struct QRS_params
{
   double SPKF;
   double NPKF; 
   double THRESHOLD1;
   double THRESHOLD2;
   int RR_AVERAGE1[RR];
   int RR_AVERAGE2[RR];
   const int RAWCycle;
   const int LOWCycle;
   const int HIGHCycle;
   const int DERCycle;
   const int SQRCycle;
   int DATA_RAW[RAWC];
   int DATA_LOW[LOWC];
   int DATA_HIGH[HIGHC];
   int DATA_DER[DERC];
   int DATA_SQR[SQRC];
   int DATA_MWI[MWI];
   int DATA_PEAKS[MWI];
   int DATA_PEAKSTIME[MWI];
   int DATA_TIMEMS;
   int peakcount;
   
} QRS_params;

void peakDetection(QRS_params *params);
int isPeak(QRS_params *params, int index);
double SPKF(QRS_params *params);

#endif //QSR_H
