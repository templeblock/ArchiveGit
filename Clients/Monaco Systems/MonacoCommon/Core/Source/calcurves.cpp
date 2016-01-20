//////////////////////////////////////////////////////////////////////////////////////////
//
// calcurves.cpp
//
// build tone and linerization curves given previous curves and new curves
//
// Sept 26, 1997
// James Vogh
//////////////////////////////////////////////////////////////////////////////////////////

#include "calcurves.h"
#include "mcotypes.h"
#include "spline.h"
#include "splint.h"
#include "safe_inter.h"
#include "picelin.h"
#include "splinesegment.h"
#include "assort.h"
//#include "errordial.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

CalCurves::CalCurves(PrintData *pd)
{
numBits = pd->bitsize;
A2B0 = &pd->A2B0;
numLinearHand = pd->setting.numLinearHand;
linearX = pd->setting.linearX;
linearY = pd->setting.linearY;
numToneHand = pd->setting.numToneHand;
toneX = pd->setting.toneX;
toneY = pd->setting.toneY;

numlinear = pd->numlinear;
sizelinear = pd->sizelinear;
linear = pd->linear;
numtone = pd->numtone;
sizetone = pd->sizetone;
tone = pd->tone;

linearname = pd->setting.linearDesc;

tonename = pd->setting.toneDesc;

linearDate = &pd->setting.linearDate;

}

// build the cmyk input curve based on curve data
McoStatus CalCurves::buildToneCurve(void)
{
double *y2;
int i,j;
int16 *curve18,*inverscurve;
int16 *linearDot;
double *linearp;
double x,out,out2;
double	max;
double  *orig;
int32	index;
unsigned short	*s_out;
unsigned char	*c_out;
double *y2_2 = 0L,*tone2X = 0L;

// build the tone curve
y2 =  new double [MAX_TONE_HAND];


// see if the number of entries is enough



if (numBits == 8)
	{
	for (i=0; i<numtone; i++)
		{
		//sinterp_init(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],numToneHand[i],1e30,1e30,y2);
		orig = tone[i];
		c_out = (unsigned char*)A2B0->input_tables[i];
		for (j=0; j<A2B0->input_entries; j++)
			{
			x = 100.0*(double)j/((double)A2B0->input_entries-1.0);
			//sinterp_eval(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],y2,numToneHand[i],x,&out);
			SplineMultiSegment(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],numToneHand[i],x,&out);
			index = ((sizetone-1)*out/100.0)+0.5;
			if (index >= sizetone) index = sizetone-1;
			c_out[j] = orig[index];
			}
		}
	}
else if (numBits == 16)
	{
	if (A2B0->input_entries < 4096)
		{
		A2B0->input_entries = 4096;
		for (i=0; i<numtone; i++)
			{
			if (A2B0->input_tables[i]) delete A2B0->input_tables[i];
			A2B0->input_tables[i] = (char*)new short[4096];
			}
		}
	tone2X = new double[sizetone];
	y2_2 = new double[sizetone];
	for (i=0; i<sizetone; i++) tone2X[i] = 100*(double)i/((double)sizetone-1);
		
	for (i=0; i<numtone; i++)
		{
		//sinterp_init(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],numToneHand[i],1e30,1e30,y2);
		sinterp_init(tone2X,tone[i],sizetone,1e30,1e30,y2_2);
		s_out = (unsigned short*)A2B0->input_tables[i];
		for (j=0; j<A2B0->input_entries; j++)
			{
			x = 100.0*(double)j/((double)A2B0->input_entries-1.0);
			//sinterp_eval(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],y2,numToneHand[i],x,&out);
			SplineMultiSegment(&toneX[i*MAX_TONE_HAND],&toneY[i*MAX_TONE_HAND],numToneHand[i],x,&out);
			sinterp_eval(tone2X,tone[i],y2_2,sizetone,out,&out2);
			
			s_out[j] = out2;
			}
		}
	delete tone2X;
	delete y2_2;
	}
delete y2;	
	
return MCO_SUCCESS;
}


