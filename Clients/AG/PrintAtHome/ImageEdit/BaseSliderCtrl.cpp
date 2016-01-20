#include "stdafx.h"
#include "BaseSliderCtrl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

BOOL SaveBitmap(LPCTSTR lpFileName, CBitmap& Bitmap, HPALETTE hPal = NULL)
{
	BOOL bResult = FALSE;
	
	PICTDESC stPictDesc;
	stPictDesc.cbSizeofstruct = sizeof(PICTDESC);
	stPictDesc.picType = PICTYPE_BITMAP;
	stPictDesc.bmp.hbitmap = Bitmap;
	stPictDesc.bmp.hpal = hPal;
	
	LPPICTURE pPicture;
	HRESULT hr = OleCreatePictureIndirect( &stPictDesc, IID_IPicture, FALSE,
		reinterpret_cast<void**>(&pPicture) );
	if ( SUCCEEDED(hr) )
	{
		LPSTREAM pStream;
		hr = CreateStreamOnHGlobal( NULL, TRUE, &pStream );
		if ( SUCCEEDED(hr) )
		{
			long lBytesStreamed = 0;
			hr = pPicture->SaveAsFile( pStream, TRUE, &lBytesStreamed );
			if ( SUCCEEDED(hr) )
			{
				CString strFileName = _T("C:\\");
				strFileName += lpFileName;
				HANDLE hFile = CreateFile(strFileName, 
					GENERIC_WRITE, 
					FILE_SHARE_READ, 
					NULL,
					CREATE_ALWAYS, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL );
				if ( hFile )
				{
					HGLOBAL hMem = NULL;
					GetHGlobalFromStream( pStream, &hMem );
					LPVOID lpData = GlobalLock( hMem );
					
					DWORD dwBytesWritten;
					bResult = WriteFile( hFile, lpData, lBytesStreamed, &dwBytesWritten, NULL );
					bResult &= ( dwBytesWritten == (DWORD)lBytesStreamed );
					
					// clean up
					GlobalUnlock(hMem);
					CloseHandle(hFile);
				}
			}
			// clean up         
			pStream->Release();
		}
		// clean up      
		pPicture->Release();
	}
	
	return bResult;   
}

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CBaseSliderCtrl, CSliderCtrl)
BEGIN_MESSAGE_MAP(CBaseSliderCtrl, CSliderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CBaseSliderCtrl::CBaseSliderCtrl()
{
	m_bRedraw = false;
}

/////////////////////////////////////////////////////////////////////////////
CBaseSliderCtrl::~CBaseSliderCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
void CBaseSliderCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	///////////////////////////////////////////////////////////////////////////////
	// A slider (track control) is drawn in several steps:
	//	1. Erase
	//	2. Tics
	//	3. Channel
	//	4. Thumb
	//
	// It would be nice to capture when the background has been painted and
	// before the other sub-pieces have been painted. Then we could just
	// substitute our own painting routine. But this doesn't seem to be available.
	//
	// So this routine captures the tics by inserting operations before painting the thumb.
	//
	// The stages include CDDS_PREPAINT, which is just before painting of the entire
	// control. However, unless the *pResult parameter is set to CDRF_NOTIFYITEMDRAW,
	// we will get notification for the control as a whole, not for each piece-part.
	// So the first thing to do is set *pResult. Thereafter, we must intercept
	// the sub-parts.
	//
	// We don't care about painting the background (we will re-paint later on). We
	// don't care about PREPAINT on the CHANNEL or the TICS since we will overwrite
	// everything when we get to the THUMB.
	/////////////////////////////////////////////////////////////////////////////////

	LPNMCUSTOMDRAW lpcd = (LPNMCUSTOMDRAW)pNMHDR;
	CDC* pDC = CDC::FromHandle(lpcd->hdc);
	DWORD dwStyle = GetStyle();
	switch(lpcd->dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		}
		case CDDS_ITEMPREPAINT:
		{
			switch(lpcd->dwItemSpec)
			{
				case TBCD_TICS:
				{
					*pResult = CDRF_DODEFAULT;
					break;
				}
				case TBCD_THUMB:
				{
					*pResult = CDRF_DODEFAULT;
					break;
				}
				case TBCD_CHANNEL:
				{
//j					DrawChannel(pDC, lpcd);
					*pResult = CDRF_DODEFAULT;
					break;
				}
			}

			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CBaseSliderCtrl::DrawChannel(CDC* pDC, LPNMCUSTOMDRAW lpcd)
{
	CRect crect;
	GetClientRect(crect);
	int iWidth = crect.Width();
	int iHeight = crect.Height();

	COLORREF crOldBack = pDC->SetBkColor(RGB(0,0,0));			// set to Black
	COLORREF crOldText = pDC->SetTextColor(RGB(255,255,255));	// set to White

	CDC SaveCDC;
	SaveCDC.CreateCompatibleDC(pDC);
	CBitmap SaveCBmp;
	SaveCBmp.CreateCompatibleBitmap(&SaveCDC, iWidth, iHeight);
	CBitmap* SaveCBmpOld = SaveCDC.SelectObject(&SaveCBmp);
	SaveCDC.BitBlt(0, 0, iWidth, iHeight, pDC, crect.left, crect.top, SRCCOPY);
	SaveBitmap(_T("MonoTicsMask.bmp"), SaveCBmp);

	// This bit does the tics marks transparently.
	// create a memory dc to hold a copy of the oldbitmap data that includes the tics,
	// because when we add the background in we will lose the tic marks
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBM;
	memBM.CreateCompatibleBitmap(pDC, iWidth, iHeight);
	CBitmap* oldbm = memDC.SelectObject(&memBM);

	//set the colours for the monochrome mask bitmap
	memDC.BitBlt(0, 0, iWidth, iHeight, pDC, 0, 0, SRCCOPY);
	SaveBitmap(_T("0.bmp"), memBM);
	memDC.SetBkColor(pDC->GetBkColor());
	memDC.SetTextColor(pDC->GetTextColor());
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);
	SaveBitmap(_T("1.bmp"), memBM);
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCAND);
	SaveBitmap(_T("2.bmp"), memBM);
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);
	SaveBitmap(_T("3.bmp"), memBM);
	pDC->BitBlt(0, 0, iWidth, iHeight, &memDC, 0, 0, SRCCOPY);

	//restore and clean up
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);
	SaveCDC.SelectObject(SaveCBmpOld);
	SaveCBmp.DeleteObject();
	SaveCDC.DeleteDC();			
	memDC.SelectObject(oldbm);
	memBM.DeleteObject();
	memDC.DeleteDC();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBaseSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}
