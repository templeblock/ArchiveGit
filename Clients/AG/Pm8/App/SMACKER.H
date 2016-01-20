/*
// $Workfile: SMACKER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SMACKER.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:56   Fred
// Initial revision.
// 
//    Rev 1.0   31 Mar 1997 10:37:54   Fred
// Initial revision.
*/

#ifndef __SMACKER_H__
#define __SMACKER_H__

#ifdef WIN32
#define __RAD32__
#else
#define __RADWIN__
#endif

// needed for smacker movie playing
#include "../smacker/rad.h"
#include "../smacker/smack.h"

class CSmacker : public CObject
{
	DECLARE_DYNAMIC(CSmacker)

	// return values from PlayMovie()
	enum
	{
		SmkFailure,		// something went wrong
		SmkDone,			// movie fully played
		SmkInterrupt	// movie cut short by user
	};

public:
	CSmacker(CWnd* pParent, LPCSTR pszFilename);
	virtual ~CSmacker();

	int PlayMovie(BOOL bNoInterrupt = FALSE);
	void SetMaxFrameRate(BOOL bMax = TRUE);
	void SetSoundOff(BOOL bOff = TRUE);

private:
	int DoAdvance(CDC* pDC, int nX, int nY);

	// Data
	SmackBuf* m_sbuf;
	Smack* m_smk;

	CString m_csPathname;
	CWnd* m_pParent;
	BOOL m_bMaxFrameRate;
	BOOL m_bSoundOff;
};

#endif // __SMACKER_H__
