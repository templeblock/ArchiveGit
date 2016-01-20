/******************************************************************************
* File: NewInterpolate.cpp
*
* Copyright (C) 1995 - 1997, Monaco Systems, Incorporated and ONYX Graphics 
*		Corporation.  All Rights Reserved.
*
* Purpose: Implementation for the NLinterp classes. These classes apply 
*					 8 or 16 bit sparse LUT's to a set of data using linear 
*					 interpolation. NLinterp3 is used for 3 dimensional LUT's.
*					 NLinterp4 is used for 4 dimensional LUT's.
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include "NewInterpolate.h"


/******************************************************************************
* CLASS NLinterp - base class
******************************************************************************/

/******************************************************************************
* Name: NLinterp::NLinterp
*
* Purpose: constructor
******************************************************************************/
NLinterp::NLinterp(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels)
{
	m_pTable = pTable;
	m_nInputChannels = nInputChannels;
	m_nGridPoints = nGridPoints;
	m_nOutputChannels = nOutputChannels;
	m_offset_g = nGridPoints * nInputChannels;
	m_offset_r = nGridPoints * nGridPoints * nInputChannels;
	m_nSkipChannels = nOutputChannels - nInputChannels;

}


/******************************************************************************
* CLASS NLinterp3 - 3 dimensional 8 bit LUT application
******************************************************************************/

/******************************************************************************
* Name: NLinterp3::NLinterp3
*
* Purpose: constructor
******************************************************************************/
NLinterp3::NLinterp3(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels)
	: NLinterp(pTable, nInputChannels, nGridPoints, nOutputChannels)
{	
	double pos;
	long i, index, weight;
	long numsteps = nGridPoints - 1;

	m_nSkipChannels = nOutputChannels - 3;

	for(i=0; i<256; i++) {
		pos = (double)(i * numsteps) / 255.0;

		index = (long)pos;
		weight = (long)(256.0 * (pos - (double)index) + 0.5);

		if( !weight) {
			if( index) {
				index--;
				weight = 256;
			}
		}
		
		m_index_r[i] = index * m_offset_r;
		m_index_g[i] = index * m_offset_g;
		m_index_b[i] = index * m_nInputChannels;

		m_weight[i] = weight;
	}	
}

/******************************************************************************
* Name: NLinterp3::interp
*
* Purpose: applies the 3 dimensional 8 bit LUT to pSrc placing the result in 
*					 pDest
******************************************************************************/
void NLinterp3::interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest)
{
	long i, j, n;
	register unsigned long sum;
	register long p[8];
	register unsigned long weight[8];
	register unsigned long rsub, gsub, bsub;
	register unsigned long rnot, gnot, bnot;
	long	c, m, y;

	//3 dimensional interpolation
	for( i = cbSrc; i; i--){
			
		c = (long)*pSrc++;
		m = (long)*pSrc++;
		y = (long)*pSrc++;

		pSrc += m_nSkipChannels;

		rsub = m_weight[c];
		gsub = m_weight[m];
		bsub = m_weight[y];

		rnot = 256 - rsub;
		gnot = 256 - gsub;
		bnot = 256 - bsub;
			
		weight[4] = gnot * bnot;
		weight[5] = gsub * bnot;
		weight[6] = gsub * bsub;
		weight[7] = gnot * bsub;
		
		weight[0] = rnot * weight[4];
		weight[1] = rnot * weight[5];
		weight[2] = rnot * weight[6];
		weight[3] = rnot * weight[7];
		
		weight[4] *= rsub;
		weight[5] *= rsub;
		weight[6] *= rsub;
		weight[7] *= rsub;
		
		p[0] = (m_index_r[c] + m_index_g[m] + m_index_b[y]);
		p[1] = p[0] + m_offset_g;
		p[2] = p[1] + m_nInputChannels;
		p[3] = p[0] + m_nInputChannels;
		
		p[4] = p[0] + m_offset_r;
		p[5] = p[4] + m_offset_g;
		p[6] = p[5] + m_nInputChannels;
		p[7] = p[4] + m_nInputChannels;

		//finally, interpolation
		for( j = 0; j < m_nInputChannels; j++){
			sum = 0;
			for( n = 0; n < 8; n++){
				sum += m_pTable[p[n]++] * weight[n];
			}
			*pDest++ = (unsigned char)(sum >> 24);
		}				
	}					
}

/******************************************************************************
* CLASS NLinterp3s - 3 dimensional 16 bit LUT application
******************************************************************************/

/******************************************************************************
* Name: NLinterp3s::NLinterp3s
*
* Purpose: constructor
******************************************************************************/
NLinterp3s::NLinterp3s(unsigned short *pTable, long nInputChannels, long nGridPoints, long nOutputChannels)
	: NLinterp((unsigned char*)pTable, nInputChannels, nGridPoints, nOutputChannels)
{	
	double pos;
	long i, index, weight;
	long numsteps = nGridPoints - 1;

	m_nSkipChannels = nOutputChannels - 3;

	for(i=0; i<256; i++) {
		pos = (double)(i * numsteps) / 255.0;

		index = (long)pos;
		weight = (long)(32.0 * (pos - (double)index) + 0.5);

		if( !weight) {
			if( index) {
				index--;
				weight = 32;
			}
		}
		
		m_index_r[i] = index * m_offset_r;
		m_index_g[i] = index * m_offset_g;
		m_index_b[i] = index * m_nInputChannels;

		m_weight[i] = weight;
	}	
}

