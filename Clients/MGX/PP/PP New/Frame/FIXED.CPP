/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "mgxframe.h"
#include "frameint.h"

/***********************************************************************/
LFIXED16  fget16( long numer, long denom )
/***********************************************************************/
{
long    val;
char    flag;

if ( numer < 0L )
	{
	numer = -numer;
	flag = 1;
	}
else    flag = 0;
if ( denom < 0L )
	{
	denom = -denom;
	flag = !flag;
	}

while ( numer > 0xFFFFL )
	{
	numer++; numer >>= 1;
	denom++; denom >>= 1;
	}

if ( !denom )
	return(0);

val = ((unsigned long)numer << 16) / denom; /* always round it down */

if ( flag )
	return( -val );
else    return( val );
}


/***********************************************************************/
LFIXED16  special_fget16( LFIXED16 numer, LFIXED16 denom )
/***********************************************************************/
{
long    val;
char    flag;
int shifted;

if ( numer < 0L )
	{
	numer = -numer;
	flag = 1;
	}
else    flag = 0;

if (denom < 0L)
	{
	denom = -denom;
	flag = !flag;
	}

if (!denom)
	return(0L);

shifted = 0;
// shift numer as high as we can
while (shifted < 16 && !(numer & 0x80000000L))
	{
	numer <<= 1;
	++shifted;
	}
val = ((unsigned long)numer) / denom; /* always round it down */
shifted = 16-shifted;
if (shifted)
	val <<= shifted;

if ( flag )
	return( -val );
else    return( val );
}

/***********************************************************************/
long    fmul16( long a, long b )
/***********************************************************************/
{
DWORD t;
BYTE negative;

if ( negative = (a<0) )
	a = -a;
if ( b<0 )
	{
	b = -b;
	negative ^= 1;
	}

t = ( (ULONG)LOWORD(a) * LOWORD(b) ) + HALF;
t = HIWORD(t);

if ( HIWORD(a) )
	{
	t += ( (ULONG)HIWORD(a) * HIWORD(b) ) << 16;
	t += ( (ULONG)HIWORD(a) * LOWORD(b) );
	}
if ( HIWORD(b) )
	t += ( (ULONG)LOWORD(a) * HIWORD(b) );
if ( negative )
	return( -((long)t) );
else    return( t );
}

/***********************************************************************/
long    tmul16( long a, long b )
/***********************************************************************/
{
DWORD t;
BYTE negative;

if ( negative = (a<0) )
	a = -a;
if ( b<0 )
	{
	b = -b;
	negative ^= 1;
	}

t = ( (ULONG)LOWORD(a) * LOWORD(b) );
t = HIWORD(t);

if ( HIWORD(a) )
	{
	t += ( (ULONG)HIWORD(a) * HIWORD(b) ) << 16;
	t += ( (ULONG)HIWORD(a) * LOWORD(b) );
	}
if ( HIWORD(b) )
	t += ( (ULONG)LOWORD(a) * HIWORD(b) );
if ( negative )
	return( -((long)t) );
else    return( t );
}

// only set once, so its ok
static char cDecimal = '\0';

/***********************************************************************/
LPSTR FixedAscii( LFIXED Fixed, LPSTR lpAscii, int Precision )
/***********************************************************************/
{
int         i, len, intg, frac;
char        flag, c;
 char szFixed[16];
LPSTR       p;
LFIXED		temp;

if ( Fixed < 0L )
	{
	Fixed = -Fixed;
	flag = 1;
	}
else    flag = 0;

temp = FRACTION(Fixed);

if ( !cDecimal )
	{
	GetProfileString( "intl", "sDecimal", ".", szFixed, sizeof(szFixed) );
	cDecimal = szFixed[0];
	}

if ( Precision >= -1 ) // Allow the caller to force the decimal to be '.'
	c = cDecimal;
else    c = '.';

if ( Precision < 0 )
	Precision = 10000;
else    {
	i = Precision;
	Precision = 1;
	while ( --i >= 0 )
		Precision *= 10;
	}

intg = WHOLE(Fixed);
frac = FMUL( Precision, temp );
if ( frac >= Precision )
	{
	frac = 0;
	intg += 1;
	}

// Add the interger part
if ( flag )
	{
	szFixed[0] = '-';
	Int2Ascii( szFixed+1, intg );
	}
else    Int2Ascii( szFixed, intg );

// Add the fractional part, and the decimal character
len = lstrlen( szFixed );
Int2Ascii( &szFixed[len], Precision+frac );
szFixed[len] = c;

// Remove trailing 0's
len = lstrlen( szFixed );
p = &szFixed[len] - 1;
while( *p == '0' )
	*p-- = '\0';

// Make sure a trailing 0 follows the European decimal character
if ( c == ',' && (len = lstrlen( szFixed )) && szFixed[len-1] == ',' )
	lstrcat( szFixed, "0" );

lstrcpy( lpAscii, szFixed );
return( lpAscii );
}


