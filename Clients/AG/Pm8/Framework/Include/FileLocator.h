// ****************************************************************************
//
//  File Name:			UNCLocator.h
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RUNCLocator class
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
//  $Logfile:: /PM8/Framework/Include/FileLocator.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef _FILELOCATOR_H_
#define _FILELOCATOR_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RChunkStorage;

class FrameworkLinkage RFileLocator : public RObject
{
friend RChunkStorage& operator<<(RChunkStorage& lhs, RFileLocator& rhs);
friend RChunkStorage& operator>>(RChunkStorage& lhs, RFileLocator& rhs);

public:
	enum EMediaType
	{
		kUnknown,
		kRemovable,
		kFixed,
		kRemote,
		kCompactDisc,
		kRamDisk
	};
	
	// Construction/destuction
	RFileLocator();	
	RFileLocator(const RFileLocator& rhs);
	~RFileLocator();

	// Operations
	void								Set(const RMBCString& rPath);
	void								Reset();
	
	RFileLocator&					operator=(const RFileLocator& rhs);

	// Accessors
	inline const RMBCString&	GetUNCPath();	
	inline uLONG					GetSerialNumber();
	inline EMediaType				GetMediaType();
	
private:
	RMBCString						m_rUNCPath;	
	uLONG								m_uSerialNumber;
	EMediaType						m_eMediaType;	

	void								SetFilePath(const RMBCString& rPath);
	void								SetVolumeInfo(const RMBCString& rPath);		
	
// Debugging support
#ifdef TPSDEBUG
public :
	virtual void Validate() const;
#endif //TPSDEBUG
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_FILELOCATOR_H_