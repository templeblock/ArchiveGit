#include "stdafx.h"
#include "EditFunctions.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CEditFunctions::CEditFunctions() 
{
	m_hWnd = NULL;
	m_AGCBFormat = 0;
	m_pClipboardObject = NULL;
	m_bIsADuplicate = false;
}

//////////////////////////////////////////////////////////////////////
CEditFunctions::~CEditFunctions()
{
	if (m_pClipboardObject && m_bIsADuplicate)
		delete m_pClipboardObject;
}

//////////////////////////////////////////////////////////////////////
void CEditFunctions::Register(HWND hWnd)
{
	m_hWnd = hWnd;
	m_AGCBFormat = ::RegisterClipboardFormat((LPSTR)"AGObject");
}

//////////////////////////////////////////////////////////////////////
CAGSym* CEditFunctions::PasteObject()
{
	if (::IsClipboardFormatAvailable(m_AGCBFormat))
	{
		if (m_pClipboardObject)
			return m_pClipboardObject->Duplicate();
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////
bool CEditFunctions::CutObject(CAGSym* pSym)
{
	return HandleCutCopy(pSym, true/*bCut*/);
}

//////////////////////////////////////////////////////////////////////
bool CEditFunctions::CopyObject(CAGSym* pSym)
{
	return HandleCutCopy(pSym, false/*bCut*/);
}

//////////////////////////////////////////////////////////////////////
bool CEditFunctions::HandleCutCopy(CAGSym* pSym, bool bCut)
{
	if (!pSym)
		return false;

	bool bCopyFullObject = true;
	if (pSym->IsText() || pSym->IsAddAPhoto())
	{ // Don't copy the full object if all we want is the selected text
		CAGSymText* pText = (CAGSymText*)pSym;
		if (pText->GetSelection() && !pText->GetSelection()->IsSliverCursor())
			bCopyFullObject = false;
	}

	if (bCopyFullObject)
	{
		if (m_pClipboardObject && m_bIsADuplicate)
			delete m_pClipboardObject;

		if (!pSym->IsImage() || bCut)
		{
			m_pClipboardObject = pSym->Duplicate();
			m_bIsADuplicate = true;
		}
		else
		{
			m_pClipboardObject = pSym;
			m_bIsADuplicate = false;
		}
	}

	if (!::OpenClipboard(m_hWnd))
		return false;

	::EmptyClipboard();

	if (bCopyFullObject)
		::SetClipboardData(m_AGCBFormat, NULL);

	if (pSym->IsImage())
	{
		HANDLE hData = ((CAGSymImage*)pSym)->CopyDIB();
		if (hData)
			::SetClipboardData(CF_DIB, hData);
	}
	else
	if (pSym->IsText() || pSym->IsAddAPhoto())
	{
		HANDLE hData = ((CAGSymText*)pSym)->CopyText(bCut);
		if (hData)
			::SetClipboardData(CF_TEXT, hData);
	}

	::CloseClipboard();

	return bCopyFullObject;
}

//////////////////////////////////////////////////////////////////////
void CEditFunctions::DeleteObject(CAGSym* pSym)
{
	if (!pSym)
		return;

	if (pSym != m_pClipboardObject)
		return;

	if (m_pClipboardObject && m_bIsADuplicate)
		delete m_pClipboardObject;

	m_pClipboardObject = pSym->Duplicate();
	m_bIsADuplicate = true;
}

//////////////////////////////////////////////////////////////////////
void CEditFunctions::NotifyDocumentClose()
{
	if (m_pClipboardObject && !m_bIsADuplicate)
	{
		m_pClipboardObject = m_pClipboardObject->Duplicate();
		m_bIsADuplicate = true;
	}
}
