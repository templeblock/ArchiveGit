// ****************************************************************************
//
//  File Name:			FrameworkComplexTypes.h
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of more complex types that needed some other
//							files to be included.
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
//  $Logfile:: /PM8/Framework/Include/FrameworkComplexTypes.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_FRAMEWORKCOMPLEXTYPES_H_
#define	_FRAMEWORKCOMPLEXTYPES_H_

// Component ids
typedef RMBCString YComponentType;

// Component bounding rect
typedef RRealVectorRect YComponentBoundingRect;

typedef RArray<YDataFormat> YDataFormatCollection;

//	Id's passed around for Merge Information (be it Address List or Custom List)
typedef	uBYTE YMergeId;
typedef	RArray< YMergeId >					RMergeFieldContainer;
typedef	RMergeFieldContainer::YIterator	RMergeFieldIterator;

#endif	// _FRAMEWORKCOMPLEXTYPES_H_