// ****************************************************************************
//
//  File Name:			TpsStl.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Includes the STL files. We need to jump through a few
//							hoops to avoid conflicts with MFC.
//
//							I got most of this off of CompuServe.
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
//  $Logfile:: /PM8/Framework/Include/TpsStl.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_TPSSTL_H_
#define		_TPSSTL_H_

#ifdef	_WINDOWS
	// Turn off some warnings

	// string too long - truncated to 255 characters
	#pragma warning(disable: 4786)

	// unary minus operator applied to unsigned type, result still unsigned
	#pragma warning(disable: 4146)

	// '>' : signed/unsigned mismatch
	#pragma warning(disable: 4018)

	// same type qualifier used more than once
	#pragma warning(disable: 4114)
	
	// The memory allocation tracking placement operators give the STL a headache.
	// Disable them.  (This may need work for the release version!)
	#ifdef	new
		#undef new
		#define _REDEF_NEW
	#endif	// new

	// Pre include these files so that the apropriate symbols are NOT defined in
	// the namespace std...  (Causes major headaches!)
	#include <afx.h>
	#include <afxdisp.h>
	#include <new.h>
	#include <typeinfo.h>
	#include <stddef.h>
	#include <stdlib.h>
	#include <limits.h>
	#include <iostream.h>
#endif	// _WINDOWS

// include the STL in the std namespace. This is the namespace recommended by ANSI
namespace std
	{
	#include <algo.h>
	#include <function.h>
	#include <vector.h>
	}

#ifdef	_WINDOWS
	#undef	new
	#ifdef _REDEF_NEW
		#define new DEBUG_NEW
		#undef _REDEF_NEW
	#endif	// new
#endif	// _WINDOWS

#endif	// _TPSSTL_H_
