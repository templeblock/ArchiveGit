////////////////////////////////////////////////////////////////////////////
//	calib_multi.c														  //
//																		  //
//	calibrate by building multiple calibrations and and blending the 	  //
//  calibrations together												  //
//																		  //
//  Created on Feb 16, 1996												  //
//  By James Vogh														  //
////////////////////////////////////////////////////////////////////////////

#include "monacoprint.h"
//#include "preprocess.h"
#include <math.h>
#include "calib_multi.h"

//#include "mem_dbg.h"


#define NUM_INTERATIONS 40
#define MIN_DIST 0.005

//#define NUM_INTERATIONS 2
//#define MIN_DIST 1


#define START_CMY_SEARCH_D 0.25

// WARNING!! do not leave at 2
#define MAX_COUNT 60
//#define MIN_D	0.04
#define MIN_D	0.1
//#define MIN_SPEED 0.0001
#define MIN_SPEED 0.001

// The base calibration functions

calib_base::calib_base(void)
{
//calib = 0L;
}

calib_base::~calib_base(void)
{
//if (calib != 0L) delete calib;
}



McoStatus calib_base::interpolate_9(double *cmyk,double *lab)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;



a = 10*cmyk[0];
if (a < 0) a = 0;
if (a > 10) a = 10;
b = 10*cmyk[1];
if (b < 0) b = 0;
if (b > 10) b = 10;
c = 10*cmyk[2];
if (c < 0) c = 0;
if (c > 10) c = 10;

a1 = a;
if (a1 == 7) a1 = 6;
if (a1 == 9) a1 = 8;
ai1 = a1;
if (ai1 > 8)
	{
	ai1 = 8;
	}
else if (ai1 > 6) ai1 = 7;
a2 = a1+1;
if (a2 == 7) a2 = 8;
if (a2 == 9) a2 = 10;
ai2 = a2;
if (ai2 > 8)
	{
	ai2 = 8;
	}
else if (ai2 > 6) ai2 = 7;

b1 = b;
if (b1 == 7) b1 = 6;
if (b1 == 9) b1 = 8;
bi1 = b1;
if (bi1 > 8)
	{
	bi1 = 8;
	}
else if (bi1 > 6) bi1 = 7;
b2 = b1+1;
if (b2 == 7) b2 = 8;
if (b2 == 9) b2 = 10;
bi2 = b2;
if (bi2 > 8)
	{
	bi2 = 8;
	}
else if (bi2 > 6) bi2 = 7;

c1 = c;
if (c1 == 7) c1 = 6;
if (c1 == 9) c1 = 8;
ci1 = c1;
if (ci1 > 8)
	{
	ci1 = 8;
	}
else if (ci1 > 6) ci1 = 7;
c2 = c1+1;
if (c2 == 7) c2 = 8;
if (c2 == 9) c2 = 10;
ci2 = c2;
if (ci2 > 8)
	{
	ci2 = 8;
	}
else if (ci2 > 6) ci2 = 7;


t = (double)(a - a1)/(a2-a1);
u = (double)(b - b1)/(b2-b1);
v = (double)(c - c1)/(c2-c1);


lab[0] = (1-t)*(1-u)*(1-v)*_patch[ai1*9*3+bi1*3+ci1*81*3] + 
				 (1-t)*(1-u)*v*_patch[ai1*9*3+bi1*3+ci2*81*3] +
				 (1-t)*u*(1-v)*_patch[ai1*9*3+bi2*3+ci1*81*3] +
				 (1-t)*u*v*_patch[ai1*9*3+bi2*3+ci2*81*3] +
				 t*(1-u)*(1-v)*_patch[ai2*9*3+bi1*3+ci1*81*3] +
				 t*(1-u)*v*_patch[ai2*9*3+bi1*3+ci2*81*3] +
				 t*u*(1-v)*_patch[ai2*9*3+bi2*3+ci1*81*3] +
				 t*u*v*_patch[ai2*9*3+bi2*3+ci2*81*3];						

lab[1] = (1-t)*(1-u)*(1-v)*_patch[ai1*9*3+bi1*3+ci1*81*3+1] + 
				 (1-t)*(1-u)*v*_patch[ai1*9*3+bi1*3+ci2*81*3+1] +
				 (1-t)*u*(1-v)*_patch[ai1*9*3+bi2*3+ci1*81*3+1] +
				 (1-t)*u*v*_patch[ai1*9*3+bi2*3+ci2*81*3+1] +
				 t*(1-u)*(1-v)*_patch[ai2*9*3+bi1*3+ci1*81*3+1] +
				 t*(1-u)*v*_patch[ai2*9*3+bi1*3+ci2*81*3+1] +
				 t*u*(1-v)*_patch[ai2*9*3+bi2*3+ci1*81*3+1] +
				 t*u*v*_patch[ai2*9*3+bi2*3+ci2*81*3+1];
				 
lab[2] = (1-t)*(1-u)*(1-v)*_patch[ai1*9*3+bi1*3+ci1*81*3+2] + 
				 (1-t)*(1-u)*v*_patch[ai1*9*3+bi1*3+ci2*81*3+2] +
				 (1-t)*u*(1-v)*_patch[ai1*9*3+bi2*3+ci1*81*3+2] +
				 (1-t)*u*v*_patch[ai1*9*3+bi2*3+ci2*81*3+2] +
				 t*(1-u)*(1-v)*_patch[ai2*9*3+bi1*3+ci1*81*3+2] +
				 t*(1-u)*v*_patch[ai2*9*3+bi1*3+ci2*81*3+2] +
				 t*u*(1-v)*_patch[ai2*9*3+bi2*3+ci1*81*3+2] +
				 t*u*v*_patch[ai2*9*3+bi2*3+ci2*81*3+2];

return MCO_SUCCESS;							 
}


McoStatus calib_base::interpolate_33(double *cmyk,double *lab,double *table)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
double inv_max_cal = 32/max_cal;

if (direction)
	{
	a = inv_max_cal - cmyk[0]*inv_max_cal;
	b = inv_max_cal - cmyk[1]*inv_max_cal;
	c = inv_max_cal - cmyk[2]*inv_max_cal;
	}
