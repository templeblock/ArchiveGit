/*
// $Workfile: PMWTEMPL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMWTEMPL.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     7/18/98 12:08p Jayn
// 
//    Rev 1.0   14 Aug 1997 15:24:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:56   Fred
// Initial revision.
// 
//    Rev 1.2   15 Jul 1997 15:56:56   Fred
// Flushes font cache when last project is closed
// 
//    Rev 1.1   07 May 1996 12:44:48   Jay
// From PMW2
// 
//    Rev 1.3   06 May 1996 16:45:44   JAY
// Project type support for New dialog.
// 
//    Rev 1.2   18 Jan 1996 17:09:56   JAY
// New project configuration class
// 
//    Rev 1.1   13 Sep 1995 11:48:20   JAY
// New 32-bit changes
// 
//    Rev 1.0   20 Jul 1995 12:52:56   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "file.h"
#include "pmwtempl.h"
#include "pmwcfg.h"
#include "pmgfont.h"
#ifndef WIN32
#include <auxdata.h>					// for AfxFindResourceHandle().
#endif

CPmwDocTemplate::CPmwDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
		: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
	DeleteMenu();
	m_nProjectType = 0;
}

void CPmwDocTemplate::AddDocument(CDocument* pDoc)
{
	CreateMenu();
	CMultiDocTemplate::AddDocument(pDoc);
}

void CPmwDocTemplate::RemoveDocument(CDocument* pDoc)
{
	CMultiDocTemplate::RemoveDocument(pDoc);
	if (GetFirstDocPosition() == NULL)
	{
		DeleteMenu();
	}

	// Check if there are now no active documents in PrintMaster. If so, delete
	// the contents of the font cache. This should make it much less likely that
	// the font cache will consume tremendous amounts of memory.
	BOOL fDocumentActive = FALSE;

	// Check the scratch template.
	if ((GET_PMWAPP()->m_pScratchTemplate != NULL)
	 && (GET_PMWAPP()->m_pScratchTemplate->GetFirstDocPosition() != NULL))
	{
		fDocumentActive = TRUE;
	}
	else
	{
		// Loop through each of the registered template types.
#ifdef WIN32
	   POSITION tpos = GET_PMWAPP()->GetFirstDocTemplatePosition();
#else
	   POSITION tpos = GET_PMWAPP()->m_templateList.GetHeadPosition();
#endif
	   while (tpos)
		{
#ifdef WIN32
	      CDocTemplate* pTemplate = GET_PMWAPP()->GetNextDocTemplate(tpos);
#else
		   CDocTemplate* pTemplate = (CDocTemplate*)(GET_PMWAPP()->m_templateList).GetNext(tpos);
#endif
			if (pTemplate->GetFirstDocPosition() != NULL)
			{
				// There is an active document.
				fDocumentActive = TRUE;
				break;
			}
		}
	}

	// If there is no active document, flush the font cache.
	if (!fDocumentActive)
	{
		od(">>> FLUSH FONT CACHE <<<\n\r");
		pFontCache->DeleteAll();
	}
}

void CPmwDocTemplate::CreateMenu(void)
{
	if (m_hMenuShared == NULL)
	{
		HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(m_nIDResource),
															 RT_MENU);
		m_hMenuShared = ::LoadMenu(hInst, MAKEINTRESOURCE(m_nIDResource));
	}
}

void CPmwDocTemplate::DeleteMenu(void)
{
	if (m_hMenuShared != NULL)
	{
		::DestroyMenu(m_hMenuShared);
		m_hMenuShared = NULL;
	}
}

CDocTemplate::Confidence CPmwDocTemplate::MatchDocType(const char* pszPathName,
					CDocument*& rpDocMatch)
{
	CDocTemplate::Confidence match = CDocTemplate::MatchDocType(pszPathName, rpDocMatch);

#if 0
	// This code causes PMOs to not load in Classic.
	// If we want to have the old functionalty (not allowing foreign project types opened
	// in a default way), then we need to put this back in with some provision for PMOs.
	if (!GetConfiguration()->SupportsUnknownProjects(FALSE)
			&& (match == CDocTemplate::yesAttemptForeign))
	{
		match = CDocTemplate::noAttempt;
	}
#endif
	return match;
}