// convert to percent dot
void CalCurves::_convertPercentDot(double *in, double *out,int n)
{
int i;
double	z,Tt,Ts;

z = in[0];
Ts = pow(10,(-in[n-1]+z));
for (i=0; i<n; i++)
	{
	 Tt = pow(10,(-in[i]+z));
	 out[i] = 100*(1-Tt)/(1-Ts);
	}	 
}

// convert to percent
void CalCurves::_convertPercent(double *in, double *out,int n)
{
int i;
double	max;

max = in[n-1];
for (i=0; i<n; i++)
	{
	out[i] = 100*in[i]/max;
	}	 
}

// convert precent dot to density
void CalCurves::_convertPercentDotToDensity(double *in, double *out,double solid,int n)
{
int i;
double	Tt,Ts;


Ts = pow(10,(-solid));
for (i=0; i<n; i++)
	{
	out[i] = 2-log10(100+(-1+Ts)*in[i]);
	}	 
}

// data for 18% dot curve
double data18Per[] = {  
0.049988,
0.064514,
0.0793,
0.09433,
0.109619,
0.125168,
0.141006,
0.15715,
0.173584,
0.190323,
0.207397,
0.224808,
0.242554,
0.260681,
0.27919,
0.298096,
0.317398,
0.337143,
0.357346,
0.378021,
0.39917,
0.420853,
0.443069,
0.465866,
0.489258,
0.513275,
0.537949,
0.563339,
0.589462,
0.616364,
0.644104,
0.672714,
0.702255,
0.732803,
0.764404,
0.797134,
0.83107,
0.866318,
0.902939,
0.941071,
0.980789,
1.022263,
1.065598,
1.110962,
1.158508,
1.208435,
1.26091,
1.316116,
1.374268,
1.435516,
1.5
};



McoStatus CalCurves::_build18PerCurve(double *curve)
{
int i;
double x[51];
double a;

for (i=0; i<51; i++) x[i] = i*2;

for (i=0; i< (1<<numBits); i++)
	{
	piceLin_0(x,data18Per,0L,51,100*(double)i/((1<<numBits)-1),&a);
	curve[i] = a;
	}

return MCO_SUCCESS;
}

McoStatus CalCurves::_build18PerCurve(double *curve,int n)
{
int i;
double x[51];
double a;

for (i=0; i<51; i++) x[i] = i*2;

for (i=0; i< n; i++)
	{
	piceLin_0(x,data18Per,0L,51,100*(double)i/(n-1),&a);
	curve[i] = a;
	}

return MCO_SUCCESS;
}

// clean up any reversals that might be in the data before the inverse is calculated
McoStatus CalCurves::cleanReversals(double *curve, double *values,int32 *size)
{
int i,j,count;
int reverse = 0;
double kernal[5];
double kernal_sum = 0;
double dist;
double *curve_temp = 0L;
McoStatus state = MCO_SUCCESS;
// first search for reversals



for (i=1; i<*size; i++)
	{
	if (curve[i] <= curve[i-1]) reverse++;
	}
if (reverse == 0) return MCO_SUCCESS;

// try smoothing
curve_temp = new double[*size];


count = 0;
while ((reverse > 0) && (count < 8))
	{
	for (i=0; i<*size; i++) curve_temp[i] = curve[i];
	for (i=1; i<*size-1; i++) if ((curve[i] <= curve[i-1]) || (curve[i] >= curve[i+1]))
		{
		kernal_sum = 0;
		
		if (i>1)
			{
			dist = values[i]-values[i-2];
			kernal[0] = 1-exp(-(dist*dist+0.03)/225);
			kernal_sum += kernal[0];
			}			

		dist = values[i]-values[i-1];
		kernal[1] = 1-exp(-(dist*dist+0.03)/225);
		kernal_sum += kernal[1];

		kernal[2] = 1;
		kernal_sum++;

		dist = values[i]-values[i+1];
		kernal[3] = 1-exp(-(dist*dist+0.03)/225);
		kernal_sum += kernal[3];
		
		if (i+2<*size)
			{
			dist = values[i]-values[i+2];
			kernal[4] = 1-exp(-(dist*dist+0.03)/225);
			kernal_sum += kernal[4];
			}
		
		for (j=0; j<5; j++) kernal[j] = kernal[j]/kernal_sum;
		curve_temp[i] = 0;
		if (i-2>0) curve_temp[i] += curve[i-2]*kernal[0];
		if (i-1>0) curve_temp[i] += curve[i-1]*kernal[1];
		curve_temp[i] += curve[i]*kernal[2];
		if (i+1<*size) curve_temp[i] += curve[i+1]*kernal[3];
		if (i+2<*size) curve_temp[i] += curve[i+2]*kernal[4];
		}
		
	reverse = 0;
	for (i=1; i<*size; i++)
		{
		if (curve_temp[i] <= curve_temp[i-1]) reverse++;
		}
	for (i=0; i<*size; i++) curve[i] = curve_temp[i];
	if (reverse == 0) 
		{
		state = MCO_SUCCESS;
		goto bail;
		}
	count++;
	}
// there is still a reversal
// will now go ahead and delete points

for (i=1; i<*size-1; i++)
	{
	while (curve[i] <= curve[i-1])
		{
		for (j=i-1; j<*size-1; j++) 
			{
			curve[j] = curve[j+1];
			values[j] = values[j+1];
			}
		(*size)--;
		}
	}
for (i=*size-2; i>0; i--)
	{
	if (curve[i] >= curve[i+1])
		{
		for (j=i; j<*size-1; j++) 
			{
			curve[j] = curve[j+1];
			values[j] = values[j+1];
			}
		curve[*size-1] = curve[i]+1;
		(*size)--;
		}
	}

bail:
if (curve_temp != 0L) delete curve_temp;
return state;
}

