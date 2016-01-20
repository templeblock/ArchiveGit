////////////////////////////////////////////////////////////////////////////////
//	rawdata.c																  //
//																			  //
//	The code for dealing with the format of the raw data					  //
//	James Vogh																  //
//  Nov 18, 1996															  //
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "monacoprint.h"
#include "rawdata.h"
#include "mcomem.h"
#include "spline3.h"
#include "spline.h"
#include "splint.h"
#include "picelin.h"
#include "linearize_data.h"
#include "fileformat.h"
#include "assort.h"
//#include "mcoicc.h"

#ifdef powerc
#include "SLFunctions.h"
#endif

RawData::RawData(void)
{
type = No_Patches;
dataH = 0L;
linearDataH = 0L;
numTags = 0;
format.total = 0;
format.numslow = 0;
format.numc = 0;
format.numLinear = 0;
format.slowValues = 0L;
format.cubedim = 0L;
format.cmpValues = 0L;
format.linearValues = 0L;

patches.strips = 1;
patches.sheets = 100000;
patches.rows = 32;
patches.columns = (format.total + format.numLinear*format.numc)/32;
if ((format.total + format.numLinear*format.numc)%32) patches.columns++;
patches.starting = 0;
patches.direction = 1;
patches.width = 100;
patches.height = 100;

strcpy(desc,"");
}

RawData::~RawData(void)
{
if (format.slowValues != 0L) delete format.slowValues;
if (format.cubedim != 0L) delete format.cubedim;
if (format.cmpValues != 0L) delete format.cmpValues;
if (format.linearValues != 0L) delete format.linearValues;
if (dataH != 0L) McoDeleteHandle(dataH);
if (linearDataH != 0L) McoDeleteHandle(linearDataH);
}

// set up from an external source (like the XRITE)
McoStatus RawData::SetUp(int32 ty,int32 numc,int32 slow,int32 linear,int32 *dims)
{
int32 i,j,dimp = 1,total = 0,dimCount = 0;
double	*data;

for (i=0; i<slow; i++) 
	{
	dimCount += dims[i];
	dimp = 1;
	if (slow >1) for (j=0; j<numc-1; j++) dimp *= dims[i];
	else for (j=0; j<numc; j++) dimp *= dims[i];
	total += dimp;
	}
	
type = (PatchType)ty;
switch (type) {
	case CMYKPatches:
		ordering = Order_MCYK;
		break;
	case RGBPatches:
		ordering = Order_RGB;
		break;
	}

format.total = total;
format.numc = numc;
format.numslow = slow;
format.numLinear = linear;
// create a new set of slow values
if (format.slowValues != 0L) delete format.slowValues;
format.slowValues = 0L;
if (slow)
	{
	format.slowValues = new double[slow];
	if (!format.slowValues) return MCO_MEM_ALLOC_ERROR;
	}
	
// create a new cube dimension array
if (format.cubedim != 0L) delete format.cubedim;
format.cubedim = 0L;
if (slow)
	{
	format.cubedim = new int32[slow];
	if (!format.cubedim) return MCO_MEM_ALLOC_ERROR;
	for (i=0; i<slow; i++) format.cubedim[i] = dims[i];
	}

	
// create a new linear values array
if (format.linearValues != 0L) delete format.linearValues;
format.linearValues = 0L;
if (linear)  
	{
	format.linearValues = new double[linear];
	if (!format.linearValues) return MCO_MEM_ALLOC_ERROR;
	}


// values of the components
if (format.cmpValues != 0L) delete format.cmpValues;
format.cmpValues = 0L;
if (dimCount)
	{
	format.cmpValues = new double[dimCount];
	if (!format.cmpValues) return MCO_MEM_ALLOC_ERROR;
	}

// create the handle that holds the actual values 
if (dataH != 0L) McoDeleteHandle(dataH);
dataH = 0L;
if (total)
	{
	dataH = McoNewHandle(total*3*sizeof(double));
	if (dataH == 0L) return MCO_MEM_ALLOC_ERROR;
	}

// the values for the linear data
if (linearDataH != 0L) McoDeleteHandle(linearDataH);
linearDataH = 0L;
if (linear > 0) 
	{
	linearDataH = McoNewHandle(linear*3*format.numc*sizeof(double));
	if (linearDataH == 0L) return MCO_MEM_ALLOC_ERROR;
	}
// fill buffers with data
if (dataH)
	{
	data = (double*)McoLockHandle(dataH);
	for (i=0; i<format.total*3; i++) data[i] = -1000;
	McoUnlockHandle(dataH);
	}
if (linearDataH)
	{
	data = (double*)McoLockHandle(linearDataH);
	for (i=0; i<format.numLinear*format.numc*3; i++) data[i] = -1000;	
	McoUnlockHandle(linearDataH);
	}
return MCO_SUCCESS;
}



// get the value of the component for slow and num
int32 RawData::getcmpValues(int32 slow,int32 num)
{
int i,c = 0;

for (i=0; i<slow; i++)
	{
	c+=format.cubedim[i];
	}
	
c += num;
return format.cmpValues[c];
}


// get the index in data for slow value of k
int32 RawData::getDataIndex(int32 slow)
{
int i,c=0;

if (format.numc == 3)
	{
	for (i=0; i<slow; i++) c+=format.cubedim[i]*format.cubedim[i]*3;
	}
if (format.numc == 4)
	{
	for (i=0; i<slow; i++) c+=format.cubedim[i]*format.cubedim[i]*format.cubedim[i]*3;
	}
return c;
}

// get the lab of the paper
McoStatus RawData::getPaperLab(double *lab)
{
double *data;

if (dataH) 
	{
	data = (double*)McoLockHandle(dataH);

	if (type == CMYKPatches) // first patch
		{
		lab[0] = data[0];
		lab[1] = data[1];
		lab[2] = data[2];
		}
	if (type == RGBPatches) // last patch
		{
		lab[0] = data[(format.total-1)*3];
		lab[1] = data[(format.total-1)*3+1];
		lab[2] = data[(format.total-1)*3+2];
		}
	McoLockHandle(dataH);
	}
else if (linearDataH)
	{
	data = (double*)McoLockHandle(linearDataH);
	
	lab[0] = data[0];
	lab[1] = data[1];
	lab[2] = data[2];
	McoLockHandle(linearDataH);
	}
else return MCO_FAILURE;
return MCO_SUCCESS;
}


