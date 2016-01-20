/*
// $Workfile: SPELLENG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLENG.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:25:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.0   31 Jul 1996 18:23:32   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "spelleng.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpellEngine::CSpellEngine()
{
	m_hLibrary = NULL;

	// Session management entry points.
	m_OpenSession = NULL;
	m_CloseSession = NULL;

	// Lexicon management entry points.
	m_OpenLex = NULL;
	m_CreateLex = NULL;
	m_CloseLex = NULL;
	m_AddToLex = NULL;
	m_GetLexInfo = NULL;

	// Word spell check entry points.
	m_CheckWord = NULL;
	m_Suggest = NULL;
}

CSpellEngine::~CSpellEngine()
{
	Shutdown();
}

BOOL CSpellEngine::Startup(void)
{
	// If we are already open, there is nothing to do.
	if (IsValid())
	{
		return TRUE;
	}

#ifdef WIN32
	LPCSTR pszDLLName = "SSCE4132.DLL";
#else
	LPCSTR pszDLLName = "SSCE4116.DLL";
#endif

	// We use the path manager to create a full path. This means that we will
	// ONLY be loading the DLL if it is in our directory. If this is undesirable,
	// (e.g. if we want to also be able to load from WINDOWS dir, etc), then
	// just pass the base name.
	CString csDLLName = GetGlobalPathManager()->ExpandPath(pszDLLName);

	// We don't want any error messages while we're loading the library.
	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);

	// Load the library.
	if ((m_hLibrary = LoadLibrary(csDLLName)) < (HINSTANCE)HINSTANCE_ERROR)
	{
		// Failed!
		ASSERT(FALSE);
		m_hLibrary = NULL;
	}

	// Restore the previous error mode.
	SetErrorMode(wOldSem);

	// And get the entry points from the DLL.
	return GetEntryPoints();
}

void CSpellEngine::Shutdown(void)
{
	if (m_hLibrary != NULL)
	{
		::FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}
}

BOOL CSpellEngine::GetEntryPoints(void)
{
	if (IsValid())
	{
		// Session management entry points.
		m_OpenSession = (FN_OpenSession)GetProcAddress(m_hLibrary, "SSCE_OpenSession");
		m_CloseSession = (FN_CloseSession)GetProcAddress(m_hLibrary, "SSCE_CloseSession");

		// Lexicon management entry points.
		m_OpenLex = (FN_OpenLex)GetProcAddress(m_hLibrary, "SSCE_OpenLex");
		m_CreateLex = (FN_CreateLex)GetProcAddress(m_hLibrary, "SSCE_CreateLex");
		m_CloseLex = (FN_CloseLex)GetProcAddress(m_hLibrary, "SSCE_CloseLex");
		m_AddToLex = (FN_AddToLex)GetProcAddress(m_hLibrary, "SSCE_AddToLex");
		m_GetLexInfo = (FN_GetLexInfo)GetProcAddress(m_hLibrary, "SSCE_GetLexInfo");

		// Word spell check entry points.
		m_CheckWord = (FN_CheckWord)GetProcAddress(m_hLibrary, "SSCE_CheckWord");
		m_Suggest = (FN_Suggest)GetProcAddress(m_hLibrary, "SSCE_Suggest");

		// Make sure we got all our entry points.
		if (m_OpenSession == NULL
			 || m_CloseSession == NULL
			 || m_OpenLex == NULL
			 || m_CreateLex == NULL
			 || m_CloseLex == NULL
			 || m_AddToLex == NULL
			 || m_GetLexInfo == NULL
			 || m_CheckWord == NULL
			 || m_Suggest == NULL)
		{
			ASSERT(FALSE);
			Shutdown();
		}
	}

	return IsValid();
}

static inline S16 InvalidEntryPoint(void)
{
	ASSERT(FALSE);
	return SSCE_UNSUPPORTED_ACTION_ERR;
}

// Session management
S16 CSpellEngine::OpenSession(void)
{
	if (m_OpenSession == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_OpenSession();
}

void CSpellEngine::CloseSession(S16 sid)
{
	if (m_CloseSession == NULL)
	{
		InvalidEntryPoint();
		return;
	}

	m_CloseSession(sid);
}

// Lexicon management.
S16 CSpellEngine::OpenLex(S16 sid, const char FAR *fileName, S32 memBudget)
{
	if (m_OpenLex == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_OpenLex(sid, fileName, memBudget);
}

S16 CSpellEngine::CreateLex(S16 sid, const char FAR *fileName, S16 type, S16 lang)
{
	if (m_CreateLex == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_CreateLex(sid, fileName, type, lang);
}

S16 CSpellEngine::CloseLex(S16 sid, S16 lexId)
{
	if (m_CloseLex == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_CloseLex(sid, lexId);
}

S16 CSpellEngine::AddToLex(S16 sid, S16 lexId, const SSCE_CHAR FAR *word, const SSCE_CHAR FAR *otherWord)
{
	if (m_AddToLex == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_AddToLex(sid, lexId, word, otherWord);
}

S16 CSpellEngine::GetLexInfo(S16 sid, S16 lexId, S32 FAR *size, S16 FAR *format, S16 FAR *type, S16 FAR *lang)
{
	if (m_GetLexInfo == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_GetLexInfo(sid, lexId, size, format, type, lang);
}

// Word spell check.
S16 CSpellEngine::CheckWord(S16 sid, U32 options, const SSCE_CHAR FAR *word, SSCE_CHAR FAR *replWord, S16 replWordSz)
{
	if (m_CheckWord == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_CheckWord(sid, options, word, replWord, replWordSz);
}

S16 CSpellEngine::Suggest(S16 sid, const SSCE_CHAR FAR *word, S16 depth, SSCE_CHAR FAR *suggBfr, S32 suggBfrSz, S16 FAR *scores, S16 scoreSz)
{
	if (m_Suggest == NULL)
	{
		return InvalidEntryPoint();
	}

	return m_Suggest(sid, word, depth, suggBfr, suggBfrSz, scores, scoreSz);
}