/******************************************************************************
* Name: NLinterp3s::interp
*
* Purpose: applies the 3 dimensional 16 bit LUT to pSrc placing the result in 
*					 pDest
******************************************************************************/
void NLinterp3s::interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest)
{
	long i, j, n;
	register unsigned long sum;
	register long p[8];
	register unsigned long weight[8];
	register unsigned long rsub, gsub, bsub;
	register unsigned long rnot, gnot, bnot;
	long	c, m, y;
	unsigned short *pTable;
	
	pTable = (unsigned short*)m_pTable;

	//3 dimensional interpolation
	for( i = cbSrc; i; i--){
			
		c = (long)*pSrc++;
		m = (long)*pSrc++;
		y = (long)*pSrc++;
		
		pSrc += m_nSkipChannels;

		rsub = m_weight[c];
		gsub = m_weight[m];
		bsub = m_weight[y];

		rnot = 32 - rsub;
		gnot = 32 - gsub;
		bnot = 32 - bsub;
			
		weight[4] = gnot * bnot;
		weight[5] = gsub * bnot;
		weight[6] = gsub * bsub;
		weight[7] = gnot * bsub;
		
		weight[0] = rnot * weight[4];
		weight[1] = rnot * weight[5];
		weight[2] = rnot * weight[6];
		weight[3] = rnot * weight[7];
		
		weight[4] *= rsub;
		weight[5] *= rsub;
		weight[6] *= rsub;
		weight[7] *= rsub;
		
		p[0] = (m_index_r[c] + m_index_g[m] + m_index_b[y]);
		p[1] = p[0] + m_offset_g;
		p[2] = p[1] + m_nInputChannels;
		p[3] = p[0] + m_nInputChannels;
		
		p[4] = p[0] + m_offset_r;
		p[5] = p[4] + m_offset_g;
		p[6] = p[5] + m_nInputChannels;
		p[7] = p[4] + m_nInputChannels;

		//finally, interpolation
		for( j = 0; j < m_nInputChannels; j++){
			sum = 0;
			for( n = 0; n < 8; n++){
				sum += pTable[p[n]++] * weight[n];
			}
			*pDest++ = (unsigned char)(sum >> 23);
		}				
	}					
}

/******************************************************************************
* CLASS NLinterp4 - 4 dimensional 8 bit LUT application
******************************************************************************/

/******************************************************************************
* Name: NLinterp4::NLinterp4
*
* Purpose: constructor
******************************************************************************/
NLinterp4::NLinterp4(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels)
	: NLinterp((unsigned char*)pTable, nInputChannels, nGridPoints, nOutputChannels)
{
	double pos;
	long i, index, weight;
	long numsteps = nGridPoints - 1;

	m_offset_b = m_nGridPoints * m_nInputChannels;
	m_offset_g = m_nGridPoints * m_nGridPoints * m_nInputChannels;
	m_offset_r = m_nGridPoints * m_nGridPoints * m_nGridPoints * m_nInputChannels;

	m_nSkipChannels = nOutputChannels - 4;

	for(i=0; i<256; i++) {
		pos = (double)(i * numsteps) / 255.0;

		index = (long)pos;
		weight = (long)(64.0 * (pos - (double)index) + 0.5);

		if( !weight) {
			if( index) {
				index--;
				weight = 64;
			}
		}
		
		m_index_r[i] = index * m_offset_r;
		m_index_g[i] = index * m_offset_g;
		m_index_b[i] = index * m_offset_b;
		m_index_w[i] = index * m_nInputChannels;

		m_weight[i] = weight;
	}	
}

