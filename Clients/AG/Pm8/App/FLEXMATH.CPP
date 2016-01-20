/*
// FLEXMATH.C
//
//		Copyright (c) 1993 MicroLogic Software, Inc.
// 
//	Support for "flex-point" math in PrintMaster Gold.
//
// $Log: /PM8/App/FLEXMATH.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:30   Fred
// Initial revision.
// 
//    Rev 1.1   26 Aug 1996 18:44:48   Jay
// Fixed the infamous "flipped warp text" bug.
// 
//    Rev 1.0   14 Mar 1996 13:47:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:30   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:46   JAY
// Initial revision.
// 
//    Rev 1.1   30 Nov 1994 16:33:24   JAY
// stdafx.h; merged database code from album
//
//   Rev 1.0   16 Dec 1993 16:25:32   JAY
// 
// 
//    Rev 1.1   03 Jun 1993 17:07:56   MACDUFF
// Fixed bug in FlexToFraction where denominator could be zero.
// 
//    Rev 1.0   02 Jun 1993 23:51:28   MACDUFF
// Initial revision.
//
// $Header: /PM8/App/FLEXMATH.CPP 1     3/03/99 6:05p Gbeddow $
*/

/*
	A flex point number is a hybrid fixed-point number.  It is a 32-bit
	number, where the high bit determines the meaning of the lower 31 bits.
	If the high bit is clear, the remaining bits represent a 15.16 fixed
	point number.  If the high bit is set, they represent a 23.8 fixed
	point number.
*/

#include "stdafx.h"
#include "flexmath.h"
#include "utils.h"

#define IS_15DOT16(flex)  ((flex & 0x80000000L) == 0)

/*
// FlexToFrac
//
//		Convert a "flex point" number to a ratio of two unsigned 16-bit
// integers.  The resulting denominator will always be a power of two.
// The numerator will be even unless the denominator is one, in which
// case it may be odd.
//
// 	If the flex number is two big to represent as a fraction, the
// numerator is set to FFFF and the denominator to one.
*/

PUBLIC VOID FlexToFrac(FLEX aFlex, uint16 *aNum, uint16 *aDenom)
{
	uint32 nn;
	int16 shift;
	uint16 num, denom;

	if (!IS_15DOT16(aFlex))
	{
		if ((aFlex & 0x7f000000L) != 0)
		{
			/* Overflow */
			*aNum = 0xffff;
			*aDenom = 1;
			return;
		}
		nn = aFlex << 8;
	}
	else
	{
		nn = aFlex;
	}

#define intpart	(nn & 0xffff0000L)
#define fracpart	(nn & 0x0000ffffL)
#define int_highbit (nn & 0x80000000L)
#define frac_highbit (nn & 0x00008000L)

	shift = 0;
//	while (fracpart != 0 && int_highbit == 0 && shift < 15)
	while (fracpart != 0 && int_highbit == 0 && shift < 14)
	{
		++shift;
		nn <<= 1;
	}

	denom = 1 << shift;
	num = (uint16)(intpart >> 16);
	if (num != 0xffff && frac_highbit != 0)
	{
		++num;
	}
#undef intpart
#undef fracpart
#undef int_highbit
#undef frac_highbit

	*aNum = num;
	*aDenom = denom;
}

/*
// ShiftedLongToFlex
//
//		This function takes a 32-bit integer and a shift and produces a
// "flex-point" version of the fraction 'aLong >> aShift'.
//
*/

PUBLIC FLEX ShiftedLongToFlex(uint32 aLong, uint16 aShift)
{
	int16 int_bits;
	FLEX result;

	int_bits = bits_required_by(aLong) - aShift;

	if (int_bits <= 15)
	{
		result = aLong << (16 - aShift);
	}
	else if (int_bits <= 23)
	{
		result = aLong << (8 - aShift);
		result |= 0x80000000L;
	}
	else
	{
		result = 0xffffffffL;
	}
	return result;
}

PUBLIC FLEX ScaleFlex(FLEX aFlex, uint16 aNum, uint16 aDenom)
{
	FLEX nn;
	int16 bits;
	int16 shift;

	nn = aFlex;
	if (!IS_15DOT16(nn))
	{
		nn &= 0x7fffffffL;
		shift = 8;
	}
	else
	{
		shift = 16;
	}

	nn = ext_SafeLongMulDiv(nn, aNum, aDenom);

  	bits = bits_required_by(nn) - shift;

	if (bits <= 15)
	{
		if (shift != 16)
		{
		 	nn <<= (16 - shift);
		}
	}
	else if (bits <= 23)
	{
		if (shift != 8)
		{
			nn >>= (16 - shift);
		}
		nn |= 0x80000000L;
	}
	else
	{
		nn = 0xffffffffL;
	}
	return nn;
}

/*
// SplitFlex, UnsplitFlex
//
//		Utilities to enable PMG to store flex-point numbers as two
// separate SHORTs, misnamed the integer part and fractional part.
// This for backwards compatibility with the old representation of
// point size in the STYLE structure.
*/

PUBLIC VOID SplitFlex(FLEX aFlex, USHORT *aIntPart, USHORT *aFracPart)
{
	*aIntPart = (USHORT)(aFlex >> 16);
	*aFracPart = (USHORT)(aFlex & 0x0000ffffL);
}

PUBLIC FLEX UnsplitFlex(USHORT aIntPart, USHORT aFracPart)
{
	return (((uint32)aIntPart << 16) + (uint32)aFracPart);
}