/***********************************************************************/
LFIXED AsciiFixed( LPSTR  lpAscii )
/***********************************************************************/
{
 char	szFixed[16];
char           neg;
LPSTR          pFixed, pFrac, p;
LFIXED         Fixed, Frac;
int            len;

STATICTABLE int  table[] = { 1, 10, 100, 1000, 10000 };

#define DECIMAL 4

if ( !cDecimal )
	{
	GetProfileString( "intl", "sDecimal", ".", szFixed, sizeof(szFixed) );
	cDecimal = szFixed[0];
	}

pFixed = szFixed;
lstrcpy( pFixed, lpAscii );

/* See if there is a fractional part */
pFrac = 0;
p = pFixed;
neg = 0;
while ( *p )
	{
	if ( *p == '-' )
		neg = 1; /* to trap the '-.123' cases */
	if ( *p == cDecimal )
		{
		*p = '\0';
		pFrac = p+1;
		break;
		}
	p++;
	}

/* The integer part */
Fixed = FGET( Ascii2Int( pFixed ), 1 );
if ( pFrac )
	{
	if ( ( len = lstrlen( pFrac ) ) > 0 )
		{
		if ( len > DECIMAL )
			{
			len = DECIMAL;
			pFrac[len] = '\0';
			}
		/* The fractional part */
		Frac = FGET( Ascii2Int( pFrac ), table[len] );
		if ( Fixed < 0 || neg )
			Fixed -= Frac;
		else    Fixed += Frac;
		}
	}

return( Fixed );
}


#ifdef UNUSED

/***********************************************************************/
LFIXED  fget( long numer, long denom )
/***********************************************************************/
{
CFixed n1 = numer;
CFixed n2 = denom;

   return( n1 / n2 );
}

/***********************************************************************/
LFIXED  fget( long numer, CFixed denom )
/***********************************************************************/
{
CFixed n1 = numer;

   return( n1 / denom );
}

/***********************************************************************/
LFIXED fget( CFixed numer, CFixed denom )
/***********************************************************************/
{

   return( numer / denom );
}

#endif

#ifdef UNUSED

/***********************************************************************/
CFixed fmul( CFixed a, CFixed b )
/***********************************************************************/
{
   return( a * b );
}

/***********************************************************************/
long     fmul( long a, CFixed b )
/***********************************************************************/
{
   CFixed   c;

   c = b * a;
   return( c.Whole() );
}

/***********************************************************************/
int      fmul( int a, CFixed b )
/***********************************************************************/
{
   CFixed   c;

   c = b * ( long )a;
   return( ( int )c.Whole() );
}

/***********************************************************************/
long     fmul( CFixed b, long a )
/***********************************************************************/
{
   CFixed   c;

   c = b * a;
   return( c.Whole() );
}

/***********************************************************************/
int      fmul( CFixed b, int a )
/***********************************************************************/
{
   CFixed   c;

   c = b * ( long )a;
   return( ( int )c.Whole() );
}

/***********************************************************************/
int      fmul( int b, int a )
/***********************************************************************/
{
   CFixed   c;

   c = b * a;
   return( ( int )c.Whole() );
}
#endif

#ifdef UNUSED
  
#define LOPART(l)           ((DWORD)(l) & 0x0000FFFF)
#define HIPART(l)           ((DWORD)(l) >> 16)

