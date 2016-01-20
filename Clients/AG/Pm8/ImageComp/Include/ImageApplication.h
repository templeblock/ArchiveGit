// ****************************************************************************
//
//  File Name:			ImageApplication.h
//
//  Project:			Image Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the ImageApplication class
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
//  $Logfile:: /PM8/ImageComp/Include/ImageApplication.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_IMAGEAPPLICATION_H_
#define		_IMAGEAPPLICATION_H_

#include "ComponentApplication.h"

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RImageApplication
//
//  Description:	The Bitmap Application.
//
// ****************************************************************************
//
class RImageApplication : public RComponentApplication
{
public:
	// Component overrides
	virtual void							GetSupportedComponentTypes(RComponentTypeCollection& rComponentTypeCollection) const;
	virtual BOOLEAN						CanPasteDataFormat(YDataFormat yDataFormat, YComponentType& yComponentType) const;
	virtual RComponentDocument*		CreateNewDocument(const RComponentAttributes& rComponentAttributes, const YComponentType& yComponentType, BOOLEAN fLoading);

#ifdef	TPSDEBUG
public:
	virtual void							Validate() const;
#endif
};

#endif _IMAGEAPPLICATION_H_
