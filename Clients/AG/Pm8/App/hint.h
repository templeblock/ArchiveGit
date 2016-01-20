//
// $Workfile: hint.h $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
//
// Revision History:
//
// $Log: /PM8/App/hint.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 7     9/28/98 7:23p Hforman
// remove DAILY_TIP hint type
// 
// 6     7/21/98 7:55p Hforman
// zeroing out class members in constructor
// 
// 5     5/31/98 4:13p Psasse
// newest version of the help fields
// 
// 4     5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 3     12/22/97 5:25p Hforman
// 
// 2     12/12/97 5:19p Hforman
// work in progress
// 
// 1     12/05/97 5:11p Hforman
// 
//

#ifndef __HINT_H__
#define __HINT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Helpful Hint enums and class

enum HINT_WINDOW  // container types
{
	HELPFULHINT,
	DESIGNTIP,
};

enum HINT_TYPE
{
	// "list" type hints -- choose one from list
	PROJECT_HINT,	// Shown once when user enters project type.
	PLACE_HINT,		// Shown once when user enters certain areas.

	// single types -- display by name
	ACTION_HINT,	// Shown once when user does a specific action.

#if AS_NEEDED
	ONETIME_HINT,	// Shown once. Registration, etc.
	ERROR_HINT,		// Shown whenever user makes an error or stupid action.
#endif
};

// places where hints are shown
enum HINT_PLACE
{
	PLACE_Workspace,
	PLACE_Projects,
	PLACE_ArtGallery,
	PLACE_Sentiments,
	PLACE_AddressBook,
	PLACE_NewProjectDialog,
	PLACE_MasterPage,
	PLACE_BrandNew,
	PLACE_PrintDialog,
	PLACE_WithHelp,
	PLACE_Hub,
	PLACE_ReadyMade,
	PLACE_Last
};

class CHint
{
public:
	CHint()
	{
		m_ProjectOrPlace = 0;
		m_nHelpID = 0;
		m_nTutorialID = 0;
		m_fShownInSession = FALSE;
		m_nTimesShown = 0;
	}

	~CHint()		{}

	void			DoSerialize(CArchive& ar);

	HINT_TYPE	m_HintType;			// type of hint
	HINT_WINDOW	m_WinType;			// type of container
	int			m_ProjectOrPlace;	// enum (PROJECT_HINT or PLACE_HINT only)
	CString		m_strName;			// name of hint (ACTION_HINT only)
	CString		m_strTitle;			// title of hint
	CString		m_strText;			// text of hint
	CString		m_strWaveTitle;		// Name of associated Wave file
	UINT			m_nHelpID;			// help ID (if help available)
	UINT			m_nTutorialID;		// tutorial ID (if tutorial available)

	BOOL			m_fShownInSession;// has hint been seen this session?
	int			m_nTimesShown;		// how many times has user seen this hint?
};

// I'm putting this code here because this class is used in multiple
// projects. Makes things real easy.
inline void	CHint::DoSerialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_HintType;
		ar << m_WinType;
		ar << m_ProjectOrPlace;
		ar << m_strName;
		ar << m_strTitle;
		ar << m_strText;
		ar << m_nHelpID;
		ar << m_nTutorialID;
		ar << m_strWaveTitle;
	}
	else
	{
		int nTemp;
		ar >> nTemp;
		m_HintType = (HINT_TYPE)nTemp;
		ar >> nTemp;
		m_WinType = (HINT_WINDOW)nTemp;
		ar >> m_ProjectOrPlace;
		ar >> m_strName;
		ar >> m_strTitle;
		ar >> m_strText;
		ar >> m_nHelpID;
		ar >> m_nTutorialID;
		ar >> m_strWaveTitle;
	}
};

#endif // __HINT_H__
