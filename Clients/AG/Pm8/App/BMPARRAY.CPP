/*
// $Workfile: BMPARRAY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/BMPARRAY.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:12   Fred
// Initial revision.
// 
//    Rev 1.2   01 Aug 1996 14:18:04   FRED
// Fixed NULL handle bug
// 
//    Rev 1.1   01 Aug 1996 09:23:12   Fred
// More 16-bit changes
// 
//    Rev 1.0   28 Jul 1996 13:52:00   Fred
// Initial revision.
*/ 

#include "stdafx.h"
#include "sortarry.h"
#include "bmparray.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern CPalette *pOurPal;		// THE PrintMaster Palette (from PMWVIEW.CPP)

/////////////////////////////////////////////////////////
// CBitmapResourceArray

CBitmapResourceArray::CBitmapResourceArray()
{
}

CBitmapResourceArray::~CBitmapResourceArray()
{
	DeleteAll();
}
	
void CBitmapResourceArray::DeleteAll(void)
{
	DeleteAt(0, GetSize());
}

void CBitmapResourceArray::DeleteAt(int nIndex, int nCount /*=1*/)
{
	for (int i = 0; i < nCount; i++)
	{
		if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
		{
			delete GetAt(nIndex+i);
			SetAt(nIndex+i, NULL);
		}
	}

	INHERITED::RemoveAt(nIndex, nCount);
}

BOOL CBitmapResourceArray::SetResourceNames(LPCSTR pszResourceNames)
{
	BOOL fResult = FALSE;

	CMlsStringArray Names;
	TRY
	{
		DeleteAll();

		Names.SetNames(pszResourceNames);
		if (Names.GetSize() > 0)
		{
			for (int nName = 0; nName < Names.GetSize(); nName++)
			{
				Item* pItem = NULL;
				TRY
				{
					pItem = new Item;
					if (!pItem->Initialize(Names.GetAt(nName)))
					{
						AfxThrowUserException();
					}
					Add(pItem);
					fResult = TRUE;
				}
				CATCH_ALL(e)
				{
					delete pItem;
					pItem = NULL;
				}
				END_CATCH_ALL;
			}

			fResult = GetSize() > 0;
		}
	}
	END_TRY

	return fResult;
}

BOOL CBitmapResourceArray::GetBitmap(CBitmap& Bitmap, const CRect& crRect)
{
	BOOL fResult = FALSE;

	// Delete any existing bitmap.
	if (Bitmap.GetSafeHandle() != NULL)
	{
		Bitmap.DeleteObject();
	}

	double dMinDistortion;
	int nMinIndex = -1;

	// Run through all the bitmaps and find the one with the minimum distortion.
	for (int nIndex = 0; nIndex < GetSize(); nIndex++)
	{
		Item* pItem = GetAt(nIndex);
		ASSERT(pItem != NULL);
		if (pItem != NULL)
		{
			int nWidth;
			int nHeight;
			if (pItem->GetSize(nWidth, nHeight))
			{
				if ((nWidth != 0) && (nHeight != 0))
				{
					double dXDistortion = ((double)crRect.Width())/((double)nWidth);
					if (dXDistortion < 1.0)
					{
						if (dXDistortion < 0.0001)
						{
							dXDistortion = 0.0001;
						}
						dXDistortion = 1.0/dXDistortion;
					}

					double dYDistortion = ((double)crRect.Height())/((double)nHeight);
					if (dYDistortion < 1.0)
					{
						if (dYDistortion < 0.0001)
						{
							dYDistortion = 0.0001;
						}
						dYDistortion = 1.0/dYDistortion;
					}

					if ((nMinIndex == -1) || (dMinDistortion > (dXDistortion+dYDistortion)))
					{
						nMinIndex = nIndex;
						dMinDistortion = dXDistortion+dYDistortion;
					}
				}
			}
		}
	}

	if (nMinIndex >= 0)
	{
		Item* pItem = GetAt(nMinIndex);
		ASSERT(pItem != NULL);
		if (pItem != NULL)
		{
			if (pItem->GetBitmap(Bitmap))
			{
				fResult = TRUE;
			}
		}
	}

	return fResult;
}

/////////////////////////////////////////////////////////
// CBitmapResourceArray::Item