// a multidimensional make refrence
// for CMYK data generates a table of CMYK that matches the 
// CMYK values used to create the raw data
McoStatus RawData::makeRef(double *ref)
{
int32 i,j,k,l,c = 1;
int32 m = 0;
double	*cmpPtr;

cmpPtr = format.cmpValues;

switch (type) {
	case CMYKPatches:	// for cmyk patches
		for (i=0; i<format.numslow; i++)
			{
			for (j=0; j<format.cubedim[i]; j++)
				for (k=0; k<format.cubedim[i]; k++)
					for (l=0; l<format.cubedim[i]; l++)
						{
						ref[m++] = cmpPtr[k];
						ref[m++] = cmpPtr[l];
						ref[m++] = cmpPtr[j];
					//	ref[m++] = format.slowValues[i];
						}
			cmpPtr += format.cubedim[i];
			}
		break;
	case RGBPatches:	// for rgb patches
		for (i=0; i<format.numslow; i++)
			{
			for (j=0; j<format.cubedim[i]; j++)
				for (k=0; k<format.cubedim[i]; k++)
						{
						ref[m++] = format.slowValues[i];
						ref[m++] = cmpPtr[j];
						ref[m++] = cmpPtr[k];
						}
			cmpPtr += format.cubedim[i];
			}
		break;
		}
return MCO_SUCCESS;
}

// Smooth the lab space of the patches, smoothing only the L channel
McoStatus RawData::smooth_patch(void)
{
int32	f,h,i,j,k;
int32	im,jm,km;
int32	l,m,n;
int32	p,q,r;
int32	s;
int32	num_4;
int32	st;
int32	index,indexl;
double a[4] = {0.4,0.05,0.017,0.012}; // the 3D kernal
double b[4] = {0.4, 0.1, 0.05};	
McoHandle	patchTempH;
double	*labh;
double	*labh2;

	if (format.total <= 0) return MCO_OBJECT_NOT_INITIALIZED;

	patchTempH = McoNewHandle(sizeof(double)*format.total*3);
	if(!patchTempH)
		return MCO_MEM_ALLOC_ERROR;

	labh = (double*)McoLockHandle(dataH);
	labh2 = (double*)McoLockHandle(patchTempH);

 	for (i=0; i<format.total*3; i++) labh2[i] = labh[i];

	if (format.numc == 4) num_4 = format.numslow;
	else format.numc = 1;

	for (f=0; f<format.numslow; f++) if (format.cubedim[f] >= 5)
		{
		
		im = 3*format.cubedim[f]*format.cubedim[f];
		jm = 3*format.cubedim[f];
		km = 3;
		
		st = getDataIndex(f);
	 	for (h=0; h<2; h++)
	 		{
	 		for (j=1; j<format.cubedim[f]-1; j++)
	 			{
	 			for (k=1; k<format.cubedim[f]-1; k++)
	 				{
	 				i = (format.cubedim[f]-1)*h;
	 				index = st+i*im+j*jm+k*km;
	 				labh2[index] = 0;
					labh2[index+1] = 0;
					labh2[index+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i;
							m = j+q;
							n = k+r;
							s = abs(m-j)+abs(n-k);
							indexl = st+l*im+m*jm+n*km;
							
						 	labh2[index] += b[s]*labh[indexl];
							labh2[index+1] += b[s]*labh[indexl+1];
							labh2[index+2] += b[s]*labh[indexl+2];
							}
					}
				}
			}
			
	 	for (h=0; h<2; h++)
	 		{
	 		for (i=1; i<format.cubedim[f]-1; i++)
	 			{
	 			for (k=1; k<format.cubedim[f]-1; k++)
	 				{
	 				j = (format.cubedim[f]-1)*h;
	 				index = st+i*im+j*jm+k*km;
	 				labh2[index] = 0;
					labh2[index+1] = 0;
					labh2[index+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j;
							n = k+r;
							s = abs(l-i)+abs(n-k);		
							indexl = st+l*im+m*jm+n*km;
							
						 	labh2[index] += b[s]*labh[indexl];
							labh2[index+1] += b[s]*labh[indexl+1];
							labh2[index+2] += b[s]*labh[indexl+2];
							}
					}
				}
			}
		for (h=0; h<2; h++)
	 		{
	 		for (i=1; i<format.cubedim[f]-1; i++)
	 			{
	 			for (j=1; j<format.cubedim[f]-1; j++)
	 				{
	 				k = (format.cubedim[f]-1)*h;
	 				index = st+i*im+j*jm+k*km;
	 				labh2[index] = 0;
					labh2[index+1] = 0;
					labh2[index+2] = 0;
					for (q=-1; q<=1; q++)
						for (r=-1; r<=1; r++)
							{
							l = i+q;
							m = j+r;
							n = k;
							s = abs(l-i)+abs(m-j);
							indexl = st+l*im+m*jm+n*km;
							
						 	labh2[index] += b[s]*labh[indexl];
							labh2[index+1] += b[s]*labh[indexl+1];
							labh2[index+2] += b[s]*labh[indexl+2];
							}
					}
				}
			}
			

	 	for (i=1; i<format.cubedim[f]-1; i++)
	 		{
	 		for (j=1; j<format.cubedim[f]-1; j++)
	 			{
	 			for (k=1; k<format.cubedim[f]-1; k++)
	 				{
	 				index = st+i*im+j*jm+k*km;
	 				labh2[index] = 0;
					labh2[index+1] = 0;
					labh2[index+2] = 0;
	 				
	 				for (p=-1; p<=1; p++)
	 					for (q=-1; q<=1; q++)
	 						for (r=-1; r<=1; r++)
	 							{
	 							l = i+p;
	 							m = j+q;
	 							n = k+r;
	 							s = abs(l-i)+abs(m-j)+abs(n-k);
	 							indexl = st+l*im+m*jm+n*km;
	 							
	 							labh2[index] += a[s]*labh[indexl];
								labh2[index+1] += a[s]*labh[indexl+1];
								labh2[index+2] += a[s]*labh[indexl+2];
	 							
	 							}
			
					}
				}
			}
		}

	
		
	McoUnlockHandle(dataH);
	McoDeleteHandle(dataH);
	dataH = patchTempH;
	McoUnlockHandle(dataH);
	return MCO_SUCCESS;
}