else
	{
	a = cmyk[0]*inv_max_cal;
	b = cmyk[1]*inv_max_cal;
	c = cmyk[2]*inv_max_cal;
	}


if (a >= 32) a = 31.999999;
if (a < 0) a = 0;
if (b >= 32) b = 31.999999;
if (b < 0) b = 0;
if (c >= 32) c = 31.999999;
if (c < 0) c = 0;
			
a1 = ((int32)a)*3267;
a2 = a1 + 3267;
b1 = ((int32)b)*99;
b2 = b1 + 99;
c1 = ((int32)c)*3;
c2 = c1 + 3;

t = a - (int32)a;
u = b - (int32)b;
v = c - (int32)c;

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1+c1;
p2 = table +a1+b1+c2;
p3 = table +a1+b2+c1;
p4 = table +a1+b2+c2;
p5 = table +a2+b1+c1;
p6 = table +a2+b1+c2;
p7 = table +a2+b2+c1;
p8 = table +a2+b2+c2;
			
lab[0] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
lab[1] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
lab[2] = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
return MCO_SUCCESS;							 
}



McoStatus calib_base::interpolate_3D_n(double *cmyk,double *lab,double *table,int32 n)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai1,ai2,bi1,bi2,ci1,ci2;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
double inv_max_cal = (n-1)/max_cal;

if (direction)
	{
	a = (n-1) - cmyk[0]*inv_max_cal;
	b = (n-1) - cmyk[1]*inv_max_cal;
	c = (n-1) - cmyk[2]*inv_max_cal;
	}
else
	{
	a = cmyk[2]*inv_max_cal;
	b = cmyk[1]*inv_max_cal;
	c = cmyk[0]*inv_max_cal;
	}


if (a >= (n-1)) a = n-1.000000000001;
if (a < 0) a = 0;
if (b >= (n-1)) b = n-1.000000000001;
if (b < 0) b = 0;
if (c >= (n-1)) c = n-1.000000000001;
if (c < 0) c = 0;
			
a1 = ((int32)a)*3*n*n;
a2 = a1 + 3*n*n;
b1 = ((int32)b)*3*n;
b2 = b1 + 3*n;
c1 = ((int32)c)*3;
c2 = c1 + 3;

t = a - (int32)a;
u = b - (int32)b;
v = c - (int32)c;

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1+c1;
p2 = table +a1+b1+c2;
p3 = table +a1+b2+c1;
p4 = table +a1+b2+c2;
p5 = table +a2+b1+c1;
p6 = table +a2+b1+c2;
p7 = table +a2+b2+c1;
p8 = table +a2+b2+c2;
			
lab[0] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
lab[1] = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
lab[2] = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
return MCO_SUCCESS;							 
}

	// some values that are set here for speed
double		int_c,int_m,int_y;


inline void calib_base::interpolate_3D_n_1(double *lab,double *table)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai,bi,ci;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
int32	nn_3;
int32	n = n1;

a = (n-1) - int_c*inv_max_cal_1;
a*=0.99999999999999;
b = (n-1) - int_m*inv_max_cal_1;
b*=0.99999999999999;
c = (n-1) - int_y*inv_max_cal_1;
c*=0.99999999999999;

nn_3 = 3*n*n;	

ai = (int32)a;	
bi = (int32)b;	
ci = (int32)c;		
		
a1 = ai*nn_3;
a2 = a1 + nn_3;
b1 = bi*3*n;
b2 = b1 + 3*n;
c1 = ci*3;
c2 = c1 + 3;

t = a - ai;
u = b - bi;
v = c - ci;

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1;
p2 = p1+c2;
p1 += c1;
p3 = table +a1+b2;
p4 = p3+c2;
p3 += c1;
p5 = table +a2+b1;
p6 = p5+c2;
p5 += c1;
p7 = table +a2+b2;
p8 = p7+c2;
p7 += c1;
			
*lab++ = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
*lab++ = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
*lab = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
						 
}


inline  void calib_base::interpolate_3D_n_0(double *lab,double *table)
{
double	a,b,c;
int32	a1,a2,b1,b2,c1,c2;
int32	ai,bi,ci;
double t,u,v;
double	r1,r2,r3,r4,r5,r6,r7,r8;
double	*p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
int32	n = n0;
int32	nn_3;

a = int_y*inv_max_cal_0;
b = int_m*inv_max_cal_0;
c = int_c*inv_max_cal_0;

ai = (int32)a;
bi = (int32)b;
ci = (int32)c;

nn_3 = n*n*3;
			
a1 = ai*nn_3;
a2 = a1 + nn_3;
b1 = bi*3*n;
b2 = b1 + 3*n;
c1 = ci*3;
c2 = c1 + 3;

t = a - floor(a);
u = b - floor(b);
v = c - floor(c);

r1 = (1-u)*(1-v);
r2 = (1-u)*v;
r3 = u*(1-v);
r4 = u*v;
r5 = t*r1;
r6 = t*r2;
r7 = t*r3;
r8 = t*r4;
a = (1-t);
r1 = a*r1;
r2 = a*r2;
r3 = a*r3;
r4 = a*r4;

p1 = table +a1+b1;
p2 = p1+c2;
p1 += c1;
p3 = table +a1+b2;
p4 = p3+c2;
p3 += c1;
p5 = table +a2+b1;
p6 = p5+c2;
p5 += c1;
p7 = table +a2+b2;
p8 = p7+c2;
p7 += c1;
			
*lab++ = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));
	
*lab++ = r1*(*(p1++)) +
	r2*(*(p2++)) +
	r3*(*(p3++)) +
	r4*(*(p4++)) +
	r5*(*(p5++)) +
	r6*(*(p6++)) +
	r7*(*(p7++)) +
	r8*(*(p8++));	
	
*lab = r1*(*(p1)) +
	r2*(*(p2)) +
	r3*(*(p3)) +
	r4*(*(p4)) +
	r5*(*(p5)) +
	r6*(*(p6)) +
	r7*(*(p7)) +
	r8*(*(p8));
						 
}