CBitmapResourceArray::Item::Item()
{
	m_fHeaderValid = FALSE;
}

CBitmapResourceArray::Item::~Item()
{
}

BOOL CBitmapResourceArray::Item::Initialize(LPCSTR pszName)
{
	BOOL fResult = FALSE;

	TRY
	{
		m_csName = pszName;
		fResult = TRUE;
	}
	END_TRY

	return fResult;
}

BOOL CBitmapResourceArray::Item::GetSize(int& nWidth, int& nHeight)
{
	BOOL fResult = FALSE;

	if (LoadHeader())
	{
		nWidth = (int)(m_Header.biWidth);
		nHeight = (int)(m_Header.biHeight);

		fResult = TRUE;
	}

	return fResult;
}

BOOL CBitmapResourceArray::Item::LoadHeader(void)
{
	BOOL fResult = TRUE;

	if (!m_fHeaderValid)
	{
		fResult = FALSE;

		HRSRC hBitmapResource = NULL;
		HGLOBAL hBitmapData = NULL;
		LPBITMAPINFOHEADER pBitmapData = NULL;

		TRY
		{
			if (!m_csName.IsEmpty())
			{
				// Read in the resource.
				hBitmapResource = FindResource(AfxGetResourceHandle(), m_csName, RT_BITMAP);
				if (hBitmapResource != NULL)
				{
					hBitmapData = LoadResource(AfxGetResourceHandle(), hBitmapResource);
					if (hBitmapData != NULL)
					{
						pBitmapData = (LPBITMAPINFOHEADER)LockResource(hBitmapData);
						if (pBitmapData != NULL)
						{
							m_Header = *pBitmapData;
							m_fHeaderValid = TRUE;
							fResult = TRUE;
						}
					}
				}
			}
		}
		END_TRY

		if (pBitmapData != NULL)
		{
			UnlockResource(hBitmapData);
			pBitmapData = NULL;
		}

		if (hBitmapData != NULL)
		{
			FreeResource(hBitmapData);
			hBitmapData = NULL;
		}
	}

	return fResult;
}

BOOL CBitmapResourceArray::Item::GetBitmap(CBitmap& Bitmap)
{
	BOOL fResult = FALSE;

	fResult = FALSE;

	HRSRC hBitmapResource = NULL;
	HGLOBAL hBitmapData = NULL;
	LPBITMAPINFOHEADER pBitmapData = NULL;
	HBITMAP hBitmap = NULL;
	HDC hDC = NULL;
	HPALETTE hOldPalette = NULL;

	TRY
	{
		if (!m_csName.IsEmpty())
		{
			// Read in the resource.
			hBitmapResource = FindResource(AfxGetResourceHandle(), m_csName, RT_BITMAP);
			if (hBitmapResource != NULL)
			{
				hBitmapData = LoadResource(AfxGetResourceHandle(), hBitmapResource);
				if (hBitmapData != NULL)
				{
					pBitmapData = (LPBITMAPINFOHEADER)LockResource(hBitmapData);
					if (pBitmapData != NULL)
					{
						hDC = GetDC(NULL);
						if (hDC != NULL)
						{
							hOldPalette = SelectPalette(hDC, (HPALETTE)(pOurPal->GetSafeHandle()), FALSE);
							RealizePalette(hDC);

							hBitmap = CreateDIBitmap(
									hDC,
									pBitmapData,
									CBM_INIT,
									(LPSTR)pBitmapData+pBitmapData->biSize+256*sizeof(RGBQUAD),
									(LPBITMAPINFO)pBitmapData,
									DIB_RGB_COLORS);

							if (hBitmap != NULL)
							{
								if (Bitmap.Attach(hBitmap))
								{
									fResult = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}
	END_TRY

	if ((!fResult) && (hBitmap != NULL))
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	if (hDC != NULL)
	{
		if (hOldPalette != NULL)
		{
			SelectPalette(hDC, hOldPalette, FALSE);
			hOldPalette = NULL;
		}
		ReleaseDC(NULL, hDC);
		hDC = NULL;
	}

	if (pBitmapData != NULL)
	{
		UnlockResource(hBitmapData);
		pBitmapData = NULL;
	}

	if (hBitmapData != NULL)
	{
		FreeResource(hBitmapData);
		hBitmapData = NULL;
	}

	return fResult;
}

