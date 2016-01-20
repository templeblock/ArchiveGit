#include "stdafx.h"
#include "Spell.h"
#include "Utility.h"

#define MSGBOX_TITLE "Spell Check"
#define MIN_CORRECTION_BUFFER 500

/////////////////////////////////////////////////////////////////////////////
CSpell::CSpell(bool bUK)
{
	m_bUK = bUK;
//j m_bUK = false; //j Temporarily force bUK to false
	m_hSpellLib = NULL;
	m_pfnGetSid = NULL;
	m_pfnCheckString = NULL;
	m_pfnCheckStringDlg = NULL;
	m_pfnGetStatistics = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CSpell::~CSpell()
{
	if (m_hSpellLib)
		::FreeLibrary(m_hSpellLib);
}

/////////////////////////////////////////////////////////////////////////////
void CSpell::GetFileNames(CString& strFileDLL, CString& strFileMainLex1, CString& strFileMainLex2, CString& strFileUserLex)
{
	strFileDLL      = "ssce5332.dll";
	strFileMainLex1 = (!m_bUK ? "ssceam.tlx" : "sscebr.tlx");
	strFileMainLex2 = (!m_bUK ? "ssceam2.clx" : "sscebr2.clx");
	strFileUserLex  = "correct.tlx";
}

/////////////////////////////////////////////////////////////////////////////
bool CSpell::Init(bool bCheckFiles, UINT idFileDLL, UINT idFileMainLex1, UINT idFileMainLex2, UINT idFileUserLex)
{
	if (m_hSpellLib)
		return true;

	CString strFileDLL;
	CString strFileMainLex1;
	CString strFileMainLex2;
	CString strFileUserLex;
	GetFileNames(strFileDLL, strFileMainLex1, strFileMainLex2, strFileUserLex);

	CString strDstPath;
	GetSpellCheckerPath(strDstPath);

	if (idFileDLL || idFileMainLex1 || idFileMainLex2 || idFileUserLex)
	{
		HINSTANCE hInstance = _AtlBaseModule.GetResourceInstance();
		WriteResourceToFile(hInstance, idFileDLL,      "SPELLCHECK", strDstPath + strFileDLL);
		WriteResourceToFile(hInstance, idFileMainLex1, "SPELLCHECK", strDstPath + strFileMainLex1);
		WriteResourceToFile(hInstance, idFileMainLex2, "SPELLCHECK", strDstPath + strFileMainLex2);
		WriteResourceToFile(hInstance, idFileUserLex,  "SPELLCHECK", strDstPath + strFileUserLex);
	}

	if (bCheckFiles)
	{
		bool bOK = FileExists(strDstPath + strFileMainLex1) && FileExists(strDstPath + strFileMainLex2) && FileExists(strDstPath + strFileUserLex);
		if (!bOK)
			return false;
	}

	m_hSpellLib = ::LoadLibrary(strDstPath + strFileDLL);
	if (!m_hSpellLib)
		return false;

	PFNSetKey pSetKey = (PFNSetKey)::GetProcAddress(m_hSpellLib, "SSCE_SetKey");
	if (!pSetKey || pSetKey(0x8359C65E) != SSCE_OK_RSLT)
	{
		::FreeLibrary(m_hSpellLib);
		m_hSpellLib = NULL;
		return false;
	}

	// Get the function pointers from the Spell Check DLL
	PFNSetMainLexPath pMainLexPath = (PFNSetMainLexPath)::GetProcAddress(m_hSpellLib, "SSCE_SetMainLexPath");
	if (pMainLexPath)
		SSCE_S16 svMainLexPath = pMainLexPath(strDstPath);

	PFNSetUserLexPath pUserLexPath = (PFNSetUserLexPath)::GetProcAddress(m_hSpellLib, "SSCE_SetUserLexPath");
	if (pUserLexPath)
		SSCE_S16 svUserLexPath = pUserLexPath(strDstPath);

	PFNSetMainLexFiles pMainLexFiles = (PFNSetMainLexFiles)::GetProcAddress(m_hSpellLib, "SSCE_SetMainLexFiles");
	if (pMainLexFiles)
		SSCE_S16 svMainLexFiles = pMainLexFiles(strFileMainLex1 + "," + strFileMainLex2); // "ssceam.tlx,ssceam2.clx"

	PFNSetUserLexFiles pUserLexFiles = (PFNSetUserLexFiles)::GetProcAddress(m_hSpellLib, "SSCE_SetUserLexFiles");
	if (pUserLexFiles)
		SSCE_S16 svUserLexFiles = pUserLexFiles(strFileUserLex);

	PFNResetLex pResetLex = (PFNResetLex)::GetProcAddress(m_hSpellLib, "SSCE_ResetLex");
	if (pResetLex)
		pResetLex();

	PFNSetAutoCorrect pSetAutoCorrect = (PFNSetAutoCorrect)::GetProcAddress(m_hSpellLib, "SSCE_SetAutoCorrect");
	if (pSetAutoCorrect)
		pSetAutoCorrect(false);

	PFNSetMinSuggestDepth pSetMinSuggestDepth = (PFNSetMinSuggestDepth)::GetProcAddress(m_hSpellLib, "SSCE_SetMinSuggestDepth");
	if (pSetMinSuggestDepth)
		pSetMinSuggestDepth(SSCE_MAX_SUGGEST_DEPTH/2);

	PFNSetOption pSetOption = (PFNSetOption)::GetProcAddress(m_hSpellLib, "SSCE_SetOption");
	if (pSetOption)
	{
		SSCE_S16 sid = 0; // m_pfnGetSid();
		SSCE_U32 OldValue = SSCE_OK_RSLT;
		OldValue = pSetOption(sid, SSCE_IGNORE_CAPPED_WORD_OPT, false);		// Default false
		OldValue = pSetOption(sid, SSCE_IGNORE_MIXED_CASE_OPT, false);		// Default false
		OldValue = pSetOption(sid, SSCE_IGNORE_MIXED_DIGITS_OPT, false);	// Default false
		OldValue = pSetOption(sid, SSCE_IGNORE_ALL_CAPS_WORD_OPT, true);	// Default false
		OldValue = pSetOption(sid, SSCE_IGNORE_NON_ALPHA_WORD_OPT, true);	// Default true
		OldValue = pSetOption(sid, SSCE_IGNORE_DOMAIN_NAMES_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_IGNORE_HTML_MARKUPS_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_REPORT_UNCAPPED_OPT, true);			// Default false*
		OldValue = pSetOption(sid, SSCE_REPORT_MIXED_CASE_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_REPORT_MIXED_DIGITS_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_REPORT_DOUBLED_WORD_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_CASE_SENSITIVE_OPT, true);			// Default true
		OldValue = pSetOption(sid, SSCE_SPLIT_HYPHENATED_WORDS_OPT, true);	// Default true
		OldValue = pSetOption(sid, SSCE_SPLIT_CONTRACTED_WORDS_OPT, false);	// Default false
		OldValue = pSetOption(sid, SSCE_SPLIT_WORDS_OPT, false);			// Default false
		OldValue = pSetOption(sid, SSCE_SUGGEST_SPLIT_WORDS_OPT, true);		// Default false*
		OldValue = pSetOption(sid, SSCE_SUGGEST_PHONETIC_OPT, false);		// Default false
		OldValue = pSetOption(sid, SSCE_SUGGEST_TYPOGRAPHICAL_OPT, true);	// Default true
		OldValue = pSetOption(sid, SSCE_STRIP_POSSESSIVES_OPT, true);		// Default true
		OldValue = pSetOption(sid, SSCE_ALLOW_ACCENTED_CAPS_OPT, true);		// Default true
	//	OldValue = pSetOption(sid, SSCE_LANGUAGE_OPT, false);
	//	OldValue = pSetOption(sid, SSCE_CHECK_SINGLE_WORD_OPT, false);
	//	OldValue = pSetOption(sid, SSCE_REPORT_SPELLING_OPT, false);
	}

	// Get the function pointers from the Spell Check DLL
	m_pfnGetSid = (PFNGetSid)::GetProcAddress(m_hSpellLib, "SSCE_GetSid");
	m_pfnCheckString = (PFNCheckString)::GetProcAddress(m_hSpellLib, "SSCE_CheckString");
	m_pfnCheckStringDlg = (PFNCheckStringDlg)::GetProcAddress(m_hSpellLib, "SSCE_CheckBlockDlgTmplt");
	m_pfnGetStatistics = (PFNGetStatistics)::GetProcAddress(m_hSpellLib, "SSCE_GetStatistics");
	if (!m_pfnGetSid || !m_pfnCheckString || !m_pfnCheckStringDlg || !m_pfnGetStatistics)
	{
		::FreeLibrary(m_hSpellLib);
		m_hSpellLib = NULL;
		m_pfnGetSid = NULL;
		m_pfnCheckString = NULL;
		m_pfnCheckStringDlg = NULL;
		m_pfnGetStatistics = NULL;
		return false;
	}

	return (m_hSpellLib != NULL);
}

//////////////////////////////////////////////////////////////////////
bool CSpell::SpellCheck(BYTE* pTextString, int iLength, int iMaxLength, DWORD* pdwWordsChecked, DWORD* pdwWordsChanged, DWORD* pdwErrorsDetected)
{
	if (!m_pfnGetSid || !m_pfnCheckString || !m_pfnCheckStringDlg || !m_pfnGetStatistics)
		return true; // bAborted

	// Call the spell checker SILENTLY on the string of words
	SSCE_S16 sid = m_pfnGetSid();
	SSCE_S32 cursor = 0;
	SSCE_CHAR errWord[SSCE_MAX_WORD_SZ];
	SSCE_CHAR otherWord[SSCE_MAX_WORD_SZ];
	SSCE_S16 mask = m_pfnCheckString(sid, pTextString, &cursor, errWord, sizeof(errWord), otherWord, sizeof(otherWord));
	if (mask == SSCE_END_OF_BLOCK_RSLT)
	{
		// We have to count the number of words checked ourselves because m_pfnGetStatistics() returns 0
		bool bIn = false;
		int nWords = 0;
		for (int i=0; i<iLength; i++)
		{
			char c = pTextString[i];
			if (c <= ' ')
				bIn = false;
			else
			if (bIn == false)
			{
				bIn = true; 
				nWords++;
			}
		}

		*pdwWordsChecked += nWords;
		return false; // not bAborted - no words misspelled and nothing changed
	}

	// The string contains at least one misspelled word
	// Call the spell checker INTERACTIVELY on the string of words
	HINSTANCE hInstance = _AtlBaseModule.GetResourceInstance();
	SSCE_S32 nReturnCount = m_pfnCheckStringDlg(::GetActiveWindow(), pTextString, iLength, iMaxLength, true/*bContext*/, hInstance, _T("CHECKSPELLINGDLG"), NULL, NULL, NULL);
	bool bAborted = (nReturnCount < 0); // an error or the user cancelled
	m_pfnGetStatistics(pdwWordsChecked, pdwWordsChanged, pdwErrorsDetected);
	return bAborted;
}

/////////////////////////////////////////////////////////////////////////////
bool CSpell::CheckString(const CString& strText, CString& strResult, int iFlags)
{
	CWaitCursor Wait;
	
	// Set the result in case we exit early
	strResult = strText;

	if (!m_hSpellLib)
	{
		::MessageBox(NULL, "The spell check library is not properly installed", MSGBOX_TITLE, MB_OK);
		return false;
	}

	DWORD dwWordsChecked = 0;
	DWORD dwWordsChanged = 0;
	DWORD dwErrors = 0;

	// Make sure the string buffer is 25% larger for error correction
	int iLength = strText.GetLength();
	int iMaxLength = iLength + max(iLength/4, MIN_CORRECTION_BUFFER);
	BYTE* pTextString = new BYTE[iMaxLength];
	::ZeroMemory(pTextString, iMaxLength);
	strncpy_s((char*)pTextString, iMaxLength, strText, iLength);
	bool bAborted = SpellCheck(pTextString, iLength, iMaxLength, &dwWordsChecked, &dwWordsChanged, &dwErrors);
	strResult = pTextString;
	delete [] pTextString;

	LPCSTR strMgsEx = (bAborted ? "has been cancelled" : "is complete");
	::MessageBox(NULL, String("The spell check %s.\n\n%d word(s) checked\n%d word(s) changed", strMgsEx, dwWordsChecked, dwWordsChanged), MSGBOX_TITLE, MB_OK);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CSpell::GetSpellCheckerPath(CString& strPath)
{
	char szFileName[MAX_PATH];
	
	//::GetSystemDirectory(szWinPath, sizeof(szWinPath));
	// Use Temporary directory instead of System Directory to download
	// Required to bypass the Vista bug where 
	// a program cannot download files into the 
	// protected directories
	::GetTempPath(MAX_PATH,szFileName);
	strPath = szFileName;
	strPath += '\\';
}

