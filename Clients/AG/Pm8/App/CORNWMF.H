/*
// $Workfile: CORNWMF.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CORNWMF.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:36   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:34   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:14   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:12   FRED
//  
// 
//    Rev 1.1   19 Sep 1994 11:53:58   FRED
// Some enhancements.
// 
// 
//    Rev 1.0   16 Sep 1994 10:48:42   FRED
// Initial revision.
*/


class COrnamentWMF;
class COrnamentWMFState;

#ifndef	__CORNWMF_H__
#define	__CORNWMF_H__

class COrnamentWMFState : public COrnamentState
{

public:

	// management

	COrnamentWMFState();
	virtual ~COrnamentWMFState();
	
	// implementation
	
	void Init(void);
	
	DECLARE_OBJECT_PROPERTIES(COrnamentWMFState)
	DEFINE_OBJECT_PROPERTY_COPY()

public:

	DEFINE_OBJECT_PROPERTY_BOOL(Stretch, PID_Stretch, "Stretch")
};


class COrnamentWMF : public COrnament
{

public:

	// management
	
	COrnamentWMF();
	virtual ~COrnamentWMF();
	
	// implementation
	
	DEFINE_PROPERTY_STATE(COrnamentWMFState)
	
	virtual void Init(CDrawContext* pContext, LPCSTR pszFileName, BOOL fReset = TRUE);
	virtual void Init(CDrawContext* pContext, HINSTANCE hInstance, LPCSTR pszResource, BOOL fReset = TRUE);
	
	virtual BOOL AppearanceChanged(CPropertyState* pState) const;
	virtual BOOL StateChanged(CPropertyState* pState) const;

	virtual int SizeForRepetitionsX(int nRepetitions);
	virtual int SizeForRepetitionsY(int nRepetitions);
	virtual int RepetitionsForSizeX(int nSize);
	virtual int RepetitionsForSizeY(int nSize);
	virtual int UnitSizeForRepetitionsInSizeX(int nRepetitions, int nSize);
	virtual int UnitSizeForRepetitionsInSizeY(int nRepetitions, int nSize);

protected:

	// helper functions
	
	virtual void CommonInit(void);
	virtual void SizeContent(void);
	virtual void DrawMask(CDC* pDC);
	virtual void DrawImage(CDC* pDC);
	
	// members
	
	CWindowsMetaFile m_MetaFile;
};

#endif		// #ifndef __CORNWMF_H__