// Do four dimensional interpolation
// 
McoStatus RawData::doFourDInterp(double *cmyktable,int32 nums, double *vals,int useLuts)
{
int h,i,j,k,l,m;
int32 maxS,numS,maxC,total;
int32 index;
int32 dim,st;
double *y2a_L,*y2a_a,*y2a_b;
double	*patch_L,*patch_a,*patch_b;
double *temp_L,*temp_a,*temp_b;
double *temp2_L,*temp2_a,*temp2_b;
double *y2_L,*y2_a,*y2_b;
double	*patch;
double *pVals_c, *pVals_m, *pVals_y;
double *x1;
double x23[2];
double *slowVals;
double *vals_k;
double paper[3];
LinearData	*linear_data;
McoStatus state = MCO_SUCCESS;

linear_data = new LinearData;

numS = format.numslow;

total = format.total;

// derivatives
y2a_L = new double[format.total*format.numslow];
if (!y2a_L) goto mem_bail;
y2a_a = new double[format.total*format.numslow];
if (!y2a_a) goto mem_bail;
y2a_b = new double[format.total*format.numslow];
if (!y2a_b) goto mem_bail;

y2_L = new double[format.numslow];
if (!y2_L) goto mem_bail;
y2_a = new double[format.numslow];
if (!y2_a) goto mem_bail;
y2_b = new double[format.numslow];
if (!y2_b) goto mem_bail;

// temporay patch data
patch_L = new double[format.total*format.numslow];
if (!patch_L) goto mem_bail;
patch_a = new double[format.total*format.numslow];
if (!patch_a) goto mem_bail;
patch_b = new double[format.total*format.numslow];
if (!patch_b) goto mem_bail;

// temporary interpolated data that is stored here for speed
temp_L = new double[nums*numS];
if (!temp_L) goto mem_bail;
temp_a = new double[nums*numS];
if (!temp_a) goto mem_bail;
temp_b = new double[nums*numS];
if (!temp_b) goto mem_bail;

// copys of the above data where K is the fastest changing variable
temp2_L = new double[nums*numS];
if (!temp2_L) goto mem_bail;
temp2_a = new double[nums*numS];
if (!temp2_a) goto mem_bail;
temp2_b = new double[nums*numS];
if (!temp2_b) goto mem_bail;

x1 = new double[nums];
if (!x1) goto mem_bail;

getPaperLab(paper);

if (linearDataH != 0L) patch = (double*)McoLockHandle(linearDataH);
if (format.numLinear > 0) linear_data->setUp(patch,format.linearValues,paper,format.numc,format.numLinear);
if (linearDataH != 0L) McoUnlockHandle(linearDataH);

patch = (double*)McoLockHandle(dataH);

maxS = 0;
for (i=0; i<numS; i++) if (format.cubedim[i] > maxS) maxS = format.cubedim[i];

pVals_c = new double[maxS*numS];
if (!pVals_c) goto mem_bail;
pVals_m = new double[maxS*numS];
if (!pVals_m) goto mem_bail;
pVals_y = new double[maxS*numS];
if (!pVals_y) goto mem_bail;

for (h=0; h<numS; h++)
	for (i=0; i<format.cubedim[h]; i++) 
		{
		pVals_c[maxS*h+i] = getcmpValues(h,i);
		pVals_m[maxS*h+i] = getcmpValues(h,i);
		pVals_y[maxS*h+i] = getcmpValues(h,i);
		}
		
linear_data->CnvtLinear(pVals_c,maxS*numS,0);
linear_data->CnvtLinear(pVals_m,maxS*numS,1);
linear_data->CnvtLinear(pVals_y,maxS*numS,2);

slowVals = new double[numS];
if (!slowVals) goto mem_bail;
for (i=0; i<numS; i++) slowVals[i] = format.slowValues[i];
linear_data->CnvtLinear(slowVals,numS,3);


vals_k = new double[nums];
if (!vals_k) goto mem_bail;
for (i=0; i<nums; i++) vals_k[i] = vals[i];
if (!useLuts) linear_data->CnvtLinear(vals_k,nums,3);

total = format.total;

for (i=0; i<total; i++)
	{
	patch_L[i]=patch[i*3];
	patch_a[i]=patch[i*3+1];
	patch_b[i]=patch[i*3+2];
	}


// set up the splines

for (h=0; h<numS; h++)
	{
	dim = format.cubedim[h];
	st = getDataIndex(h)/3;
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_L+st,dim,dim,dim, y2a_L+total*h);
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_a+st,dim,dim,dim, y2a_a+total*h);
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_b+st,dim,dim,dim, y2a_b+total*h);
	}
	
// do the interpolation	

	for (i = 0; i< nums; i++)
		{
		if (useLuts)	
			x1[i] = vals[i]; //linear_data->CnvtLinear(vals[i],2);  // yellow, slowest change
		else
			x1[i] = linear_data->CnvtLinear(vals[i],2);  // yellow, slowest change
		}
		
		
	for (i = 0; i< nums; i++) 		// cyan
		for (j = 0; j < nums; j++)	// magenta, fastest change
			{
			// do 3D splines for each block and get values for different values of yellow and black
			for (h=0; h<numS; h++)
				{
				if (useLuts)
					{	
					x23[0] = vals[i]; //linear_data->CnvtLinear(vals[i],0);  // cyan 
					x23[1] = vals[j]; //linear_data->CnvtLinear(vals[j],1);  // magenta
					}
				else
					{	
					x23[0] = linear_data->CnvtLinear(vals[i],0);  // cyan 
					x23[1] = linear_data->CnvtLinear(vals[j],1);  // magenta
					}					
				dim = format.cubedim[h];
				st = getDataIndex(h)/3;
				
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_L+st,y2a_L+total*h,dim,dim,dim,x23,x1,nums,temp_L+nums*h);
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_a+st,y2a_a+total*h,dim,dim,dim,x23,x1,nums,temp_a+nums*h);
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_b+st,y2a_b+total*h,dim,dim,dim,x23,x1,nums,temp_b+nums*h);
				}
			// now do 1D splines on the temp data to get the actual data
			for (k=0; k<nums; k++) // yellow
				{
				// reorder the data
				for (l=0; l<numS; l++)
					{
					temp2_L[l] = temp_L[k+l*nums];
					temp2_a[l] = temp_a[k+l*nums];
					temp2_b[l] = temp_b[k+l*nums];
					}
				spline_0(slowVals,temp2_L,numS,1.0e30,1.0e30,y2_L);
				spline_0(slowVals,temp2_a,numS,1.0e30,1.0e30,y2_a);
				spline_0(slowVals,temp2_b,numS,1.0e30,1.0e30,y2_b);
				for (l=0; l<nums; l++) // black
					{
					// do interpolation and enter the values into the table	
					index = (i+j*nums+k*nums*nums+l*nums*nums*nums)*3;				
					splint_0(slowVals,temp2_L,y2_L,numS,vals_k[l],cmyktable+index);
					splint_0(slowVals,temp2_a,y2_a,numS,vals_k[l],cmyktable+index+1);
					splint_0(slowVals,temp2_b,y2_b,numS,vals_k[l],cmyktable+index+2);
				//	piceLin(format.slowValues-1,temp2_L-1,y2_L-1,numS,vals[l],cmyktable+index);
				//	piceLin(format.slowValues-1,temp2_a-1,y2_a-1,numS,vals[l],cmyktable+index+1);
				//	piceLin(format.slowValues-1,temp2_b-1,y2_b-1,numS,vals[l],cmyktable+index+2);
					}
				}
			}
			