// do a 4-D interpolation for 33x33x33x33 data
// this code calls the 3-D interpolation code
McoStatus calib_base::interpolate_4D(double *cmyk,double *lab,double *table_cmy,double *table_cmyk)
{
double *table1,*table2;
int32	Ki1,Ki2;
double  Kv,Kd;
double	lab1[3],lab2[3];
McoStatus state;
int32	tdirect;

Kv = cmyk[3]/100.0;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;

table1 = table_cmyk+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = table_cmyk+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

if (Ki1 == 0) 
	{
	tdirect = direction;
	direction = 1;
	state = interpolate_3D_n(cmyk,lab1,table_cmy,33);
	direction = tdirect;
	}
else state = interpolate_3D_n(cmyk,lab1,table1,BT_DIM);
if (state != MCO_SUCCESS) return state;
state = interpolate_3D_n(cmyk,lab2,table2,BT_DIM);
if (state != MCO_SUCCESS) return state;

lab[0] = lab1[0]*(1-Kd)+lab2[0]*Kd;
lab[1] = lab1[1]*(1-Kd)+lab2[1]*Kd;
lab[2] = lab1[2]*(1-Kd)+lab2[2]*Kd;
return state;
}

McoStatus calib_inter4::getMinMaxK(double *lab, double *minK,double *maxK,short *gamut)
{
return TableIndex->GetMinMaxK(lab,minK,maxK,gamut);
}

double calib_inter4::getSpacing(void)
{
return TableIndex->getSpacing();
}



// find the CMY value that is the best match to the lab value within the table
McoStatus calib_inter4::getBestCMYK(double *lab,double K,double *cmy,double *distance)
{
register int i,j,k,min_c,min_m,min_y,min_i;
register double	temp,d,min_d;
register double	L1,a1,b1,L2,a2,b2;
register int32 index;
double *table1,*table2;
double *table1p,*table2p;
int32	Ki1,Ki2;
double  Kv,Kd,One_M_Kd;
int32	c,m,y;
double  bt_1,bt_2,btbt;
double  ink_sum,ink_mult;
McoStatus status;
int32		m1,m2,im,jm;
double	K_comp;



int cmy_index[6];
int cm,cp,mm,mp,ym,yp;
//if (K > -0.00001)
//	{

status = TableIndex->GetIndex(lab,K,cmy_index);
if (status != MCO_SUCCESS) return status;

// check to see if out of gamut
if (cmy_index[0] != -1) 
	{	
	cm = cmy_index[0];
	cp = cmy_index[1]+1;
	mm = cmy_index[2];
	mp = cmy_index[3]+1;
	ym = cmy_index[4];
	yp = cmy_index[5]+1;
	}
else 
	{
	cm = 0;
	cp = BT_DIM-1;
	mm = 0;
	mp = BT_DIM-1;
	ym = 0;
	yp = BT_DIM-1;
	}

	
Kv = K*0.01;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;

One_M_Kd = 1-Kd;

// the address of the two tables
table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

L1 = lab[0];
a1 = lab[1];
b1 = lab[2];

m1 = BT_DIM*BT_DIM*3;
m2 = BT_DIM*3;

if (K+300 <= max_ink) // no ink limiting problem
	{
	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;;
				d = (L2-L1);
				d *=d;
				temp = (a2-a1);
				temp *= temp;
				d += temp;
				temp = (b2-b1);
				temp *= temp;
				d+= temp;
				if (d < min_d)
					{
					min_d = d;
					min_c = k;
					min_m = j;
					min_y = i;
					}
				}
			jm += m2;
			}
		im += m1;
		}
	if (direction)
		{
		c = min_y;
		m = min_m;
		y = min_c;
		}
	else
		{
		y = min_y;
		m = min_m;
		c = min_c;
		}
	cmy[0] = max_cal*(double)c/(BT_DIM-1);
	cmy[1] = max_cal*(double)m/(BT_DIM-1);
	cmy[2] = max_cal*(double)y/(BT_DIM-1);

	}
else  // have a ink limiting situation here
	{
	K_comp = linear_data->CnvtLinearInv(K,3);	
	bt_1 = max_cal/(double)(BT_DIM-1);
	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				ink_sum = C_Lut[i];
				ink_sum += M_Lut[j];
				ink_sum += Y_Lut[k];
				ink_sum += K_comp;
				
								
				if (ink_sum <= max_ink)
					{
				
					L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					d = (L2-L1);
					d *=d;
					temp = (a2-a1);
					temp *= temp;
					d += temp;
					temp = (b2-b1);
					temp *= temp;
					d+= temp;
					// calculate total ink


					if (d < min_d)
						{
						min_d = d;
						min_c = k;
						min_m = j;
						min_y = i;
						}
					}
				}
			jm += m2;
			}
		im += m1;
		}
	if (direction)
		{
		c = min_y;
		m = min_m;
		y = min_c;
		}
	else
		{
		y = min_y;
		m = min_m;
		c = min_c;
		}
	cmy[0] = max_cal*(double)c/(BT_DIM-1);
	cmy[1] = max_cal*(double)m/(BT_DIM-1);
	cmy[2] = max_cal*(double)y/(BT_DIM-1);	
	}
// calculate the CMY values	from i
	
*distance = min_d;	
return MCO_SUCCESS;
}

