// linearize data
// take a set of c,m,y,k curves and linearize c,m,y,k values for these curves
// 
// James Vogh
// Copyright 1997 Monaco Systems

#include "linearize_data.h"
#include "spline.h"
#include "splint.h"
#include "picelin.h"
#include <math.h>

LinearData::LinearData(void)
{
numc = 0;
sizes = 0L;
curves = 0L;
values = 0L;
y2 = 0L;
y3 = 0L;
}

LinearData::~LinearData(void)
{
CleanUp();
}

void LinearData::CleanUp(void)
{
int i;

if (sizes != 0L) delete sizes;
sizes = 0L;
if (curves != 0L) 
	{
	for (i=0; i<numc; i++) if (curves[i] != 0L) delete curves[i];
	delete curves;
	curves = 0L;
	}
if (values != 0L) 
	{
	for (i=0; i<numc; i++) if (values[i] != 0L) delete values[i];
	delete values;
	values = 0L;
	}

if (y2 != 0L) 
	{
	for (i=0; i<numc; i++) if (y2[i] != 0L) delete y2[i];
	delete y2;
	y2 = 0L;
	}
if (y3 != 0L) 
	{
	for (i=0; i<numc; i++) if (y3[i] != 0L) delete y3[i];
	delete y3;
	y3 = 0L;
	}
}

// remove reversals in the curve data
McoStatus LinearData::cleanReversals(double *curve, double *values,int32 *size)
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
	while ((curve[i] <= curve[i-1]) && (i <*size))
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

// use splines to linearize tables
McoStatus LinearData::setUp(double *data,double *vals, double *paper,int nc,int s)
{

int i,j;
double t,t2;
McoStatus state = MCO_SUCCESS;
int		LAB = 0;

CleanUp();

numc = nc;

sizes = new int32[numc];
curves = new double*[numc];
values = new double*[numc];
y2 = new double*[numc];
y3 = new double*[numc];
if ((sizes == 0L) || (curves == 0L) || (values == 0L) || (y2 == 0L) || (y3 == 0L)) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<numc; i++) 
	{
	sizes[i] = s;
	curves[i] = new double[s];
	values[i] = new double[s];
	y2[i] = new double[s];
	y3[i] = new double[s];
	if ((sizes[i] == 0L) || (curves[i] == 0L) || (values[i] == 0L) || (y2[i] == 0L) || (y3[i] == 0L)) return MCO_MEM_ALLOC_ERROR;
	for (j=0; j<s; j++)
		{
		values[i][j] = vals[j];
		}
	}

// determine if data is in density format
// must have paper density subtracted
for (i=0; i<numc; i++)
	{
	for (j=0; j<sizes[i]-1; j++)
		{
		if (fabs(data[i*(s-1)*3+j*3+1]) > 0.01) LAB = 1;
		if (fabs(data[i*(s-1)*3+j*3+2]) > 0.01) LAB = 1;
		}
	}

// build the curves
for (i=0; i<numc; i++)
	{
	curves[i][0] = 0;	// 0%
	for (j=0; j<sizes[i]-1; j++)
		{
		if (LAB)
			{
			t = data[i*(s-1)*3+j*3]-paper[0];	
			t2 = t*t;
			t = data[i*(s-1)*3+j*3+1]-paper[1];	
			t2 += t*t;
			t = data[i*(s-1)*3+j*3+2]-paper[2];	
			t2 += t*t;
			curves[i][j+1] = sqrt(t2);
			}
		else curves[i][j+1] = data[i*(s-1)*3+j*3];
		}
	// remve any reversals from the data
	state = cleanReversals(curves[i],values[i],&sizes[i]);
	if (state != MCO_SUCCESS) goto bail;
	// scale from 0 to 100
	for (j=0; j<sizes[i]; j++) curves[i][j] = 100*curves[i][j]/curves[i][sizes[i]-1];	
	
	// set up the splines
	spline_0(values[i],curves[i],sizes[i],1.0e30,1.0e30,y2[i]);	
	
	// set up the splines
	spline_0(curves[i],values[i],sizes[i],1.0e30,1.0e30,y3[i]);	
	}
	
	
