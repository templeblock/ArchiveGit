#include "stdafx.h"
#include "monacoview.h"
#include "ColorGamut.h"
#include "MonacoViewDlg.h"
#include "colortran.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CColorGamut, CStatic)
	//{{AFX_MSG_MAP(CColorGamut)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorGamut

/////////////////////////////////////////////////////////////////////////////
CColorGamut::CColorGamut()
{
	m_pBitsOriginal = NULL;
	m_pBitsModified = NULL;
	m_num_bytes = 0;
	m_LastLDrawn = -1;
}

/////////////////////////////////////////////////////////////////////////////
CColorGamut::~CColorGamut()
{
	if (m_pBitsOriginal)
		delete [] m_pBitsOriginal;
	if (m_pBitsModified)
		delete [] m_pBitsModified;
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::Init()
{
    HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_COLORMAP)/*szBitmap*/, IMAGE_BITMAP, 0/*dx*/, 0/*dy*/, LR_CREATEDIBSECTION); 
	if (!hBitmap)
		return;

	m_Bitmap.Attach(hBitmap);

	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	m_num_bytes = bm.bmHeight * bm.bmWidthBytes;

	m_pBitsModified = new BYTE[m_num_bytes];
	m_pBitsOriginal = new BYTE[m_num_bytes];

	if (m_pBitsOriginal)
		int num_bytes = m_Bitmap.GetBitmapBits(m_num_bytes, m_pBitsOriginal);
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::GetColor(double L, CPoint where, double* labOut)
{
	if (!labOut)
		return;

	CRect rect;
	GetClientRect(&rect);
	CSize size = rect.Size();

	double x = (0.85 * where.x) / size.cx;
	double y = (0.9  * (size.cy - where.y)) / size.cy;

	double  Lxy[3];
	Lxy[0] = L;
	Lxy[1] = x;
	Lxy[2] = y;

	sxyztolab(Lxy,labOut);

	labOut[1] *= 0.85;
	labOut[2] *= 0.85;

	if (labOut[1] < -200) labOut[1] = -200;
	if (labOut[1] >  200) labOut[1] = 200;

	if (labOut[2] < -200) labOut[2] = -200;
	if (labOut[2] >  200) labOut[2] = 200;
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::RenderColor(int L)
{
	if (L == m_LastLDrawn)
		return;

	m_LastLDrawn = L;

	if (L < 0 || L > 100)
		return;

	if (!m_pBitsOriginal || !m_pBitsModified)
		return;

#if 1
	// Build a lut to scale the original pixels
	BYTE lut[256];
	for (int i=0; i<256; i++)
	{
		double t;
		if (L <= 50)
		{
			t = ((double)L * 2 * i) / 100.0;
		}
		else
		{
			t = ((double)((L * 4) - 100) * i) / 100.0;
			if (t > 255) t = 255;
		}

		lut[i] = (BYTE)t;
	}

	// Scale the pixels
	for (i=0; i<m_num_bytes; i++)
	{
		BYTE p = m_pBitsOriginal[i];
		m_pBitsModified[i] = lut[p];
	}

#else
	// Compute the pixels
	CRect rect;
	GetClientRect(&rect);
	CSize size = rect.Size();
	CPoint pt;
	for (pt.y=0; pt.y<size.cy; pt.y++)
	{
		for (pt.x=0; pt.x<size.cx; pt.x++)
		{
			double lab[3];
			GetColor((double)L, pt, lab);
			BYTE rgb[3];
			if (lab[1] < -128 || lab[1] > 128 || 
				lab[2] < -128 || lab[2] > 128)
			{
				rgb[0] = rgb[1] = rgb[2] = 0;
			}
			else
				labtorgb(lab, rgb, 1.8/*gamma*/);
			int iOffset = 3 * (pt.y*100 + pt.x);
			m_pBitsModified[iOffset + 0] = rgb[2];
			m_pBitsModified[iOffset + 1] = rgb[1];
			m_pBitsModified[iOffset + 2] = rgb[0];
		}
	}
#endif

	int num_bytes = m_Bitmap.SetBitmapBits(m_num_bytes, m_pBitsModified);

	Invalidate(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);

	// copy m_Bitmap to the screen:
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = mdc.SelectObject(&m_Bitmap);
	DrawTriangle(&mdc);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &mdc, 0, 0, SRCCOPY);
	mdc.SelectObject(pOldBitmap);
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::DrawTriangle(CDC* pDC)
{
	double desiredLAB[3] = {m_LastLDrawn, 0, 0};  //j
	double lab[24*3];
	long i, j;

	MonacoViewDlg* pWnd = (MonacoViewDlg*)GetParent();
	if (!pWnd) return;
	
	for(i = 0; i < 24; i++)
	{
		j = i*3;
		lab[j] = desiredLAB[0];
		lab[j+1] = 400;
		lab[j+2] = i*15;
		lchtolab(&lab[j], &lab[j]);
	}

	for(i = 0; i < 24; i++)
	{
		j = i*3;
		pWnd->gamutbound((LabColorType*)(lab+j));
		lab[j+1] /= 0.85;
		lab[j+2] /= 0.85;
		labtosxyz(lab+j, lab+j);
	}

	RECT r;
	GetClientRect(&r);
	
	POINT pt[24];
	for(i = 0; i < 24; i++)
	{
		j = i*3;
		//pt[i].x = r.left + (lab[j+1])*(r.right - r.left);
		//pt[i].y = r.top + (1-lab[j+2])*(r.bottom - r.top);
		pt[i].x = (long)(r.left + (lab[j+1])*(r.right - r.left) / 0.85);
		pt[i].y = (long)(r.bottom - lab[j+2]*(r.bottom - r.top) / 0.9);
	}

	int iOldMapMode = pDC->SetMapMode(MM_TEXT);
	CPen Pen;
	if(	desiredLAB[0] <= 50)
		Pen.CreatePen(PS_SOLID, 1, RGB(255,255,255));
	else
		Pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	
	CPen* pOldPen = pDC->SelectObject(&Pen);
	pDC->MoveTo(pt[24-1]);

	POINT* p = &pt[24-1];
	for (i = 0; i < 24; i++)
	{
		if (pt[i].x != p->x || pt[i].y != p->y)
			pDC->LineTo(pt[i]);
		p = &pt[i];	
	}

	pDC->SetMapMode(iOldMapMode);
	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CStatic::OnLButtonDown(nFlags, point);

	MonacoViewDlg* pWnd = (MonacoViewDlg*)GetParent();
	if (pWnd)
	{
		double lab[3];
		GetColor((double)pWnd->m_in_l, point, lab);
		pWnd->SetPointColor((int)lab[1], (int)lab[2]);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CColorGamut::OnMouseMove(UINT nFlags, CPoint point) 
{
	CStatic::OnMouseMove(nFlags, point);

	if (!(nFlags & MK_LBUTTON)) // if the left button is not down, get out
		return;

	MonacoViewDlg* pWnd = (MonacoViewDlg*)GetParent();
	if (pWnd)
	{
		double lab[3];
		GetColor((double)pWnd->m_in_l, point, lab);
		pWnd->SetPointColor((int)lab[1], (int)lab[2]);
	}
}
