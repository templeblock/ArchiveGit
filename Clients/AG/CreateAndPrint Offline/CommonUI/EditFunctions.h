#pragma once

#include "AGDoc.h"
#include "AGMatrix.h"
#include "AGSym.h"

class CEditFunctions 
{
public:
	CEditFunctions();
	virtual ~CEditFunctions();

	void Register(HWND hWnd);
	CAGSym* PasteObject();
	bool CutObject(CAGSym* pSym);
	bool CopyObject(CAGSym* pSym);
	void DeleteObject(CAGSym* pSym);
	void NotifyDocumentClose();
protected:
	bool HandleCutCopy(CAGSym* pSym, bool bCut);

protected:
	HWND m_hWnd;
	UINT m_AGCBFormat;
	CAGSym* m_pClipboardObject;
	bool m_bIsADuplicate;
};