/******************************************************************************
* Name: NLinterp4::interp
*
* Purpose: applies the 4 dimensional 8 bit LUT to pSrc placing the result in 
*					 pDest
******************************************************************************/
void NLinterp4::interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest)
{
	long i, j, n;
	register unsigned long sum; // Note: sum might better be kept in an __int64 variable with larger weights
	register long p[16];
	register unsigned long weight[16];
	register unsigned long rsub, gsub, bsub, wsub;
	register unsigned long rnot, gnot, bnot, wnot;
	long	c, m, y, k;

	//4 dimensional interpolation
	for( i = cbSrc; i; i--){
			
		c = (long)*pSrc++;
		m = (long)*pSrc++;
		y = (long)*pSrc++;
		k = (long)*pSrc++;
		
		pSrc += m_nSkipChannels;

		rsub = m_weight[c];
		gsub = m_weight[m];
		bsub = m_weight[y];
		wsub = m_weight[k];

		rnot = 64 - rsub;
		gnot = 64 - gsub;
		bnot = 64 - bsub;
		wnot = 64 - wsub;
			
		weight[4] = gnot * bnot;
		weight[5] = gsub * bnot;
		weight[6] = gsub * bsub;
		weight[7] = gnot * bsub;
		
		weight[0] = rnot * weight[4];
		weight[1] = rnot * weight[5];
		weight[2] = rnot * weight[6];
		weight[3] = rnot * weight[7];
		
		weight[4] *= rsub;
		weight[5] *= rsub;
		weight[6] *= rsub;
		weight[7] *= rsub;

		weight[8] = weight[0] * wsub;
		weight[9] = weight[1] * wsub;
		weight[10] = weight[2] * wsub;
		weight[11] = weight[3] * wsub;

		weight[12] = weight[4] * wsub;
		weight[13] = weight[5] * wsub;
		weight[14] = weight[6] * wsub;
		weight[15] = weight[7] * wsub;

		weight[0] *= wnot;
		weight[1] *= wnot;
		weight[2] *= wnot;
		weight[3] *= wnot;
		
		weight[4] *= wnot;
		weight[5] *= wnot;
		weight[6] *= wnot;
		weight[7] *= wnot;
		
		p[0] = (m_index_r[c] + m_index_g[m] + m_index_b[y] + m_index_w[k]);
		p[1] = p[0] + m_offset_g;
		p[2] = p[1] + m_offset_b;
		p[3] = p[0] + m_offset_b;
		
		p[4] = p[0] + m_offset_r;
		p[5] = p[4] + m_offset_g;
		p[6] = p[5] + m_offset_b;
		p[7] = p[4] + m_offset_b;

		p[8] = p[0] + m_nInputChannels;
		p[9] = p[1] + m_nInputChannels;
		p[10] = p[2] + m_nInputChannels;
		p[11] = p[3] + m_nInputChannels;

		p[12] = p[4] + m_nInputChannels;
		p[13] = p[5] + m_nInputChannels;
		p[14] = p[6] + m_nInputChannels;
		p[15] = p[7] + m_nInputChannels;

		//finally, interpolation
		for( j = 0; j < m_nInputChannels; j++){
			sum = 0;
			for( n = 0; n < 16; n++){
				sum += m_pTable[p[n]++] * weight[n];
			}
			*pDest++ = (unsigned char)(sum >> 24);
		}				
	}					
}

/******************************************************************************
* CLASS NLinterp4s - 4 dimensional 16 bit LUT application
******************************************************************************/

/******************************************************************************
* Name: NLinterp4s::NLinterp4s
*
* Purpose: constructor
******************************************************************************/
NLinterp4s::NLinterp4s(unsigned short *pTable, long nInputChannels, long nGridPoints, long nOutputChannels)
	: NLinterp((unsigned char*)pTable, nInputChannels, nGridPoints, nOutputChannels)
{
	double pos;
	long i, index, weight;
	long numsteps = nGridPoints - 1;

	m_offset_b = m_nGridPoints * m_nInputChannels;
	m_offset_g = m_nGridPoints * m_nGridPoints * m_nInputChannels;
	m_offset_r = m_nGridPoints * m_nGridPoints * m_nGridPoints * m_nInputChannels;

	m_nSkipChannels = nOutputChannels - 4;

	for(i=0; i<256; i++) {
		pos = (double)(i * numsteps) / 255.0;

		index = (long)pos;
		weight = (long)(16.0 * (pos - (double)index) + 0.5);

		if( !weight) {
			if( index) {
				index--;
				weight = 16;
			}
		}
		
		m_index_r[i] = index * m_offset_r;
		m_index_g[i] = index * m_offset_g;
		m_index_b[i] = index * m_offset_b;
		m_index_w[i] = index * m_nInputChannels;

		m_weight[i] = weight;
	}	
}

/******************************************************************************
* Name: NLinterp4s::interp
*
* Purpose: applies the 4 dimensional 16 bit LUT to pSrc placing the result in 
*					 pDest
******************************************************************************/
void NLinterp4s::interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest)
{
	long i, j, n;
	register unsigned long sum; // Note: sum might better be kept in an __int64 variable with larger weights
	register long p[16];
	register unsigned long weight[16];
	register unsigned long rsub, gsub, bsub, wsub;
	register unsigned long rnot, gnot, bnot, wnot;
	long	c, m, y, k;

	//4 dimensional interpolation
	for( i = cbSrc; i; i--){
			
		c = (long)*pSrc++;
		m = (long)*pSrc++;
		y = (long)*pSrc++;
		k = (long)*pSrc++;
		
		pSrc += m_nSkipChannels;

		rsub = m_weight[c];
		gsub = m_weight[m];
		bsub = m_weight[y];
		wsub = m_weight[k];

		rnot = 16 - rsub;
		gnot = 16 - gsub;
		bnot = 16 - bsub;
		wnot = 16 - wsub;
			
		weight[4] = gnot * bnot;
		weight[5] = gsub * bnot;
		weight[6] = gsub * bsub;
		weight[7] = gnot * bsub;
		
		weight[0] = rnot * weight[4];
		weight[1] = rnot * weight[5];
		weight[2] = rnot * weight[6];
		weight[3] = rnot * weight[7];
		
		weight[4] *= rsub;
		weight[5] *= rsub;
		weight[6] *= rsub;
		weight[7] *= rsub;

		weight[8] = weight[0] * wsub;
		weight[9] = weight[1] * wsub;
		weight[10] = weight[2] * wsub;
		weight[11] = weight[3] * wsub;

		weight[12] = weight[4] * wsub;
		weight[13] = weight[5] * wsub;
		weight[14] = weight[6] * wsub;
		weight[15] = weight[7] * wsub;

		weight[0] *= wnot;
		weight[1] *= wnot;
		weight[2] *= wnot;
		weight[3] *= wnot;
		
		weight[4] *= wnot;
		weight[5] *= wnot;
		weight[6] *= wnot;
		weight[7] *= wnot;
		
		p[0] = (m_index_r[c] + m_index_g[m] + m_index_b[y] + m_index_w[k]);
		p[1] = p[0] + m_offset_g;
		p[2] = p[1] + m_offset_b;
		p[3] = p[0] + m_offset_b;
		
		p[4] = p[0] + m_offset_r;
		p[5] = p[4] + m_offset_g;
		p[6] = p[5] + m_offset_b;
		p[7] = p[4] + m_offset_b;

		p[8] = p[0] + m_nInputChannels;
		p[9] = p[1] + m_nInputChannels;
		p[10] = p[2] + m_nInputChannels;
		p[11] = p[3] + m_nInputChannels;

		p[12] = p[4] + m_nInputChannels;
		p[13] = p[5] + m_nInputChannels;
		p[14] = p[6] + m_nInputChannels;
		p[15] = p[7] + m_nInputChannels;

		//finally, interpolation
		for( j = 0; j < m_nInputChannels; j++){
			sum = 0;
			for( n = 0; n < 16; n++){
				sum += m_pTable[p[n]++] * weight[n];
			}
			*pDest++ = (unsigned char)(sum >> 24);
		}				
	}					
}


