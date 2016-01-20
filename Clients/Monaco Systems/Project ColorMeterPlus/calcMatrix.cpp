// calc matrix 
#include "stdafx.h"

#include "Color Meter Plus.h"
#include "calcMatrix.h"
#include "math.h"

void calcMatrix(ReferenceFile *MonitorData)
{
	double Bmat[3*4],Cmat[3*4];
	



	Cmat[0] = 1;
	Cmat[4] = 0;
	Cmat[8] = 0; // Red

	Cmat[1] = 0;
	Cmat[5] = 1;
	Cmat[9] = 0; //Green

	Cmat[2] = 0;
	Cmat[6] = 0;
	Cmat[10] = 1; //Blue

	Cmat[3]	= 1;
	Cmat[7] = 1;
	Cmat[11] = 1; //White

	Bmat[0] = MonitorData->Red.x *MonitorData->Red.Y / MonitorData->Red.y;
	Bmat[4] = MonitorData->Red.Y ;
	Bmat[8] = MonitorData->Red.z *MonitorData->Red.Y / MonitorData->Red.y;

	Bmat[1] = MonitorData->Green.x *MonitorData->Green.Y / MonitorData->Green.y;
	Bmat[5] = MonitorData->Green.Y ;
	Bmat[9] = MonitorData->Green.z *MonitorData->Green.Y / MonitorData->Green.y;

	Bmat[2] = MonitorData->Blue.x *MonitorData->Blue.Y / MonitorData->Blue.y;
	Bmat[6] = MonitorData->Blue.Y ;
	Bmat[10] = MonitorData->Blue.z *MonitorData->Blue.Y / MonitorData->Blue.y;

	Bmat[3] = MonitorData->White.x *MonitorData->White.Y / MonitorData->White.y;
	Bmat[7] = MonitorData->White.Y ;
	Bmat[11] = MonitorData->White.z *MonitorData->White.Y / MonitorData->White.y;

	A_solve(Cmat,3,4,Bmat,3,4,MonitorData->XYZ2RGB);

	




}// calc matrix 


void calcScalars(ReferenceFile *MonitorData)
{

  

 dYxy tempYxy;
 double XYZ[3];
 double RGB[3];
 double dMax = .0000000001;
 short i;


	if(MonitorData->ReferenceCT == 0)//absolute match has been selected!!!
	{
		MonitorData->Rref = 1.0;
		MonitorData->Bref = 1.0;
		MonitorData->Gref = 1.0;
		return;
	}

		//first convert the ColorTemperature to an xy value;

	Kelvin2xy(MonitorData->ReferenceCT,&tempYxy.x,&tempYxy.y);

	
	tempYxy.z = 1-tempYxy.x-tempYxy.y;

	// now reset the xyz values of the white reference
  
	MonitorData->White.x = tempYxy.x;
	MonitorData->White.y = tempYxy.y;
	MonitorData->White.z = tempYxy.z;

	calcMatrix(MonitorData);
		MonitorData->Rref = 1.0;
		MonitorData->Bref = 1.0;
		MonitorData->Gref = 1.0;
/*


	XYZ[0] = tempYxy.x/tempYxy.y;
	XYZ[1] = 1;
	XYZ[2] = tempYxy.z/tempYxy.y;

	// Now convert this to a scaled RGB based upon the monitor matrix

	MatMult(MonitorData->XYZ2RGB,3,3,XYZ,3,1,RGB);

	// Now rescale this by the maximum data found in the RGB array;

	if(dMax < RGB[0])dMax = RGB[0];
	if(dMax < RGB[1])dMax = RGB[1];
	if(dMax < RGB[2])dMax = RGB[2];

	RGB[0] /= dMax;
	RGB[1] /= dMax;
	RGB[2] /= dMax;

	MonitorData->Rref = RGB[0];
	MonitorData->Bref = RGB[1];
	MonitorData->Gref = RGB[2];



*/


}
 /* Matrix Routines */


/********************************************************************
 * -Name:      void Invert ( short matptr, double *order )
 *
 * -Description: Matrix Inversion using Gauss-Jordan Reduction
 *               The inverted Matrix overlays the original matrix. Partial
 *               Pivoting is not employed so this algorithm may generated
 *               singularities
 ********************************************************************/
