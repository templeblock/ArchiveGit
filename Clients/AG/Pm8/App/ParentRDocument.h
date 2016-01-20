/*	$Header: /PM8/App/ParentRDocument.h 1     3/03/99 6:08p Gbeddow $
//
//	Definition of the CPParentRDocument class.
//	This class is neccessary for the certain legacy dialogs brought in from
//	the Print Shop code base.  Size and font information are requested by these
//	processes and must be supplied through this channel.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/App/ParentRDocument.h $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     2/08/99 3:06p Rgrenfel
// Fixed default sizing to allow for all component types.
// 
// 1     2/01/99 11:42a Rgrenfel
// Wrapper for a parent renaissance document for suplying default sizing
// information for components.  Necessary for certain component
// construction dialogs.
*/

#ifndef PARENTRDOCUMENT_H
#define PARENTRDOCUMENT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StandaloneDocument.h"

#include "ObjectSize.h"	// Used for default size determination.

const	YRealDimension kMinObjectAspectThreshHold = ((1440 * 3) / 4);

/* Overload of the Renaissance Standalone document which provides an interface
	for their components to converse with our application.  This class is
	neccessary for the certain legacy dialogs brought in from the Print Shop
	code base.  Size and font information are requested by these processes and
	must be supplied through this channel.
*/
class CPParentRDocument : public RStandaloneDocument 
{
public :
	// Class constructor.
	CPParentRDocument()
		: RStandaloneDocument()
		{ }
	// Class destructor.
	virtual	~CPParentRDocument()
		{ FreeDocumentContents(); }

public :

	// @Overload for the object's maximum size.
	virtual RRealSize	GetMaxObjectSize( const YComponentType& componentType )
		{ return( RRealSize(INT_MAX, INT_MAX ) ); }
	// Overload for the object's minimum size.
	virtual RRealSize	GetMinObjectSize( const YComponentType& componentType, BOOLEAN fMaintainAspect = FALSE )
		{ return( RRealSize(1, 1) ); }
	// Overload for the default size of the object.
	virtual RRealSize	GetDefaultObjectSize( const YComponentType& componentType )
		{
			// Convert the type to one the object size class can understand.
			RObjectSize::EObjectType eType = RObjectSize::GetObjectType( componentType );

			// Determine the aspect ratio of the object based on its type.
			YRealDimension yAspectRatio = RObjectSize::GetAspectRatio( eType );
			RRealSize rSize( 5400, 5400 ); // 3.75 inches square as a default.

			rSize.m_dx *= RObjectSize::GetDefaultSizePortrait( eType );
			rSize.m_dy = rSize.m_dx / yAspectRatio;

			return rSize;
		}

	// Provides the font size information.
	virtual const RFontSizeInfo*	GetFontSizeInfo( ) const
		{ return &m_FontInfo; }

	// Not yet implemented.  Pure virtual override which is stubbed out.
	virtual void	GetSaveFileFormats( RFileFormatCollection& ) const
		{ UnimplementedCode( ); }
	// Not yet implemented.  Pure virtual override which is stubbed out.
	virtual void	SetDefaultFileFormat( )
		{ UnimplementedCode( ); }

private :
	// Needed for pure vitual function.
	RFontSizeInfo			m_FontInfo;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
	// Validates the document.
	virtual void	Validate( ) const
	{
		RStandaloneDocument::Validate();
	}
	
#endif

};

#endif // PARENTRDOCUMENT_H
