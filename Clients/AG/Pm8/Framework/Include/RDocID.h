// ****************************************************************************
//
//  File Name:			RDocID.h
//
//  Project:			Renaissance Framework
//
//  Ported By:			Richard E. Grenfell
//  Original Author:	Kevin Moore for Web Site Designer
//
//  Description:		Document ID object
//
//							Each Renaissance document has an almost unique identifier
//							associated with it.
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.
//							500 Redwood Blvd.
//							Novato, CA 94948
//
//  Copyright (C) 1998 Brøderbund Software, Inc., all rights reserved
//
//  $Logfile:: $
//   $Author:: $
//     $Date:: $
// $Revision:: $
//
// ****************************************************************************

#ifndef RDOCID_H
#define RDOCID_H

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

/////////
//
// Class Name:		RDocID
//
// Description: Container for unique Renaissance document identifier
//
class FrameworkLinkage RDocID
{
public:
	// public interface
	// default constructor
	RDocID() : m_nDocID(0)
		{}

	virtual void	Read(RArchive& archive );
	virtual void	Write(RArchive& archive ) const;

#ifdef TPSDEBUG
	// debug only, dumps RDocID information to the current dump context
	virtual void Dump(CDumpContext &dc);
#endif

	// Return the ID as a string
	RMBCString GetIDString(int nNumChars=3);

	// Resets the ID to the uninitialized state
	inline void Reset()
	{	m_nDocID = 0;	}

	// Returns TRUE if the ID is valid (non-zero)
	inline bool HasID() const
	{	return m_nDocID != 0;	}

protected:
	// Protected interface

	// Generate a new unique ID for this
	void Generate();

private:
	// @access private data
	// @cmember the identifier itself is an int
	sLONG m_nDocID;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// RDOCID_H
