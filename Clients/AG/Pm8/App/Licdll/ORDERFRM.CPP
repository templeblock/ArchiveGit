/*
// $Workfile: ORDERFRM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/ORDERFRM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:56   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 15:43:52   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:41:46   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/ORDERFRM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.1   14 Oct 1996 13:34:34   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.3   27 Mar 1996 17:03:40   JAY
// Sets orientation during printing. Sets parent correctly.
// 
//    Rev 1.2   01 Mar 1996 13:44:40   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.1   02 Feb 1996 13:04:34   JAY
//  
// 
//    Rev 1.0   02 Feb 1996 08:58:24   JAY
// Initial revision.
// 
//    Rev 1.6   29 Jan 1996 14:30:08   FRED
// Saves DC
// 
//    Rev 1.5   11 Jan 1995 16:26:24   FRED
//  
// 
//    Rev 1.4   06 Jan 1995 18:08:00   FRED
//  
// 
//    Rev 1.3   16 Dec 1994 17:09:56   FRED
//  
// 
//    Rev 1.2   06 Dec 1994 13:39:06   FRED
//  
// 
//    Rev 1.1   10 Nov 1994 13:03:00   FRED
// 
//    Rev 1.0   10 Nov 1994 11:49:58   FRED
// Initial revision.
*/

// orderfrm.cpp : implementation file
//

#include "stdafx.h"
#include "license.h"
#include "mlsprint.h"
#include "orderfrm.h"


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

#pragma warning(default : 4200)


//////////////////////////////////////////////////////////////////////////////////////
// COrderForm

COrderForm::COrderForm()
{
	m_pszFileName = NULL;
	
	m_hInstance = NULL;
	m_pszResource = NULL;
	m_pszType = NULL;
	
	m_nBoundsTop = 0;
	m_nBoundsMiddle = 0;
	m_nBoundsBottom = 0;
	m_nBoundsLeft = 0;
	m_nBoundsRight = 0;
}

COrderForm::~COrderForm()
{
}

void COrderForm::OnPrepareDC()
{
	ASSERT_VALID(m_pDC);

	// Default to one page printing if doc length not known
	if (m_pInfo != NULL)
	{
		m_pInfo->m_bContinuePrinting = TRUE;
	}
}

BOOL COrderForm::Print(CWnd* pParent)
{
	m_nItem = 0;
	m_nPage = 1;
	m_nNumberOfPages = NumberOfPages();
	
	CMlsPrintInfo PrintInfo(pParent);
	PrintInfo.SetMinPage(1);
	PrintInfo.SetMaxPage(m_nNumberOfPages);
	PrintInfo.SetOrientation(DMORIENT_PORTRAIT);
	
	return CMlsPrint::Print(&PrintInfo, pParent);
}

void COrderForm::Wmf(LPCSTR pszFileName)
{
	m_pszFileName = pszFileName;
	
	m_hInstance = NULL;
	m_pszResource = NULL;
	m_pszType = NULL;
}

void COrderForm::Wmf(HINSTANCE hInstance, LPCSTR pszResource, LPCSTR pszType)
{
	m_pszFileName = NULL;
	
	m_hInstance = hInstance;
	m_pszResource = pszResource;
	m_pszType = pszType;
}

