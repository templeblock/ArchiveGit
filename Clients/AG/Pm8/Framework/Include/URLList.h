// ****************************************************************************
//
//  File Name:			RURLList.h
//
//  Project:			Framework
//
//  Author:				Claire Schendel
//
//  Description:		Declaration of the RURLList class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************
#ifndef _RURLList_H_
#define _RURLList_H_

#include "URL.h"

const uWORD		kURLListGrowSize = 16;

// ****************************************************************************
//
//  Class Name:	RURLList
//
//  Description: The URL list class. 
//				 All references to a URL are indicies into this list.
//
// ****************************************************************************
//

class FrameworkLinkage RURLList : public RArray<RURL>
{
	public:

											RURLList( );
											RURLList( const RURLList& rhs );
		virtual								~RURLList( );

	//	Operations
	public :

		const RURL *						AddURL( RURL &pURL );
		const RURL *						FindURL( YURLId id ) const;
		YFontId								FindURL( RURL &pURL ) const;

		void								WriteURLList( RChunkStorage& storage ) const;
		void								ReadURLList( RChunkStorage& storage );
		void								Write( RArchive& archive ) const;
		void								Read( RArchive& archive );

		RURLList&							operator=( const RURLList& rhs );
};

#endif 