/*

// find the CMY value that is the best match to the lab value within the table
McoStatus calib_inter4::getBestCMYK(double *lab,double K,double *cmy,double *distance)
{
register int i,min_i;
register double	temp,d,min_d;
register double	L1,a1,b1,L2,a2,b2;
double *table1,*table2;
int32	Ki1,Ki2;
double  Kv,Kd,One_M_Kd;
int32	c,m,y;
double  bt_1,bt_2,btbt;
double  ink_sum,ink_mult;

//if (K > -0.00001)
//	{

Kv = K*0.01;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;

One_M_Kd = 1-Kd;

// the address of the two tables
table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

L1 = lab[0];
a1 = lab[1];
b1 = lab[2];

if (K+300 <= max_ink) // no ink limiting problem
	{
	// do 1D interpolation and search
	min_d = 1.0E20;
	for (i=0; i<BT_DIM*BT_DIM*BT_DIM; i++)
		{
		L2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
		a2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
		b2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
		d = (L2-L1);
		d *=d;
		temp = (a2-a1);
		temp *= temp;
		d += temp;
		temp = (b2-b1);
		temp *= temp;
		d+= temp;
		if (d < min_d)
			{
			min_d = d;
			min_i = i;
			}
		}
	if (direction)
		{
		c = min_i/(BT_DIM*BT_DIM);
		m = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
		y = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
		}
	else
		{
		y = min_i/(BT_DIM*BT_DIM);
		m = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
		c = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
		}
	cmy[0] = max_cal*(double)c/(BT_DIM-1);
	cmy[1] = max_cal*(double)m/(BT_DIM-1);
	cmy[2] = max_cal*(double)y/(BT_DIM-1);

	}
else  // have a ink limiting situation here
	{	
	bt_1 = 1/(double)BT_DIM;
	bt_2 = 1/(double)(BT_DIM*BT_DIM);
	ink_mult = max_cal/(double)(BT_DIM-1);
	// do 1D interpolation and search
	min_d = 1.0E20;
	for (i=0; i<BT_DIM*BT_DIM*BT_DIM; i++)
		{
		ink_sum = i*bt_2;
		ink_sum += (i%(BT_DIM*BT_DIM))*bt_1;
		ink_sum += (i%(BT_DIM*BT_DIM))%BT_DIM;
		ink_sum *= ink_mult;
		ink_sum += K;
		
		if (ink_sum <= max_ink)
			{
		
			L2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
			a2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
			b2 = (*(table1++))*One_M_Kd+(*(table2++))*Kd;
			d = (L2-L1);
			d *=d;
			temp = (a2-a1);
			temp *= temp;
			d += temp;
			temp = (b2-b1);
			temp *= temp;
			d+= temp;
			// calculate total ink


			if (d < min_d)
				{
				min_d = d;
				min_i = i;
				}
			}
		}
	if (direction)
		{
		c = min_i/(BT_DIM*BT_DIM);
		m = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
		y = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
		}
	else
		{
		y = min_i/(BT_DIM*BT_DIM);
		m = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
		c = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
		}
	cmy[0] = max_cal*(double)c/(BT_DIM-1);
	cmy[1] = max_cal*(double)m/(BT_DIM-1);
	cmy[2] = max_cal*(double)y/(BT_DIM-1);	
	}
// calculate the CMY values	from i
	
*distance = min_d;	
return MCO_SUCCESS;
}

*/

// find the CMY value that is the best match to the lab value within the table
double calib_inter4::getDistance(double *lab,double K,double max_dist)
{
McoStatus status = MCO_SUCCESS;
register int i,j,k,min_i;
register double	temp,d,min_d,sd;
register double	L1,a1,b1,L2,a2,b2;
double *table1,*table2;
double *table1p,*table2p;
int32	Ki1,Ki2;
double  Kv,Kd,One_M_Kd;
double  bt_1;
double  ink_sum,ink_mult;
double	dist = 0;
int cmy_index[6];
int cm,cp,mm,mp,ym,yp;
int32		index,m1,m2,im,jm;
double	K_comp;


status = TableIndex->GetIndex(lab,K,cmy_index);
if (status != MCO_SUCCESS) return status;

if (cmy_index[0] != -1) 
	{	
	cm = cmy_index[0];
	cp = cmy_index[1]+1;
	mm = cmy_index[2];
	mp = cmy_index[3]+1;
	ym = cmy_index[4];
	yp = cmy_index[5]+1;
	}
else 
	{
	cm = 0;
	cp = BT_DIM-1;
	mm = 0;
	mp = BT_DIM-1;
	ym = 0;
	yp = BT_DIM-1;
	}

m1 = BT_DIM*BT_DIM*3;
m2 = BT_DIM*3;

Kv = K*0.01;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;

One_M_Kd = 1-Kd;

// the address of the two tables
table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

L1 = lab[0];
a1 = lab[1];
b1 = lab[2];

if (K+300 <= max_ink) // no ink limiting problem
	{
	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				d = (L2-L1);
				d *=d;
				temp = (a2-a1);
				temp *= temp;
				d += temp;
				temp = (b2-b1);
				temp *= temp;
				d+= temp;
				if (d < min_d)
					{
					min_d = d;
					if (min_d < max_dist) return min_d;	
					}
				}
			jm += m2;
			}
		im += m1;
		}
	}
else  // have a ink limiting situation here
	{
	K_comp = linear_data->CnvtLinearInv(K,3);	
	bt_1 = max_cal/(double)(BT_DIM-1);	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				ink_sum = C_Lut[i];
				ink_sum += M_Lut[j];
				ink_sum += Y_Lut[k];
				ink_sum += K_comp;
				
						
				if (ink_sum <= max_ink)
					{	
					L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					d = (L2-L1);
					d *=d;
					temp = (a2-a1);
					temp *= temp;
					d += temp;
					temp = (b2-b1);
					temp *= temp;
					d+= temp;
					// calculate total ink

					if (d < min_d)
						{
						min_d = d;
						if (min_d < max_dist) return min_d;		
						}
					}
				}
			jm += m2;
			}
		im += m1;
		}
	}
// calculate the CMY values	from i
	
return min_d;
}