/******************************************************************************
* Some old interpolation stuff use by CMM
* 
* 
******************************************************************************/

//prevent the index out of range
void rangeguard(long *p, long num, long range)
{
	long i;

	for(i = 0; i < num; i++)
		if(p[i] > range)
			p[i] = range;
}

//not been tested yet
Linterp1::Linterp1(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi;
	register long ri;	
	long	c;
	unsigned long 	sum;
	double	gstep;	//grid step
	long 	index[256];
	unsigned long wei[256];
	long	range;

	range = gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (unsigned long)(256*(i/gstep - index[i]));
	}	
		
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		
		for(j = 1; j < srcp; j++)
			src++;
			
		ri = index[c]*tablep;
		
		wi = wei[c];
		
		w[0] = (256-wi);
		w[1] = wi;
				
		p[0] = ri;
		p[1] = p[0] + tablep;

		rangeguard(p, 2, range-1);

//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 2; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned char)(sum >> 8);
		}	
					
		for(j = tablep; j < desp; j++)
			dest++;		
	}					
}

//not been tested yet
Linterp1::Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi;
	register long ri;	
	long	c;
	unsigned long 	sum;
	double	gstep;	//grid step
	long 	index[256];
	unsigned long wei[256];
	long	range;

	range = gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (unsigned long)(256*(i/gstep - index[i]));
	}	
		
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		
		for(j = 1; j < srcp; j++)
			src++;
			
		ri = index[c]*tablep;
		
		wi = wei[c];
		
		w[0] = (256-wi);
		w[1] = wi;
				
		p[0] = ri;
		p[1] = p[0] + tablep;
		rangeguard(p, 2, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 2; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned char)sum;
		}	
					
		for(j = tablep; j < desp; j++)
			dest++;		
	}					
}

//not been tested yet
Linterp1::Linterp1(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest, long desp)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi;
	register long ri;	
	long	c;
	unsigned long 	sum;
	double	gstep;	//grid step
	long	range;

	range = gridp*tablep;

//some precompute
	gstep = 65535.0/(gridp - 1);	
			
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		
		for(j = 1; j < srcp; j++)
			src++;
		
		ri = (long)(c/gstep);
		wi = (unsigned long)(32768*(c/gstep - ri));
		ri *= tablep;
		w[0] = (32768 - wi);
		w[1] = wi;
						
		p[0] = ri;
		p[1] = p[0] + tablep;
		rangeguard(p, 2, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 2; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 15);
		}	
					
		for(j = tablep; j < desp; j++)
			dest++;		
	}					
}

//not been tested yet
Linterp1::Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi;
	register long ri;	
	long	c;
	double 	sum;
	double	gstep;	//grid step
	long 	index[256];
	double 	wei[256];
	long	range;

	range = gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = i/gstep - index[i];
	}	
		
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		
		for(j = 1; j < srcp; j++)
			src++;
			
		ri = index[c]*tablep;
		
		wi = wei[c];
		
		w[0] = (1.0-wi);
		w[1] = wi;
				
		p[0] = ri;
		p[1] = p[0] + tablep;
		rangeguard(p, 2, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0.0;
			for( n = 0; n < 2; n++)
				sum += table[p[n]++]*w[n];
			//normalize 65535 to 1	
			*dest++ = sum/65535.0;
		}	
					
		for(j = tablep; j < desp; j++)
			dest++;		
	}					
}