void COrderForm::Bounds(int nTop, int nMiddle, int nBottom, int nLeft, int nRight, int nTextX, int nTextY)
{
	m_nBoundsTop = nTop;
	m_nBoundsMiddle = nMiddle;
	m_nBoundsBottom = nBottom;
	m_nBoundsLeft = nLeft;
	m_nBoundsRight = nRight;
	m_nBoundsTextX = nTextX;
	m_nBoundsTextY = nTextY;
	
	m_nItemX0 = m_nBoundsLeft;
	m_nItemX12 = m_nBoundsRight;
						
	m_nItemX4 = m_nItemX0+MulDiv(m_nItemX12-m_nItemX0, 4, 64);
	m_nItemX8 = m_nItemX0+MulDiv(m_nItemX12-m_nItemX0, 57, 64);
						
	m_nItemX1 = m_nItemX0+m_nBoundsTextX;
	m_nItemX3 = m_nItemX4-m_nBoundsTextX;
	m_nItemX5 = m_nItemX4+m_nBoundsTextX;
	m_nItemX7 = m_nItemX8-m_nBoundsTextX;
	m_nItemX9 = m_nItemX8+m_nBoundsTextX;
	m_nItemX11 = m_nItemX12-m_nBoundsTextX;
						
	m_nItemX2 = (m_nItemX0+m_nItemX4)/2;
	m_nItemX6 = (m_nItemX4+m_nItemX8)/2;
	m_nItemX10 = (m_nItemX8+m_nItemX12)/2;

	m_nSeparatorSpacing = MulDiv(m_nBoundsTextY, 37, 64);
	m_nTextSpacing = MulDiv(m_nBoundsTextY, 85, 64);
	m_nCollectionSpacing = MulDiv(m_nBoundsTextY, 10, 64);
	m_nHighlightThickness = MulDiv(m_nBoundsTextY, 8, 64);
}
	
void COrderForm::AddStringMapping(LPCSTR pszFrom, LPCSTR pszTo)
{
	m_Map.SetAt(pszFrom, pszTo);
}

void COrderForm::AddIndexedStringMapping(LPCSTR pszFrom, int nIndex, LPCSTR pszTo)
{
	char szBuffer[128];
	wsprintf(szBuffer, "%s%d", pszFrom, nIndex);
	m_Map.SetAt(szBuffer, pszTo);
}

void COrderForm::RemoveStringMapping(LPCSTR pszFrom)
{
	m_Map.RemoveKey(pszFrom);
}

BOOL COrderForm::LookupStringMapping(LPCSTR pszFrom, CString& csTo)
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

void COrderForm::OnPrint()
{
	ASSERT(m_pInfo != NULL);
	ASSERT(m_pDC != NULL);
	
	if (m_nPage > (int)(m_pInfo->m_nCurPage))
	{
		m_nItem = 0;
		m_nPage = 1;
	}
	
	while ((m_nPage <= (int)(m_pInfo->m_nCurPage)) && (m_nPage <= m_nNumberOfPages))
	{
		m_fShowPage = m_nPage == (int)(m_pInfo->m_nCurPage);
		
		COrderFormMetaFile MetaFile;
	
		BOOL fSuccess = FALSE;
		
		if (m_pszFileName != NULL)
		{
			fSuccess = MetaFile.Init(m_pszFileName);
		}
		else if (m_pszResource != NULL)
		{
			fSuccess = MetaFile.Init(m_hInstance, m_pszResource, m_pszType);
		}
		
		if (fSuccess)
		{
			const ALDUS_WMF_HEADER* pHeader = MetaFile.AldusHeader();
					
			int nImageWidth = pHeader->bbox.right-pHeader->bbox.left;
			int nImageHeight = pHeader->bbox.bottom-pHeader->bbox.top;
			
			int nXResolution = m_pDC->GetDeviceCaps(LOGPIXELSX);
			int nYResolution = m_pDC->GetDeviceCaps(LOGPIXELSY);
					
			nImageWidth = MulDiv(nImageWidth, nXResolution, nYResolution);
					
			int nContentWidth = m_pInfo->m_rectDraw.Width()-(nXResolution/3);
			int nContentHeight = m_pInfo->m_rectDraw.Height()-(nYResolution/3);
					
			int nNumerator = nContentWidth;
			int nDenominator = nImageWidth;
					
			if (((long)nContentWidth*(long)nImageHeight) > ((long)nContentHeight*(long)nImageWidth))
			{
				nNumerator = nContentHeight;
				nDenominator = nImageHeight;
			}
					
			nImageWidth = MulDiv(nImageWidth, nNumerator, nDenominator);
			nImageHeight = MulDiv(nImageHeight, nNumerator, nDenominator);
			
			int nXOrigin = (m_pInfo->m_rectDraw.Width()-nImageWidth)/2;
			int nYOrigin = (m_pInfo->m_rectDraw.Height()-nImageHeight)/2;
					
			m_pDC->SaveDC();
			
			m_pDC->SetMapMode(MM_ANISOTROPIC);
			m_pDC->SetViewportOrg(nXOrigin, nYOrigin);
			m_pDC->SetViewportExt(nImageWidth, nImageHeight);
			
			RemoveStringMapping("~RPAGE");
			
			CString csPage;
			wsprintf(csPage.GetBuffer(100), "Page %d of %d", m_nPage, m_nNumberOfPages);
			csPage.ReleaseBuffer(-1);
			AddStringMapping("~RPAGE", csPage);
			
			MetaFile.Play(m_pDC, FALSE, (LPARAM)this);
			
			m_pDC->RestoreDC(-1);
		}
			
		m_nPage++;
	}
}