// find the CMY value that is the best match to the lab value within the table
double calib_inter4::getSatDistance(double *lab,double K)
{
McoStatus status = MCO_SUCCESS;
register int i,j,k,min_i;
register double	temp,d,min_d,sd;
register double	L1,a1,b1,L2,a2,b2;
double *table1,*table2;
double *table1p,*table2p;
int32	Ki1,Ki2;
double  Kv,Kd,One_M_Kd;
double  bt_1;
double  ink_sum;
double	dist = 0;
int cmy_index[6];
int cm,cp,mm,mp,ym,yp;
int32		index,m1,m2,im,jm;
double		K_comp;

//if (K > -0.00001)
//	{

status = TableIndex->GetIndex(lab,K,cmy_index);
if (status != MCO_SUCCESS) return status;

if (cmy_index[0] != -1) 
	{	
	cm = cmy_index[0];
	cp = cmy_index[1]+1;
	mm = cmy_index[2];
	mp = cmy_index[3]+1;
	ym = cmy_index[4];
	yp = cmy_index[5]+1;
	}
else 
	{
	return 1000000;
	}

m1 = BT_DIM*BT_DIM*3;
m2 = BT_DIM*3;

Kv = K*0.01;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;

One_M_Kd = 1-Kd;

// the address of the two tables
table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

L1 = lab[0];
a1 = lab[1];
b1 = lab[2];

if (K+300 <= max_ink) // no ink limiting problem
	{
	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
				d = (a2-a1);
				d *=d;
				temp = (b2-b1);
				temp *= temp;
				d += temp;
				sd = d;
				temp = 3*(L2-L1); 	// tring to serch at the same value of L 
				temp *= temp;
				d+= temp;
				if (d < min_d)
					{
					min_d = d;
					dist = sd;		
					}
				}
			jm += m2;
			}
		im += m1;
		}
	}
else  // have a ink limiting situation here
	{
	K_comp = linear_data->CnvtLinearInv(K,3);	
	bt_1 = max_cal/(double)(BT_DIM-1);	// do 1D interpolation and search
	min_d = 1.0E20;
	im = m1*ym;
	for (i=ym; i<yp; i++)
		{
		jm = mm*m2;
		for (j=mm; j<mp; j++)
			{
			index = im+jm+cm*3;
			table1p = table1+index;
			table2p = table2+index;
			for (k=cm; k<cp; k++)
				{
				ink_sum = C_Lut[i];
				ink_sum += M_Lut[j];
				ink_sum += Y_Lut[k];
				ink_sum += K_comp;				
								
				if (ink_sum <= max_ink)
					{	
					L2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					a2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					b2 = (*(table1p++))*One_M_Kd+(*(table2p++))*Kd;
					d = (a2-a1);
					d *=d;
					temp = (b2-b1);
					temp *= temp;
					d += temp;
					sd = d;
					temp = 3*(L2-L1); // tring to serch at the same value of L 
					temp *= temp;
					d+= temp;
					// calculate total ink


					if (d < min_d)
						{
						min_d = d;
						dist = sd;	
						}
					}
				}
			jm += m2;
			}
		im += m1;
		}
	}
// calculate the CMY values	from i
	
return dist;
}



// find the best solution by solving in CMY space
McoStatus calib_inter4::evalCMYSpace(double *lab,double *cmyk,double *cmy,double *dist)
{
register int32 	i,j,k;
register double L,a,b,L2,a2,b2;
double	K;
double cmyk2[4],cmy2[4];
double lab2[3],lab3[3],lab4[3];
double *lab3p,*lab4p;
int32	count = 0;
double	min_d,last_d,best_d,d,temp;
double	speed;
double  ink_sum;


double *table1,*table2;
int32	Ki1,Ki2;
double  Kv,Kd,One_Kd;


Kv = cmyk[3]*0.01;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;
One_Kd = 1-Kd;

table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

K = cmyk[3];
cmyk2[3] = K;
*dist = 0;
// get the best cmyk value
getBestCMYK(lab, K,cmy,dist);
//return MCO_SUCCESS;

speed = 0.0025*max_cal;

last_d = 1E20;

cmy2[0] = cmy[0];
cmy2[1] = cmy[1];
cmy2[2] = cmy[2];

inv_max_cal_0 = (BT_DIM-1.0000000000001)/max_cal;
inv_max_cal_1 = 32/max_cal;

L = lab[0];
a = lab[1];
b = lab[2];

// dimensions for interpolation
n0 = BT_DIM;
n1 = 33;


if (Ki1 == 0)
	{
	do 
		{

		min_d = 1E20;
		for (i=-1; i<2; i++)
			{
			int_c = cmy[0] + i*speed;
			if ((int_c >=0) && (int_c <= max_cal)) for (j=-1; j<2; j++)
				{
				int_m = cmy[1] + j*speed;
				if ((int_m >=0) && (int_m <= max_cal)) for (k=-1; k<2; k++) if (i!=0 || j!=0 || k!=0)
					{
					int_y = cmy[2] + k*speed;
					ink_sum = K+int_c+int_m+int_y;
					if ((int_y >=0) && (int_y <= max_cal) && (ink_sum <= max_ink))
						{
						interpolate_3D_n_1(lab3,cmykTable);
						interpolate_3D_n_0(lab4,table2);
						lab3p = lab3;
						lab4p = lab4;
						L2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						a2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						b2 = (*lab3p)*One_Kd+(*lab4p)*Kd;
						d = (L-L2);
						d *= d;
						temp = (a-a2);
						temp *= temp;
						d += temp;
						temp = (b-b2);
						temp *= temp;
						d += temp;
						if ((d < min_d) && (d < last_d))
							{
							cmy2[0] = int_c;
							cmy2[1] = int_m;
							cmy2[2] = int_y;
							min_d = d;
							best_d = d;
							}
						}
					}
				}
			}
		cmy[0] = cmy2[0];
		cmy[1] = cmy2[1];
		cmy[2] = cmy2[2];
		
		// decrease speed if nothing better was found
		if (min_d == 1E20) speed *= 0.5;
		
		last_d = best_d;
		
		count++;	
		} while ((count<MAX_COUNT) && (min_d > MIN_D) && (speed > MIN_SPEED*max_cal));
	}
else 
	{
	do 
		{

		min_d = 1E20;
		for (i=-1; i<2; i++)
			{
			int_c = cmy[0] + i*speed;
			if ((int_c >=0) && (int_c <= max_cal)) for (j=-1; j<2; j++)
				{
				int_m = cmy[1] + j*speed;
				if ((int_m >=0) && (int_m <= max_cal)) for (k=-1; k<2; k++) if (i!=0 || j!=0 || k!=0)
					{
					int_y = cmy[2] + k*speed;
					ink_sum = K+int_c+int_m+int_y;
					if ((int_y >=0) && (int_y <= max_cal) && (ink_sum <= max_ink))
						{
						interpolate_3D_n_0(lab3,table1);
						interpolate_3D_n_0(lab4,table2);
						lab3p = lab3;
						lab4p = lab4;
						L2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						a2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						b2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						d = (L-L2);
						d *= d;
						temp = (a-a2);
						temp *= temp;
						d += temp;
						temp = (b-b2);
						temp *= temp;
						d += temp;
						if ((d < min_d) && (d < last_d))
							{
							cmy2[0] = int_c;
							cmy2[1] = int_m;
							cmy2[2] = int_y;
							min_d = d;
							best_d = d;
							}
						}
					}
				}
			}		
			
		cmy[0] = cmy2[0];
		cmy[1] = cmy2[1];
		cmy[2] = cmy2[2];
		
		// decrease speed if nothing better was found
		if (min_d == 1E20) speed *= 0.5;
		
		last_d = best_d;
		
		count++;	
		} while ((count<MAX_COUNT) && (min_d > MIN_D) && (speed > MIN_SPEED*max_cal));
	}
	
cmyk[0] = cmy2[0];	
cmyk[1] = cmy2[1];	
cmyk[2] = cmy2[2];	

*dist = best_d;
	
return MCO_SUCCESS;
}