//this is different interpolation than others
//since the index grids are not uniform
//and the table is been used reversely
//not been tested yet
Linterp1r::Linterp1r(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp)
{
	long i, j, n;
//	register unsigned long w[8];
//	register long p[8];
//	register unsigned long wi;
//	register long ri;	
	long	c;
	double	gstep;	//grid step
	long 	index[256];
	double 	wei[256];
	double 	rtable[256];

//some precompute
	gstep = 1.0/(gridp - 1);		

	for(i = 0; i < 256; i++){
		n = i*257;
		for(j = gridp-2; j >= 0; j--){
			if( n >= table[j] ){
				index[i] = j;
				if(n == table[j])
					wei[i] = 0;
				else
					wei[i] = (double)(n - table[j])/(double)(table[j+1]-table[j]);
				break;
			}
		}		
	}	


	for(i = 0; i < 256; i++)
		rtable[i] = gstep*( (1.0-wei[i])*(double)index[i] + wei[i]*(double)(index[i]+1));

	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		
		for(j = 1; j < srcp; j++)
			src++;
					
//finally, interpolation
		for( j = 0; j < tablep; j++){
			*dest++ = rtable[i];
		}	
					
		for(j = tablep; j < desp; j++)
			dest++;		
	}					
}


//a integer version of 3 dimensional interpolation
//this spport interpolations such as RGB to RGB, RGB to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//srcsize is the # of data needs to be interpolated
//dest has the same planes as table
//can overwrite
//tested
Linterp3::Linterp3(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	double	gstep;	//grid step
	long 	index[256];
	unsigned long wei[256];
	long	range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (unsigned long)(256*(i/gstep - index[i]));
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
	//compute index	
//		ri = (long)(c/gstep);
//		gi = (long)(m/gstep);
//		bi = (long)(y/gstep);
		
	//compute weight			
//		wi = 256*(c/gstep - ri);
//		wj = 256*(m/gstep - gi);	
//		wk = 256*(y/gstep - bi);

		ri = index[c]*srcs2;
		gi = index[m]*srcs;
		bi = index[y]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
			
		w[4] = (256-wj)*(256-wk);
		w[5] = wj*(256-wk);
		w[6] = wj*wk;
		w[7] = (256-wj)*wk;
		
		w[0] = (256-wi)*w[4];
		w[1] = (256-wi)*w[5];
		w[2] = (256-wi)*w[6];
		w[3] = (256-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
		
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);

//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++){
				if(w[n] != 0)
					sum += table[p[n]++]*w[n];
				else
					p[n]++;
			}
			*dest++ = (unsigned char)(sum >> 24);
		}				
				
	}					
}

//can overwrite
//tested
Linterp3::Linterp3(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	double	gstep;	//grid step
	long	range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 65535.0/(gridp - 1);		
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
	//compute index	
		ri = (long)(c/gstep);
		gi = (long)(m/gstep);
		bi = (long)(y/gstep);
		
	//compute weight			
		wi = (unsigned long)(32*(c/gstep - ri));
		wj = (unsigned long)(32*(m/gstep - gi));	
		wk = (unsigned long)(32*(y/gstep - bi));

		ri *= srcs2;
		gi *= srcs;
		bi *= tablep;
			
		w[4] = (32-wj)*(32-wk);
		w[5] = wj*(32-wk);
		w[6] = wj*wk;
		w[7] = (32-wj)*wk;
		
		w[0] = (32-wi)*w[4];
		w[1] = (32-wi)*w[5];
		w[2] = (32-wi)*w[6];
		w[3] = (32-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
		
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);

//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++){
				if(w[n] != 0)
					sum += table[p[n]++]*w[n];
				else
					p[n]++;
			}
			*dest++ = (unsigned short)(sum >> 15);
		}				
				
	}					
}

//a integer version of 3 dimensional interpolation
//this spport interpolations such as RGB to RGB, RGB to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//srcsize is the # of data needs to be interpolated
//dest has the same planes as table
//can overwrite
//tested
Linterp3::Linterp3(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	double	gstep;	//grid step
	long 	index[256];
	unsigned long wei[256];
	long range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (unsigned long)(256*(i/gstep - index[i]));
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	
//3 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs2;
		gi = index[m]*srcs;
		bi = index[y]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
			
		w[4] = (256-wj)*(256-wk);
		w[5] = wj*(256-wk);
		w[6] = wj*wk;
		w[7] = (256-wj)*wk;
		
		w[0] = (256-wi)*w[4];
		w[1] = (256-wi)*w[5];
		w[2] = (256-wi)*w[6];
		w[3] = (256-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
		
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);

//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 16);
		}				
				
	}					
}


//a integer version of 3 dimensional interpolation
//this spport interpolations such as RGB to RGB, RGB to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//srcsize is the # of data needs to be interpolated
//dest has the same planes as table
//can overwrite
Linterp3::Linterp3(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long nouse)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	unsigned	long cmyk[4];
	long range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(32*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs2;
		gi = index[m]*srcs;
		bi = index[y]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];

		w[4] = (32-wj)*(32-wk);
		w[5] = wj*(32-wk);
		w[6] = wj*wk;
		w[7] = (32-wj)*wk;
		
		w[0] = (32-wi)*w[4];
		w[1] = (32-wi)*w[5];
		w[2] = (32-wi)*w[6];
		w[3] = (32-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
	
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);
				
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = sum >> 15;
		}				

		*dest++ = (unsigned char)(cmyk[0]/257);
		*dest++ = (unsigned char)(cmyk[1]/257 - 128);
		*dest++ = (unsigned char)(cmyk[2]/257 - 128);
	}					
}