finish:			
						
if (y2a_L) delete y2a_L;
if (y2a_a) delete y2a_a;
if (y2a_b) delete y2a_b;
			
if (y2_L) delete y2_L;
if (y2_a) delete y2_a;
if (y2_b) delete y2_b;			
			
if (patch_L) delete patch_L;
if (patch_a) delete patch_a;
if (patch_b) delete patch_b;

if (temp_L) delete temp_L;
if (temp_a) delete temp_a;
if (temp_b) delete temp_b;

if (temp2_L) delete temp2_L;
if (temp2_a) delete temp2_a;
if (temp2_b) delete temp2_b;
	
if (x1) delete x1;
if (pVals_c) delete pVals_c;		
if (pVals_m) delete pVals_m;		
if (pVals_y) delete pVals_y;			

if (slowVals) delete slowVals;
if (vals_k) delete vals_k;

if (linear_data) delete linear_data;
			
return state;

mem_bail:

state = MCO_MEM_ALLOC_ERROR;
goto finish;

}

// Do four dimensional interpolation
// with a different level of K values
McoStatus RawData::doFourDInterp(double *cmyktable,int32 nums, int32 numsK, double *vals, double *valsK)
{
int h,i,j,k,l,m;
int32 maxS,numS,maxC,total;
int32 index;
int32 dim,st;
double *y2a_L,*y2a_a,*y2a_b;
double	*patch_L,*patch_a,*patch_b;
double *temp_L,*temp_a,*temp_b;
double *temp2_L,*temp2_a,*temp2_b;
double *y2_L,*y2_a,*y2_b;
double	*patch;
double *pVals_c;
double *pVals_m;
double *pVals_y;
double *x1;
double x23[2];
double *slowVals;
double *valsK2;
LinearData	*linear_data;
double paper[3];

linear_data = new LinearData;

numS = format.numslow;

total = format.total;

// derivatives
y2a_L = new double[format.total*format.numslow];
y2a_a = new double[format.total*format.numslow];
y2a_b = new double[format.total*format.numslow];

y2_L = new double[format.numslow];
y2_a = new double[format.numslow];
y2_b = new double[format.numslow];


// temporay patch data
patch_L = new double[format.total*format.numslow];
patch_a = new double[format.total*format.numslow];
patch_b = new double[format.total*format.numslow];

// temporary interpolated data that is stored here for speed
temp_L = new double[nums*numS];
temp_a = new double[nums*numS];
temp_b = new double[nums*numS];

// copys of the above data where K is the fastest changing variable
temp2_L = new double[nums*numS];
temp2_a = new double[nums*numS];
temp2_b = new double[nums*numS];

x1 = new double[nums];

getPaperLab(paper);

if (linearDataH != 0L) patch = (double*)McoLockHandle(linearDataH);
if (format.numLinear > 0) linear_data->setUp(patch,format.linearValues,paper,format.numc,format.numLinear);
if (linearDataH != 0L) McoUnlockHandle(linearDataH);

patch = (double*)McoLockHandle(dataH);

maxS = 0;
for (i=0; i<numS; i++) if (format.cubedim[i] > maxS) maxS = format.cubedim[i];

pVals_c = new double[maxS*numS];
pVals_m = new double[maxS*numS];
pVals_y = new double[maxS*numS];

for (h=0; h<numS; h++)
	for (i=0; i<format.cubedim[h]; i++) 
		{
		pVals_c[maxS*h+i] = getcmpValues(h,i);
		pVals_m[maxS*h+i] = getcmpValues(h,i);
		pVals_y[maxS*h+i] = getcmpValues(h,i);
		}
		
linear_data->CnvtLinear(pVals_c,maxS*numS,0);
linear_data->CnvtLinear(pVals_m,maxS*numS,1);
linear_data->CnvtLinear(pVals_y,maxS*numS,2);

slowVals = new double[numS];
for (i=0; i<numS; i++) slowVals[i] = format.slowValues[i];
linear_data->CnvtLinear(slowVals,numS,3);

valsK2 = new double[numsK];
for (i=0; i<numsK; i++) valsK2[i] = valsK[i];
//linear_data->CnvtLinear(valsK2,numsK,3);

total = format.total;

for (i=0; i<total; i++)
	{
	patch_L[i]=patch[i*3];
	patch_a[i]=patch[i*3+1];
	patch_b[i]=patch[i*3+2];
	}

// set up the splines

for (h=0; h<numS; h++)
	{
	dim = format.cubedim[h];
	st = getDataIndex(h)/3;
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_L+st,dim,dim,dim, y2a_L+total*h);
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_a+st,dim,dim,dim, y2a_a+total*h);
	spline3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_b+st,dim,dim,dim, y2a_b+total*h);
	}
	