bail:
return state;
}


// use splines to linearize tables
// the polaroid version (data is in density format)
McoStatus LinearData::setUp(double *data,double *vals,int array_mult,int nc,int *s)
{

int i,j;
double t,t2;
McoStatus state = MCO_SUCCESS;

CleanUp();

numc = nc;

sizes = new int32[numc];
curves = new double*[numc];
values = new double*[numc];
y2 = new double*[numc];
y3 = new double*[numc];
if ((sizes == 0L) || (curves == 0L) || (values == 0L) || (y2 == 0L) || (y3 == 0L)) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<numc; i++) 
	{
	sizes[i] = s[i];
	curves[i] = new double[sizes[i]];
	values[i] = new double[sizes[i]];
	y2[i] = new double[sizes[i]];
	y3[i] = new double[sizes[i]];
	if ((sizes[i] == 0L) || (curves[i] == 0L) || (values[i] == 0L) || (y2[i] == 0L) || (y3[i] == 0L)) return MCO_MEM_ALLOC_ERROR;
	for (j=0; j<sizes[i]; j++)
		{
		values[i][j] = vals[i*array_mult+j];
		}
	}

// build the curves
for (i=0; i<numc; i++)
	{
	for (j=0; j<sizes[i]; j++)
		{
		curves[i][j] = data[i*array_mult+j];
		}
	// remve any reversals from the data
	state = cleanReversals(curves[i],values[i],&sizes[i]);
	if (state != MCO_SUCCESS) goto bail;
	// scale from 0 to 100
	for (j=0; j<sizes[i]; j++) curves[i][j] = 100*curves[i][j]/curves[i][sizes[i]-1];	
	
	// set up the splines
	spline_0(values[i],curves[i],sizes[i],1.0e30,1.0e30,y2[i]);	
	
	// set up the splines
	spline_0(curves[i],values[i],sizes[i],1.0e30,1.0e30,y3[i]);	
	}
	
	
bail:
return state;
}





// convert values to linear values
McoStatus LinearData::CnvtLinear(double *data, int32 num, int32 ch)
{
int i;

if (y2 == 0L) return MCO_OBJECT_NOT_INITIALIZED;

for (i=0; i<num; i++)
	{
	if (sizes[ch] < 18) splint_0(values[ch],curves[ch],y2[ch],sizes[ch],data[i],&data[i]);
	else piceLin_0(values[ch],curves[ch],y2[ch],sizes[ch],data[i],&data[i]);
	}
return MCO_SUCCESS;
}

// covert a single value
double LinearData::CnvtLinear(double data,int32 ch)
{
double temp;

if (y2 == 0L) return data;

if (sizes[ch] < 18) splint_0(values[ch],curves[ch],y2[ch],sizes[ch],data,&temp);
else piceLin_0(values[ch],curves[ch],y2[ch],sizes[ch],data,&temp);

return temp;
}

// convert values to linear values
McoStatus LinearData::CnvtLinearInv(double *data, int32 num, int32 ch)
{
int i;

if (y2 == 0L) return MCO_OBJECT_NOT_INITIALIZED;

for (i=0; i<num; i++)
	{
	if (sizes[ch] < 18) splint(curves[ch]-1,values[ch]-1,y3[ch]-1,sizes[ch],data[i],&data[i]);
	else piceLin_0(curves[ch],values[ch],y3[ch],sizes[ch],data[i],&data[i]);
	}
return MCO_SUCCESS;
}

// covert a single value
double LinearData::CnvtLinearInv(double data,int32 ch)
{
double temp;

if (y2 == 0L) return data;

if (sizes[ch] < 18) splint_0(curves[ch],values[ch],y3[ch],sizes[ch],data,&temp);
else piceLin_0(curves[ch],values[ch],y3[ch],sizes[ch],data,&temp);

return temp;
}