Linterp3::Linterp3(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
//	unsigned	long cmyk[4];
	long range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(32*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs2;
		gi = index[m]*srcs;
		bi = index[y]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];

		w[4] = (32-wj)*(32-wk);
		w[5] = wj*(32-wk);
		w[6] = wj*wk;
		w[7] = (32-wj)*wk;
		
		w[0] = (32-wi)*w[4];
		w[1] = (32-wi)*w[5];
		w[2] = (32-wi)*w[6];
		w[3] = (32-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
	
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);
				
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned char)(sum >> 23);
		}				
	}					
}


//a integer version of 3 dimensional interpolation
//this spport interpolations such as RGB to RGB, RGB to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//srcsize is the # of data needs to be interpolated
//dest has the same planes as table
//not been test yet
Linterp3::Linterp3(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[8];
	register long p[8];
	register unsigned long wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(32*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		for(j = 3; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs2;
		gi = index[m]*srcs;
		bi = index[y]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];

		w[4] = (32-wj)*(32-wk);
		w[5] = wj*(32-wk);
		w[6] = wj*wk;
		w[7] = (32-wj)*wk;
		
		w[0] = (32-wi)*w[4];
		w[1] = (32-wi)*w[5];
		w[2] = (32-wi)*w[6];
		w[3] = (32-wi)*w[7];
		
		w[4] *= wi;
		w[5] *= wi;
		w[6] *= wi;
		w[7] *= wi;
	
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + tablep;
		p[3] = p[0] + tablep;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + tablep;
		p[7] = p[4] + tablep;
		rangeguard(p, 8, range-1);
				
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 15);
		}								
	}	
					
}

//this might be obsolete
//floating point version
Linterp3::Linterp3(double *nouse, unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register double w[8];
	register long p[8];
	register double wi, wj, wk;
	register long ri, gi, bi;	
	long	srcs, srcs2;
	long	c, m, y;
	long	datachannel;
	double 	sum;
	double  cmyk[4];
	double	gstep;	//grid step
	unsigned char *des;
	long range;

	range = gridp*gridp*gridp*tablep;
		
	datachannel = 3;	
	srcs = gridp;
	srcs2 = srcs*srcs;
	
//assume from 0 to 255 which 256
	gstep = 255.0/(gridp - 1);		
		
	unsigned char* test;
	test = src;	
	des = dest;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
	
	//compute index	
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		
		ri = (long)(c/gstep);
		gi = (long)(m/gstep);
		bi = (long)(y/gstep);
		
	//compute weight			
		wi = (double)(c - ri*gstep)/gstep;
		wj = (double)(m - gi*gstep)/gstep;	
		wk = (double)(y - bi*gstep)/gstep;
		
		w[0] = (1-wi)*(1-wj)*(1-wk);
		w[1] = (1-wi)*wj*(1-wk);
		w[2] = (1-wi)*wj*wk;
		w[3] = (1-wi)*(1-wj)*wk;
		
		w[4] = wi*(1-wj)*(1-wk);
		w[5] = wi*wj*(1-wk);
		w[6] = wi*wj*wk;
		w[7] = wi*(1-wj)*wk;
	
	
		ri *= srcs2;
		gi *= srcs;
		
		//red channel
		p[0] = (ri+gi+bi);
		p[1] = p[0] + srcs;
		p[2] = p[1] + 1;
		p[3] = p[0] + 1;
		
		p[4] = p[0] + srcs2;
		p[5] = p[4] + srcs;
		p[6] = p[5] + 1;
		p[7] = p[4] + 1;
		rangeguard(p, 8, range-1);
				
		for( j = 0; j < 8; j++)
			p[j] *= datachannel;
		
		for( j = 0; j < datachannel; j++){
			sum = 0.0;
			for( n = 0; n < 8; n++)
				sum += table[p[n]++]*w[n];
			cmyk[j] = sum;
		}				
	
		*des++ = (unsigned char)(cmyk[0]/257.0 + 0.5);
		*des++ = (unsigned char)(cmyk[1]/257.0 - 128.0 + 0.5);
		*des++ = (unsigned char)(cmyk[2]/257.0 - 128.0 + 0.5);
	}			
		
}