void COrderForm::GetTitle(CString& csTitle)
{
	csTitle = "Order Form";
}

BOOL COrderForm::EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pRecord, METARECORD** ppNewRecord, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fEnumerate = TRUE;
	
	if (fEnumerate)
	{
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
					
					// Check for special "~SITEMS".
					
					if (csFrom == "~SITEMS")
					{
						m_nCurrentY = m_nBoundsTop;
						m_nSummaryY = m_nCurrentY;
						m_nBottomY = (m_nPage > 1) ? m_nBoundsBottom : m_nBoundsMiddle;
						
						BOOL fFirstItem = TRUE;
						BOOL fNeedSeparator = FALSE;
						
						while (m_nItem < m_Items.ItemCount())
						{
							COrderFormItem* pItem = m_Items.Item(m_nItem++);
							
							if (pItem != NULL)
							{
								int nSpaceNeeded = m_nSeparatorSpacing+m_nTextSpacing;
								
								if (m_nItem < m_Items.ItemCount())
								{
									nSpaceNeeded += m_nSeparatorSpacing+m_nTextSpacing;
								}
								
								if (pItem->Number() == 0)
								{
									nSpaceNeeded += m_nSeparatorSpacing+m_nTextSpacing+m_nCollectionSpacing;
								}
								
								if (m_nCurrentY > m_nBottomY-nSpaceNeeded)
								{
									m_nItem--;
									break;
								}
								
							   if (pItem->Number() < 0)
							   {
							   	if (fNeedSeparator)
							   	{
							   		PlaySeparatorLine(pObject, nObject, pData);
							   		fNeedSeparator = FALSE;
										m_nSummaryY = m_nCurrentY;
							   	}
							   	
							   	PlaySummaryLine(pItem->Number(), pItem->Name(), pItem->Price(), pItem->Highlight(), pObject, nObject, pData);
							   }
							   else
							   {
									if (fFirstItem)
									{
										PlayItemTitle(pObject, nObject, pData);
										fFirstItem = FALSE;
										fNeedSeparator = TRUE;
									}
								
							   	PlayItemLine(pItem->Number(), pItem->Name(), pItem->Price(), pObject, nObject, pData);
									m_nSummaryY = m_nCurrentY;
							   }
							}
						}
						
				   	if (fNeedSeparator)
				   	{
				   		PlaySeparatorLine(pObject, nObject, pData);
				   		fNeedSeparator = FALSE;
							m_nSummaryY = m_nCurrentY;
				   	}
				   	
						PlayLineRecord(m_nItemX0, m_nBoundsTop, m_nItemX0, m_nCurrentY, pObject, nObject, pData);
						PlayLineRecord(m_nItemX4, m_nBoundsTop, m_nItemX4, m_nSummaryY, pObject, nObject, pData);
						PlayLineRecord(m_nItemX8, m_nBoundsTop, m_nItemX8, m_nCurrentY, pObject, nObject, pData);
						PlayLineRecord(m_nItemX12, m_nBoundsTop, m_nItemX12, m_nCurrentY, pObject, nObject, pData);
						
						if (m_nItem < m_Items.ItemCount())
						{
							m_nCurrentY += m_nTextSpacing;
							PlayTextOutRecord("Continued on next page...", (m_nItemX0+m_nItemX12)/2, m_nCurrentY, COrderForm::ALIGN_Center, pObject, nObject, pData);
						}
					}
				}
			}
		}
	}
		
	if (fEnumerate)
	{
		fEnumerate = EnumerateRecord(pRecord);
	}
	
	if (fEnumerate)
	{
		fEnumerate = m_fShowPage;
	}
	
	return fEnumerate;
}