// find the best solution by solving in CMY space
McoStatus calib_inter4::evalCMYSpace(double *lab,double *cmyk)
{
register int32 	i,j,k;
register double L,a,b,L2,a2,b2;
double	K;
double cmy[4];
double cmyk2[4],cmy2[4];
double lab2[3],lab3[3],lab4[3];
double *lab3p,*lab4p;
int32	count = 0;
double	min_d,last_d,best_d,d,temp;
double	speed;
double	ink_sum;


double *table1,*table2;
int32	Ki1,Ki2;
double  Kv,Kd,One_Kd;


Kv = cmyk[3]/100.0;
if (Kv > 1.0) Kv = 1.0;
if (Kv < 0.0) Kv = 0.0;
if (Kv >= 1.0) Ki1 = BT_DIM-2;
else if (Kv <= 0.0) Ki1 = 0;
else Ki1 = (int32)(Kv*(BT_DIM-1));

Ki2 = Ki1+1;

Kd = Kv*(BT_DIM-1)-(double)Ki1;
One_Kd = 1-Kd;

table1 = cmykTableBig+Ki1*BT_DIM*BT_DIM*BT_DIM*3;
table2 = cmykTableBig+Ki2*BT_DIM*BT_DIM*BT_DIM*3;

K = cmyk[3];
cmyk2[3] = K;

// get the best cmyk value
//getBestCMYK(lab, K,cmy,dist);
//return MCO_SUCCESS;
cmy[0] = cmyk[0];
cmy[1] = cmyk[1];
cmy[2] = cmyk[2];
cmy[3] = cmyk[3];

speed = 0.0025*max_cal;

last_d = 1E20;

cmy2[0] = cmy[0];
cmy2[1] = cmy[1];
cmy2[2] = cmy[2];

inv_max_cal_0 = (BT_DIM-1.0000000000001)/max_cal;
inv_max_cal_1 = 32/max_cal;

L = lab[0];
a = lab[1];
b = lab[2];

// dimensions for interpolation
n0 = BT_DIM;
n1 = 33;


if (Ki1 == 0)
	{
	do 
		{

		min_d = 1E20;
		for (i=-1; i<2; i++)
			{
			int_c = cmy[0] + i*speed;
			if ((int_c >=0) && (int_c <= max_cal)) for (j=-1; j<2; j++)
				{
				int_m = cmy[1] + j*speed;
				if ((int_m >=0) && (int_m <= max_cal)) for (k=-1; k<2; k++) if (i!=0 || j!=0 || k!=0)
					{
					int_y = cmy[2] + k*speed;
					ink_sum = K+int_c+int_m+int_y;
					if ((int_y >=0) && (int_y <= max_cal) && (ink_sum <= max_ink))
						{
						interpolate_3D_n_1(lab3,cmykTable);
						interpolate_3D_n_0(lab4,table2);
						lab3p = lab3;
						lab4p = lab4;
						L2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						a2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						b2 = (*lab3p)*One_Kd+(*lab4p)*Kd;
						d = (L-L2);
						d *= d;
						temp = (a-a2);
						temp *= temp;
						d += temp;
						temp = (b-b2);
						temp *= temp;
						d += temp;
						if ((d < min_d) && (d < last_d))
							{
							cmy2[0] = int_c;
							cmy2[1] = int_m;
							cmy2[2] = int_y;
							min_d = d;
							best_d = d;
							}
						}
					}
				}
			}
		cmy[0] = cmy2[0];
		cmy[1] = cmy2[1];
		cmy[2] = cmy2[2];
		
		// decrease speed if nothing better was found
		if (min_d == 1E20) speed *= 0.5;
		
		last_d = best_d;
		
		count++;	
		} while ((count<MAX_COUNT) && (min_d > MIN_D) && (speed > MIN_SPEED*max_cal));
	}
else 
	{
	do 
		{

		min_d = 1E20;
		for (i=-1; i<2; i++)
			{
			int_c = cmy[0] + i*speed;
			if ((int_c >=0) && (int_c <= max_cal)) for (j=-1; j<2; j++)
				{
				int_m = cmy[1] + j*speed;
				if ((int_m >=0) && (int_m <= max_cal)) for (k=-1; k<2; k++) if (i!=0 || j!=0 || k!=0)
					{
					int_y = cmy[2] + k*speed;
					ink_sum = K+int_c+int_m+int_y;
					if ((int_y >=0) && (int_y <= max_cal) && (ink_sum <= max_ink))
						{
						interpolate_3D_n_0(lab3,table1);
						interpolate_3D_n_0(lab4,table2);
						lab3p = lab3;
						lab4p = lab4;
						L2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						a2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						b2 = (*lab3p++)*One_Kd+(*lab4p++)*Kd;
						d = (L-L2);
						d *= d;
						temp = (a-a2);
						temp *= temp;
						d += temp;
						temp = (b-b2);
						temp *= temp;
						d += temp;
						if ((d < min_d) && (d < last_d))
							{
							cmy2[0] = int_c;
							cmy2[1] = int_m;
							cmy2[2] = int_y;
							min_d = d;
							best_d = d;
							}
						}
					}
				}
			}		
			
		cmy[0] = cmy2[0];
		cmy[1] = cmy2[1];
		cmy[2] = cmy2[2];
		
		// decrease speed if nothing better was found
		if (min_d == 1E20) speed *= 0.5;
		
		last_d = best_d;
		
		count++;	
		} while ((count<MAX_COUNT) && (min_d > MIN_D) && (speed > MIN_SPEED*max_cal));
	}
	
cmyk[0] = cmy2[0];	
cmyk[1] = cmy2[1];	
cmyk[2] = cmy2[2];	
	
return MCO_SUCCESS;
}