// restore the linear data because it has been deleted
McoStatus CalCurves::restoreLinear(void)
{
int i,j;
unsigned short	*s_out;
unsigned char	*c_out;


A2B0->output_entries = sizelinear;

if (numBits == 8)
	{
	for (i=0; i<numlinear; i++)
		{
		c_out = (unsigned char*)A2B0->output_tables[i];
		for (j=0; j<sizelinear; j++)
			{
			c_out[j] = linear[i][j];
			}
		}
	}
else 
	{
	for (i=0; i<numlinear; i++)
		{
		s_out = (unsigned short*)A2B0->output_tables[i];
		for (j=0; j<sizelinear; j++)
			{
			s_out[j] = linear[i][j];
			}
		}
	}	

return MCO_SUCCESS;
}


// check to make sure that either all linear data is present or alll is not present
short CalCurves::checkLinearData(void)
{
int i,f = 0;

for (i=0; i<numlinear; i++) if (numLinearHand[i] < 2) f = 1;

if (!f) return 0;

for (i=0; i<numlinear; i++) if (numLinearHand[i] > 1)
	{
	return 1;
	}
return 0;
}


McoStatus CalCurves::buildLinearCurve(int monaco)
{
int i,j;
int32 *inverscurve;
double *linearDot;
double *linearX2;
long	linnum[4];
double *linearp;
double *y2;
double x,out,out2;
double	max;
double  *orig;
unsigned short	*s_out;
unsigned char	*c_out;
double *y2_2 = 0L,*linear2X = 0L;
double	*curve18;

for (i=0; i<numlinear; i++) if (numLinearHand[i] < 2) return restoreLinear();

// build the 18% curve
curve18 = new double[1<<numBits];
if (!curve18) return MCO_MEM_ALLOC_ERROR;
_build18PerCurve(curve18);
// convert to %dot
_convertPercentDot(curve18,curve18,1<<numBits);


linearDot = new double [numlinear*MAX_LINEAR_HAND];
linearX2 = new double [numlinear*MAX_LINEAR_HAND];



for (i=0; i<numlinear; i++)
	{
	linearp = &linearY[i*MAX_LINEAR_HAND];
	for (j=0; j<numLinearHand[i]; j++)
		{
		linearDot[j+i*MAX_LINEAR_HAND] = linearp[j];
		linearX2[j+i*MAX_LINEAR_HAND]= linearX[i*MAX_LINEAR_HAND+j];
		}
	linnum[i] = numLinearHand[i];
	// remove reversals
	cleanReversals(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],&linnum[i]);
	max = linearp[linnum[i]-1];
	_convertPercentDot(&linearDot[i*MAX_LINEAR_HAND],&linearDot[i*MAX_LINEAR_HAND],linnum[i]);
	}
	