// do the interpolation	

	for (i = 0; i< nums; i++)
		{
		x1[i] = vals[i]; //linear_data->CnvtLinear(vals[i],2);  // yellow, slowest change
		}
		
	for (i = 0; i< nums; i++) 		// cyan
		for (j = 0; j < nums; j++)	// magenta, fastest change
			{
			// do 3D splines for each block and get values for different values of yellow and black
			for (h=0; h<numS; h++)
				{	
				x23[0] = vals[i]; //linear_data->CnvtLinear(vals[i],0);  // cyan 
				x23[1] = vals[j]; //linear_data->CnvtLinear(vals[j],1);  // magenta
				dim = format.cubedim[h];
				st = getDataIndex(h)/3;
				
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_L+st,y2a_L+total*h,dim,dim,dim,x23,x1,nums,temp_L+nums*h);
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_a+st,y2a_a+total*h,dim,dim,dim,x23,x1,nums,temp_a+nums*h);
				splint3(pVals_y+h*maxS,pVals_c+h*maxS,pVals_m+h*maxS,patch_b+st,y2a_b+total*h,dim,dim,dim,x23,x1,nums,temp_b+nums*h);
				}
			// now do 1D splines on the temp data to get the actual data
			for (k=0; k<nums; k++) // yellow
				{
				// reorder the data
				for (l=0; l<numS; l++)
					{
					temp2_L[l] = temp_L[k+l*nums];
					temp2_a[l] = temp_a[k+l*nums];
					temp2_b[l] = temp_b[k+l*nums];
					}
				spline_0(slowVals,temp2_L,numS,1.0e30,1.0e30,y2_L);
				spline_0(slowVals,temp2_a,numS,1.0e30,1.0e30,y2_a);
				spline_0(slowVals,temp2_b,numS,1.0e30,1.0e30,y2_b);
				for (l=0; l<numsK; l++) // black
					{
					// do interpolation and enter the values into the table	
					index = (i+j*nums+k*nums*nums+l*nums*nums*nums)*3;				
					splint_0(slowVals,temp2_L,y2_L,numS,valsK2[l],cmyktable+index);
					splint_0(slowVals,temp2_a,y2_a,numS,valsK2[l],cmyktable+index+1);
					splint_0(slowVals,temp2_b,y2_b,numS,valsK2[l],cmyktable+index+2);
					//piceLin(format.slowValues-1,temp2_L-1,y2_L-1,numS,vals[l],cmyktable+index);
					//piceLin(format.slowValues-1,temp2_a-1,y2_a-1,numS,vals[l],cmyktable+index+1);
					//piceLin(format.slowValues-1,temp2_b-1,y2_b-1,numS,vals[l],cmyktable+index+2);
					}
				}
			}
			
						
delete y2a_L;
delete y2a_a;
delete y2a_b;
			
delete y2_L;
delete y2_a;
delete y2_b;			
			
delete patch_L;
delete patch_a;
delete patch_b;

delete temp_L;
delete temp_a;
delete temp_b;

delete temp2_L;
delete temp2_a;
delete temp2_b;
	
delete x1;
delete pVals_c;		
delete pVals_m;		
delete pVals_y;

delete slowVals;
delete valsK2;		

delete linear_data;	
			
return MCO_SUCCESS;
}



// Do four dimensional interpolation
// 
McoStatus RawData::doThreeDInterp(double *rgbtable,int32 nums, double *vals)
{
int h,i,j,k,l,m;
int32 maxS,numS,maxC,total;
int32 index;
int32 dim,st;
double *y2a_L,*y2a_a,*y2a_b;
double	*patch_L,*patch_a,*patch_b;
double *temp_L,*temp_a,*temp_b;
double *temp2_L,*temp2_a,*temp2_b;
double y2_L,y2_a,y2_b;
double	*patch;
double *pVals_r, *pVals_g, *pVals_b;
double *x1;
double x23[2];
double paper[3];
LinearData	*linear_data;

linear_data = new LinearData;

numS = format.numslow;

total = format.total;

// derivatives
y2a_L = new double[format.total];
y2a_a = new double[format.total];
y2a_b = new double[format.total];



// temporay patch data
patch_L = new double[format.total];
patch_a = new double[format.total];
patch_b = new double[format.total];

// temporary interpolated data that is stored here for speed
temp_L = new double[nums];
temp_a = new double[nums];
temp_b = new double[nums];

// copys of the above data where K is the fastest changing variable
temp2_L = new double[nums];
temp2_a = new double[nums];
temp2_b = new double[nums];

x1 = new double[nums];

getPaperLab(paper);

if (linearDataH != 0L) patch = (double*)McoLockHandle(linearDataH);
if (format.numLinear > 0) linear_data->setUp(patch,format.linearValues,paper,format.numc,format.numLinear);
if (linearDataH != 0L) McoUnlockHandle(linearDataH);

patch = (double*)McoLockHandle(dataH);

maxS = 0;
for (i=0; i<numS; i++) if (format.cubedim[i] > maxS) maxS = format.cubedim[i];

pVals_r = new double[maxS];
pVals_g = new double[maxS];
pVals_b = new double[maxS];

for (h=0; h<numS; h++)
	for (i=0; i<format.cubedim[h]; i++) 
		{
		pVals_r[maxS*h+i] = getcmpValues(h,i);
		pVals_g[maxS*h+i] = getcmpValues(h,i);
		pVals_b[maxS*h+i] = getcmpValues(h,i);
		}
		
linear_data->CnvtLinear(pVals_r,maxS,0);
linear_data->CnvtLinear(pVals_g,maxS,1);
linear_data->CnvtLinear(pVals_b,maxS,2);


total = format.total;

for (i=0; i<total; i++)
	{
	patch_L[i]=patch[i*3];
	patch_a[i]=patch[i*3+1];
	patch_b[i]=patch[i*3+2];
	}


// set up the splines


dim = format.cubedim[0];
spline3(pVals_b,pVals_g,pVals_r,patch_L,dim,dim,dim, y2a_L);
spline3(pVals_b,pVals_g,pVals_r,patch_a,dim,dim,dim, y2a_a);
spline3(pVals_b,pVals_g,pVals_r,patch_b,dim,dim,dim, y2a_b);

	
// do the interpolation	

	for (i = 0; i< nums; i++)
		{
		x1[i] = vals[i]; // blue, slowest change
		}
		
		
	for (i = 0; i< nums; i++) 		// green
		for (j = 0; j < nums; j++)	// red, fastest change
			{
	
			x23[0] = vals[i]; //g
			x23[1] = vals[j]; //r
			dim = format.cubedim[0];
			
			splint3(pVals_b,pVals_g,pVals_r,patch_L,y2a_L,dim,dim,dim,x23,x1,nums,temp_L);
			splint3(pVals_b,pVals_g,pVals_r,patch_a,y2a_a,dim,dim,dim,x23,x1,nums,temp_a);
			splint3(pVals_b,pVals_g,pVals_r,patch_b,y2a_b,dim,dim,dim,x23,x1,nums,temp_b);
			// now do 1D splines on the temp data to get the actual data
			for (k=0; k<nums; k++) // blue
				{
				// reorder the data
				index = (j+i*nums+k*nums*nums)*3;			
				rgbtable[index] = temp_L[k];
				rgbtable[index+1] = temp_a[k];
				rgbtable[index+2] = temp_b[k];
				}
			}
			
						
delete y2a_L;
delete y2a_a;
delete y2a_b;
					
			
delete patch_L;
delete patch_a;
delete patch_b;

delete temp_L;
delete temp_a;
delete temp_b;

delete temp2_L;
delete temp2_a;
delete temp2_b;
	
delete x1;
delete pVals_r;		
delete pVals_g;		
delete pVals_b;			

delete linear_data;
			
return MCO_SUCCESS;
}