// the calibration based on 4-D interpolation

calib_inter4::calib_inter4(calib_base **cal,int dir,double max_c,double mi,BuildTableIndex *ti,LinearData	*ld)
{
//calib = cal;
_patch = 0L;
iterations = NUM_INTERATIONS;
direction = dir;
max_cal = max_c;
thermd = 0L;
max_ink = mi;
TableIndex = ti;
linear_data = ld;
}

McoStatus calib_inter4::set_up(double* rgbcolor, double *refrgbin,int32 dimension, 
							  int32 total_loc, McoHandle c_tH,McoHandle cb_tH)
{
int i,j;
double	bt_1;

_cmykTableH = c_tH;
_cmykTableBigH = cb_tH;

// set up linearization
bt_1 = max_cal/(double)(BT_DIM-1);


for (j=0; j<BT_DIM; j++)
	{
	if (direction)
		{
		C_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,0);
		M_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,1);
		Y_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,2);
		}
	else 
		{
		C_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,2);
		M_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,1);
		Y_Lut[j] = linear_data->CnvtLinearInv(j*bt_1,0);
		}
	}


return MCO_SUCCESS;
}

// convert a cmyk value to a lab value
McoStatus calib_inter4::convtCMYK_LAB(double *cmyk,double *lab)
{
double	*cmykTable;
double	*cmykTableBig;

if (_cmykTableH != 0L) cmykTable = (double*)McoLockHandle(_cmykTableH);
if (_cmykTableBigH != 0L) cmykTableBig = (double*)McoLockHandle(_cmykTableBigH);

interpolate_4D(cmyk,lab,cmykTable,cmykTableBig);

McoUnlockHandle(_cmykTableH);
McoUnlockHandle(_cmykTableBigH);

return MCO_SUCCESS;
}

// call these two functions to start processing
McoStatus calib_inter4::start(void)
{
if (_cmykTableH != 0L) cmykTable = (double*)McoLockHandle(_cmykTableH);
else return MCO_OBJECT_NOT_INITIALIZED;
if (_cmykTableBigH != 0L) cmykTableBig = (double*)McoLockHandle(_cmykTableBigH);
else return MCO_OBJECT_NOT_INITIALIZED;
return MCO_SUCCESS;
}

McoStatus calib_inter4::end(void)
{
McoUnlockHandle(_cmykTableH);
McoUnlockHandle(_cmykTableBigH);

return MCO_SUCCESS;
}


/*
// get the distance to the best match
double calib_inter4::getDistance(double *lab,double *cmyk)
{
double dist;
double K;

K = cmyk[3];
dist = -1;
getBestCMYK(lab, K,cmyk,&dist);

return dist;
}

*/

McoStatus calib_inter4::eval(double *lab,double *cmyk,int32 num,double *m_dist)
{
int i;
double *l,*c;
double cmy[4];
double min_dist;


l = lab;
c = cmyk;
for (i=0; i<num; i++)
	{	
	 evalCMYSpace(l,c,cmy,&min_dist);
	
	l+=3;
	c+=4;	
	}

	
*m_dist = min_dist;	
	
return MCO_SUCCESS;
}





// find the RGB value that is the best match to the lab value within the table
McoStatus calib_inter3::getBestRGB(double *lab,double *rgb,double *distance)
{
register int i,min_i;
register double	temp,d,min_d;
register double	L1,a1,b1,L2,a2,b2;
double *table1,*table2;
int32	Ki1,Ki2;
double  Kv,Kd,One_M_Kd;
int32	r,g,b;
double  bt_1,bt_2,btbt;
double  ink_sum,ink_mult;


// the address of the two tables
table1 = cmykTableBig;

L1 = lab[0];
a1 = lab[1];
b1 = lab[2];

if (max_ink <= 0) // no ink limiting problem
	{
	// do 1D interpolation and search
	min_d = 1.0E20;
	for (i=0; i<BT_DIM*BT_DIM*BT_DIM; i++)
		{
		L2 = *(table1++);
		a2 = *(table1++);
		b2 = *(table1++);
		d = (L2-L1);
		d *=d;
		temp = (a2-a1);
		temp *= temp;
		d += temp;
		temp = (b2-b1);
		temp *= temp;
		d+= temp;
		if (d < min_d)
			{
			min_d = d;
			min_i = i;
			}
		}
	b = min_i/(BT_DIM*BT_DIM);
	g = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
	r = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
	
	rgb[0] = max_cal*(double)r/(BT_DIM-1);
	rgb[1] = max_cal*(double)g/(BT_DIM-1);
	rgb[2] = max_cal*(double)b/(BT_DIM-1);

	}
else  // have a ink limiting situation here
	{	
	bt_1 = 1/(double)BT_DIM;
	bt_2 = 1/(double)(BT_DIM*BT_DIM);
	ink_mult = max_cal/(double)(BT_DIM-1);
	// do 1D interpolation and search
	min_d = 1.0E20;
	for (i=0; i<BT_DIM*BT_DIM*BT_DIM; i++)
		{
		L2 = *(table1++);
		a2 = *(table1++);
		b2 = *(table1++);
		d = (L2-L1);
		d *=d;
		temp = (a2-a1);
		temp *= temp;
		d += temp;
		temp = (b2-b1);
		temp *= temp;
		d+= temp;
		// calculate total ink

		ink_sum = i*bt_2;
		ink_sum += (i%(BT_DIM*BT_DIM))*bt_1;
		ink_sum += (i%(BT_DIM*BT_DIM))%BT_DIM;
		ink_sum *= ink_mult;
		if ((d < min_d) && (ink_sum > max_ink))
			{
			min_d = d;
			min_i = i;
			}
		}
	b = min_i/(BT_DIM*BT_DIM);
	g = (min_i%(BT_DIM*BT_DIM))/BT_DIM;
	r = (min_i%(BT_DIM*BT_DIM))%BT_DIM;
	
	rgb[0] = max_cal*(double)r/(BT_DIM-1);
	rgb[1] = max_cal*(double)g/(BT_DIM-1);
	rgb[2] = max_cal*(double)b/(BT_DIM-1);
	}
// calculate the CMY values	from i
	
*distance = min_d;	
return MCO_SUCCESS;
}