void Invert ( double *matptr,  short order )
{
	short i,j,k;
	double *aptr,*bptr,*cptr;


   for(k = 0;k < order; k++)
      {
      for(j = 0; j < order; j++)
         if(j != k)
            {
            aptr =  matptr + (order *k) + j;
            bptr =  matptr + (order * k) + k;
            *aptr = (*aptr) / (*bptr);
            /*matptr[k][j] = matptr [k][j] / matptr[k][k] */
            }

      bptr = matptr + (order *k) + k;
      *bptr = 1.0 / (*bptr);

       /* matptr[k][k] = 1.0 / matptr[k][k] */

      for(i = 0; i < order; i++)
         if( i != k)
            for(j = 0; j < order; j++)
               if(j != k)
                  {
                  aptr = matptr + (i * order) + j;
                  bptr = matptr + (k * order) + j;
                  cptr = matptr + (i * order) + k;
                  *aptr = (*aptr) - ((*bptr) * (*cptr));
                  }

      /* matptr [i][j] = matptr[i][j] - matptr[k][j] * matptr[i][k] */

      for( i = 0; i < order; i++)
         if(i != k)
            {
            aptr = matptr + (order *i) + k;
            bptr = matptr + (order *k) + k;
            *aptr = ((*aptr) * (*bptr));
            *aptr = -(*aptr);
            }
      }


/*matptr[i][k]  = - matptr[i][k] * matptr[k][k] */

}


/******************************************************************/
void Trans ( double *inptr, short nrow, short ncol, double *outptr )
{
	short  i,j;

   for(i = 0; i < nrow; i++)
    for(j = 0; j < ncol; j++)
     outptr[j * nrow + i] = inptr[i * ncol + j];
}

/********************************************************************/
short MatMult ( double *aptr, short nrow, short pcol, double *bptr, short prow, short ncol, double *outptr)
/********************************************************************/
{
	double *cptr, *dptr, *eptr;
	short i,j,k,ibase;

   if(pcol != prow)
    return(-1); /* The matrices are not order correctly! */
   
   for ( i = 0; i < nrow; i++)
    for( j = 0; j < ncol; j++){
     cptr = outptr + (i * ncol) + j;
     *cptr = 0.0000;
     ibase = i * pcol;

       for (k= 0; k < pcol; k++){
        dptr = aptr + ibase + k;
        eptr = bptr + ( k * ncol) + j;
        *cptr = *cptr + (*dptr) * (*eptr);
       }
     }
   return(0);
}
 
short A_solve(double *c_mat,short c_rows,short c_cols,double *b_mat,short b_rows,short b_cols, double *a_mat)
{
	/******************************************************************\
	* This function performs a least squares solution to the equation
	* C = A * B, where the matrix A is the one that needs to be solved.
	* The numerics are all double precision. The routine assumes that the
	* user has ordered the matrices properly                            *
	\*******************************************************************/

double *bt, *bbt, *cbt;

	bt = (double *)malloc(b_cols * b_rows * sizeof(double));
	bbt = (double *)malloc(b_rows * b_rows * sizeof(double));
	cbt =(double *)malloc(c_rows * b_rows * sizeof(double));
	if(cbt == NULL)return(-1);

	Trans(b_mat,b_rows,b_cols,bt); /* compute transpose of B */
	MatMult(c_mat,c_rows,c_cols,bt,b_cols,b_rows,cbt); /* post multiply B * C */
	MatMult(b_mat,b_rows,b_cols,bt,b_cols,b_rows,bbt); /* calculate bb transpose */
	Invert(bbt,b_rows);/* invert this result */
	MatMult(cbt,c_rows,b_rows,bbt,b_rows,b_rows,a_mat);

	free(bt);
	free(bbt);
	free(cbt);

	return(0);
	
}

//ColorTemp routines
#include "tlocus.dat"

#define START_TEMP 4000
#define END_TEMP 24950
#define TEMP_INC 50

short Kelvin2xy(double ColorTemp,double *x, double *y)
{
  double T1,T2,T3,xd,yd;
  int err;
  err = 0;
  T1 = ColorTemp;
  T2 = T1*T1;
  T3 = T2*T1;

  if (T1 < 4000.0)
    return(-1);

  if ((T1 >= 4000) && (T1 < 7000))
    {
    xd = -4607000000./T3;
    xd += 2967800./T2;
    xd += 99.11/T1;
    xd += 0.244063;
    err = find_yd(xd,&yd);
    }
  if ((T1 >=7000) && (T1 < 25000))
    {
    xd = -2006400000./T3;
    xd += 1901800./T2;
    xd += 247.48/T1;
    xd += 0.237040;
    err =find_yd(xd,&yd);
    }
    *x = xd;
    *y = yd;
 return(err);

}

short find_yd(double x,double *yd)
{
  /* This function uses a quadradic solution to solve for the y term
    of the color temperture function.  it is an equation of the form
      y = 2.87x - 3x^2 -.2750*/

double a,b,c;

  a = -3.00 *x * x;
  b = 2.870* x;
  c= -.275;

  *yd = a + b + c;
  return(0);
}