McoStatus RawData::copyRawData(RawData *tocopy)
{
McoStatus state;
int i,j,m= 0;
double *dataf,*datat;
int32	ty;

ty = (int32)tocopy->type;

if (ty == 0) return MCO_SUCCESS;

state = SetUp(ty,tocopy->format.numc,tocopy->format.numslow,tocopy->format.numLinear,tocopy->format.cubedim);
if (state != MCO_SUCCESS) return state;

for (i=0; i<format.numslow; i++)
	{
	format.slowValues[i] = tocopy->format.slowValues[i];
	for (j=0; j<format.cubedim[i]; j++) 
		{
		format.cmpValues[m] = tocopy->format.cmpValues[m];
		m++;
		}
	}
	
for (i=0; i<format.numLinear; i++) format.linearValues[i] = tocopy->format.linearValues[i];
patches = tocopy->patches;

dataf = (double*)McoLockHandle(tocopy->dataH);
datat = (double*)McoLockHandle(dataH);
for (i=0; i<format.total*3; i++) datat[i] = dataf[i];

McoUnlockHandle(tocopy->dataH);
McoUnlockHandle(dataH);

if ((tocopy->linearDataH != 0L) && (linearDataH != 0L))
	{
	dataf = (double*)McoLockHandle(tocopy->linearDataH);
	datat = (double*)McoLockHandle(linearDataH);
	for (i=0; i<format.numLinear*3*format.numc; i++) datat[i] = dataf[i];
	
	McoUnlockHandle(tocopy->linearDataH);
	McoUnlockHandle(linearDataH);
	
	}



return state;
}

McoStatus RawData::copyOutOfPatchData(PatchFormat *tocopy)
{
McoStatus state;
int i,j,m= 0;
double *dataf,*datat;
int32	ty;
long	*steps;

ty = (int32)tocopy->patchType;

if (ty == 0) return MCO_SUCCESS;

steps = new long[tocopy->numCube];
if (!steps) return MCO_MEM_ALLOC_ERROR;

for (i=0; i<tocopy->numCube; i++) steps[i] = tocopy->steps[i];

state = SetUp(ty,tocopy->numComp,tocopy->numCube,tocopy->numLinear,steps);
delete steps;
if (state != MCO_SUCCESS) return state;

for (i=0; i<format.numslow; i++)
	{
	format.slowValues[i] = tocopy->blacks[i];
	for (j=0; j<format.cubedim[i]; j++) 
		{
		format.cmpValues[m] = tocopy->cmy[m];
		m++;
		}
	}
	
for (i=0; i<format.numLinear; i++) format.linearValues[i] = tocopy->linear[i];

dataf = tocopy->data;
datat = (double*)McoLockHandle(dataH);
for (i=0; i<format.total*3; i++) datat[i] = dataf[i];

McoUnlockHandle(dataH);

if (linearDataH != 0L)
	{
	dataf = dataf = tocopy->ldata;
	datat = (double*)McoLockHandle(linearDataH);
	for (i=0; i<format.numLinear*3*format.numc; i++) datat[i] = dataf[i];
	McoUnlockHandle(linearDataH);
	
	}
	
strcpy(desc,tocopy->name);
desc[99] = 0;

return state;
}



McoStatus RawData::copyIntoPatchData(PatchFormat *tocopy)
{
McoStatus state = MCO_SUCCESS;
int i,j,m= 0;
double *dataf,*datat;
int32	dimCount = 0;

for (i=0; i<format.numslow; i++) 
	{
	dimCount += format.cubedim[i];
	}

tocopy->patchType = type;
tocopy->numComp = format.numc;
tocopy->numCube = format.numslow;
tocopy->numLinear = format.numLinear;

if (tocopy->steps) delete tocopy->steps;
tocopy->steps = 0L;
if (tocopy->blacks) delete tocopy->blacks;
tocopy->blacks = 0L;
if (tocopy->cmy) delete tocopy->cmy;
tocopy->cmy = 0L;
if (tocopy->linear) delete tocopy->linear;
tocopy->linear = 0L;
if (tocopy->data) delete tocopy->data;
tocopy->data = 0L;
if (tocopy->ldata) delete tocopy->ldata;
tocopy->ldata = 0L;

if (!type) return MCO_SUCCESS;

tocopy->steps = new unsigned long[format.numslow];
if (!tocopy->steps) return MCO_MEM_ALLOC_ERROR;
tocopy->blacks = new unsigned long[format.numslow];
if (!tocopy->blacks) return MCO_MEM_ALLOC_ERROR;
tocopy->cmy = new unsigned long[dimCount];
if (!tocopy->cmy) return MCO_MEM_ALLOC_ERROR;
tocopy->data = new double[format.total*3];
if (!tocopy->data) return MCO_MEM_ALLOC_ERROR;
if (format.numLinear)
	{
	tocopy->linear = new double[format.numLinear];
	if (!tocopy->linear) return MCO_MEM_ALLOC_ERROR;
	tocopy->ldata = new double[format.numLinear*format.numc*3];
	if (!tocopy->ldata) return MCO_MEM_ALLOC_ERROR;
	}

m = 0;
for (i=0; i<format.numslow; i++)
	{
	tocopy->steps[i] = format.cubedim[i];
	tocopy->blacks[i] = format.slowValues[i];
	for (j=0; j<format.cubedim[i]; j++) 
		{
		tocopy->cmy[m] = format.cmpValues[m];
		m++;
		}
	}
	
for (i=0; i<format.numLinear; i++)  tocopy->linear[i] = format.linearValues[i];

dataf = tocopy->data;
datat = (double*)McoLockHandle(dataH);
for (i=0; i<format.total*3; i++) dataf[i] = datat[i];

McoUnlockHandle(dataH);

if (linearDataH != 0L)
	{
	dataf = dataf = tocopy->ldata;
	datat = (double*)McoLockHandle(linearDataH);
	for (i=0; i<format.numLinear*3*format.numc; i++) dataf[i] = datat[i];
	McoUnlockHandle(linearDataH);
	}

strcpy(tocopy->name,desc);

return state;
}