//    
// Constructors
//

    CFixed::CFixed()
      {
      fraction = 0;
      whole = 0;
	   fSign = whole < 0;
      }
      
    CFixed::CFixed( long initWhole )
      {
      whole = initWhole;
      fraction = 0;
	   fSign = whole < 0;
      }
      
    CFixed::CFixed( long initWhole, long initFraction )
      {
      whole = initWhole;
      fraction = initFraction;
	   fSign = whole < 0;
      }

//
// Overloaded Assignment Operators
//

   CFixed & CFixed::operator = ( long value ) 
      {
      fraction = 0;
      whole = value;
	   fSign = whole < 0;
      return( *this );
      }

   CFixed & CFixed::operator = ( int value ) 
      {
      fraction = 0;
      whole = value;
	   fSign = whole < 0;
      return( *this );
      }

   CFixed & CFixed::operator = ( double value ) 
      {
	   double temp;

      whole = ( long )value;
	   fSign = whole < 0;
	   temp = value - (double)whole;
	   if (temp < 0)
	   		{
			if (!whole)
				fSign = TRUE;
	   		temp = -temp;
			}
      fraction = (DWORD)( temp * (double)0x7FFFFFFF );
	   fraction += fraction;
      return( *this );
      }

   CFixed & CFixed::operator += ( CFixed value )
      {
      return( *this = *this + value);
      }

   CFixed & CFixed::operator += ( long value )
      {
      return( *this = *this + value);
      }

   CFixed & CFixed::operator -= ( CFixed value )
      {
      return( *this = *this - value);
      }

   CFixed & CFixed::operator -= ( long value )
      {
      return( *this = *this - value);
      }

   CFixed & CFixed::operator *= ( CFixed value )
      {
      return( *this = *this * value );
      }

   CFixed & CFixed::operator *= ( long value )
      {
      return( *this = *this * value );
      }


   CFixed   CFixed::operator /= ( CFixed value )
      {
      return( *this = *this / value );
      }


   CFixed   CFixed::operator /= ( long value )
      {
      return( *this = *this / value );
      }

   CFixed   CFixed::operator /= ( int value )
      {
      return( *this = *this / value );
      }

   CFixed    CFixed::operator >>= ( int value ) 
      {
      return( *this = *this >> value );
      }  

   CFixed    CFixed::operator <<= ( int value ) 
      {
      return( *this = *this << value );
      }  
        
//
// Overloaded Relational Operators
//

   BOOL     CFixed::operator < ( CFixed value ) 
      {
      return( ( whole < value.whole ) || ( whole == value.whole &&
                ( !fSign && !value.fSign ? fraction < value.fraction :
						fSign && value.fSign ? value.fraction < fraction :
						fSign && !value.fSign ? FALSE : TRUE  ) ) );
      }

   BOOL     CFixed::operator < ( long value ) 
      {
		CFixed	n1( value, 0 );

      return( *this < n1 );
		}


   BOOL     CFixed::operator > ( CFixed value ) 
      {
      return( ( whole > value.whole ) || ( whole == value.whole &&
                ( !fSign && !value.fSign ? fraction > value.fraction :
						fSign && value.fSign ? value.fraction > fraction :
						fSign && !value.fSign ? TRUE : FALSE  ) ) );
      }

   BOOL     CFixed::operator > ( long value ) 
      {
		CFixed	n1( value, 0 );

      return( *this > n1 );
      }

   BOOL     CFixed::operator <= ( CFixed value ) 
      {
      return( *this < value || *this == value );
      }

   BOOL     CFixed::operator <= ( long value ) 
      {
		CFixed	n1( value, 0 );

      return( *this < n1 || *this == n1 );
      }

   BOOL     CFixed::operator >= ( CFixed value ) 
      {
      return( *this > value || *this == value );
      }

   BOOL     CFixed::operator >= ( long value ) 
      {
		CFixed	n1( value, 0 );

      return( *this > n1 || *this == n1 );
      }