// find the best solution by solving in CMY space
McoStatus calib_inter3::evalRGBSpace(double *lab,double *cmyk,double *cmy,double *dist)
{
register int32 	i,j,k;
register double L,a,b,L2,a2,b2;
double	K;
double cmyk2[4],cmy2[4];
double lab2[3],lab3[3],lab4[3];
double *lab3p,*lab4p;
int32	count = 0;
double	min_d,last_d,best_d,d,temp;
double	speed;
double  ink_sum;



// get the best cmyk value
getBestRGB(lab,cmy,dist);
//return MCO_SUCCESS;

speed = 0.0025*max_cal;

last_d = 1E20;

cmy2[0] = cmy[0];
cmy2[1] = cmy[1];
cmy2[2] = cmy[2];

inv_max_cal_0 = (BT_DIM-1.0000000000001)/max_cal;
inv_max_cal_1 = 32/max_cal;

L = lab[0];
a = lab[1];
b = lab[2];

// dimensions for interpolation
n0 = BT_DIM;
n1 = 33;


do 
	{

	min_d = 1E20;
	for (i=-1; i<2; i++)
		{
		int_c = cmy[1] + i*speed;  // green
		if ((int_c >=0) && (int_c <= max_cal)) for (j=-1; j<2; j++)
			{
			int_m = cmy[0] + j*speed; //red
			if ((int_m >=0) && (int_m <= max_cal)) for (k=-1; k<2; k++) if (i!=0 || j!=0 || k!=0)
				{
				int_y = cmy[2] + k*speed; //blue
				ink_sum = int_c+int_m+int_y;
				if ((int_y >=0) && (int_y <= max_cal) && (ink_sum > max_ink))
					{
					interpolate_3D_n_1(lab3,cmykTable);
					lab3p = lab3;
					lab4p = lab4;
					L2 = *lab3p++;
					a2 = *lab3p++;
					b2 = *lab3p;
					d = (L-L2);
					d *= d;
					temp = (a-a2);
					temp *= temp;
					d += temp;
					temp = (b-b2);
					temp *= temp;
					d += temp;
					if ((d < min_d) && (d < last_d))
						{
						cmy2[1] = int_c;
						cmy2[0] = int_m;
						cmy2[2] = int_y;
						min_d = d;
						best_d = d;
						}
					}
				}
			}
		}
	cmy[0] = cmy2[0];
	cmy[1] = cmy2[1];
	cmy[2] = cmy2[2];
	
	// decrease speed if nothing better was found
	if (min_d == 1E20) speed *= 0.5;
	
	last_d = best_d;
	
	count++;	
	} while ((count<MAX_COUNT) && (min_d > MIN_D) && (speed > MIN_SPEED*max_cal));
	
cmyk[0] = cmy2[0];	
cmyk[1] = cmy2[1];	
cmyk[2] = cmy2[2];	


//cmyk[0] = cmy[0];	
//cmyk[1] = cmy[1];	
//cmyk[2] = cmy[2];	

*dist = best_d;
	
return MCO_SUCCESS;
}

// get the distance to the best match
double calib_inter3::getDistance(double *lab,double K,double max_dist)
{
double dist;
double rgb[3];

getBestRGB(lab,rgb,&dist);
return dist;
}


calib_inter3::calib_inter3(calib_base **cal,int dir,double max_c,double mi,ThermObject *td)
{
calib = cal;
_patch = 0L;
iterations = NUM_INTERATIONS;
direction = dir;
max_cal = max_c;
thermd = td;
max_ink = 3*max_cal - mi;
}

// call these two functions to start processing
McoStatus calib_inter3::start(void)
{
if (_cmykTableH != 0L) cmykTable = (double*)McoLockHandle(_cmykTableH);
else return MCO_OBJECT_NOT_INITIALIZED;
if (_cmykTableBigH != 0L) cmykTableBig = (double*)McoLockHandle(_cmykTableBigH);
else return MCO_OBJECT_NOT_INITIALIZED;
return MCO_SUCCESS;
}

McoStatus calib_inter3::end(void)
{
McoUnlockHandle(_cmykTableH);
McoUnlockHandle(_cmykTableBigH);

return MCO_SUCCESS;
}

McoStatus calib_inter3::set_up(double* rgbcolor, double *refrgbin,int32 dimension, 
							  int32 total_loc, McoHandle c_tH,McoHandle cb_tH)
{
_cmykTableH = c_tH;
_cmykTableBigH = cb_tH;
return MCO_SUCCESS;
}

McoStatus calib_inter3::eval(double *lab,double *cmyk,int32 num,double *m_dist)
{
int i;
double scale;
double *l,*c;
double cmy[4];
double min_dist;

scale = 8000.0/(double)num;

l = lab;
c = cmyk;
for (i=0; i<num; i++)
	{	
	 evalRGBSpace(l,c,cmy,&min_dist);
	if ((i%20) && (thermd != 0L)) if (!thermd->DisplayTherm(2000+(int)(i*scale),Process_BuildingTable)) goto bail;
	l+=3;
	c+=4;	
	}

	
*m_dist = min_dist;	
bail:	
return MCO_SUCCESS;
}
