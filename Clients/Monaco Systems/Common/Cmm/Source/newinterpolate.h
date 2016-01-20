/******************************************************************************
* NewInterpolate.h
* 
* Copyright (C) 1995 - 1997, Monaco Systems, Incorporated and ONYX Graphics 
*		Corporation.  All Rights Reserved.
*
* Purpose: Interface for the NLinterp classes. These classes apply 
*					 8 or 16 bit sparse LUT's to a set of data using linear 
*					 interpolation. NLinterp3 is used for 3 dimensional LUT's.
*					 NLinterp4 is used for 4 dimensional LUT's.
******************************************************************************/

#ifndef NEW_INTERPOLATE_H
#define NEW_INTERPOLATE_H

/******************************************************************************
* Includes
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
* Class
******************************************************************************/

/******************************************************************************
* Class: NLinterp
* 
* Purpose: base class for LUT application using linear interpolation
******************************************************************************/
class NLinterp	{

protected:
	unsigned char* m_pTable;
	long					m_nGridPoints;
	long					m_nInputChannels;
	long 					m_nOutputChannels;
	long          			m_nSkipChannels;
	long 					m_index_r[256];
	long					m_index_g[256];
	long					m_index_b[256];
	long					m_index_w[256];
	unsigned long m_weight[256];
	long					m_offset_g, m_offset_r;


public:
	// construction
	NLinterp(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels);

	// operations
	virtual void interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest) = 0;
};

/******************************************************************************
* Class: NLinterp3
* 
* Purpose: 3 dimensional 8 bit LUT application class
******************************************************************************/
class NLinterp3: public NLinterp{

public:
	// construction
	NLinterp3(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels);
	
	// operations
	virtual void interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest);
};


/******************************************************************************
* Class: NLinterp3s
* 
* Purpose: 3 dimensional 16 bit LUT application class
******************************************************************************/
class NLinterp3s: public NLinterp{

public:
	// construction
	NLinterp3s(unsigned short *pTable, long nInputChannels, long nGridPoints, long nOutputChannels);
	
	// operations
	virtual void interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest);
};


/******************************************************************************
* Class: NLinterp4
* 
* Purpose: 4 dimensional 8 bit LUT application class
******************************************************************************/
class NLinterp4: public NLinterp{

protected:
	long m_offset_b;
	
public:
	// construction
	NLinterp4(unsigned char *pTable, long nInputChannels, long nGridPoints, long nOutputChannels);

	// operations
	virtual void interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest);
};


/******************************************************************************
* Class: NLinterp4s
* 
* Purpose: 4 dimensional 16 bit LUT application class
******************************************************************************/
class NLinterp4s: public NLinterp{

protected:
	long m_offset_b;
	
public:
	// construction
	NLinterp4s(unsigned short *pTable, long nInputChannels, long nGridPoints, long nOutputChannels);

	// operations
	virtual void interp(unsigned char *pSrc, long cbSrc, unsigned char *pDest);
};

/******************************************************************************
* Some old interpolation stuff use by CMM
* 
* 
******************************************************************************/

class Linterp1{
private:
protected:
public:

Linterp1(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest, long desp);

Linterp1(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp);
	
//do nothing
~Linterp1(void){};
	
};	

class Linterp1r{
private:
protected:
public:

Linterp1r(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, double *dest, long desp);
	
~Linterp1r(void){};	
};

class Linterp3{
private:
protected:
public:

//table:char, src:char, dest:char
Linterp3(unsigned char *table, long tablep, long gridp, 
	unsigned char *src, long srcp, long srcsize, unsigned char *dest);

Linterp3(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest);
	
//table:char, src:char, dest:short
Linterp3(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);

//table:short, src:char, dest:char
//this only works for ICC RGB to LAB
Linterp3(unsigned short *table, long tablep, long gridp, 
	unsigned char *src,long srcp, long srcsize, unsigned char *dest, long nouse);

//this works for other case
Linterp3(unsigned short *table, long tablep, long gridp, 
	unsigned char *src,long srcp, long srcsize, unsigned char *dest);

//not been test yet
//table:short, src:char, dest:short
Linterp3(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);

//this might be obsolete	
Linterp3(double *nouse, unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest);

//do nothing
~Linterp3(void){};

};

class Linterp4{
private:
protected:
public:

Linterp4(unsigned char *table, long tablep, long gridp, 
	unsigned char *src, long srcp, long srcsize, unsigned char *dest);

Linterp4(unsigned short *table, long tablep, long gridp, unsigned short *src, 
	long srcp, long srcsize, unsigned short *dest);

//not been test yet
Linterp4(unsigned char *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);
	
//not been test yet
Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned short *dest);
	
Linterp4(unsigned short *table, long tablep, long gridp, unsigned char *src, 
	long srcp, long srcsize, unsigned char *dest);

//do nothing
~Linterp4(void){};
};


#endif

