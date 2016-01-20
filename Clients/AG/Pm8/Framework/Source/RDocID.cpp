// ****************************************************************************
//
//  File Name:			RDocID.cpp
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

#include	"FrameworkIncludes.h"
#include "RDocID.h"
#include "Configuration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////
// Writes the RDocID object to a storage.
//
void RDocID::Write(
	RArchive& rArchive )	const // The storage to save the ID within.
{
	rArchive << m_nDocID;
}


/////////
// Reads the RDocID object from a storage.
//
void RDocID::Read(
	RArchive& rArchive )	// The storage to read the ID from.
{
	rArchive >> m_nDocID;
}

#ifdef _DEBUG
/////////
// Dumps RDocID information to a CDumpContext object
//
void RDocID::Dump(CDumpContext &dc)
{
	dc << "RDocID::m_nDocID = " << m_nDocID << " (" << GetIDString().operator CString() << ")\n";
}
#endif

/////////
// Get the ID in a base 36 string format.
//
// Returns: A string of <p nNumChars> characters representing the unique ID
//
RMBCString RDocID::GetIDString(
	int nNumChars)		// default = 3, number of chars to return
{
	// trap the unlikely bad argument
	if (nNumChars <= 0)
	{
		ASSERT(FALSE);
		nNumChars = 3;		// substitute the default
	}
	if (nNumChars > 8)
	{
		ASSERT(FALSE);
		nNumChars = 8;		// more than 8 would be all '0's anyway
	}

	// if there's no doc ID yet, generate one
	if (!HasID())
	{
		Generate();
	}

	// here's an array of 36 chars to pick from for the output string
	static char cBase36[36] =
	{
		'0','1','2','3','4','5','6','7','8','9',
		'a','b','c','d','e','f','g','h','i','j',
		'k','l','m','n','o','p','q','r','s','t',
		'u','v','w','x','y','z'	
	};

	// init an empty string
	CString str;
	str.Empty();

	// creating working variable based on Doc ID
	int nWork = m_nDocID;

	// loop for each output character
	while (nNumChars--)	
	{
		// put ASCII of modulo 36 in the string
		str += cBase36[nWork % 36];
		// divide working variable for next iteration
		nWork /= 36;
	}

	// return the filled in string
	return str;
}

/////////
// Generate a new identifier.  This reads the last identifier from the
//	registry, increments it, assigns the new one to this object and writes
//	it back out to the registry.
//
void RDocID::Generate()
{
	// get last ID from registry
	int nLastID = 0; // AfxGetApp()->GetProfileInt("Document", "LastDocID", 0);
	RDocIDConfiguration rRegistryDocID;
	RMBCString rstrValue;
	if (rRegistryDocID.GetStringValue( RDocIDConfiguration::kLastDocumentID, rstrValue ))
	{
		nLastID = atoi( rstrValue );
	}

	if (nLastID)	// if LastDocID was found
	{
		nLastID++;	// increment it for our new ID
	}
	else	// no nLastID found
	{
		// generate a new number less than 36^3 from the time
		nLastID = int(::time(NULL) % 46656) + 1;
	}

	// save our new ID
	m_nDocID = nLastID;

	// write back to registry
	// AfxGetApp()->WriteProfileInt("Document", "LastDocID", nLastID);
	CString strNewID;
	strNewID.Format( "%d", nLastID );
	rRegistryDocID.SetStringValue( RDocIDConfiguration::kLastDocumentID, RMBCString(strNewID) );
}