//
// Overloaded Equality Operators
//

   BOOL     CFixed::operator == ( CFixed value )
      { return( value.whole == whole && value.fraction == fraction &&
	   			value.fSign == fSign ); }

   BOOL     CFixed::operator == ( long value )
      {
		CFixed	n1( value, 0 );

		return( *this == n1 ); }

   BOOL     CFixed::operator != ( CFixed value )
      {
	   return((value.whole != whole) || (value.fraction != fraction) ||
	   		(value.fSign != fSign));
	   }

   BOOL     CFixed::operator != ( long value )
      {
		CFixed	n1( value, 0 );

		return( *this != n1 ); }


//
// Overloaded Logical Operators
//

   BOOL     CFixed::operator ! ( )
      { return( whole == 0 && fraction == 0 ); }

   BOOL     CFixed::operator && ( CFixed value )
      {
	   return( ( whole != 0 || fraction != 0 ) &&
					 ( value.whole != 0 || value.fraction != 0 ) );
	   }

   BOOL     CFixed::operator || ( CFixed value )
      { return( whole != 0 || value.fraction != 0 ||
					 value.whole != 0 || value.fraction != 0 ); }


//
// Overloaded Shift Operators
//

   CFixed    CFixed::operator >> ( int value ) 
      {
      CFixed   temp;

      temp.fraction = ( fraction >> value ) | ( whole << ( 32 - value ) );
      temp.whole = whole >> value;
		temp.fSign = temp.whole < 0;
      return( temp  ); }  

   CFixed    CFixed::operator << ( int value ) 
      {
      CFixed   temp;

      temp.whole = ( whole << value ) | ( fraction >> ( 32 - value ) );
      temp.fraction = fraction << value;
		temp.fSign = temp.whole < 0;
      return( temp  ); }  
        
//
// Overloaded Unary Operators
//                    

   CFixed   CFixed::operator + ( )
      {
 		CFixed temp = *this;


       temp.whole = +temp.whole; return( temp ); }

   CFixed   CFixed::operator - ( )
      {
 
 		CFixed temp = *this;


       temp.whole = -temp.whole;
		if (temp.whole)
			temp.fSign = temp.whole < 0;
		else if (temp.fraction)
			temp.fSign = !temp.fSign;
		else
			temp.fSign = FALSE;
			
		return( temp ); }

   CFixed   CFixed::operator ++ ( )
      {
 		CFixed n1(0, 1);
       return( *this = *this + n1); }

   CFixed   CFixed::operator -- ( )
      {
 		CFixed n1(0, 1);
       return( *this = *this - n1); }

   CFixed   CFixed::operator ++ ( int )
      {
	   CFixed	n1(0, 1);
	   CFixed   n2 = *this;

	   *this = *this + n1;
       return( n2 ); }

   CFixed   CFixed::operator -- ( int )
      {
	   CFixed	n1(0, 1);
	   CFixed	n2 = *this;

	   *this = *this - n1;
       return( n2 ); }

        
