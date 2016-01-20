/*=======================================================================*\

	FIXED.H - Fixed Point include header file.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef _FIXED_H_
#define _FIXED_H_

#ifndef WIN32
#define USING_FIXED16
#endif

typedef long  LFIXED16;
typedef LFIXED16 FAR *LPLFIXED16;
typedef LPLFIXED16 FAR *LPPLFIXED16;

#ifdef USING_FIXED16
#define FMUL16(num, fixed) fmul16( (long)num, (LFIXED16)fixed)
#define FGET16(n, d) fget16((long)n, (long)d)
#define TMUL16(num, fixed) tmul16( (long)num, (LFIXED16)fixed)
#define FP 16
#define WM 0xFFFF0000L
#define FINT(f) ((int)HIWORD(f))
#define FRAC(f) ((unsigned)LOWORD(f))
#define TOFIXED(num) ( (num)>0 ? ((long)(num))<<16 : -(((long)-(num))<<16) )
#define FHALF   0x00008000L
#define FONE    0x00010000L
#define UNITY		65536L
#define HALF		32768L
#define fix2int(num) ((int)((num) >> FP))
#define ROUND(num) ((int)(((num) + HALF) >> FP))
#define fix2fix(num) ((LFIXED16)((unsigned long)(num) >> FP))

typedef  LFIXED16 LFIXED;
#define DOUBLE(n) ((double)(n) / 65536.)
#define DBL2FIX(num) ((LFIXED16)(num * 65536.))
#define WHOLE(n) FINT(n)
#define WHOLEFIX(n) ((n) & WM)
#define FRACTION(n) FRAC(n)
#define FRACTION32(n) ((long)FRACTION(n))
#define FGET(n1, n2) FGET16(n1, n2)
#define FROUND(n) ROUND(n)
#define POSFROUND(n) ROUND(n)
#define FMUL(n1, n2) FMUL16(n1, n2)
#define FIXMUL(n1, n2) (DBL2FIX(DOUBLE(n1) * DOUBLE(n2)))
#define FIXDIV(n1, n2) (DBL2FIX(DOUBLE(n1) / DOUBLE(n2)))
#define FIXDIVWHOLE(n1, n2) ((n1) / (n2))
#define TMUL(n1, n2) TMUL16(n1, n2)
#define GETFIXED16(n) (n)
#define FUNITY UNITY
#define FDIV2(n) ((n)/2)
#define MAKEFRACTION(n) (n)
#define TINYRATE 1
#define MAKEFIXED(n) TOFIXED(n)

#else // Win32 (ifndef USING_FIXED16)

typedef double LFIXED;
#define DBL2FIX(num) (num)
#define WHOLE(n) ((int)((n) < 0 ? (n)-1 : (n)))
#define WHOLEFIX(n) WHOLE(n)
#define FRACTION(n) ((n) - (int)(n))
#define FRACTION32(n) ((long)(FRACTION(n) * (double)0x7FFFFFFF))
#define FGET(n1, n2) ((double)(n1) / (double)(n2))
#define ROUND(n) ((int)((n) < 0 ? (n) - 0.5 : (n) + 0.5))
#define FROUND(n) ((int)((n) < 0 ? (n) - 0.5 : (n) + 0.5))
#define POSFROUND(n) ((int)((n) + 0.5))
#define FMUL(n1, n2) FROUND((double)(n1) * (double)(n2))
#define FIXMUL(n1, n2) ((double)(n1) * (double)(n2))
#define FIXDIV(n1, n2) ((double)(n1) / (double)(n2))
#define FIXDIVWHOLE(n1, n2) (WHOLE((n1) / (n2)))
#define TMUL(n1, n2) (WHOLE((double)(n1) * (double)(n2)))
#define GETFIXED16(n) ((long)((n) * 65536.0))
#define DOUBLE(n) (n)
#define FUNITY 1.0
#define UNITY  1.0
#define HALF   0.5
//#define UNITY		65536L
//#define HALF		32768L

#define FHALF  0.5
#define FONE   1.0
#define FDIV2(n) ((n) / 2)
#define MAKEFRACTION(n) ((double)(n) / (double)0x7FFFFFFF)
#define TINYRATE (1.0 / (double)0x7FFFFFFF)
#define TOFIXED(num) ( (double)(num) )
#define MAKEFIXED(n) ((LFIXED)(n))
#define fix2int(num) ((int)num)

#endif // ifdef USING_FIXED16

typedef LFIXED FAR *LPLFIXED;
typedef LPLFIXED FAR *LPPLFIXED;

//**********************************************************************
// Fixed-Point Point definition
//**********************************************************************
typedef struct _fpoint
	{
	LFIXED	fx;
	LFIXED	fy;
} FPOINT;
typedef FPOINT FAR *LPFPOINT;

//**********************************************************************
// Fixed-Point Rectangle definition
//**********************************************************************
typedef struct _frect
	{
	LFIXED	left;
	LFIXED	top;
	LFIXED	right;
	LFIXED	bottom;
} FRECT;
typedef FRECT FAR *LPFRECT;

LPSTR    FixedAscii( LFIXED Fixed, LPSTR lpAscii, int Precision );
LFIXED   AsciiFixed( LPSTR lpAscii );
LFIXED16  fget16( long numer, long denom );
LFIXED16  special_fget16( LFIXED16 numer, LFIXED16 denom );
long    fmul16( long a, long b );
long    tmul16( long a, long b );

#ifdef UNUSED
LFIXED & fget( long numer, long denom );
CFixed & fget( long numer, CFixed & denom );
CFixed & fget( CFixed & numer, CFixed & denom );
LFIXED   special_fget( LFIXED numer, LFIXED denom );
CFixed & fmul( CFixed & a, CFixed & b );
long     fmul( long a, CFixed & b );
int      fmul( int a, CFixed & b );
long     fmul( CFixed & b, long a );
int      fmul( CFixed & b, int a );
int      fmul( int b, int a );
long     tmul( long a, long b );
/////////////////////////////////////////////////////////////////////////////
// CFixed - A 32:32 fixed point class.

class CFixed 
{

 public:
  
//    
// Constructors
//

    CFixed();
    CFixed( int initWhole );
    CFixed( long initWhole );
	 CFixed( double initDouble );
    CFixed( long initWhole, long initFraction);

//
// Overloaded Assignment Operators
//

   CFixed & operator = ( long value );
   CFixed & operator = ( int value );
   CFixed & operator = ( const double & value );
   CFixed & operator += ( const CFixed & value );
   CFixed & operator += ( long value );
   CFixed & operator -= ( const CFixed & value );
   CFixed & operator -= ( long value );
   CFixed & operator *= ( const CFixed & value );
   CFixed & operator *= ( long value );
   CFixed & operator /= ( const CFixed & value );
   CFixed & operator /= ( long value );
   CFixed & operator /= ( int value );
   CFixed & operator >>= ( int value );
   CFixed & operator <<= ( int value );

//
// Overloaded Relational Operators
//

   BOOL     operator < ( const CFixed & value );
   BOOL     operator < ( long value );
   BOOL     operator > ( const CFixed & value ) ;
   BOOL     operator > ( long value ); 
   BOOL     operator <= ( const CFixed & value );
   BOOL     operator <= ( long value ); 
   BOOL     operator >= ( const CFixed & value ); 
   BOOL     operator >= ( long value ); 

//
// Overloaded Equality Operators
//

   BOOL     operator == ( const CFixed & value );
   BOOL     operator == ( long value );
   BOOL     operator != ( const CFixed & value );
   BOOL     operator != ( long value );

//
// Overloaded Logical Operators
//

   BOOL     operator ! ( );
   BOOL     operator && ( const CFixed & value );
   BOOL     operator || ( const CFixed & value );

//
// Overloaded Shift Operators
//

   CFixed   operator >> ( int value );
   CFixed   operator << ( int value );

//
// Overloaded Unary Operators
//                    

   CFixed & operator + ( );
   CFixed operator - ( );
   CFixed & operator ++ ( );
   CFixed & operator -- ( );
   CFixed operator ++ ( int );
   CFixed operator -- ( int );

//
// Overloaded Arithmetic Operators
//                    

   CFixed operator + ( const CFixed & value );
   CFixed operator + ( long value );
   CFixed operator - ( const CFixed & value );
   CFixed operator - ( long value );
   CFixed operator * ( const CFixed & value );
   CFixed operator * ( long value );
   CFixed operator / ( const CFixed & value );
   CFixed operator / ( long value );
   CFixed operator / ( int value );

//
// Members Funtions
//

int	  CFixed::Whole();
CFixed   CFixed::Fraction();
int      CFixed::Round();
double & CFixed::Double();
long     CFixed::GetFixed16();
long 	  CFixed::Fraction32();

static CFixed & CFixed::Unity()
       {
	    return( CFixed( 1.0 ) );
		}


//
// Members
//

private:
	double	num;

};

         
//    
// Constructors
//

inline    CFixed::CFixed()
	 	{
		num = 0;
		}

inline    CFixed::CFixed( int initWhole )
	 	{
		num = initWhole;
		}

inline    CFixed::CFixed( long initWhole )
	 	{
		num = initWhole;
		}

inline	 CFixed::CFixed( double initDouble )
	 	{
		num = initDouble;
		}

inline    CFixed::CFixed( long initWhole, long initFraction)
      {
      num = initWhole;
      num += ((double)initFraction / (double)0x7FFFFFFF);
      }

//
// Overloaded Assignment Operators
//

inline   CFixed & CFixed::operator = ( long value )
		{
		num = value;
		return (*this);
		}

inline   CFixed & CFixed::operator = ( int value )
		{
		num = value;
		return (*this);
		}

inline   CFixed & CFixed::operator = ( const double & value )
		{
		num = value;
		return(*this);
		}

inline   CFixed & CFixed::operator += ( const CFixed & value )
		{
		num += value.num;
		return(*this);
		}

inline   CFixed & CFixed::operator += ( long value )
		{
		num += value;
		return(*this);
		}

inline   CFixed & CFixed::operator -= ( const CFixed & value )
		{
		num -= value.num;
		return(*this);
		}

inline   CFixed & CFixed::operator -= ( long value )
		{
		num -= value;
		return(*this);
		}

inline   CFixed & CFixed::operator *= ( const CFixed & value )
		{
		num *= value.num;
		return(*this);
		}

inline   CFixed & CFixed::operator *= ( long value )
		{
		num *= value;
		return(*this);
		}

inline   CFixed & CFixed::operator /= ( const CFixed & value )
		{
		num /= value.num;
		return(*this);
		}

inline   CFixed & CFixed::operator /= ( long value )
		{
		num /= value;
		return(*this);
		}

inline   CFixed & CFixed::operator /= ( int value )
		{
		num /= value;
		return(*this);
		}

inline   CFixed & CFixed::operator >>= ( int value )
      	{
	   	if (value == 1)
			num /= 2;
      	return( *this  );
		}  

inline   CFixed & CFixed::operator <<= ( int value )
      	{
	   	if (value == 1)
	   		num *= 2;
      	return( *this  );
		}  

//
// Overloaded Relational Operators
//

inline   BOOL     CFixed::operator < ( const CFixed & value )
		{
		return( num < value.num );
		}

inline   BOOL     CFixed::operator < ( long value )
		{
		return( num < value );
		}

inline   BOOL     CFixed::operator > ( const CFixed & value ) 
		{
		return( num > value.num );
		}

inline   BOOL     CFixed::operator > ( long value ) 
		{
		return( num > value );
		}

inline   BOOL     CFixed::operator <= ( const CFixed & value ) 
		{
		return( num <= value.num );
		}

inline   BOOL     CFixed::operator <= ( long value ) 
		{
		return( num <= value );
		}

inline   BOOL     CFixed::operator >= ( const CFixed & value ) 
		{
		return( num >= value.num );
		}

inline   BOOL     CFixed::operator >= ( long value ) 
		{
		return( num >= value );
		}

//
// Overloaded Equality Operators
//

inline   BOOL     CFixed::operator == ( const CFixed & value )
		{
		return( num == value.num );
		}

inline   BOOL     CFixed::operator == ( long value )
		{
		return( num == value );
		}

inline   BOOL     CFixed::operator != ( const CFixed & value )
		{
		return( num != value.num );
		}

inline   BOOL     CFixed::operator != ( long value )
		{
		return( num != value );
		}

//
// Overloaded Logical Operators
//

inline   BOOL     CFixed::operator ! ( )
		{
		return (!num);
		}

inline   BOOL     CFixed::operator && ( const CFixed & value )
		{
		return (num && value.num);
		}

inline   BOOL     CFixed::operator || ( const CFixed & value )
		{
		return (num || value.num);
		}

//
// Overloaded Shift Operators
//

inline   CFixed   CFixed::operator >> ( int value )
      	{
      	CFixed   temp = *this;

		if (value == 1)
			temp.num /= 2;
      	return( temp  );
		}  

inline   CFixed   CFixed::operator << ( int value )
      	{
      	CFixed   temp = *this;

	   	if (value == 1)
	   		temp.num *= 2;
      	return( temp  );
		}

//
// Overloaded Unary Operators
//                    

inline   CFixed & CFixed::operator + ( )
		{
		return(*this);
		}

inline   CFixed   CFixed::operator - ( )
		{
		return( CFixed( -num ) );
		}

inline   CFixed & CFixed::operator ++ ( )
		{
		++num;
		return(*this);
		}

inline   CFixed & CFixed::operator -- ( )
		{
		--num;
		return(*this);
		}

inline   CFixed   CFixed::operator ++ ( int )
		{
		CFixed temp = *this;
		++num;
		return(temp);
		}

inline   CFixed   CFixed::operator -- ( int )
		{
		CFixed temp = *this;
		--num;
		return(temp);
		}

//
// Overloaded Arithmetic Operators
//                    

inline   CFixed   CFixed::operator + ( const CFixed & value )
		{
		return( CFixed( num + value.num ) );
		}

inline   CFixed   CFixed::operator + ( long value )
		{
		return( CFixed( num + value ) );
		}

inline   CFixed   CFixed::operator - ( const CFixed & value )
		{
		return( CFixed( num - value.num ) );
		}

inline   CFixed  CFixed::operator - ( long value )
		{
		return( CFixed( num - value ) );
		}

inline   CFixed   CFixed::operator * ( const CFixed & value )
		{
		return( CFixed( num * value.num ) );
		}

inline   CFixed   CFixed::operator * ( long value )
		{
		return( CFixed( num * value ) );
		}

inline   CFixed   CFixed::operator / ( const CFixed & value )
		{
		return( CFixed( num / value.num ) );
		}

inline   CFixed   CFixed::operator / ( long value )
		{
		return( CFixed( num / value ) );
		}

inline   CFixed   CFixed::operator / ( int value )
		{
		return( CFixed( num / value ) );
		}

//
// Members Funtions
//

inline int	CFixed::Whole()
		{
		return ((int)(num < 0 ? num-1 : num));
		}

inline CFixed  CFixed::Fraction()
		{
		return (CFixed(num - (int)num));
		}



inline int    CFixed::Round()
		{
		return ((int)(num >= 0 ? num+0.5 : num-0.5));
		}

inline double & CFixed::Double()
		{
		return( num );
		}

inline long   CFixed::GetFixed16()
		{
		return ((long)(num * 65536.0));
		}

inline long CFixed::Fraction32()
      	{
		return((long)( Fraction().num * (double)0x7FFFFFFF ));
		}

typedef class CFixed FAR *LPCFIXED;
#endif
#endif // _FIXED_H_



