// ****************************************************************************
//
//  File Name:			DataInterface.h
//
//  Project:			Renaissance Application 
//
//  Author:				Lance Wilson
//
//  Description:		Declaration of the IDataInterface interface
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 482-4400
//
//  Copyright (C) 1999 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Interfaces/DataInterface.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DATAINTERFACE_H_
#define		_DATAINTERFACE_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

#include "Interface.h"

class RBuffer;
class RImage;

const	YInterfaceId	kBufferInterfaceId		= 500;
const	YInterfaceId	kImageBufferInterfaceId	= 501;

// ****************************************************************************
//
//  Class Name:	IBufferInterface
//
//  Description:	Definition of the ILinkedData interface. 
//
// ****************************************************************************
//
class IBufferInterface  : public IUnknownInterface
{
public:

	virtual HRESULT			GetFileName( RMBCString& strFileName ) = 0;
	virtual HRESULT			Read( RBuffer& buffer ) = 0;
	virtual HRESULT			Write( RBuffer& buffer ) = 0;
};

// ****************************************************************************
//
//  Class Name:	IImageBuffer
//
//  Description:	Definition of the IImageBuffer interface. 
//
// ****************************************************************************
//
class IImageBuffer  : public IBufferInterface
{
public:

	virtual RImage*			CreateImage() = 0;
	virtual HRESULT			ExportImage() = 0;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_DATAINTERFACE_H_
