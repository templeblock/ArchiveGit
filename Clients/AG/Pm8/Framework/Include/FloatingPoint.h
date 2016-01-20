// ****************************************************************************
//
//  File Name:			FloatingPoint.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Floating point support functions
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/FloatingPoint.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FLOATINGPOINT_H_
#define		_FLOATINGPOINT_H_

//
//	The YFloatType is declaired as a float and not a double because we need precision,
//		but we need the speed of float more importantly.
typedef float YFloatType;

const YFloatType kSizeDelta = 0.01;
const YFloatType kTransformDelta	= 0.00001;
const YFloatType kFloatMax = FLT_MAX;

// ****************************************************************************
//
//  Function Name:	AreFloatsEqual( )
//
//  Description:		Comparision function for floating point numbers
//
//  Returns:			TRUE if the floats are equal within a delta
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN AreFloatsEqual( YFloatType f1, YFloatType f2, YFloatType delta = kSizeDelta )
	{
/*
	YFloatType temp = f1 - f2;
	if( temp > delta || temp < -delta )
		return FALSE;
	return TRUE;
*/

__asm 
		{
		fld             f1
		fld             f2
		fsub
		fabs
		
		fcomp           delta
		fnstsw          ax
		test            ah,1
		je              not_equal
		}
//	equal:
		return 1;
	not_equal:
		return 0;
	}

// ****************************************************************************
//
//  Function Name:	AreFloatsLT( )
//
//  Description:		Comparision function for floating point numbers
//
//  Returns:			TRUE if f1 is < f2
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN AreFloatsLT( YFloatType f1, YFloatType f2, YFloatType delta = kSizeDelta )
	{
	return( f1 < f2 && !AreFloatsEqual( f1, f2, delta ) );
	}

// ****************************************************************************
//
//  Function Name:	AreFloatsGT( )
//
//  Description:		Comparision function for floating point numbers
//
//  Returns:			TRUE if f1 is > f2
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN AreFloatsGT( YFloatType f1, YFloatType f2, YFloatType delta = kSizeDelta )
	{
	return( f1 > f2 && !AreFloatsEqual( f1, f2, delta ) );
	}

// ****************************************************************************
//
//  Function Name:	AreFloatsLTE( )
//
//  Description:		Comparision function for floating point numbers
//
//  Returns:			TRUE if f1 is <= f2
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN AreFloatsLTE( YFloatType f1, YFloatType f2, YFloatType delta = kSizeDelta )
	{
	return( f1 < f2 || AreFloatsEqual( f1, f2, delta ) );
	}

// ****************************************************************************
//
//  Function Name:	AreFloatsGTE( )
//
//  Description:		Comparision function for floating point numbers
//
//  Returns:			TRUE if f1 is >= f2
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN AreFloatsGTE( YFloatType f1, YFloatType f2, YFloatType delta = kSizeDelta )
	{
	return( f1 > f2 || AreFloatsEqual( f1, f2, delta ) );
	}

// ****************************************************************************
//
//  Function Name:	Round( )
//
//  Description:		Rounds a floating point number 
//
//  Returns:			The rounded number
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline int Round( YFloatType n )
	{
	if( n >= 0.0 )
		return (int)( n + 0.5001 );
	else
		return (int)( n - 0.4999 );
	}

// ****************************************************************************
//
//  Function Name:	Round( )
//
//  Description:		Rounds a floating point number 
//
//  Returns:			The rounded number
//							
//  Exceptions:		None
//
// ****************************************************************************
//
inline YFloatType Round(YFloatType yValue, uWORD uDecimal)
{
	YFloatType yPowerOf10 = (YFloatType)pow(10.0, (double)uDecimal);
	return ((YFloatType)::Round(yValue * yPowerOf10)) / yPowerOf10;
}

#endif	//	_FLOATINGPOINT_H_
