// ****************************************************************************
//
//  File Name:			HeadlinesCanned.c
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Interface to define cannned headline effects
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlinesCanned.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include "WarpPath.h"
#include "HeadlinesCanned.h"

#ifndef	IMAGER
//	#include "RenaissanceResource.h"
#endif


const	int	kPathArraySize	= 512;

// ****************************************************************************
//
//  Function Name:	DefineHeadlineWarp( )
//
//  Description:		
//
//  Returns:			Boolean indicating successfull completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN  DefineHeadlineWarp( short warpId, RHeadlineGraphic* pHeadline )
{
BOOLEAN					success = FALSE;

	if ( warpId == kNoWarpShape )
		success = pHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( pHeadline->IsHorizontal() ) );
	else if ( warpId < kWarpShapeLast )
		success = pHeadline->ApplyPathToWarp( warpId );

	return success;	 
}


// ****************************************************************************
//
//  Function Name:	DefineHeadlinePath( )
//
//  Description:		
//
//  Returns:			Boolean indicating successfull completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN  DefineHeadlinePath( short warpId, RHeadlineGraphic* pHeadline )
{
BOOLEAN					success = FALSE;

	if ( warpId == kNoWarpShape )
		success = pHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( pHeadline->IsHorizontal() ) );
	else if ( warpId == kWarpPath00 )
	{
		success = pHeadline->ApplyNonDistort( RHeadlineGraphic::GetDefaultHeadlineSize( pHeadline->IsHorizontal() ) );
		if ( success )
			success = pHeadline->ApplyDistort( FALSE );
	}
	else if ( warpId < kWarpPathLast )
		success = pHeadline->ApplyPathToFollow( warpId );

	return success;	 
}





