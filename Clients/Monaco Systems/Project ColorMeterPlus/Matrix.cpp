/* Matrix.c

 This is a collection of matrix manipulation routines which is used
 to perform matrix calculations.  The routines which are current as
 of Nov 20,1987 are:

 MatMult(mat1,nrows,ncols,mat2,mrows,mcols,outmat)
  double *mat1,*mat2,*outmat;
  int nrows,ncols,mrows,mcols;

 Invert(matptr,order)
  double *matptr;
  int order;

 Trans(inptr,nrow,ncol,outptr)
  double *inptr,*outptr;
  int nrow,ncol;


  Some points to note:
   All routines are double precision, hence all data must be declared
   double in the calling routines.

   These routines donot allocate memory.  All memory must be allocated in
   the main.

*/
#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

/********************************************************************/
void Invert(double *matptr,int order)
 
/********************************************************************/
 
 /* Matrix Inversion using Gauss-Jordan Reduction
 The inverted Matrix overlays the original matrix. Partial Pivoting is
 not employed so this algorithm may generated singularities */

{
int i,j,k;
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
void Trans(double *inptr,int nrow,int ncol,double *outptr)



/******************************************************************/
{
 int i,j;

   for(i = 0; i < nrow; i++)
    for(j = 0; j < ncol; j++)
     outptr[j * nrow + i] = inptr[i * ncol + j];
}

/********************************************************************/
MatMult(double *aptr,int nrow, int pcol,double *bptr,int prow,int ncol,double *outptr)
 
/********************************************************************/
{
 double *cptr, *dptr, *eptr;
 int i,j,k,ibase;

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
 
A_solve(double *c_mat,int c_rows,int c_cols,double *b_mat,int b_rows,int b_cols, double *a_mat)
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