//a integer version of 4 dimensional interpolation
//this spport interpolations such as CMYK to RGB, CMYK to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//dest has the same planes as table
//srcsize is the # of data needs to be interpolated
//can overwrite
//tested
Linterp4::Linterp4(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register unsigned long w[16];
	register long p[16];
	register unsigned long wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(64*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	srcs3 = gridp*gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		for(j = 4; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs3;
		gi = index[m]*srcs2;
		bi = index[y]*srcs;
		ki = index[k]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
		wq = wei[k];

		w[0] = (64-wi)*(64-wj)*(64-wk)*(64-wq);
		w[1] = (64-wi)*wj*(64-wk)*(64-wq);
		w[2] = (64-wi)*wj*wk*(64-wq);
		w[3] = (64-wi)*(64-wj)*wk*(64-wq);
		
		w[4] = wi*(64-wj)*(64-wk)*(64-wq);
		w[5] = wi*wj*(64-wk)*(64-wq);
		w[6] = wi*wj*wk*(64-wq);
		w[7] = wi*(64-wj)*wk*(64-wq);

		w[8] = (64-wi)*(64-wj)*(64-wk)*wq;
		w[9] = (64-wi)*wj*(64-wk)*wq;
		w[10] = (64-wi)*wj*wk*wq;
		w[11] = (64-wi)*(64-wj)*wk*wq;
		
		w[12] = wi*(64-wj)*(64-wk)*wq;
		w[13] = wi*wj*(64-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(64-wj)*wk*wq;
		
	
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + tablep;
		p[9] = p[1] + tablep;
		p[10] = p[2] + tablep;
		p[11] = p[3] + tablep;
	
		p[12] = p[4] + tablep;
		p[13] = p[5] + tablep;
		p[14] = p[6] + tablep;
		p[15] = p[7] + tablep;
		rangeguard(p, 16, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned char)(sum >> 24);
		}								
	}	
}

//tested
Linterp4::Linterp4(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[16];
	register long p[16];
	register unsigned long wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	unsigned long 	sum;
//	register long index[256];
//	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*gridp*tablep;

//some precompute
	gstep = 65535.0/(gridp - 1);		
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	srcs3 = gridp*gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		for(j = 4; j < srcp; j++)
			src++;


	//compute index	
		ri = (long)(c/gstep);
		gi = (long)(m/gstep);
		bi = (long)(y/gstep);
		ki = (long)(k/gstep);
		
	//compute weight			
		wi = (unsigned long)(16*(c/gstep - ri));
		wj = (unsigned long)(16*(m/gstep - gi));	
		wk = (unsigned long)(16*(y/gstep - bi));
		wq = (unsigned long)(16*(k/gstep - ki));
		
		ri *= srcs3;
		gi *= srcs2;
		bi *= srcs;
		ki *= tablep;

		w[0] = (16-wi)*(16-wj)*(16-wk)*(16-wq);
		w[1] = (16-wi)*wj*(16-wk)*(16-wq);
		w[2] = (16-wi)*wj*wk*(16-wq);
		w[3] = (16-wi)*(16-wj)*wk*(16-wq);
		
		w[4] = wi*(16-wj)*(16-wk)*(16-wq);
		w[5] = wi*wj*(16-wk)*(16-wq);
		w[6] = wi*wj*wk*(16-wq);
		w[7] = wi*(16-wj)*wk*(16-wq);

		w[8] = (16-wi)*(16-wj)*(16-wk)*wq;
		w[9] = (16-wi)*wj*(16-wk)*wq;
		w[10] = (16-wi)*wj*wk*wq;
		w[11] = (16-wi)*(16-wj)*wk*wq;
		
		w[12] = wi*(16-wj)*(16-wk)*wq;
		w[13] = wi*wj*(16-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(16-wj)*wk*wq;
		
	
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + tablep;
		p[9] = p[1] + tablep;
		p[10] = p[2] + tablep;
		p[11] = p[3] + tablep;
	
		p[12] = p[4] + tablep;
		p[13] = p[5] + tablep;
		p[14] = p[6] + tablep;
		p[15] = p[7] + tablep;
		rangeguard(p, 16, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 16);
		}								
	}	
}

