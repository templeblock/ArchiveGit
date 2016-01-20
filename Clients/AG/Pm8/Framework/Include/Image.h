// ****************************************************************************
//
//  File Name:			Image.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RImage class (from which RBitmapImage
//							and RVectorImage will be derived)
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
//  $Logfile:: /PM8/Framework/Include/Image.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef uLONG YImageHandle;

class RDrawingSurface;
class RDataTransferSource;
class RDataTransferTarget;
class RChunkStorage;

class FrameworkLinkage RImage : public RObject
{
// Operations
public:	
	virtual void						Initialize(YImageHandle yHandle) = 0;
	virtual void						Render(RDrawingSurface& rDS, const RIntRect& rDestRect) = 0;
	virtual void						Read(RArchive& rArchive) = 0;
	virtual void						Write(RArchive& rArchive) const = 0;	
	virtual void						Copy(RDataTransferTarget& rDataTarget, YDataFormat yDataFormat) const = 0;	
	virtual void						Paste(const RDataTransferSource& rDataSource, YDataFormat yDataFormat) = 0;	

	virtual void						operator=(const RImage&) { ; }
	
	virtual YFloatType				GetAspectRatio() const = 0;
	virtual YImageHandle				GetSystemHandle() const = 0;
	virtual RRealSize					GetSizeInLogicalUnits() const = 0;

	virtual void						LockImage( )	{ ; }	//	No base implementation
	virtual void						UnlockImage( ) { ; }

#ifdef TPSDEBUG
public:
	virtual void						Validate() const {};
#endif // TPSDEBUG
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif // _IMAGE_H_