BOOL COrderForm::PlaySeparatorLine(HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	m_nCurrentY += m_nSeparatorSpacing;
	
	BOOL fResult =
		PlayLineRecord(m_nItemX0, m_nCurrentY, m_nItemX12, m_nCurrentY, pObject, nObject, pData);
		
	return fResult;
}

BOOL COrderForm::PlayItemTitle(HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	m_nCurrentY += m_nTextSpacing;
	
	BOOL fResult =
		PlayTextOutRecord("#", m_nItemX2, m_nCurrentY, ALIGN_Center, pObject, nObject, pData)
	&&	PlayTextOutRecord("Item Description", m_nItemX6, m_nCurrentY, ALIGN_Center, pObject, nObject, pData)
	&&	PlayTextOutRecord("Price", m_nItemX10, m_nCurrentY, ALIGN_Center, pObject, nObject, pData)
	&& PlaySeparatorLine(pObject, nObject, pData);
	
	return fResult;
}

BOOL COrderForm::PlayItemLine(int nNumber, LPCSTR pszDescription, LPCSTR pszPrice, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	m_nCurrentY += m_nTextSpacing;
	
	char szNumber[10];
	
	if (nNumber == 0)
	{
		szNumber[0] = '\0';
		m_nCurrentY += m_nCollectionSpacing;
	}
	else
	{
		_itoa(nNumber, szNumber, 10);
	}
	
	BOOL fResult =
		PlayTextOutRecord(szNumber, m_nItemX2, m_nCurrentY, ALIGN_Center, pObject, nObject, pData)
	&&	PlayTextOutRecord(pszDescription, m_nItemX5, m_nCurrentY, ALIGN_Left, pObject, nObject, pData)
	&&	PlayTextOutRecord(pszPrice, m_nItemX11, m_nCurrentY, ALIGN_Right, pObject, nObject, pData);
	
	return fResult;
}

BOOL COrderForm::PlaySummaryLine(int nNumber, LPCSTR pszDescription, LPCSTR pszPrice, BOOL fHighlight, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	int nTopY = m_nCurrentY;
	
	m_nCurrentY += m_nTextSpacing;
	int nTextY = m_nCurrentY;
	
	BOOL fResult =
		PlaySeparatorLine(pObject, nObject, pData)
	&& PlayHighlightRectangle(fHighlight, m_nItemX8, nTopY, m_nItemX12, m_nCurrentY, pObject, nObject, pData)
	&& PlayTextOutRecord(pszDescription, m_nItemX7, nTextY, ALIGN_Right, pObject, nObject, pData)
	&&	PlayTextOutRecord(pszPrice, (nNumber == -2) ? m_nItemX10 : m_nItemX11, nTextY, (nNumber == -2) ? ALIGN_Center : ALIGN_Right, pObject, nObject, pData);
	
	return fResult;
}

BOOL COrderForm::PlayHighlightRectangle(BOOL fHighlight, int nX0, int nY0, int nX1, int nY1, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	CPen cpNew;
	CBrush cbNew;
	
	if (fHighlight)
	{
		if (cpNew.CreatePen(PS_SOLID, m_nHighlightThickness, RGB(0,0,0)))
		{
			if (cbNew.CreateSolidBrush(RGB(255,255,255)))
			{
				fResult = PlayRectangleRecord(nX0, nY0, nX1, nY1, &cpNew, &cbNew, pObject, nObject, pData);
			}
		}
	}
	
	return fResult;
}