//a integer version of 4 dimensional interpolation
//this spport interpolations such as CMYK to RGB, CMYK to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//dest has the same planes as table
//srcsize is the # of data needs to be interpolated
//can overwrite
//tested
Linterp4::Linterp4(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[16];
	register long p[16];
	register unsigned long wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(64*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	srcs3 = gridp*gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		for(j = 4; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs3;
		gi = index[m]*srcs2;
		bi = index[y]*srcs;
		ki = index[k]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
		wq = wei[k];

		w[0] = (64-wi)*(64-wj)*(64-wk)*(64-wq);
		w[1] = (64-wi)*wj*(64-wk)*(64-wq);
		w[2] = (64-wi)*wj*wk*(64-wq);
		w[3] = (64-wi)*(64-wj)*wk*(64-wq);
		
		w[4] = wi*(64-wj)*(64-wk)*(64-wq);
		w[5] = wi*wj*(64-wk)*(64-wq);
		w[6] = wi*wj*wk*(64-wq);
		w[7] = wi*(64-wj)*wk*(64-wq);

		w[8] = (64-wi)*(64-wj)*(64-wk)*wq;
		w[9] = (64-wi)*wj*(64-wk)*wq;
		w[10] = (64-wi)*wj*wk*wq;
		w[11] = (64-wi)*(64-wj)*wk*wq;
		
		w[12] = wi*(64-wj)*(64-wk)*wq;
		w[13] = wi*wj*(64-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(64-wj)*wk*wq;
		
	
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + tablep;
		p[9] = p[1] + tablep;
		p[10] = p[2] + tablep;
		p[11] = p[3] + tablep;
	
		p[12] = p[4] + tablep;
		p[13] = p[5] + tablep;
		p[14] = p[6] + tablep;
		p[15] = p[7] + tablep;
		rangeguard(p, 16, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 16);
		}								
	}	
					
}

//a integer version of 4 dimensional interpolation
//this spport interpolations such as CMYK to RGB, CMYK to LAB
//tablep is how many planes in the table, assume interleave format
//srcp is the planes in src, assume interleave format
//dest has the same planes as table
//srcsize is the # of data needs to be interpolated
//can overwrite
//tested
Linterp4::Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest)
{
	long i, j, n;
	register unsigned long w[16];
	register long p[16];
	register unsigned long wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(16*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	srcs3 = gridp*gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		for(j = 4; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs3;
		gi = index[m]*srcs2;
		bi = index[y]*srcs;
		ki = index[k]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
		wq = wei[k];

		w[0] = (16-wi)*(16-wj)*(16-wk)*(16-wq);
		w[1] = (16-wi)*wj*(16-wk)*(16-wq);
		w[2] = (16-wi)*wj*wk*(16-wq);
		w[3] = (16-wi)*(16-wj)*wk*(16-wq);
		
		w[4] = wi*(16-wj)*(16-wk)*(16-wq);
		w[5] = wi*wj*(16-wk)*(16-wq);
		w[6] = wi*wj*wk*(16-wq);
		w[7] = wi*(16-wj)*wk*(16-wq);

		w[8] = (16-wi)*(16-wj)*(16-wk)*wq;
		w[9] = (16-wi)*wj*(16-wk)*wq;
		w[10] = (16-wi)*wj*wk*wq;
		w[11] = (16-wi)*(16-wj)*wk*wq;
		
		w[12] = wi*(16-wj)*(16-wk)*wq;
		w[13] = wi*wj*(16-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(16-wj)*wk*wq;
		
	
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + tablep;
		p[9] = p[1] + tablep;
		p[10] = p[2] + tablep;
		p[11] = p[3] + tablep;
	
		p[12] = p[4] + tablep;
		p[13] = p[5] + tablep;
		p[14] = p[6] + tablep;
		p[15] = p[7] + tablep;
		rangeguard(p, 16, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned short)(sum >> 16);
		}								
	}	
					
}

//not tested yet
Linterp4::Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest)
{
	long i, j, n;
	register unsigned long w[16];
	register long p[16];
	register unsigned long wi, wj, wk, wq;
	register long ri, gi, bi, ki;	
	long	srcs, srcs2, srcs3;
	long	c, m, y, k;
	unsigned long 	sum;
	register long index[256];
	register long wei[256];
	double	gstep;
	long range;

	range = gridp*gridp*gridp*gridp*tablep;

//some precompute
	gstep = 255.0/(gridp - 1);		
	for(i = 0; i < 256; i++){
		index[i] = (long)(i/gstep);
		wei[i] = (long)(16*(i/gstep - index[i]) + 0.5);
	}	
		
//some initialization				
	srcs = gridp*tablep;
	srcs2 = gridp*gridp*tablep;
	srcs3 = gridp*gridp*gridp*tablep;

//4 dimensional interpolation
	for( i = 0; i < srcsize; i++){
			
		c = (long)*src++;
		m = (long)*src++;
		y = (long)*src++;
		k = (long)*src++;
		
		for(j = 4; j < srcp; j++)
			src++;
			
		ri = index[c]*srcs3;
		gi = index[m]*srcs2;
		bi = index[y]*srcs;
		ki = index[k]*tablep;
		
		wi = wei[c];
		wj = wei[m];
		wk = wei[y];
		wq = wei[k];

		w[0] = (16-wi)*(16-wj)*(16-wk)*(16-wq);
		w[1] = (16-wi)*wj*(16-wk)*(16-wq);
		w[2] = (16-wi)*wj*wk*(16-wq);
		w[3] = (16-wi)*(16-wj)*wk*(16-wq);
		
		w[4] = wi*(16-wj)*(16-wk)*(16-wq);
		w[5] = wi*wj*(16-wk)*(16-wq);
		w[6] = wi*wj*wk*(16-wq);
		w[7] = wi*(16-wj)*wk*(16-wq);

		w[8] = (16-wi)*(16-wj)*(16-wk)*wq;
		w[9] = (16-wi)*wj*(16-wk)*wq;
		w[10] = (16-wi)*wj*wk*wq;
		w[11] = (16-wi)*(16-wj)*wk*wq;
		
		w[12] = wi*(16-wj)*(16-wk)*wq;
		w[13] = wi*wj*(16-wk)*wq;
		w[14] = wi*wj*wk*wq;
		w[15] = wi*(16-wj)*wk*wq;
		
	
		p[0] = (ri+gi+bi+ki);
		p[1] = p[0] + srcs2;
		p[2] = p[1] + srcs;
		p[3] = p[0] + srcs;
		
		p[4] = p[0] + srcs3;
		p[5] = p[4] + srcs2;
		p[6] = p[5] + srcs;
		p[7] = p[4] + srcs;
		
		p[8] = p[0] + tablep;
		p[9] = p[1] + tablep;
		p[10] = p[2] + tablep;
		p[11] = p[3] + tablep;
	
		p[12] = p[4] + tablep;
		p[13] = p[5] + tablep;
		p[14] = p[6] + tablep;
		p[15] = p[7] + tablep;
		rangeguard(p, 16, range-1);
		
//finally, interpolation
		for( j = 0; j < tablep; j++){
			sum = 0;
			for( n = 0; n < 16; n++)
				sum += table[p[n]++]*w[n];
			*dest++ = (unsigned char)(sum >> 24);
		}								
	}	
					
}