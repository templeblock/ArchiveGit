// ****************************************************************************
//
//  File Name:			NameAddress.h
//
//  Project:			Renaissance
//
//  Author:				Michael Houle
//
//  Description:		Declaration of the RAddress class and address formatting functions
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
//  $Logfile:: /PM8/Framework/Include/NameAddress.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_NAMEADDRESS_H_
#define		_NAMEADDRESS_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//
// @Class Container for Addresses and Address Manipulation
class FrameworkLinkage RNameAddress
{
	public :

	static void		FormatNameAddress( RMBCString& output, const RMBCString& format,
									const RMBCString& firstName, const RMBCString& lastName,	const RMBCString& address,
									const RMBCString& city, const RMBCString& state, const RMBCString& zip, const RMBCString& country );

	static void		FormatNameAddress( RMergeFieldContainer& output, const RMBCString& format,
										YMergeId firstName, YMergeId lastName, YMergeId address,
										YMergeId city, YMergeId state, YMergeId zip, YMergeId country );
} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _NAMEADDRESS_H_