// build the inverse curve
y2 =  new double [MAX_LINEAR_HAND];

if (!monaco)
	{
	if (numBits == 8)
		{
		for (i=0; i<numlinear; i++)
			{
			sinterp_init(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],linnum[i],1e30,1e30,y2);
			orig = linear[i];
			c_out = (unsigned char*)A2B0->output_tables[i];
			for (j=0; j<sizelinear; j++)
				{
				x = curve18[(long)(orig[j]+0.5)];
				sinterp_eval(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],y2,linnum[i],x,&out);
				c_out[j] = ((1<<numBits)-1)*out*0.01;
				}
			}
		}
	else if (numBits == 16)
		{
		if (A2B0->output_entries < 4096)
			{
			A2B0->output_entries = 4096;
			for (i=0; i<numlinear; i++)
				{
				if (A2B0->output_tables[i]) delete A2B0->output_tables[i];
				A2B0->output_tables[i] = (char*)new short[4096];
				}
			}
		linear2X = new double[sizelinear];
		y2_2 = new double[sizelinear];
		for (i=0; i<sizelinear; i++) linear2X[i] = 100*(double)i/((double)sizelinear-1.0);	
				
		for (i=0; i<numlinear; i++)
			{
			sinterp_init(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],linnum[i],1e30,1e30,y2);
			sinterp_init(linear2X,linear[i],sizelinear,1e30,1e30,y2_2);
			orig = linear[i];
			s_out = (unsigned short*)A2B0->output_tables[i];
			for (j=0; j<A2B0->output_entries; j++)
				{
				x = 100.0*(double)j/((double)A2B0->output_entries-1.0);
				sinterp_eval(linear2X,linear[i],y2_2,sizelinear,x,&out);
				out = curve18[(long)out];
				sinterp_eval(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],y2,linnum[i],out,&out2);
				s_out[j] = ((1<<numBits)-1)*out2*0.01;
				}
			}
		delete linear2X;
		delete y2_2;
		}
	}
else 	// build the curve without using the backup because this is a monaco file
	{	
	if (numBits == 8)
		{
		for (i=0; i<numlinear; i++)
			{	
			sinterp_init(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],linnum[i],1e30,1e30,y2);
			c_out = (unsigned char*)A2B0->output_tables[i];
			for (j=0; j<sizelinear; j++)
				{
				x = 100*(double)j/(sizelinear-1);
				sinterp_eval(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],y2,linnum[i],x,&out);
				c_out[j] = ((1<<numBits)-1)*out*0.01;
				}
			}
		}
	else if (numBits == 16)
		{
		
		if (A2B0->output_entries < 4096)
			{
			A2B0->output_entries = 4096;
			for (i=0; i<numlinear; i++)
				{
				if (A2B0->output_tables[i]) delete A2B0->output_tables[i];
				A2B0->output_tables[i] = (char*)new short[4096];
				}
			}	
		
		for (i=0; i<numlinear; i++)
			{
			sinterp_init(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],linnum[i],1e30,1e30,y2);
			s_out = (unsigned short*)A2B0->output_tables[i];
			for (j=0; j<A2B0->output_entries; j++)
				{
				x = 100*(double)j/(A2B0->output_entries-1);
				sinterp_eval(&linearDot[i*MAX_LINEAR_HAND],&linearX2[i*MAX_LINEAR_HAND],y2,linnum[i],x,&out);
				s_out[j] = ((1<<numBits)-1)*out*0.01;
				}
			}
		}
	}
	
		
delete y2;
delete linearDot;
delete linearX2;
delete curve18;	
	
return MCO_SUCCESS;
}

// the next two functions create the initial backup luts