// copy just the patch information
McoStatus RawData::copyPatchData(RawData *tocopy)
{

patches = tocopy->patches;

return MCO_SUCCESS;
}


// see if the formats are the same
int	RawData::sameFormat(RawData *data)
{
int i,j,c=0;

if (format.total != data->format.total) return 0;
if (format.numc != data->format.numc) return 0;
if (format.numslow != data->format.numslow) return 0;

for (i=0; i<format.numslow; i++)
	{
	if (format.slowValues[i] != data->format.slowValues[i]) return 0;
	if (format.cubedim[i] != data->format.cubedim[i]) return 0;
	for (j=0; j<format.cubedim[i]; j++)
		{
		if (format.cmpValues[c] != data->format.cmpValues[c]) return 0;
		c++;
		}
	}

if (format.numLinear != data->format.numLinear) return 0;
for (i=0; i<format.numLinear; i++)
	{
	if (format.linearValues[i] != data->format.linearValues[i]) return 0;
	}
return 1;
}


// save the data to a text file
McoStatus RawData::saveData(FILE *ofsb)
{

	long 				i, j, k,c,m;
	double 				*dataptr;
	
	dataptr = (double *)McoLockHandle(dataH);
	
	fprintf(ofsb,MONACO_PATCHES_HEAD);

	fprintf(ofsb, "%ld %ld %ld %ld\n",type,format.numc,format.numslow,format.numLinear);
	
	for (i=0; i<format.numslow; i++)
		fprintf(ofsb, "%ld ",format.cubedim[i]);
		
	fprintf(ofsb, "\n");
	
	
	for (i=0; i<format.numslow; i++)
		fprintf(ofsb, "%f ",format.slowValues[i]);
	
	fprintf(ofsb, "\n");
	

	
	m = 0;	
	for (i=0; i<format.numslow; i++) 
		{
		for (j=0; j<format.cubedim[i]; j++)
			{
			fprintf(ofsb, "%f ",format.cmpValues[m++]);
			}
		fprintf(ofsb, "\n");	
		}
	
	for (i=0; i<format.numLinear; i++)
		fprintf(ofsb, "%f ",format.linearValues[i]);
		
	if (format.numLinear >0) fprintf(ofsb, "\n");
	
	c = 0;
	for(i = 0; i < format.total; i++)
		{
		j = i*3;
		fprintf( ofsb, "%f \t %f \t %f\n", dataptr[j],dataptr[j+1],dataptr[j+2]);
		c++;
		}

	McoUnlockHandle(dataH);
	
	
	if (linearDataH != 0L)
		{	
		dataptr = (double*)McoLockHandle(linearDataH);
		for (k=0; k<format.numc; k++)
			{
			for (i=0; i<format.numLinear-1; i++)
				{
				j = k*3*(format.numLinear-1)+i*3;
				fprintf( ofsb, "%f \t %f \t %f\n",dataptr[j],dataptr[j+1],dataptr[j+2]);
				}
			}

		McoUnlockHandle(linearDataH);
		}	
		
	return MCO_SUCCESS;
}

// load the data from a file
McoStatus RawData::loadData(FILE *fs)
{
	McoStatus 			status = MCO_SUCCESS;
	short				i,j,k,l,m,n;
	double *patch, *patchcopy;
	int32				type,numc,numslow,numlinear;
	int32				*dims,total = 0, dimCount = 0;
	int32				dimp = 1;
	RawData 			*patchd;
	double				*_patch;
	char				junk[255];

	
	fgets(junk,50,fs);
	n = strcmp(junk,MONACO_PATCHES_HEAD);
	if (n) 
		{
		if ((junk[0] != '1') && (junk[0] != '2')) return MCO_FILE_DATA_ERROR;
		if ((junk[0] == '1') && (junk[2] != '4')) return MCO_FILE_DATA_ERROR;
		if ((junk[0] == '2') && (junk[2] != '3')) return MCO_FILE_DATA_ERROR;
		if (hasLetters(junk)) return MCO_FILE_DATA_ERROR;
		l = strlen(junk);
		if ((l != 8) && (l != 9)) return MCO_FILE_DATA_ERROR;
		sscanf(junk,"%ld %ld %ld %ld",&type,&numc,&numslow,&numlinear);
		}
	else fscanf(fs, "%ld %ld %ld %ld",&type,&numc,&numslow,&numlinear);
	
	dims = new int32 [numslow];
	
	for (i=0; i<numslow; i++)
		fscanf(fs, "%ld",&dims[i]);

	
	status = SetUp(type,numc,numslow,numlinear,dims);
	if (status != MCO_SUCCESS) goto bail;

	_patch = (double*)McoLockHandle(dataH);
	
	for (i=0; i<numslow; i++)
		fscanf(fs, "%lf",&format.slowValues[i]);
	
	m = 0;	
	for (i=0; i<numslow; i++) 
		{
		for (j=0; j<format.cubedim[i]; j++)
			{
			fscanf(fs, "%lf",&format.cmpValues[m++]);
			}
		}
		
	for (i=0; i<format.numLinear; i++)
		{
		if (!feof(fs)) fscanf(fs,"%lf",&format.linearValues[i]);
		}
		
	for(i = 0; i < format.total; i++){
		j = i*3;
		if (!feof(fs)) fscanf(fs, "%lf \t %lf \t %lf\n", &_patch[j], &_patch[j+1], &_patch[j+2]);
		else 
			{
			_patch[j] = 0;
			_patch[j+1] = 0; 
			_patch[j+2] = 0;
			}
		}
		
	McoUnlockHandle(dataH);	
		
	if (linearDataH != 0L)
		{	
		_patch = (double*)McoLockHandle(linearDataH);
		
		for (k=0; k<format.numc; k++)
			{
			for (i=0; i<format.numLinear-1; i++) // patches do not include 0%
				{
				j = k*3*(format.numLinear-1)+i*3;
				if (!feof(fs)) fscanf(fs, "%lf \t %lf \t %lf\n", &_patch[j], &_patch[j+1], &_patch[j+2]);
				else 
					{
					_patch[j] = 0;
					_patch[j+1] = 0; 
					_patch[j+2] = 0;
					}
				}
			}
		McoUnlockHandle(linearDataH);
		}	

	patches.strips = 1;
	patches.sheets = 100000;
	patches.rows = 32;
	patches.columns = (format.total + format.numLinear*format.numc)/32;
	if ((format.total + format.numLinear*format.numc)%32) patches.columns++;
	patches.starting = 0;
	patches.direction = 1;
	patches.width = 100;
	patches.height = 100;

bail:
	delete dims;	
	return status;
}

