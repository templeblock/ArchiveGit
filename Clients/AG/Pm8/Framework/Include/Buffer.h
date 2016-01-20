// ****************************************************************************
//
//  File Name:			Buffer.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RBuffer class
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
//  $Logfile:: /PM8/Framework/Include/Buffer.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifndef _CHUNKY_STORAGE_H_
#include "ChunkyStorage.h"
#endif

#ifndef _STORAGESUPPORT_H_
#include "StorageSupport.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class FrameworkLinkage RBuffer : public RObject
{
friend FrameworkLinkage RArchive& operator<<(RArchive& lhs, const RBuffer& rhs) {lhs << rhs.m_uBufferSize; if (rhs.m_uBufferSize > 0) lhs.Write(rhs.m_uBufferSize, rhs.m_pBuffer); return lhs;}
friend FrameworkLinkage RArchive& operator>>(RArchive& lhs, RBuffer& rhs) {uLONG uBufferSize; lhs >> uBufferSize;	rhs.Resize(uBufferSize); if (uBufferSize > 0) lhs.Read(uBufferSize, (uBYTE*)rhs.m_pBuffer); return lhs;}

public:
	// Construction/Destruction
								RBuffer() {m_pBuffer = NULL; m_uBufferSize = 0;}
	virtual					~RBuffer() {delete [] m_pBuffer;}

	// Operations
	virtual void			Resize(uLONG uBufferSize) {delete [] m_pBuffer; m_pBuffer = new uBYTE[uBufferSize]; m_uBufferSize = uBufferSize;}
	virtual void			Empty() {delete [] m_pBuffer; m_pBuffer = NULL; m_uBufferSize = 0;}

	// Accessors
	uBYTE*					GetBuffer() const {return m_pBuffer;}
	uLONG						GetBufferSize() const {return m_uBufferSize;}	

protected:
	uBYTE*					m_pBuffer;
	uLONG						m_uBufferSize;

#ifdef TPSDEBUG
public:
	virtual void Validate() const {}
#endif //TPSDEBUG
};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_BUFFER_H_