// create a backup of the luts if the icc is not a Monaco file
McoStatus CalCurves::backupLuts(void)
{
int i,j;
unsigned short	*s_out;
unsigned char	*c_out;


if (numBits == 8)
	{
	for (i=0; i<A2B0->output_channels; i++)
		{
		c_out = (unsigned char*)A2B0->output_tables[i];
		for (j=0; j<A2B0->output_entries; j++)
			{
			linear[i][j] = c_out[j];
			}
		}
	for (i=0; i<A2B0->input_channels; i++)
		{
		c_out = (unsigned char*)A2B0->input_tables[i];
		for (j=0; j<A2B0->input_entries; j++)
			{
			tone[i][j] = c_out[j];
			}
		}
	}
else 
	{
	for (i=0; i<A2B0->output_channels; i++)
		{
		s_out = (unsigned short*)A2B0->output_tables[i];
		for (j=0; j<A2B0->output_entries; j++)
			{
			linear[i][j] = s_out[j];
			}
		}
	for (i=0; i<A2B0->input_channels; i++)
		{
		s_out = (unsigned short*)A2B0->input_tables[i];
		for (j=0; j<A2B0->input_entries; j++)
			{
			tone[i][j] = s_out[j];
			}
		}
	}		
return MCO_SUCCESS;	
}

// create a backup of the luts if the icc is not a Monaco file
McoStatus CalCurves::copyBackUps(void)
{
int i,j;
unsigned short	*s_out;
unsigned char	*c_out;


A2B0->input_entries = sizetone;
A2B0->output_entries = sizelinear;

if (numBits == 8)
	{
	for (i=0; i<numlinear; i++)
		{
		c_out = (unsigned char*)A2B0->output_tables[i];
		for (j=0; j<sizelinear; j++)
			{
			c_out[j] = linear[i][j];
			}
		}
	for (i=0; i<numtone; i++)
		{
		c_out = (unsigned char*)A2B0->input_tables[i];
		for (j=0; j<sizetone; j++)
			{
			c_out[j] = tone[i][j];
			}
		}
	}
else 
	{
	for (i=0; i<numlinear; i++)
		{
		s_out = (unsigned short*)A2B0->output_tables[i];
		for (j=0; j<sizelinear; j++)
			{
			s_out[j] = linear[i][j];
			}
		}
	for (i=0; i<numtone; i++)
		{
		s_out = (unsigned short*)A2B0->input_tables[i];
		for (j=0; j<sizetone; j++)
			{
			s_out[j] = tone[i][j];
			}
		}
	}		
	
	
return MCO_SUCCESS;	
}


// the 4D table is not based on linearized patch data, create a backup set 
// that will work on a linearized printer
McoStatus CalCurves::createBackupLuts(void)
{
int i,j;
int32 *inverscurve;
double *linearp;
double *x18,y2_18[51],*y2;
double x,out,out2;
double	max;
double  *orig;
unsigned short	*s_out;
unsigned char	*c_out;
double *curve18;

curve18 = new double[sizelinear];
if (!curve18) return MCO_MEM_ALLOC_ERROR;

y2 =  new double [sizelinear];
if (!y2) return MCO_MEM_ALLOC_ERROR;
x18 = new double[sizelinear];
if (!x18) return MCO_MEM_ALLOC_ERROR;

// build the inverse curve (which is an 18% curve)
for (i=0; i<sizelinear; i++) x18[i] = 100.0*(double)i/(sizelinear-1);


// create the linear luts
//  just put the inverse of the 18% curve here,
// the output curve and linerization curve combine to produce a linear curve
for (i=0; i<numlinear; i++)
	{
	_build18PerCurve(curve18,sizelinear);
	_convertPercentDot(curve18,curve18,sizelinear);
	_convertPercentDotToDensity(curve18,curve18,linearY[i*MAX_LINEAR_HAND+numLinearHand[i]-1],sizelinear);
		
	sinterp_init(curve18,x18,sizelinear,1e30,1e30,y2);
	for (j=0; j<sizelinear; j++)
		{
		x = linearY[i*MAX_LINEAR_HAND+numLinearHand[i]-1]*(double)j/(sizelinear-1);
		sinterp_eval(curve18,x18,y2,sizelinear,x,&out);
		linear[i][j] = 2.55*out;
		}
	}

// create the linear tone curve

if (numBits == 8)
	{
	for (i=0; i<numtone; i++)
		{
		c_out = (unsigned char*)A2B0->input_tables[i];
		for (j=0; j<sizetone; j++)
			{
			tone[i][j] = c_out[j];
			}
		}
	}
else 
	{
	for (i=0; i<numtone; i++)
		{
		s_out = (unsigned short*)A2B0->input_tables[i];
		for (j=0; j<sizetone; j++)
			{
			tone[i][j] = s_out[j];
			}
		}
	}			

if (y2) delete y2;
if (curve18) delete curve18;
if (x18) delete x18;
	
return MCO_SUCCESS;
}