McoStatus RawData::loadStruct(void *data)
{
	typedef struct{
		long patchtype;	//rgb or cmyk
		long numcomp;	//num of components
		long numcube;	//num of cmy cubes with different black
		long numlinear;	//num of linear patches
		long *steps;	//num of steps in each cube with different black
		double *blacks;	//black values for different cmy cubes
		double *cmy;	//cmy values for different black
		double *linear;	//linear patches
		double *data;	//cmyk data
		double *ldata;	//all the linear data
	} PATCH;
	PATCH *in;
	McoStatus 			status;
	Str255				inname;
	short				i,j,k,m;
	double *patch, *patchcopy;
	int32				type,numc,numslow,numlinear;
	int32				*dims,total = 0, dimCount = 0;
	int32				dimp = 1;
	RawData 			*patchd;
	double				*_patch;
	
	in = (PATCH*)data;

	type = in->patchtype;	//rgb or cmyk
	numc = in->numcomp;	//num of components
	numslow = in->numcube;	//num of cmy cubes with different black
	numlinear = in->numlinear;	//num of linear patches
	
	dims = new int32 [numslow];
	
	for (i=0; i<numslow; i++)
		dims[i] = in->steps[i];

	status = SetUp(type,numc,numslow,numlinear,dims);

	_patch = (double*)McoLockHandle(dataH);
	
	for (i=0; i<numslow; i++)
		format.slowValues[i] = in->blacks[i];
	
	m = 0;	
	for (i=0; i<numslow; i++) 
		{
		for (j=0; j<format.cubedim[i]; j++)
			{
				format.cmpValues[m] = in->cmy[m];
				m++;
			}
		}
		
	for (i=0; i<format.numLinear; i++)
		{
			format.linearValues[i] = in->linear[i];	
		}
		
	for(i = 0; i < format.total; i++){
		j = i*3;
		_patch[j] = in->data[j];
		_patch[j+1] = in->data[j+1]; 
		_patch[j+2] = in->data[j+2];
	}
		
	McoUnlockHandle(dataH);	
		
	if (linearDataH != NULL)
		{	
		_patch = (double*)McoLockHandle(linearDataH);
		
		for (k=0; k<format.numc; k++)
			{
			for (i=0; i<format.numLinear-1; i++) // patches do not include 0%
				{
				j = k*3*(format.numLinear-1)+i*3;
				_patch[j] = in->ldata[j];
				_patch[j+1] = in->ldata[j+1]; 
				_patch[j+2] = in->ldata[j+2];
				}
			}
		McoUnlockHandle(linearDataH);
		}	



	delete dims;	
	return MCO_SUCCESS;
}

McoStatus RawData::SetDesc(char *d)
{
if (strlen(d) > 100) return MCO_FAILURE;
strcpy(desc,d);
return MCO_SUCCESS;
}

McoStatus RawData::GetDesc(char *d)
{
strcpy(d,desc);
return MCO_SUCCESS;
}


McoStatus RawData::readICCFile(McoXformIcc *link)
{
int i,j,k,l,m;
McoStatus state = MCO_SUCCESS;
//McoColorSync *lab = 0L,*list[2];
//McoXformIcc	*prof_lab = 0L;
double	*ref = 0L;
double	*data;
unsigned char	cmyk[4];
unsigned char	labv[4];

/*lab = new McoColorSync;
if (!lab) return MCO_MEM_ALLOC_ERROR;
state = lab->GetProfile(LAB_PROFILE_NAME);
if (state) goto bail;

prof_lab = new McoXformIcc;
if (!prof_lab) return MCO_MEM_ALLOC_ERROR;
list[0] = prof;
list[1] = lab;

state = prof_lab->buildCW(list,2);	
if (state) goto bail;
*/

if (format.total > 0)
	{
	ref = new double[4*format.total];
	if (!ref) return MCO_MEM_ALLOC_ERROR;

	m = 0;
	if (ordering == Order_MCYK)
		{
		for (l=0; l<format.numslow; l++)
			{
			for (i=0; i<format.cubedim[l]; i++) 
				{
				for (j=0; j<format.cubedim[l]; j++)
					{
					for (k=0; k<format.cubedim[l]; k++)
						{
						ref[m++] = getcmpValues(l,j);
						ref[m++] = getcmpValues(l,k);
						ref[m++] = getcmpValues(l,i);
						ref[m++] = format.slowValues[l];
						}
					}
				}
			}	
		}
	else return MCO_FAILURE;
	
	if (!dataH) return MCO_OBJECT_NOT_INITIALIZED;
	data = (double*)McoLockHandle(dataH);
	
	for (i=0; i<format.total; i++)
		{
		cmyk[0] = (unsigned char)(2.55*ref[i*4]);
		cmyk[1] = (unsigned char)(2.55*ref[i*4+1]);
		cmyk[2] = (unsigned char)(2.55*ref[i*4+2]);
		cmyk[3] = (unsigned char)(2.55*ref[i*4+3]);
		
		state = link->evalColors(cmyk,labv,1);
		if (state) goto bail;
	
		data[i*3] = labv[0]/2.55;
		data[i*3+1] = labv[1] - 128.0;
		data[i*3+2] = labv[2] - 128.0;
		}
	McoUnlockHandle(dataH);
	delete ref;
	ref = 0L;
	}

if (format.numLinear > 0)
	{
	
	m = 0;
	if (!linearDataH) return MCO_OBJECT_NOT_INITIALIZED;
	data = (double*)McoLockHandle(linearDataH);
	
	
	for (i=0; i<format.numc; i++)
		{
		for (j=0; j<format.numc; j++) cmyk[j] = 0;
		for (j=1; j<format.numLinear; j++)
			{
			cmyk[i] = 255.0*(double)j/(format.numLinear-1);
			state = link->evalColors(cmyk,labv,1);
			if (state) goto bail;
	
			data[m*3] = labv[0]/2.55;
			data[m*3+1] = labv[1] - 128.0;
			data[m*3+2] = labv[2] - 128.0;
			m++;
			}
		}
	McoUnlockHandle(linearDataH);
	}
	

bail:
//if (lab) delete lab;
//if (prof_lab) delete prof_lab;
if (ref) delete ref;

return state;
}




