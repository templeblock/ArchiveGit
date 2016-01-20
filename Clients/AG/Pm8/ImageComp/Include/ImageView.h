// ****************************************************************************
//
//  File Name:			ImageView.h
//
//  Project:			Image
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RImageView class
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
//  $Logfile:: /PM8/ImageComp/Include/ImageView.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _IMAGEVIEW_H_
#define _IMAGEVIEW_H_

#ifndef	_COMPONENTVIEW_H_
#include "ComponentView.h"
#endif

#ifndef	_IMAGEDOCUMENT_H_
#include "ImageDocument.h"
#endif

#ifndef	_INTERFACE_H_
#include "Interface.h"
#endif

// ****************************************************************************
//
//  Class Name:	RImageView
//
//  Description:	The Bitmap view.
//
// ****************************************************************************
class RImageView : public RComponentView
{
public:
	// Construction & Destruction
	RImageView(const YComponentBoundingRect& rBoundingRect, RComponentDocument* pDocument, RView* pParentView);

public :
	RImageDocument*			GetImageDocument( ) const;

protected:
	// Operations
	virtual void				OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint );
	virtual void				Scale(const RRealPoint& rCenterOfScaling, const RRealSize& rScaleFactor, BOOLEAN fMaintainAspectRatio);

	virtual void				CopyBoundingRect( RComponentView* pSource );
	virtual void				CopyTransform( RComponentView* pSource );

	//	Functions for supporting extensions to the API through components
public :
	virtual BOOLEAN			GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const;
	virtual RInterface*		GetInterface( YInterfaceId id ) const;
	virtual BOOLEAN			EnableRenderCacheByDefault( ) const;
	virtual BOOLEAN			HitTest( const RRealPoint& point ) const;
	virtual void				CalculateOutlinePath( );

private :
	 BOOLEAN						PointInPath(const RRealPoint& point, RPath* rPath)const; 

// Debugging stuff
#ifdef TPSDEBUG
public:
	virtual void Validate() const;
#endif
};

// ****************************************************************************
//
//  Function Name:	RImageView::GetImageDocument( )
//
//  Description:		Return the document associated with this view 
//							(casted to a RImageDocument)
//
//  Returns:			(RImageDocument*)GetRDocument( );
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RImageDocument* RImageView::GetImageDocument( ) const
{
	TpsAssertIsObject( RImageDocument, GetRDocument( ));
	return static_cast<RImageDocument*> (GetRDocument( ) );
}

#endif _IMAGEVIEW_H_
