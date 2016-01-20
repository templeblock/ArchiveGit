
#ifndef _COMMONTYPES_H_
#define _COMMONTYPES_H_

#include "LaunchInterfaces.h"
#include "ProjectInfo.h"

#define UM_PUSH_DIALOG		(WM_USER + 100)
#define UM_POP_DIALOG		(WM_USER + 101)
#define UM_REMOVE_UNUSED	(WM_USER + 102)
#define UM_UPDATE_DATA		(WM_USER + 103)

extern CProjectInfo _ProjectInfo;

// ****************************************************************************
//
//  Function Name:	NumElements
//
//  Description:		Macro which resovles to the number of elements in the
//							specified array.
//
//  Returns:			The number of elements in the array
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
#define NumElements( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )

#endif // _COMMONTYPES_H_