//
// Overloaded Arithmetic Operators
//                    

   CFixed   CFixed::operator + ( CFixed value )
	{
	CFixed   temp = *this;
	BOOL		fTempSign;

	if (temp.fSign == value.fSign)
		{
		temp.whole += value.whole;
		if (temp.fSign)
			{
			temp.whole -= 
			HIPART
				(
				HIPART( LOPART(temp.fraction) + LOPART(value.fraction) ) 
				+
				HIPART(temp.fraction) + HIPART(value.fraction)
				);
			}
		else
			{
			temp.whole += 
			HIPART
				(
				HIPART( LOPART(temp.fraction) + LOPART(value.fraction) ) 
				+
				HIPART(temp.fraction) + HIPART(value.fraction)
				);
			}
		temp.fraction += value.fraction;
		return(temp);
     	}
	else
		{
		if (fTempSign = temp.fSign)
			temp = -temp;
		else
			value = -value;
		if (temp > value)
			{
			temp.whole -= value.whole;
			if (temp.fraction >= value.fraction)
				temp.fraction -= value.fraction;
			else // do borrow
				{
				temp.fraction = value.fraction - temp.fraction;
				temp.fraction = ~temp.fraction;
				--temp.whole;
				}
			if (fTempSign)
				temp = -temp;
			return(temp);
			}
		else
			{
			value.whole -= temp.whole;
			if (value.fraction >= temp.fraction)
				value.fraction -= temp.fraction;
			else  // do borrow
				{
				value.fraction = temp.fraction - value.fraction;
				value.fraction = ~value.fraction;
				--value.whole;
				}
			if (!fTempSign)
				value = -value;
			return(value);
			}
		}
	}

   CFixed   CFixed::operator + ( long value )
      {
	   CFixed	n1(value, 0);

	   return(n1 = *this + n1);
	   }

   CFixed   CFixed::operator - ( CFixed value )
      {
      CFixed   temp = *this;

	   temp = temp + (-value);
      return( temp ); }

   CFixed   CFixed::operator - ( long value )
      {
	   CFixed	n1(value, 0);

	   return(n1 = *this - n1);
	   }

   CFixed   CFixed::operator * ( CFixed value )
      {
      CFixed   n1 = *this, n2;
	   BOOL neg;
	   DWORD fraction;

       if( neg = n1.fSign )
         n1 = -n1;
       if( value.fSign )
         {
         neg = !neg;
         value = -value;
         }
 
       n2.whole = n1.whole * value.whole;

		n2.whole += ( LOPART( n1.fraction ) * HIPART( value.whole ) ) >> 16;
       n2.whole += ( HIPART( n1.fraction ) * LOPART( value.whole ) ) >> 16;
       n2.whole += HIPART( n1.fraction ) * HIPART( value.whole );              
    
       n2.whole += ( LOPART( value.fraction ) * HIPART( n1.whole ) ) >> 16;
       n2.whole += ( HIPART( value.fraction ) * LOPART( n1.whole ) ) >> 16;
       n2.whole += HIPART( value.fraction ) * HIPART( n1.whole );              

       fraction = HIPART( n1.fraction ) * HIPART( value.fraction ); 
		n2.fraction = fraction;
       fraction += ( LOPART( n1.fraction ) * HIPART( value.fraction ) ) >> 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
       fraction += ( HIPART( n1.fraction ) * LOPART( value.fraction ) ) >> 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;

       fraction += LOPART( n1.fraction ) * LOPART( value.whole );
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
       fraction += ( LOPART( n1.fraction ) * HIPART( value.whole ) ) << 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
       fraction += ( HIPART( n1.fraction ) * LOPART( value.whole ) ) << 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
    								
       fraction += LOPART( value.fraction ) * LOPART( n1.whole );
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
       fraction += ( LOPART( value.fraction ) * HIPART( n1.whole ) ) << 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
       fraction += ( HIPART( value.fraction ) * LOPART( n1.whole ) ) << 16;
		if (fraction < n2.fraction)
			++n2.whole;
		n2.fraction = fraction;
    
       if( neg )
         	n2 = -n2;

       return( n2 ); }

   CFixed   CFixed::operator * ( long value )
      {
      CFixed   n1(value, 0);

	   return(n1 = *this * n1);
	   }

   CFixed   CFixed::operator / ( CFixed value )
      {
      CFixed   n1;

       return( n1 = this->Double() / value.Double() ); }

   CFixed   CFixed::operator / ( long value )
      	{
     	CFixed   n1(value, 0);
		return( n1 = *this / n1 ); }

   CFixed   CFixed::operator / ( int value )
      {
     	CFixed   n1(value, 0);
		return( n1 = *this / n1 ); }

//
// Members Funtions
//

int      CFixed::Round()
         {
         CFixed n1( 0, 0x80000000 );

		  if (whole < 0)
		  	n1 = -n1;
      
         n1 += *this;
         return( ( int )n1.whole );
         }
         

double   CFixed::Double()
         {
         double   d1;
      
         d1 = ( double )whole;
		  if (fSign)
         	d1 -= ( double )fraction / 0xFFFFFFFF;
		  else
         	d1 += ( double )fraction / 0xFFFFFFFF;
         return( d1 );
         }
         
long     CFixed::GetFixed16()
         {

         return( ( long )( ( whole << 16 ) | ( ( WORD )( fraction >> 16 ) ) ) );
         }
#endif         

