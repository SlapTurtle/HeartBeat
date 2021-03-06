#define RAWC 13
#define LOWC 33
#define HIGHC 5
#define DERC 1
#define SQRC 31
#define MWIC 3
#define RPEAKC 8
#define PEAKC 250

#ifndef QSR_H
#define QSR_H

typedef struct QRS_params
{
   double SPKF;
   double NPKF; 
   double THRESHOLD1;
   double THRESHOLD2;
   double RR_AVERAGE1;
   double RR_AVERAGE2;
   double RR_LOW;
   double RR_HIGH;
   double RR_MISS;
   int LAST_RPEAK;
   const int RAWCycle;
   const int LOWCycle;
   const int HIGHCycle;
   const int DERCycle;
   const int SQRCycle;
   const int MWICycle;
   const int RPEAKCycle;
   const int PeakCyle;
   int Index_Raw;
   int Index_Low;
   int Index_High;
   int Index_Der;
   int Index_Sqr;
   int Index_Mwi;
   int Index_RPeak;
   int Index_RPeak_OK;
   int DATA_RAW[RAWC];
   int DATA_LOW[LOWC];
   int DATA_HIGH[HIGHC];
   int DATA_DER[DERC];
   int DATA_SQR[SQRC];
   int DATA_MWI[MWIC];
   int DATA_PEAKS[PEAKC];
   int DATA_PEAKSTIME[PEAKC];
   int DATA_RPEAKS[PEAKC];
   int DATA_RPEAKS_OK[PEAKC];
   int DATA_TIMEMS;
   int dead_timems;
   int peakcount;
   int RRmissed;
   
} QRS_params;

void peakDetection(QRS_params *params);
void setValues1(QRS_params *params, int c);
void setValues2(QRS_params *params, int c);
void output(QRS_params *params, int c);
int isPeak(QRS_params *params, int index);
double SPKF(QRS_params *params, int c);
double SPKF2(QRS_params *params, int c);
double NPKF(QRS_params *params, int c);
double THRESHOLD1(QRS_params *params);
double THRESHOLD2(QRS_params *params);
char *appendSpaces(char in[], int max);

#endif //QSR_H
