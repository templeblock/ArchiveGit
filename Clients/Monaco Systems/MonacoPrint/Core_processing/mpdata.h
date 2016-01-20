#ifndef MP_DATA_H
#define MP_DATA_H

#include "csprofile.h"

#define MONACO_CREATED_ICC	(0)
#define	MONACOP_CREATED_ICC	(1)
#define MONACO_EDITED_ICC	(2)
#define MONACOP_EDITED_ICC	(2)
#define	NOT_MONACO_ICC		(3)



// flags to be used with the flag variable

#define DIFFERENT_LINEAR	(1)


typedef struct PatchFormat{
	unsigned long 					patchType;
	unsigned long 					numComp;			
	unsigned long 					numCube;
	unsigned long 					numLinear;				
	unsigned long					*steps;	
	unsigned long					*blacks;					
	unsigned long					*cmy;						
	double							*linear;					
	double							*data;					
	double							*ldata;	
	char							name[100];				
	
} PatchFormat;


typedef struct TweakElement {
double 	lab_d[3];
double 	lab_g[3];
double	lab_p[3];
double	Lr,Cr;
unsigned long	type;
char	name[40];
} TweakElement;


typedef struct SettingFormat {
	unsigned long 					tableType;
	unsigned long 					separationType;			
	unsigned long 					blackType;
	unsigned long 					totalInkLimit;				
	unsigned long 					blackInkLimit;				
	unsigned long 					saturation;		
	unsigned long 					colorBalance;
	unsigned long 					colorBalanceCut;
	unsigned long 					linear;
	unsigned long 					inkLinearize;
	unsigned long 					inkNeutralize;
	unsigned long 					inkHighlight;
	unsigned long 					simulate;
	unsigned long 					smooth;
	unsigned long 					revTableSize;
	unsigned long 					smoothSimu;
	unsigned long 					kOnly;
	double							blackCurve[101];
	unsigned long					numBlackHand;
	double							blackX[31];
	double							blackY[31];
	unsigned long					numLinearHand[6];
	double							linearX[MAX_LINEAR_HAND*6];
	double							linearY[MAX_LINEAR_HAND*6];
	char							linearDesc[100];
	unsigned long					numToneHand[6];
	double							toneX[MAX_TONE_HAND*6];
	double							toneY[MAX_TONE_HAND*6];
	char							toneDesc[100];
	unsigned long					flag;
	unsigned long					reserved[99];
} SettingFormat;


typedef struct PrintData{
	PatchFormat	srcpatch;
	PatchFormat	destpatch;
	SettingFormat	setting;
	unsigned long	numlinear;		//can not be larger than 6
	unsigned long	sizelinear;
	double			*linear[6];
	unsigned long	numtone;	//can not be larger than 6
	unsigned long 	sizetone;
	double			*tone[6];
	unsigned long	numTweaks;
	TweakElement	*tweakele;
	Ctype			A2B0;	
	McoHandle		tableH;
	unsigned long	icctype;	//monaco icc or not
	unsigned long	bitsize;	//8 or 16 bit
} PrintData;

#endif
