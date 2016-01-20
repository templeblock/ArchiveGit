/*
// $Workfile: REGFORM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGFORM.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   01 Oct 1996 10:38:50   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "resource.h"
#include "mlsprint.h"
#include "regform.h"

#pragma warning(disable : 4200)

struct META_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
};

struct MEXTTEXTOUT_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	short int rdY;
	short int rdX;
	WORD rdCount;
	WORD rdOptions;
	RECTS rdRectangle;
	char rdString[];
	// followed by optional array of intercharacter distances
};

struct MPOLYGON_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	WORD rdCount;
	POINTS rdPoints[];
};

struct MPOLYPOLYGON_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	WORD rdCount;    // number of polygons
	WORD rdCounts[]; // polygon point counts
	// followed by polygon points
};

struct MPOLYLINE_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	WORD rdCount;
	POINTS rdPoints[];
};

struct MTEXTOUT_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	WORD rdLength;
	char rdString[2];
	// String is null filled to even length
	// and followed by Y and X positions.
	short int rdY;
	short int rdX;
};

#pragma pack(1)
typedef struct tagLOGFONT16
{
    short   lfHeight;
    short   lfWidth;
    short   lfEscapement;
    short   lfOrientation;
    short   lfWeight;
    BYTE    lfItalic;
    BYTE    lfUnderline;
    BYTE    lfStrikeOut;
    BYTE    lfCharSet;
    BYTE    lfOutPrecision;
    BYTE    lfClipPrecision;
    BYTE    lfQuality;
    BYTE    lfPitchAndFamily;
    char    lfFaceName[32];
} LOGFONT16;
#pragma pack()

struct MCREATEFONTINDIRECT_RECORD
{
	DWORD rdSize;
	WORD rdFunction;
	LOGFONT16 rdFont;
};

#pragma warning(default : 4200)


//////////////////////////////////////////////////////////////////////////////////////
// CRegistrationForm

CRegistrationForm::CRegistrationForm()
{
	m_pszFileNameTop = NULL;
	m_pszFileNameMiddle = NULL;
	m_pszFileNameBottom = NULL;
	
	m_hInstance = NULL;
	m_pszResourceTop = NULL;
	m_pszResourceMiddle = NULL;
	m_pszResourceBottom = NULL;
	m_pszType = NULL;
}

CRegistrationForm::~CRegistrationForm()
{
}

void CRegistrationForm::OnPrepareDC()
{
	ASSERT_VALID(m_pDC);

	// Default to one page printing if doc length not known
	if (m_pInfo != NULL)
	{
		m_pInfo->m_bContinuePrinting = TRUE;
	}
}

BOOL CRegistrationForm::Print(CWnd* pParent)
{
	CMlsPrintInfo PrintInfo(pParent);
	PrintInfo.SetMinPage(1);
	PrintInfo.SetMaxPage(NumberOfPages());
	PrintInfo.SetOrientation(DMORIENT_PORTRAIT);
	
	return CMlsPrint::Print(&PrintInfo, pParent);
}

// Set WMFs by file name.
void CRegistrationForm::Wmf(LPCSTR pszFileNameTop, LPCSTR pszFileNameMiddle, LPCSTR pszFileNameBottom)
{
	m_pszFileNameTop = pszFileNameTop;
	m_pszFileNameMiddle = pszFileNameMiddle;
	m_pszFileNameBottom = pszFileNameBottom;
	
	m_hInstance = NULL;
	m_pszResourceTop = NULL;
	m_pszResourceMiddle = NULL;
	m_pszResourceBottom = NULL;
	m_pszType = NULL;
}

// Set WMFs by resource name.
void CRegistrationForm::Wmf(HINSTANCE hInstance, LPCSTR pszResourceTop, LPCSTR pszResourceMiddle, LPCSTR pszResourceBottom, LPCSTR pszType)
{
	m_pszFileNameTop = NULL;
	m_pszFileNameMiddle = NULL;
	m_pszFileNameBottom = NULL;
	
	m_hInstance = hInstance;
	m_pszResourceTop = pszResourceTop;
	m_pszResourceMiddle = pszResourceMiddle;
	m_pszResourceBottom = pszResourceBottom;
	m_pszType = pszType;
}

// Functions to manage string mappings.
void CRegistrationForm::AddStringMapping(LPCSTR pszFrom, LPCSTR pszTo)
{
	m_Map.SetAt(pszFrom, pszTo);
}

void CRegistrationForm::AddIndexedStringMapping(LPCSTR pszFrom, int nIndex, LPCSTR pszTo)
{
	char szBuffer[128];
	wsprintf(szBuffer, "%s%d", pszFrom, nIndex);
	m_Map.SetAt(szBuffer, pszTo);
}

void CRegistrationForm::RemoveStringMapping(LPCSTR pszFrom)
{
	m_Map.RemoveKey(pszFrom);
}

BOOL CRegistrationForm::LookupStringMapping(LPCSTR pszFrom, CString& csTo)
{
	BOOL fMapped = m_Map.Lookup(pszFrom, csTo);
	
	if (!fMapped)
	{
		if (pszFrom[0] == '~')
		{
			csTo = "";
			fMapped = TRUE;
		}
	}
	
	return fMapped;
}

// The main printing function.
void CRegistrationForm::OnPrint()
{
	ASSERT(m_pInfo != NULL);
	ASSERT(m_pDC != NULL);
	
	// Open up the meta files. There are three of them since the registration form
	// is intended to be folded into a 3-panel thing which gets mailed in. The fold
	// lines are always positions at 1/3 and 2/3 of the way down the page (adjusted
	// a little to avoid crowding the folded paper.) These lines define the areas
	// available for the three panels. Each meta file is played back into the
	// appropriate area on the page. The WMFs are not necessaryily scaled by the
	// same amounts, especially if the margins are not symmetric. Each one is made
	// as large as possible within its panel while keeping the aspect ration constant.

	CRegistrationFormMetaFile MetaFileTop;
	CRegistrationFormMetaFile MetaFileMiddle;
	CRegistrationFormMetaFile MetaFileBottom;

	BOOL fSuccess = FALSE;
	
	if (m_pszFileNameTop != NULL)
	{
		fSuccess =
				MetaFileTop.Init(m_pszFileNameTop)
			&& MetaFileMiddle.Init(m_pszFileNameMiddle)
			&& MetaFileBottom.Init(m_pszFileNameBottom);
	}
	else if (m_pszResourceTop != NULL)
	{
		fSuccess =
				MetaFileTop.Init(m_hInstance, m_pszResourceTop, m_pszType)
			&& MetaFileMiddle.Init(m_hInstance, m_pszResourceMiddle, m_pszType)
			&& MetaFileBottom.Init(m_hInstance, m_pszResourceBottom, m_pszType);
	}
	
	if (fSuccess)
	{
		HDC hDC = m_pDC->GetSafeHdc();

		if (hDC != NULL)
		{
			// Get size information for the printer and paper.
			int nPaperWidth = 0;
			int nPaperHeight = 0;
			int nLeftOffset = 0;
			int nTopOffset = 0;

			int nXResolution = GetDeviceCaps(hDC, LOGPIXELSX);
			int nYResolution = GetDeviceCaps(hDC, LOGPIXELSY);

			int nPrintableWidth = GetDeviceCaps(hDC, HORZRES);
			int nPrintableHeight = GetDeviceCaps(hDC, VERTRES);

			POINT p;

			if (::Escape(hDC, GETPHYSPAGESIZE, NULL, NULL, &p) > 0)
			{
				nPaperWidth = p.x;
				nPaperHeight = p.y;
			}
			else
			{
				nPaperWidth = nPrintableWidth;
				nPaperHeight = nPrintableHeight;
			}

			if (::Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &p) > 0)
			{
				nLeftOffset = p.x;
				nTopOffset = p.y;
			}
			else
			{
				nLeftOffset = (nPaperWidth - nPrintableWidth)/2;
				nTopOffset = (nPaperHeight - nPrintableHeight)/2;
			}

			// Figure out where to put the divider lines.
			int nTopDivider = (nPaperHeight/3)-nTopOffset-(nYResolution/16);
			int nBottomDivider = ((nPaperHeight*2)/3)-nTopOffset+(nYResolution/16);
			int nSeparator = nYResolution/3;

			// Build the rectangles for the various panels.
			CRect crTop(
				0,
				0,
				nPrintableWidth,
				nTopDivider-nSeparator);

			CRect crMiddle(
				0,
				nTopDivider+nSeparator,
				nPrintableWidth,
				nBottomDivider-nSeparator);

			CRect crBottom(
				0,
				nBottomDivider+nSeparator,
				nPrintableWidth,
				nPrintableHeight);

			// Draw the panel contents.
			PrintWmf(MetaFileTop, crTop);
			PrintWmf(MetaFileMiddle, crMiddle);
			PrintWmf(MetaFileBottom, crBottom);

			// Draw the divider lines.
			CPen Pen;
			if (Pen.CreateStockObject(BLACK_PEN))
			{
				CPen* pOldPen = m_pDC->SelectObject(&Pen);
				if (pOldPen != NULL)
				{
					m_pDC->MoveTo(0, nTopDivider);
					m_pDC->LineTo(nPrintableWidth, nTopDivider);

					m_pDC->MoveTo(0, nBottomDivider);
					m_pDC->LineTo(nPrintableWidth, nBottomDivider);

					m_pDC->SelectObject(pOldPen);
					pOldPen = NULL;
				}
			}

			// Draw some helpful text near the divider lines.
			int nFontHeight = (6*nYResolution)/72;

			LOGFONT lfFont;
			lfFont.lfHeight = -nFontHeight;
			lfFont.lfWidth = 0;
			lfFont.lfEscapement = 0;
			lfFont.lfOrientation = 0;
			lfFont.lfWeight = 400;
			lfFont.lfItalic = FALSE;
			lfFont.lfUnderline = FALSE;
			lfFont.lfStrikeOut = FALSE;
			lfFont.lfCharSet = ANSI_CHARSET;
			lfFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
			lfFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
			lfFont.lfQuality = DEFAULT_QUALITY;
			lfFont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
			strcpy(lfFont.lfFaceName, "Arial");

			CFont Font;
			if (Font.CreateFontIndirect(&lfFont))
			{
				CFont* pOldFont = m_pDC->SelectObject(&Font);
				if (pOldFont != NULL)
				{
					COLORREF clOldTextColor = m_pDC->SetTextColor(RGB(0,0,0));
					int nOldBkMode = m_pDC->SetBkMode(TRANSPARENT);

					CRect crRect;
					CString csFormat;
					CString csDate;
					CString csText;

					// Date printed.
					TRY
					{
						crRect.SetRect(
							0,
							nTopDivider-2*nFontHeight,
							nPrintableWidth,
							nTopDivider-1*nFontHeight);
						csFormat.LoadString(IDS_Printed);
						LookupStringMapping("~LDATE", csDate);
						if (!csDate.IsEmpty())
						{
							csText.Format(csFormat, (LPCSTR)csDate);
						}
						m_pDC->DrawText((LPCSTR)csText, csText.GetLength(), crRect, DT_LEFT|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
					}
					END_TRY

					// Fold back and outside.
					TRY
					{
						crRect.SetRect(
							0,
							nTopDivider-2*nFontHeight,
							nPrintableWidth,
							nTopDivider-1*nFontHeight);
						csText.LoadString(IDS_FoldBackAndOutside);
						m_pDC->DrawText((LPCSTR)csText, csText.GetLength(), crRect, DT_CENTER|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
					}
					END_TRY

					// Do not staple.
					TRY
					{
						crRect.SetRect(
							0,
							nBottomDivider-2*nFontHeight,
							nPrintableWidth,
							nBottomDivider-1*nFontHeight);
						csText.LoadString(IDS_DoNotStaple);
						m_pDC->DrawText((LPCSTR)csText, csText.GetLength(), crRect, DT_CENTER|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
					}
					END_TRY

					// Fold back and inside.
					TRY
					{
						crRect.SetRect(
							0,
							nBottomDivider+1*nFontHeight,
							nPrintableWidth,
							nBottomDivider+2*nFontHeight);
						csText.LoadString(IDS_FoldBackAndInside);
						m_pDC->DrawText((LPCSTR)csText, csText.GetLength(), crRect, DT_CENTER|DT_NOCLIP|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
					}
					END_TRY

					m_pDC->SetBkMode(nOldBkMode);
					m_pDC->SetTextColor(clOldTextColor);

					m_pDC->SelectObject(pOldFont);
					pOldFont = NULL;
				}

				Font.DeleteObject();
			}
		}
	}
}

// Play a WMF (with string substitution) into a rectangle.
void CRegistrationForm::PrintWmf(CRegistrationFormMetaFile& MetaFile, const CRect& crRect)
{
	const ALDUS_WMF_HEADER* pHeader = MetaFile.AldusHeader();
			
	int nImageWidth = pHeader->bbox.right-pHeader->bbox.left;
	int nImageHeight = pHeader->bbox.bottom-pHeader->bbox.top;
	
	int nXResolution = m_pDC->GetDeviceCaps(LOGPIXELSX);
	int nYResolution = m_pDC->GetDeviceCaps(LOGPIXELSY);
			
	nImageWidth = MulDiv(nImageWidth, nXResolution, nYResolution);
			
	int nContentWidth = crRect.Width();
	int nContentHeight = crRect.Height();
			
	int nNumerator = nContentWidth;
	int nDenominator = nImageWidth;
			
	if (((long)nContentWidth*(long)nImageHeight) > ((long)nContentHeight*(long)nImageWidth))
	{
		nNumerator = nContentHeight;
		nDenominator = nImageHeight;
	}
			
	nImageWidth = MulDiv(nImageWidth, nNumerator, nDenominator);
	nImageHeight = MulDiv(nImageHeight, nNumerator, nDenominator);
	
	int nXOrigin = (nContentWidth-nImageWidth)/2;
	int nYOrigin = (nContentHeight-nImageHeight)/2;
			
	if (m_pDC->SaveDC() != 0)
	{
		m_pDC->SetMapMode(MM_ANISOTROPIC);
		m_pDC->SetViewportOrg(nXOrigin+crRect.left, nYOrigin+crRect.top);
		m_pDC->SetViewportExt(nImageWidth, nImageHeight);
		
		MetaFile.Play(m_pDC, FALSE, (LPARAM)this);

		m_pDC->RestoreDC(-1);
	}
}

void CRegistrationForm::GetTitle(CString& csTitle)
{
	TRY
	{
		csTitle.LoadString(IDS_RegistrationForm);
	}
	END_TRY
}

BOOL CRegistrationForm::EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pRecord, METARECORD** ppNewRecord, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fEnumerate = TRUE;
	
	if (fEnumerate)
	{
		// Check for CreateFontIndirect record.
			
		if (pRecord->rdFunction == META_CREATEFONTINDIRECT)
		{
			MCREATEFONTINDIRECT_RECORD* pCreateFontIndirectRecord = (MCREATEFONTINDIRECT_RECORD*)pRecord;

			// We have a CreateFontIndirect record, do any face name substitutions.

			CString csOldName = pCreateFontIndirectRecord->rdFont.lfFaceName;
			CString csNewName;

			// Translate font names used by Illustrator into Windows native names.
			if (csOldName.CompareNoCase("ArialMT") == 0)
			{
				csNewName = "Arial";
			}
			else if (csOldName.CompareNoCase("Courier") == 0)
			{
				csNewName = "Courier New";
			}

			if (!csNewName.IsEmpty())
			{
				int nRecordSize = (sizeof(MCREATEFONTINDIRECT_RECORD)+1) & ~1;

				MCREATEFONTINDIRECT_RECORD* pNewCreateFontIndirectRecord = (MCREATEFONTINDIRECT_RECORD*)(new char[nRecordSize]);
				ASSERT(pNewCreateFontIndirectRecord != NULL);

				if (pNewCreateFontIndirectRecord != NULL)
				{
					pNewCreateFontIndirectRecord->rdSize = nRecordSize/2;
					pNewCreateFontIndirectRecord->rdFunction = pCreateFontIndirectRecord->rdFunction;
					pNewCreateFontIndirectRecord->rdFont = pCreateFontIndirectRecord->rdFont;

					// Set the face name.

					lstrcpy(pNewCreateFontIndirectRecord->rdFont.lfFaceName, csNewName);

					// Pass the new record back.

					*ppNewRecord = (METARECORD*)pNewCreateFontIndirectRecord;
					pRecord = (METARECORD*)pNewCreateFontIndirectRecord;
				}
			}
		}

		// Check for TextOut record.
			
		if (pRecord->rdFunction == META_TEXTOUT)
		{
			MTEXTOUT_RECORD* pTextOutRecord = (MTEXTOUT_RECORD*)pRecord;
				
			// We have a TextOut record, extract the string that is being output.
				
			CString csFrom;
			char *pszFrom = csFrom.GetBuffer(pTextOutRecord->rdLength);
			ASSERT(pszFrom != NULL);
				
			if (pszFrom != NULL)
			{
				if ((pTextOutRecord->rdLength > 2) && (pTextOutRecord->rdString[0] == '~'))
				{
					enum StringAlignment
					{
						ALIGN_Left,
						ALIGN_Center,
						ALIGN_Right
					};
						
					StringAlignment nAlignment = ALIGN_Left;
						
					if (pTextOutRecord->rdString[1] == 'C')
					{
						nAlignment = ALIGN_Center;
					}
					else if (pTextOutRecord->rdString[1] == 'R')
					{
						nAlignment = ALIGN_Right;
					}
						
					memcpy(pszFrom, &(pTextOutRecord->rdString[0]), pTextOutRecord->rdLength);
					csFrom.ReleaseBuffer(pTextOutRecord->rdLength);
					
					// Check if the string should be translated.
							
					CString csTo;
						
					if (LookupStringMapping(csFrom, csTo))
					{
						// Build a new record with the new string.
						int nToSize = csTo.GetLength();
						int nToSizeNorm = (nToSize+1) & ~1;
								
						int nRecordSize =
								sizeof(pTextOutRecord->rdSize)
							+  sizeof(pTextOutRecord->rdFunction)
							+  sizeof(pTextOutRecord->rdLength)
							+	nToSizeNorm
							+  sizeof(pTextOutRecord->rdX)
							+  sizeof(pTextOutRecord->rdY);
									
						MTEXTOUT_RECORD* pNewTextOutRecord = (MTEXTOUT_RECORD*)(new char[nRecordSize]);
						ASSERT(pNewTextOutRecord != NULL);
									
						if (pNewTextOutRecord != NULL)
						{
							pNewTextOutRecord->rdSize = nRecordSize/2;
							pNewTextOutRecord->rdFunction = pTextOutRecord->rdFunction;
							pNewTextOutRecord->rdLength = nToSize;
							memcpy(&(pNewTextOutRecord->rdString[0]), csTo, nToSize);
									
							short int* pCoord = (short int*)(((char*)(&(pTextOutRecord->rdString[0]))+((pTextOutRecord->rdLength+1)/2)*2));
							short int* pNewCoord = (short int*)(((char*)(&(pNewTextOutRecord->rdString[0]))+nToSizeNorm));
									
							pNewCoord[0] = pCoord[0];
							pNewCoord[1] = pCoord[1];
									
							if (nAlignment != ALIGN_Left)
							{
								CSize czFromSize = pData->pDC->GetTextExtent(csFrom, csFrom.GetLength());
								CSize czToSize = pData->pDC->GetTextExtent(csTo, csTo.GetLength());
									
								if (nAlignment == ALIGN_Center)
								{
									pNewCoord[1] -= (czToSize.cx-czFromSize.cx)/2;
								}
								else if (nAlignment == ALIGN_Right)
								{
									pNewCoord[1] -= (short int)(czToSize.cx-czFromSize.cx);
								}
							}
								
							*ppNewRecord = (METARECORD*)pNewTextOutRecord;
							pRecord = (METARECORD*)pNewTextOutRecord;
						}
					}
				}
			}
		}
	}
		
	return TRUE;
}

int CRegistrationForm::NumberOfPages(void)
{
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
// CRegistrationFormMetaFile

BOOL CRegistrationFormMetaFile::EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	
	CRegistrationForm* pOrderForm = (CRegistrationForm*)(pData->lParam);
	ASSERT(pOrderForm != NULL);
	
	if (pOrderForm != NULL)
	{
		METARECORD* pNewRecord = NULL;
	
		if (pOrderForm->EnumMetaFileProc(pObject, (METARECORD FAR*)pMetaRecord, &pNewRecord, nObject, pData))
		{
			if (pNewRecord != NULL)
			{
				pMetaRecord = pNewRecord;
			}

			fResult = CWindowsMetaFile::EnumMetaFileProc(pObject, pMetaRecord, nObject, pData);
		}
	
		if (pNewRecord != NULL)
		{
			delete [] pNewRecord;
		}
	}
	
	return fResult;
}
