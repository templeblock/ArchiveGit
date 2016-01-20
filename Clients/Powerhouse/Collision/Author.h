#ifndef _author_h
#define _author_h

const int MaxHeadDesc = 40;
const int MaxVer = 20;
const int MaxLastMod = 20;

typedef struct tagExcelOrig
{
	char szNewName[80];
	char szOldName[40];
	char szF3[20];
	char szDescr[120];
	char szF5[30];
	char szF6[15];
	char szF7[15];
	char szF8[15];
	char szF9[15];
	char szF10[15];
	char szF11[15];
	long lStartFrame;
	long lEndFrame;
} EXCEL_ORIG;

#endif