BOOL COrderForm::PlayTextOutRecord(LPCSTR pszString, int nX, int nY, StringAlignment nAlignment, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	MTEXTOUT_RECORD* pRecord = NULL;

	int nRecordSize =
			sizeof(pRecord->rdSize)
		+  sizeof(pRecord->rdFunction)
		+  sizeof(pRecord->rdLength)
		+	((lstrlen(pszString)+1)/2)*2
		+  sizeof(pRecord->rdX)
		+  sizeof(pRecord->rdY);
		
	pRecord = (MTEXTOUT_RECORD*)(new char[nRecordSize]);
	ASSERT(pRecord != NULL);
									
	if (pRecord != NULL)
	{
		pRecord->rdSize = nRecordSize/2;
		pRecord->rdFunction = META_TEXTOUT;
		pRecord->rdLength = lstrlen(pszString);
		
		memcpy(&(pRecord->rdString[0]), pszString, lstrlen(pszString)+1);
		
		short int* pCoord = (short int*)(((char*)(&(pRecord->rdString[0]))+((pRecord->rdLength+1)/2)*2));
		
		pCoord[0] = nY;
		pCoord[1] = nX;
									
		if (nAlignment != ALIGN_Left)
		{
			CSize czSize = pData->pDC->GetTextExtent(pszString, lstrlen(pszString));
									
			if (nAlignment == ALIGN_Center)
			{
				pCoord[1] -= czSize.cx/2;
			}
			else if (nAlignment == ALIGN_Right)
			{
				pCoord[1] -= (short int)czSize.cx;
			}
		}
		
		if (m_fShowPage)
		{
			fResult = pData->pObject->CWindowsMetaFile::EnumMetaFileProc(pObject, (METARECORD*)pRecord, nObject, pData);
		}
	}
	
	delete pRecord;
	pRecord = NULL;
	
	return fResult;
}

BOOL COrderForm::PlayLineRecord(int nX0, int nY0, int nX1, int nY1, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	MPOLYLINE_RECORD* pRecord = NULL;
	CPen* pcpOld = NULL;
	CPen cpNew;
	
	if (cpNew.CreatePen(PS_SOLID, 0, RGB(0,0,0)))
	{
		int nRecordSize =
				sizeof(pRecord->rdSize)
			+  sizeof(pRecord->rdFunction)
			+  sizeof(pRecord->rdCount)
			+	sizeof(POINTS)*2;
			
		pRecord = (MPOLYLINE_RECORD*)(new char[nRecordSize]);
		ASSERT(pRecord != NULL);
										
		if (pRecord != NULL)
		{
			pcpOld = pData->pDC->SelectObject(&cpNew);
			
			if (pcpOld != NULL)
			{
				pRecord->rdSize = nRecordSize/2;
				pRecord->rdFunction = META_POLYLINE;
				pRecord->rdCount = 2;
				pRecord->rdPoints[0].x = nX0;
				pRecord->rdPoints[0].y = nY0;
				pRecord->rdPoints[1].x = nX1;
				pRecord->rdPoints[1].y = nY1;
				
				if (m_fShowPage)
				{
					fResult = pData->pObject->CWindowsMetaFile::EnumMetaFileProc(pObject, (METARECORD*)pRecord, nObject, pData);
				}
				
				pData->pDC->SelectObject(pcpOld);
			}
		}
	}
	
	delete pRecord;
	pRecord = NULL;
	
	return fResult;
}