double xy2Kelvin(double x, double y)
{
  double mindif, CloseTemp,newdif,u,v,udif,vdif;
  int i,temp_index;
  xyL  xy;
  uvL uv;

  xy.x = x;
  xy.y = y;
  xy.z = 1-x-y;
  /* convert data to u'v' space */

  xyL_uvL(&xy,&uv);
  u= uv.u;
  v = uv.v;

  mindif = 1000000.00;
  /* find the closest color temp within TEMP_INC Kelvins */

  for (i=0; i<(END_TEMP-START_TEMP)/TEMP_INC; i++)
    {
    udif = u - BlackBodyLocus[i].u;
    vdif = v - BlackBodyLocus[i].v;
    newdif = udif*udif;
    newdif += vdif*vdif;
    newdif = sqrt(newdif);

    if(newdif < mindif)
      {
      CloseTemp = BlackBodyLocus[i].ColorTemp;
      mindif = newdif;
      temp_index = i;
      }
    }
  return(CloseTemp);
}

/* Routines from Color.c */

void Find_XYZRGB (xyL *Red,xyL *Green,xyL *Blue,xyL *White,double *TheMatrix)
{
	double Rxy,Rzy,Gxy,Gzy,Bxy,Bzy,Wxy,Wzy;
	double DataMatrix[9],WhiteMatrix[3],lmnMatrix[3],Temp[9];
	short  i;

	Rxy = Red->x/Red->y;
	Rzy = Red->z/Red->y;
	
	Gxy = Green->x/Green->y;
	Gzy = Green->z/Green->y;
	
	Bxy = Blue->x/Blue->y;
	Bzy = Blue->z/Blue->y;
	
	Wxy = White->x/White->y;
	Wzy = White->z/White->y;

	/* build DataMatrix */
	
	DataMatrix[0] = Rxy;
	DataMatrix[1] = Gxy;
	DataMatrix[2] = Bxy;
	DataMatrix[3] = 1.0;
	DataMatrix[4] = 1.0;
	DataMatrix[5] = 1.0;
	DataMatrix[6] = Rzy;
	DataMatrix[7] = Gzy;
	DataMatrix[8] = Bzy;
	
	/* build WhiteMatrix */
	
	WhiteMatrix[0] = Wxy;
	WhiteMatrix[1] = 1.0;
	WhiteMatrix[2] = Wzy;
	
	/* solve for lmn matrix */
	
	Invert(DataMatrix,3); /*routine from matrix library */
	
	MatMult(DataMatrix,3,3,WhiteMatrix,3,1,lmnMatrix);
	
	/* solve for XYZ->RGB matrix */
	
	TheMatrix[0] = lmnMatrix[0]/Red->y * Red->x;
	TheMatrix[1] = lmnMatrix[0]/Red->y * Red->y;
	TheMatrix[2] = lmnMatrix[0]/Red->y * Red->z;
	TheMatrix[3] = lmnMatrix[1]/Green->y * Green->x;
	TheMatrix[4] = lmnMatrix[1]/Green->y * Green->y;
	TheMatrix[5] = lmnMatrix[1]/Green->y * Green->z;
	TheMatrix[6] = lmnMatrix[2]/Blue->y * Blue->x;
	TheMatrix[7] = lmnMatrix[2]/Blue->y * Blue->y;
	TheMatrix[8] = lmnMatrix[2]/Blue->y * Blue->z;

	Invert(TheMatrix,3);

	for(i=0; i<9; i++) {
		Temp[i] = TheMatrix[i];
	}

	Trans(Temp,3,3,TheMatrix);

}

void xyL_XYZ ( xyL *xyLptr, XYZ *XYZptr )
{
	double XYZsum;
	
	XYZsum = xyLptr->L / xyLptr->y;
	XYZptr->X = xyLptr->x * XYZsum;
	XYZptr->Y = xyLptr->L;
	XYZptr->Z = (1.0 - xyLptr->x - xyLptr->y) * XYZsum;
}
/*******************************************************
 * -Name:        xyL_uvL ( xyL *xyLptr, uvL *uvLptr )
 *
 * -Description: Converts xyL data to u' v' L data, thats uv PRIME data.
*******************************************************/
void xyL_uvL( xyL *xyLptr, uvL *uvLptr )
{
	double denom;
	
	denom = (12.0 * xyLptr->y) - (2.0 * xyLptr->x) + 3.0;
	uvLptr->u = (4.0 * xyLptr->x) / denom;
	uvLptr->v = (9.0 * xyLptr->y) / denom;
	uvLptr->L = xyLptr->L;
	uvLptr->w = 1.0 - (uvLptr->u + uvLptr->v);
}


