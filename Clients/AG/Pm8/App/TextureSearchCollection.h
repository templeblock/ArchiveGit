// ****************************************************************************
//
//  File Name:			TextureSearchCollection.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of class that performs a category search on
//							a texture collection file, suitable for use as the
//							parameterized type of an RCollectionSearcher
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _TextureSearchCollection_H_
#define _TextureSearchCollection_H_


#include "BackdropSearchCollection.h" // RBackdropSearchCollection base class


class RTextureSearchCollection : public RBackdropSearchCollection
{
public:
						RTextureSearchCollection( RChunkStorage *aChunkStorage )
						: RBackdropSearchCollection( aChunkStorage )
						{}
						// construct an RTextureSearchCollection from aChunkStorage

	virtual void	SetSearchCriteria( const RSearchCriteria* /* aSearchCriteria */ )
						{}
						// do nothing - all textures are always displayed

	virtual BOOLEAN MatchSearchCriteria()
						{ return TRUE; }
						// compare the current index entry to search criteria
						// set by SetSearchCriteria()
						// return TRUE if equal, else FALSE
						// always return TRUE - all textures are always displayed
};


#endif	// _TextureSearchCollection_H_