BOOL COrderForm::PlayRectangleRecord(int nX0, int nY0, int nX1, int nY1, CPen* pcpPen, CBrush* pcbBrush, HANDLETABLE* pObject, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	MPOLYGON_RECORD* pRecord = NULL;
	CPen* pcpOldPen = NULL;
	CBrush* pcbOldBrush = NULL;
	
	int nRecordSize =
			sizeof(pRecord->rdSize)
		+  sizeof(pRecord->rdFunction)
		+  sizeof(pRecord->rdCount)
		+	sizeof(POINTS)*4;
			
	pRecord = (MPOLYGON_RECORD*)(new char[nRecordSize]);
	ASSERT(pRecord != NULL);
										
	if (pRecord != NULL)
	{
		pcpOldPen = pData->pDC->SelectObject(pcpPen);
			
		if (pcpOldPen != NULL)
		{
			pcbOldBrush = pData->pDC->SelectObject(pcbBrush);
				
			if (pcbOldBrush != NULL)
			{
				pRecord->rdSize = nRecordSize/2;
				pRecord->rdFunction = META_POLYGON;
				pRecord->rdCount = 4;
				pRecord->rdPoints[0].x = nX0;
				pRecord->rdPoints[0].y = nY0;
				pRecord->rdPoints[1].x = nX1;
				pRecord->rdPoints[1].y = nY0;
				pRecord->rdPoints[2].x = nX1;
				pRecord->rdPoints[2].y = nY1;
				pRecord->rdPoints[3].x = nX0;
				pRecord->rdPoints[3].y = nY1;
					
				if (m_fShowPage)
				{
					fResult = pData->pObject->CWindowsMetaFile::EnumMetaFileProc(pObject, (METARECORD*)pRecord, nObject, pData);
				}
					
				pData->pDC->SelectObject(pcbOldBrush);
			}
			
			pData->pDC->SelectObject(pcpOldPen);
		}
	}
	
	delete pRecord;
	pRecord = NULL;
	
	return fResult;
}

BOOL COrderForm::NumberOfPages(void)
{
	int nNumberOfPages = 0;
	BOOL fStartNewPage = TRUE;
	int nCurrentY;
	int nBottomY;
	BOOL fFirstItem;
	BOOL fNeedSeparator;
	
	int nItem = 0;
	
	while (nItem < m_Items.ItemCount())
	{
		COrderFormItem* pItem = m_Items.Item(nItem++);
		
		if (pItem != NULL)
		{
			if (fStartNewPage)
			{
				nNumberOfPages++;
				nCurrentY = m_nBoundsTop;
				nBottomY = (nNumberOfPages > 1) ? m_nBoundsBottom : m_nBoundsMiddle;
				fFirstItem = TRUE;
				fNeedSeparator = FALSE;
				fStartNewPage = FALSE;
			}
								
			int nSpaceNeeded = m_nSeparatorSpacing+m_nTextSpacing;
										
			if (nItem < m_Items.ItemCount())
			{
				nSpaceNeeded += m_nSeparatorSpacing+m_nTextSpacing;
			}
										
			if (pItem->Number() == 0)
			{
				nSpaceNeeded += m_nSeparatorSpacing+m_nTextSpacing+m_nCollectionSpacing;
			}
										
			if (nCurrentY > nBottomY-nSpaceNeeded)
			{
				nItem--;
				fStartNewPage = TRUE;
			}
			
			else
			{
			   if (pItem->Number() < 0)
			   {
			   	if (fNeedSeparator)
			   	{
						nCurrentY += m_nSeparatorSpacing;
			   		fNeedSeparator = FALSE;
			   	}
										   	
					nCurrentY += m_nTextSpacing;
					nCurrentY += m_nSeparatorSpacing;
			   }
			   else
			   {
					if (fFirstItem)
					{
						nCurrentY += m_nTextSpacing;
						nCurrentY += m_nSeparatorSpacing;
						fFirstItem = FALSE;
						fNeedSeparator = TRUE;
					}
											
					nCurrentY += m_nTextSpacing;
			   }
			}
		}
	}
	
	return nNumberOfPages;
}

