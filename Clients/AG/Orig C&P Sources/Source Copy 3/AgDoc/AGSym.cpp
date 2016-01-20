//==========================================================================//
// AGSym.cpp - CAGSymImage and CAGSymText classes
//==========================================================================//
#include "stdafx.h"
#include "AGSym.h"
#include "AGDib.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym::CAGSym(SYMTYPE SymType)
{
	m_SymType = SymType;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSym::~CAGSym()
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSym::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	pInput->Read(&m_Matrix, sizeof(m_Matrix));
	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSym::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	pOutput->Write(&m_Matrix, sizeof(m_Matrix));
	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSymImage::CAGSymImage() : CAGSym(ST_IMAGE)
{
	m_pDIB = NULL;
	m_DestRect.left = m_DestRect.right = m_DestRect.top = m_DestRect.bottom = 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSymImage::~CAGSymImage()
{
	Free();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSymImage::Draw(CAGDC& dc)
{
	if (!m_pDIB)
		return;

	dc.PushModelingMatrix(m_Matrix);
	dc.StretchBlt(m_DestRect, DibPtr(m_pDIB), (BITMAPINFO*)m_pDIB);
	dc.PopModelingMatrix();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSymImage::Free()
{
	if (!m_pDIB)
		return;

	free(m_pDIB);
	m_pDIB = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymImage::HitTest(POINT Pt) const
{
	RECT r = m_DestRect;
	m_Matrix.Inverse().Transform(&Pt, 1);
	return (::PtInRect(&r, Pt) != 0);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSymImage::LoadDIB(const BITMAPINFOHEADER* pHdr, const BYTE* pBits)
{
	Free();

	if (pHdr->biCompression == BI_RGB && (pHdr->biBitCount == 1 || pHdr->biBitCount == 4 || pHdr->biBitCount == 8 || pHdr->biBitCount == 24))
	{
		BITMAPINFOHEADER bi;
		bi = *pHdr;
		bi.biCompression = BI_RGB;

		if (!bi.biSizeImage)
			bi.biSizeImage = DibSizeImage(&bi);
		if (!bi.biClrUsed)
			bi.biClrUsed = DibNumColors(&bi);

		if (m_pDIB = (BITMAPINFOHEADER*)malloc(DibSize(&bi)))
		{
			*m_pDIB = bi;

			if (bi.biClrUsed)
				memcpy((void*)DibColors(m_pDIB), (void*)DibColors(pHdr), DibPaletteSize(pHdr));

			BYTE* pNewBits = (BYTE*)DibPtr(m_pDIB);
			const BYTE* pSrcBits;
			if (pBits)
				pSrcBits = pBits;
			else
				pSrcBits = (BYTE*)(DibColors(pHdr) + bi.biClrUsed);

			memcpy(pNewBits, pSrcBits, bi.biSizeImage);
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymImage::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	Free();

	bool bReturn = CAGSym::Read(pInput);

	DWORD dwSize = 0;
	pInput->Read(&dwSize, sizeof(dwSize));

	if (dwSize > 0)
	{
		m_pDIB = (BITMAPINFOHEADER*)malloc(dwSize);
		pInput->Read(m_pDIB, dwSize);
		pInput->Read(&m_DestRect, sizeof(m_DestRect));
	}

	return bReturn;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymImage::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);

	if (m_pDIB)
	{
		DWORD dwSize = DibSize(m_pDIB);
		pOutput->Write(&dwSize, sizeof(dwSize));
		pOutput->Write(m_pDIB, dwSize);
		pOutput->Write(&m_DestRect, sizeof(m_DestRect));
	}
	else
	{
		DWORD dwSize = 0;
		pOutput->Write(&dwSize, sizeof(dwSize));
	}

	return bReturn;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSymText::CAGSymText() : CAGSym(ST_TEXT)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSymText::~CAGSymText()
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSymText::Draw(CAGDC& dc)
{
	dc.PushModelingMatrix(m_Matrix);
	DrawColumn(dc);
	dc.PopModelingMatrix();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymText::HitTest(POINT Pt) const
{
	RECT r = m_DestRect;
	m_Matrix.Inverse().Transform(&Pt, 1);
	return (::PtInRect(&r, Pt) != 0);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymText::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	bool bReturn = CAGSym::Read(pInput);
	ReadColumn(pInput);

	return (bReturn);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGSymText::Write(CAGDocIO* pOutput)
{
	if (!pOutput)
		return false; // should never happen

	bool bReturn = CAGSym::Write(pOutput);
	WriteColumn(pOutput);

	return (bReturn);
}