// curve import and export functions


McoStatus CalCurves::importTone(char *fname)
{
int 				n,i,j,l;
FILE 				*fs;
char				junk[255];
int					numt = -1;

fs = fopen( (char*)fname, "r" );
if(!fs)
	return MCO_FAILURE;
	
fgets(junk,50,fs);
n = strcmp2(junk,"DryJet Tone Curve\n");
if (n) return MCO_FILE_DATA_ERROR;

fscanf(fs,"%d\n",&numt);

if (numt != numtone) {fclose(fs); return MCO_FAILURE;}
	
for (i=0; i<numtone; i++)
	{
	fscanf(fs,"%d\n",&numToneHand[i]);
	if (numToneHand[i] > MAX_TONE_HAND) {fclose(fs); return MCO_FAILURE;}
	for (j=0; j<numToneHand[i]; j++)
		{
		fscanf(fs,"%lf %lf\n",&toneX[i*MAX_TONE_HAND+j],&toneY[i*MAX_TONE_HAND+j]);
		}
	}
fclose(fs);
return MCO_SUCCESS;
}

McoStatus CalCurves::exportTone(char *fname)
{
int i,j;
FILE 				*fs;

fs = fopen( (char*)fname, "w" );
if(!fs)
	return MCO_FAILURE;
	
fprintf(fs,"DryJet Tone Curve\n");

fprintf(fs,"%d\n",numtone);	
	
for (i=0; i<numtone; i++)
	{
	fprintf(fs,"%d\n",numToneHand[i]);
	for (j=0; j<numToneHand[i]; j++)
		{
		fprintf(fs,"%f %f\n",toneX[i*MAX_TONE_HAND+j],toneY[i*MAX_TONE_HAND+j]);
		}
	}
fclose(fs);
return MCO_SUCCESS;
}

McoStatus CalCurves::importLinear(char *fname)
{
int 				n,i,j,l;
FILE 				*fs;
char				junk[255];
int					numl = -1;

fs = fopen( (char*)fname, "r" );
if(!fs)
	return MCO_FAILURE;
	
fgets(junk,50,fs);
n =strcmp(junk,"DryJet Linearization\n");
if (n) return MCO_FILE_DATA_ERROR;


fscanf(fs,"%d\n",&numl);

if (numl != numlinear) {fclose(fs); return MCO_FAILURE;}
	
for (i=0; i<numlinear; i++)
	{
	fscanf(fs,"%d\n",&numLinearHand[i]);
	if (numLinearHand[i] > MAX_LINEAR_HAND) {fclose(fs); return MCO_FAILURE;}
	for (j=0; j<numLinearHand[i]; j++)
		{
		fscanf(fs,"%lf %lf\n",&linearX[i*MAX_LINEAR_HAND+j],&linearY[i*MAX_LINEAR_HAND+j]);
		if (j == 0) linearY[i*MAX_TONE_HAND+j] = 0.0;
		}
	}
fclose(fs);

return MCO_SUCCESS;
}

McoStatus CalCurves::exportLinear(char *fname)
{
FILE 				*fs;
int i,j;

fs = fopen( (char*)fname, "w" );
if(!fs)
	return MCO_FAILURE;
	
fprintf(fs,"DryJet Linearization\n");
fprintf(fs,"%d\n",numlinear);	
	
for (i=0; i<numlinear; i++)
	{
	fprintf(fs,"%d\n",numLinearHand[i]);
	for (j=0; j<numLinearHand[i]; j++)
		{
		fprintf(fs,"%f %f\n",linearX[i*MAX_LINEAR_HAND+j],linearY[i*MAX_LINEAR_HAND+j]);
		}
	}
fclose(fs);
return MCO_SUCCESS;
}