BOOL COrderForm::EnumerateRecord(METARECORD* pRecord)
{
	BOOL fEnumerate = TRUE;
	
	if (m_nPage > 1)
	{
		int nY = 0;
		
		switch (pRecord->rdFunction)
		{
			case META_EXTTEXTOUT:
			{
				nY = ((MEXTTEXTOUT_RECORD*)pRecord)->rdY;
				break;
			}
			
			case META_POLYGON:
			{
				if (((MPOLYGON_RECORD*)pRecord)->rdCount > 0)
				{
					nY = ((MPOLYGON_RECORD*)pRecord)->rdPoints[0].y;
				}
				
				break;
			}
			
			case META_POLYPOLYGON:
			{
				if (((MPOLYPOLYGON_RECORD*)pRecord)->rdCount > 0)
				{
					if (((MPOLYPOLYGON_RECORD*)pRecord)->rdCounts[0] > 0)
					{
						nY = ((POINTS*)(&(((MPOLYPOLYGON_RECORD*)pRecord)->rdCounts[((MPOLYPOLYGON_RECORD*)pRecord)->rdCount])))->y;
					}
				}
				
				break;
			}
			
			case META_POLYLINE:
			{
				if (((MPOLYLINE_RECORD*)pRecord)->rdCount > 0)
				{
					nY = ((MPOLYLINE_RECORD*)pRecord)->rdPoints[0].y;
				}
				
				break;
			}
			
			case META_TEXTOUT:
			{
				nY = ((short int*)(((char*)(&(((MTEXTOUT_RECORD*)pRecord)->rdString[0]))+((((MTEXTOUT_RECORD*)pRecord)->rdLength+1)/2)*2)))[0];
				break;
			}
		}
		
		fEnumerate = (nY < m_nBoundsMiddle) || (nY > m_nBoundsBottom);
	}
	
	return fEnumerate;
}
		
BOOL COrderForm::IsPenColor(CDC* pDC, COLORREF clColor)
{
	BOOL fResult = FALSE;
	
	if (pDC != NULL)
	{
		HDC hDC;
		
		hDC = pDC->GetSafeHdc();
		
		if (hDC != NULL)
		{
			HPEN hPen;
			
			hPen = (HPEN)::SelectObject(hDC, GetStockObject(BLACK_PEN));
			
			if (hPen != NULL)
			{
				LOGPEN Pen;
				
				if (::GetObject(hPen, sizeof(Pen), (void *)(&Pen)) != 0)
				{
					fResult = 
						(GetRValue(Pen.lopnColor) == GetRValue(clColor))
					&& (GetGValue(Pen.lopnColor) == GetGValue(clColor))
					&& (GetBValue(Pen.lopnColor) == GetBValue(clColor));
				}
				
				::SelectObject(hDC, hPen);
				hPen = NULL;
			}
		}
	}
	
	return fResult;
}

void COrderForm::AddItem(int nNumber, const CString& csName, const CString& csPrice, BOOL fHighlight)
{
	COrderFormItem* pItem = new COrderFormItem(nNumber, csName, csPrice, fHighlight);
			
	if (pItem != NULL)
	{
		m_Items.Add(pItem);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
// COrderFormMetaFile

BOOL COrderFormMetaFile::EnumMetaFileProc(HANDLETABLE* pObject, METARECORD* pMetaRecord, int nObject, CWindowsMetaFile::EnumData* pData)
{
	BOOL fResult = TRUE;
	
	COrderForm* pOrderForm = (COrderForm*)(pData->lParam);
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
			delete pNewRecord;
		}
	}
	
	return fResult;
}


COrderFormItems::COrderFormItems()
{
}

COrderFormItems::~COrderFormItems()
{
	RemoveAll();
}

void COrderFormItems::RemoveAll(void)
{
	int nItems = ItemCount();
	
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		COrderFormItem* pItem = Item(nItem);
		
		if (pItem != NULL)
		{
			delete pItem;
		}
	}
	
	CPtrArray::RemoveAll();
}

COrderFormItem::COrderFormItem(int nNumber, const CString& csName, const CString& csPrice, BOOL fHighlight)
{
	m_nNumber = nNumber;
	m_csName = csName;
	m_csPrice = csPrice;
	m_fHighlight = fHighlight;
}

COrderFormItem::~COrderFormItem()
{
}

