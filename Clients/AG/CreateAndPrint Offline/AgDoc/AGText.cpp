#include "stdafx.h"
#include "AGSym.h"
#include "AGText.h"
#include "AGDC.h"

#include "scappint.h"
#include "sccallbk.h"
#include "scstyle.h"
#include "scselect.h"
#include "scapptex.h"
#include "sccolumn.h"
#include "scparagr.h"
#include "sctextli.h"
#include "scstream.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define NEW_SPEC_FLAG1 0x8000

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
#define LBUTTON (GetAsyncKeyState(VK_LBUTTON)<0)

static long ReadFunc(APPCtxPtr pTextCtx, void* ptr, long size);
static long WriteFunc(APPCtxPtr pTextCtx, void* ptr, long size);

/* Some important info about Windows text metrics: All values in logical units
	tmHeight: The height of the character - in sync with the lfHeight field of the LOGFONT.
		tmHeight = tmAscent + tmDescent
		If lfHeight > 0 (representing the font's line spacing), tmHeight is approx equal to lfHeight
		If lfHeight < 0 (representing the font's point size), tmHeight - tmInternalLeading is approx equal to abs(lfHeight)

	tmAscent: The vertical size of the character above the baseline. 

	tmDescent: The vertical size of the character below the baseline. 

	tmInternalLeading: A vertical size included in the tmHeight value that is usually occupied by diacritics
	on some capital letters. The point size of the font = tmHeight - tmInternalLeading. 

	tmExternalLeading: An additional amount of line spacing beyond tmHeight recommended by the designer
	of the font for spacing successive lines of text.

	tmAveCharWidth: The average width of lowercase letters in the font. 

	tmMaxCharWidth: The width of the widest character. For a fixed-pitch font, this is the same as tmAveCharWidth. 

	tmOverhang: Extra width added to a raster font character when synthesizing italic or boldface.
	When a raster font is italicized, the tmAveCharWidth value remains unchanged, because a string of italicized
	characters has the same overall width as the same string of normal characters. For boldfacing, Windows must
	slightly expand the width of each character. For a boldface font, the tmAveCharWidth value less the tmOverhang
	value equals the tmAveCharWidth value for the same font without boldfacing.
*/

//////////////////////////////////////////////////////////////////////
CAGSpec::CAGSpec()
{
	::ZeroMemory(&m_Font, sizeof(m_Font));
	m_HorzJust = eFlushLeft;
	m_sLineLeadPct = 0;
	m_sFlags = 0;

	m_LineWidth = LT_None;
	m_LineColor = RGB(0,0,0);

	m_FillType = FT_Solid;
	m_FillColor = RGB(0,0,0);
	m_FillColor2 = RGB(255,255,0);
}

//////////////////////////////////////////////////////////////////////
CAGSpec::CAGSpec(const LOGFONT& Font, eTSJust HorzJust, short sLineLeadPct, int LineWidth, COLORREF LineColor, FillType Fill, COLORREF FillColor, COLORREF FillColor2)
{
	m_Font = Font;
	m_HorzJust = HorzJust;
	m_sLineLeadPct = sLineLeadPct;
	m_sFlags = 0;

	m_LineWidth = LineWidth;
	m_LineColor = LineColor;

	m_FillType = Fill;
	m_FillColor = FillColor;
	m_FillColor2 = FillColor2;
}

//////////////////////////////////////////////////////////////////////
CAGSpec::CAGSpec(const CAGSpec& Spec)
{
	m_Font = Spec.m_Font;
	m_HorzJust = Spec.m_HorzJust;
	m_sLineLeadPct = Spec.m_sLineLeadPct;
	m_sFlags = Spec.m_sFlags;

	m_LineWidth = Spec.m_LineWidth;
	m_LineColor = Spec.m_LineColor;

	m_FillType = Spec.m_FillType;
	m_FillColor = Spec.m_FillColor;
	m_FillColor2 = Spec.m_FillColor2;
}

//////////////////////////////////////////////////////////////////////
void CAGSpec::operator= (const CAGSpec& Spec)
{
	m_Font = Spec.m_Font;
	m_HorzJust = Spec.m_HorzJust;
	m_sLineLeadPct = Spec.m_sLineLeadPct;
	m_sFlags = Spec.m_sFlags;

	m_LineWidth = Spec.m_LineWidth;
	m_LineColor = Spec.m_LineColor;

	m_FillType = Spec.m_FillType;
	m_FillColor = Spec.m_FillColor;
	m_FillColor2 = Spec.m_FillColor2;
}

//////////////////////////////////////////////////////////////////////
bool LogFontCompare(const LOGFONTA& a, const LOGFONTA& b)
{
	return a.lfHeight 			== b.lfHeight			&&
		   a.lfWidth			== b.lfWidth			&&
		   a.lfEscapement 		== b.lfEscapement		&&
		   a.lfOrientation		== b.lfOrientation		&&
		   a.lfWeight 			== b.lfWeight			&&
		   (a.lfItalic != 0)	== (b.lfItalic != 0)	&&
		   (a.lfUnderline != 0)	== (b.lfUnderline != 0) &&
		   (a.lfStrikeOut != 0)	== (b.lfStrikeOut != 0) &&
		   a.lfCharSet			== b.lfCharSet			&&
		   a.lfOutPrecision		== b.lfOutPrecision 	&&
		   a.lfClipPrecision	== b.lfClipPrecision	&&
		   a.lfQuality			== b.lfQuality			&&
		   a.lfPitchAndFamily 	== b.lfPitchAndFamily	&&
		   !::strcmp(a.lfFaceName, b.lfFaceName);
}

//////////////////////////////////////////////////////////////////////
bool CAGSpec::operator== (const CAGSpec& Spec) const
{
	return (LogFontCompare(m_Font, Spec.m_Font) &&
			m_HorzJust == Spec.m_HorzJust &&
			m_sLineLeadPct == Spec.m_sLineLeadPct &&
			m_sFlags == Spec.m_sFlags &&
			m_LineWidth == Spec.m_LineWidth &&
			m_LineColor == Spec.m_LineColor &&
			m_FillType == Spec.m_FillType &&
			m_FillColor == Spec.m_FillColor &&
			m_FillColor2 == Spec.m_FillColor2);
}

//////////////////////////////////////////////////////////////////////
bool CAGSpec::Read(CAGDocIO* pInput)
{
	if (!pInput)
		return false; // should never happen

	pInput->Read(&m_Font, sizeof(m_Font));
	pInput->Read(&m_FillColor, sizeof(m_FillColor));
	pInput->Read(&m_HorzJust, sizeof(m_HorzJust));
	pInput->Read(&m_FillColor2, sizeof(m_FillColor2)); //j was m_nAboveParaLeading
	pInput->Read(&m_LineColor, sizeof(m_LineColor)); //j was m_nBelowParaLeading
	pInput->Read(&m_sLineLeadPct, sizeof(m_sLineLeadPct)); //j was loword of m_nLeading
	pInput->Read(&m_sFlags, sizeof(m_sFlags)); //j was hiword of m_nLeading, guaranteed zero

	// The 4 values (m_FillColor2, m_LineColor, m_sLineLeadPct, m_sFlags) were originally used for obsolete leading features
	if (!(m_sFlags & NEW_SPEC_FLAG1))
	{ // Obsolete leading features; do a conversion
		CAGIC TextIC("DISPLAY");
		TextIC.SetFont(m_Font, false/*bTransform*/);
		TEXTMETRIC tm;
		TextIC.GetTextMetrics(&tm);

		int iLineLead = (!m_sLineLeadPct ? 0 : m_sLineLeadPct - tm.tmHeight); // Value read from disk was old leading value
		m_sLineLeadPct = dtoi((1000.0 * iLineLead) / tm.tmHeight);
		m_FillColor2 = RGB(255,255,0); // Value read from disk was old leading value
		m_LineColor = RGB(0,0,0); // Value read from disk was old leading value
		m_sFlags = 0; // Value read from disk was old leading value
	}

	// Some texts have eRagJustified which need to be defaulted to eFlushLeft
	// Stonehand seems to treat it as eRagRight
	m_HorzJust = (m_HorzJust == eRagJustified)?eFlushLeft:m_HorzJust;
	// Pull m_FillType out of the m_FillColor field
	BYTE byte = (BYTE)(m_FillColor >> 24);
	m_FillColor &= 0x00FFFFFF;
	m_FillType = (byte == 255/*it's illegal*/ ? FT_Solid : (FillType)byte);
	// Pull m_LineWidth out of the m_LineColor field
	byte = (BYTE)(m_LineColor >> 24);
	m_LineColor &= 0x00FFFFFF;
	m_LineWidth = (byte == 0 ? LT_None : (byte == 255 ? LT_Hairline : byte));
	if (m_FillType == FT_None && m_LineWidth == LT_None)
		m_FillType = FT_Solid;

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CAGSpec::Write(CAGDocIO* pOutput) const
{
	// The 4 values (m_FillColor2, m_LineColor, m_sLineLeadPct, m_sFlags) were originally used for obsolete leading features
	unsigned short sFlags = NEW_SPEC_FLAG1 | m_sFlags;

	// Stuff m_FillType back into the m_FillColor field
	BYTE byte = (BYTE)m_FillType;
	COLORREF FillColor = ((DWORD)byte << 24) | (m_FillColor & 0x00FFFFFF);
	// Stuff m_LineWidth back into the m_LineColor field
	byte = (m_LineWidth == LT_None ? 0 : (m_LineWidth == LT_Hairline ? 255 : (BYTE)m_LineWidth));
	COLORREF LineColor = ((DWORD)byte << 24) | (m_LineColor & 0x00FFFFFF);

	pOutput->Write(&m_Font, sizeof(m_Font));
	pOutput->Write(&FillColor, sizeof(FillColor));
	pOutput->Write(&m_HorzJust, sizeof(m_HorzJust));
	pOutput->Write(&m_FillColor2, sizeof(m_FillColor2)); //j was m_nAboveParaLeading
	pOutput->Write(&LineColor, sizeof(LineColor)); //j was m_nBelowParaLeading
	pOutput->Write(&m_sLineLeadPct, sizeof(m_sLineLeadPct)); //j was loword of m_nLeading
	pOutput->Write(&sFlags, sizeof(sFlags)); //j was hiword of m_nLeading, guaranteed zero
	return true;
}

//////////////////////////////////////////////////////////////////////
void CAGSpec::DrawTextAligned(CAGDC* pDC, LPCSTR pString, int* pWidths, RECT& RectAlign, eVertJust VertJust) const
{
	int iHorzAlign = 0;
	if (m_HorzJust == eFlushLeft)	iHorzAlign = -1;	else
	if (m_HorzJust == eRagCentered)	iHorzAlign = 0;		else
	if (m_HorzJust == eFlushRight)	iHorzAlign = +1;

	int iVertAlign = 0;
	if (VertJust == eVertTop)		iVertAlign = -1;	else
	if (VertJust == eVertCentered)	iVertAlign = 0;		else
	if (VertJust == eVertBottom)	iVertAlign = +1;

	DrawText(pDC, pString, pWidths, iHorzAlign, iVertAlign, &RectAlign);
}

//////////////////////////////////////////////////////////////////////
void CAGSpec::DrawText(CAGDC* pDC, LPCSTR pString, int* pWidths, int x, int y, RECT* pRectAlign) const
{
	if (!pDC)
		return;

	if (!pString)
		return;

	pDC->SetFont(m_Font, true/*bTransform*/);

	int iStringLen = strlen(pString);
	if (!iStringLen)
		return;

	COLORREF LineColor = (m_LineWidth == LT_None ? CLR_NONE : m_LineColor);
	COLORREF FillColor = (m_FillType == FT_None ? CLR_NONE : m_FillColor);
	bool bInvisible = (LineColor == CLR_NONE && FillColor == CLR_NONE);
	if (bInvisible) // don't let it be invisible
		LineColor = RGB(0,0,0);

	// Handle the simple (legacy) case separately
	if (m_FillType == FT_Solid && m_LineWidth == LT_None)
	{
		pDC->DrawText(pString, iStringLen, pWidths, LineColor, LT_None, FillColor, x, y, pRectAlign);
		return;
	}

	HRGN hClipRegion = pDC->SaveClipRegion();
	if (!pDC->SetClipText(pString, iStringLen, pWidths, x, y, pRectAlign))
	{ // Clipping to the text path failed! 	Handle the drawing just like the simple case above
		pDC->DrawText(pString, iStringLen, pWidths, LineColor, LT_None, FillColor, x, y, pRectAlign);
		pDC->RestoreClipRegion(hClipRegion);
		return;
	}

	// Fill the interior
	if (m_FillType != FT_None)
	{
		RECT StringRect = {0,0,0,0};
		pDC->GetClipRect(&StringRect);

		POINT pt[4];
		pt[0].x = StringRect.left;	pt[0].y = StringRect.top;
		pt[1].x = StringRect.right;	pt[1].y = StringRect.top;
		pt[2].x = StringRect.right;	pt[2].y = StringRect.bottom;
		pt[3].x = StringRect.left;	pt[3].y = StringRect.bottom;

		if (m_FillType == FT_SweepRight || m_FillType == FT_SweepDown)
			pDC->FillLinearGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_SweepRight)); 
		else
		if  (m_FillType == FT_RadialCenter || m_FillType == FT_RadialCorner)
			pDC->FillRadialGradient(pt, m_FillColor, m_FillColor2, (m_FillType == FT_RadialCenter));
		else
			pDC->FillRectangle(pt, m_FillColor);
	}

	// Unlike the other shapes, all text outlines are drawn AFTER the clip region has been restored
	// So the outline is half inside the text, and half outside 
	pDC->RestoreClipRegion(hClipRegion);

	// Draw the outline
	int LineWidth = LT_None;
	if (m_LineWidth >= LT_Normal)
		LineWidth = dtoi(pDC->GetViewToDeviceMatrix().TransformDistance(m_LineWidth));
	else		
	if (m_LineWidth == LT_Hairline || bInvisible)
		LineWidth = LT_Hairline;

	pDC->DrawText(pString, iStringLen, pWidths, LineColor, LineWidth, CLR_NONE, x, y, pRectAlign);
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeFillType(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_FillType = pNewSpecTemplate->m_FillType;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeFillColor(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_FillColor = pNewSpecTemplate->m_FillColor;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeFillColor2(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_FillColor2 = pNewSpecTemplate->m_FillColor2;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeLineWidth(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_LineWidth = pNewSpecTemplate->m_LineWidth;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeLineColor(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_LineColor = pNewSpecTemplate->m_LineColor;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeTextSize(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_Font.lfHeight = pNewSpecTemplate->m_Font.lfHeight;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeTypeface(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	lstrcpy(pSpec->m_Font.lfFaceName, pNewSpecTemplate->m_Font.lfFaceName);
	pSpec->m_Font.lfWeight = pNewSpecTemplate->m_Font.lfWeight;
	pSpec->m_Font.lfItalic = pNewSpecTemplate->m_Font.lfItalic;
	pSpec->m_Font.lfCharSet = pNewSpecTemplate->m_Font.lfCharSet;
	pSpec->m_Font.lfOutPrecision = pNewSpecTemplate->m_Font.lfOutPrecision;
	pSpec->m_Font.lfClipPrecision = pNewSpecTemplate->m_Font.lfClipPrecision;
	pSpec->m_Font.lfQuality = pNewSpecTemplate->m_Font.lfQuality;
	pSpec->m_Font.lfPitchAndFamily = pNewSpecTemplate->m_Font.lfPitchAndFamily;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CAGSpec::CallbackChangeEmphasis(CAGSpec* pSpec, CAGSpec* pNewSpecTemplate)
{
	if (!pSpec || !pNewSpecTemplate)
		return false;

	pSpec->m_Font.lfWeight = pNewSpecTemplate->m_Font.lfWeight;
	pSpec->m_Font.lfItalic = pNewSpecTemplate->m_Font.lfItalic;
	pSpec->m_Font.lfUnderline = pNewSpecTemplate->m_Font.lfUnderline;
	return true;
}

//////////////////////////////////////////////////////////////////////
CAGText::CAGText()
{
	::SetRect(&m_DestRect, 0, 0, 0, 0);
	m_pColumn = NULL;
	m_pSelection = NULL;
	m_hPaintDC = NULL;
	m_pSym = NULL;
	m_pDC = NULL;
	m_pSavedDC = NULL;
	m_bTempDCInUse = false;
	m_bSelectionIsShown = false;
	m_bAllowBlinkCaret = false;
	m_pDocIO = NULL;
	m_pWriteTSList = NULL;
	m_bAutoSelect = false;
	m_bOverstrike = false;
	m_LastMousePoint.Invalidate();
	SetDirty(false);
}

//////////////////////////////////////////////////////////////////////
CAGText::~CAGText()
{
	if (m_pColumn)
		SCCOL_Delete(m_pColumn, 0);
}

//////////////////////////////////////////////////////////////////////
void CAGText::CopyStream(scColumn* pSrcColumn)
{
	if (!m_pColumn || !pSrcColumn)
		return;

	scStream* pStreamSrc = NULL;
	SCCOL_GetStream(pSrcColumn, pStreamSrc);
	if (!pStreamSrc)
		return;
	scStream* pStreamDst = NULL;
	SCSTR_Copy(pStreamSrc, pStreamDst);
	if (!pStreamDst)
		return;
	SCFS_PasteStream(m_pColumn, pStreamDst, 0);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SwapStreams(scColumn* pColumn)
{
	if (!m_pColumn || !pColumn)
		return;

	scStream* pStream1 = NULL;
	SCCOL_GetStream(pColumn, pStream1);
	if (!pStream1)
		return;
	scRedispList colRedisp;
	SCSTR_Cut(pStream1, &colRedisp);
	scStream* pStream2 = NULL;
	SCCOL_GetStream(m_pColumn, pStream2);
	if (!pStream2)
		return;
	SCSTR_Cut(pStream2, &colRedisp);
	SCFS_PasteStream(pColumn, pStream2, 0);
	SCFS_PasteStream(m_pColumn, pStream1, 0);
}

//////////////////////////////////////////////////////////////////////
void CAGText::Invalidate(scXRect& rRect)
{
	if (!m_pDC)
		return;

	RECT rect = { rRect.x1, rRect.y1, rRect.x2, rRect.y2 };
	::OffsetRect(&rect, m_DestRect.left, m_DestRect.top);
	m_pDC->GetSymbolToDeviceMatrix().Transform(rect);
	::InvalidateRect(m_pDC->GetWnd(), &rect, false);
	m_pDC->SetDirty(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::Redisplay(scRedispList& colRedisp)
{
	bool bPaint = false;

	scColRedisplay colRect;

	for (int limit = 0; limit < colRedisp.GetNumItems(); limit++)
	{
		colRedisp.GetDataAt(limit, (ElementPtr)&colRect);

		if (colRect.fHasDamage)
		{
			scXRect rRect(colRect.fDamageRect);

			if (!bPaint)
				ShowSelection(false);
			bPaint = true;
			Invalidate(rRect);
		}
		
		if (colRect.fHasRepaint)
		{
			scXRect rRect(colRect.fRepaintRect);

			if (!bPaint)
				ShowSelection(false);
			bPaint = true;
			Invalidate(rRect);
		}

		if (colRect.fImmediateRedisplay)
		{
			scXRect rRect(colRect.fImmediateArea.fImmediateRect);

			if (!bPaint)
				ShowSelection(false);
			bPaint = true;
			SCCOL_UpdateLine(colRect.fColumnID, colRect.fImmediateArea, this);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::Create(const RECT& DestRect)
{
	m_DestRect = DestRect;

	SCCOL_New(this, m_pColumn, WIDTH(m_DestRect), HEIGHT(m_DestRect));
	SCFS_Recompose(m_pColumn, 0);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetDestRect(const RECT& DestRect)
{
	m_DestRect = DestRect;
	scRedispList colRedisp;
	SCCOL_SetSize(m_pColumn, WIDTH(m_DestRect), HEIGHT(m_DestRect), &colRedisp);
	// Don't call Redisplay(colRedisp) here since the caller will redraw the entire symbol
}

//////////////////////////////////////////////////////////////////////
void CAGText::DrawAll(CAGDC* pDC)  
{
	if (!pDC)
		return;

	if (m_bTempDCInUse)
		return; // should never happen

	m_bTempDCInUse = true;
	m_pSavedDC = m_pDC;
	m_pDC = pDC;

	scXRect rClipRect(0, 0, WIDTH(m_DestRect), HEIGHT(m_DestRect));

	SCCOL_Update(m_pColumn, rClipRect, this);

	m_pDC = m_pSavedDC;
	m_pSavedDC = NULL;
	m_bTempDCInUse = false;
}

//////////////////////////////////////////////////////////////////////
void CAGText::DrawCaret(HDC hDC, const CAGMatrix& Matrix, const RECT& rect)
{
	POINT pt[2];
	pt[0].x = rect.left;	pt[0].y = rect.top;
	pt[1].x = rect.right;	pt[1].y = rect.bottom;
	Matrix.Transform(pt, 2);

	int OldROP2 = ::SetROP2(hDC, R2_NOT);
	::MoveToEx(hDC, pt[0].x, pt[0].y, NULL);
	::LineTo(hDC, pt[1].x, pt[1].y);
	::SetROP2(hDC, OldROP2);
}

//////////////////////////////////////////////////////////////////////
void CAGText::DrawSelection(HDC hDC, const CAGMatrix& Matrix, const RECT& rect)
{
	if (m_bAutoSelect)
		return;

	POINT pt[5];
	pt[0].x = rect.left;	pt[0].y = rect.top;
	pt[1].x = rect.right;	pt[1].y = rect.top;
	pt[2].x = rect.right;	pt[2].y = rect.bottom;
	pt[3].x = rect.left;	pt[3].y = rect.bottom;
	pt[4].x = rect.left;	pt[4].y = rect.top;
	Matrix.Transform(pt, 5);

	CAGBrush Brush(BLACK_BRUSH, eStock);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, Brush);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, ::GetStockObject(NULL_PEN));
	int OldROP2 = ::SetROP2(hDC, R2_NOT);
	::Polygon(hDC, pt, 5);
	::SetROP2(hDC, OldROP2);
	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
}

//////////////////////////////////////////////////////////////////////
void CAGText::DrawHilite(const scXRect& rRect, APPDrwCtx pTextCtx, BOOL bSliverCursor)
{
	CAGText* pText = (CAGText*)pTextCtx;
	if (pText)
		pText->DrawHilite(rRect, bSliverCursor);
}

//////////////////////////////////////////////////////////////////////
void CAGText::DrawHilite(const scXRect& rRect, BOOL bSliverCursor)
{
	if (!m_pDC)
		return;

	HDC hDC = (m_hPaintDC ? m_hPaintDC : m_pDC->GetRawHDC());
	if (!hDC)
		return;

	if (!m_pSym)
		return;

	RECT rect = { rRect.x1, rRect.y1, rRect.x2, rRect.y2 };
	RECT DestRect = GetDestRect();
	::OffsetRect(&rect, DestRect.left, DestRect.top);

	if (WIDTH(rect) < 0 || HEIGHT(rect) < 0)
		return;

	// Draw directly to the screen
	CAGMatrix MatrixSaved = m_pDC->SetSymbolMatrix(m_pSym->GetMatrix());
	CAGMatrix Matrix = m_pDC->GetSymbolToDeviceMatrix();

	if (!WIDTH(rect))
	{
		rect.right++;
		::IntersectRect(&rect, &rect, &DestRect);
		rect.right = rect.left;
		DrawCaret(hDC, Matrix, rect);
	}
	else
	{
		::IntersectRect(&rect, &rect, &DestRect);
		DrawSelection(hDC, Matrix, rect);
	}

	m_pDC->SetSymbolMatrix(MatrixSaved);
}

//////////////////////////////////////////////////////////////////////
void CAGText::StartEdit(CAGDC* pDC, CAGSym* pSym)
{
	if (!pDC)
		return;

	if (m_bTempDCInUse) // if we are in the middle of drawing, update the saved DC
		m_pSavedDC = pDC;
	else
		m_pDC = pDC;

	m_pSym = pSym;
	m_bOverstrike = false;
	m_LastMousePoint.Invalidate();

	if (IsEmpty() || !m_pSelection)
	{
		TypeSpec ts(NULL/*pSpec*/);
		SCCOL_InitialSelect(m_pColumn, ts, m_pSelection);
		ShowSelection(true);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::EndEdit()
{
	ShowSelection(false);
	m_pSelection = NULL;

	if (m_bTempDCInUse) // if we are in the middle of drawing, update the saved DC
		m_pSavedDC = NULL;
	else
		m_pDC = NULL;

	m_pSym = NULL;
	m_bOverstrike = false;
	m_LastMousePoint.Invalidate();
}

//////////////////////////////////////////////////////////////////////
void CAGText::SaveSelection()
{
	if (!m_pSelection)
		return;

	SCSEL_Decompose2(m_pSelection, m_SavedSelectionMark, m_SavedSelectionPoint);
}

//////////////////////////////////////////////////////////////////////
void CAGText::RestoreSelection()
{
	if (!m_pSelection)
		return;

	ShowSelection(false);
	
	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (pStream)
		SCSEL_Restore(pStream, m_SavedSelectionMark, m_SavedSelectionPoint, m_pSelection, false);

	ShowSelection(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SelectAll(bool bShow)
{
	if (bShow)
		ShowSelection(false);
	scMuPoint muPt(0, 0);
	SCCOL_SelectSpecial(m_pColumn, muPt, eAllSelect, m_pSelection);
	if (bShow)
		ShowSelection(true);
}

#ifdef CHAR_UNDO_UNTESTED //j
	static scKeyRecord keyRecUndo;
	static scScrapPtr pScrapUndo;
#endif CHAR_UNDO_UNTESTED //j

//////////////////////////////////////////////////////////////////////
bool CAGText::Delete()
{
	scKeyRecord keyRec;
	keyRec.keycode() = (UCS2)scForwardDelete;

#ifdef CHAR_UNDO_UNTESTED //j
	if (pScrapUndo)
	{
		SCSCR_Free(pScrapUndo);
		pScrapUndo = NULL;
	}

	if (m_pSelection && !m_pSelection->IsSliverCursor())
		SCSEL_CopyText(m_pSelection, pScrapUndo);
#endif CHAR_UNDO_UNTESTED //j

	bool bOverflow = false;
	scRedispList colRedisp;
	SCSEL_InsertKeyRecords(m_pSelection, 1, &keyRec, &bOverflow, &colRedisp);
	Redisplay(colRedisp);
	SetDirty(true);

#ifdef CHAR_UNDO_UNTESTED //j
	// Copy the key record and invert it so we can perform text undo
	keyRecUndo = keyRec;
	keyRecUndo.Invert();
#endif CHAR_UNDO_UNTESTED //j

	if (bOverflow)
		SCCOL_CheckOverflow(m_pColumn, &bOverflow);

	return bOverflow;
}

//////////////////////////////////////////////////////////////////////
HANDLE CAGText::CopyText(bool bCut)
{
	bool bCaretShowing = (m_pSelection && m_pSelection->IsSliverCursor());
	if (!m_pSelection || bCaretShowing)
	{
		m_bAutoSelect = true;
		SaveSelection();
		SelectAll(false/*bShow*/);
	}

	scScrapPtr pScrap = NULL;
	SCSEL_CopyText(m_pSelection, pScrap);

	if (m_bAutoSelect)
	{
		RestoreSelection();
		m_bAutoSelect = false;
	}

	if (!pScrap)
		return NULL;

	SystemMemoryObject ScrapMemory;
	SCSCR_ConToSys(pScrap, ScrapMemory);
	SCSCR_Free(pScrap);

	BYTE* pScrapData = (BYTE*)ScrapMemory.LockHandle();
	long dwSize = ScrapMemory.HandleSize();
	HANDLE hMemory = ::GlobalAlloc(GPTR, dwSize);
	if (hMemory)
	{
		BYTE* pMemory = (BYTE*)::GlobalLock(hMemory);
		if (pMemory)
			memcpy(pMemory, pScrapData, dwSize);
		::GlobalUnlock(hMemory);
	}

	ScrapMemory.UnlockHandle();

	if (bCut)
		Delete();

	return hMemory;
}

//////////////////////////////////////////////////////////////////////
void CAGText::PasteText(char* pRawText, CAGSpec* pAGSpec)
{
	if (!pRawText)
		return;

	if (!pAGSpec)
		return;

	if (!m_pSelection)
		return;

	scScrapPtr pScrap = NULL;
	TypeSpec ts(pAGSpec);
	SCSCR_SysToCon(pScrap, pRawText, ts);
	if (!pScrap)
		return;

	// Edit operation coming up - undraw the selection before the edit
	ShowSelection(false);

	scRedispList colRedisp;
	SCSEL_PasteText(m_pSelection, pScrap, ts, &colRedisp);
	MoveSelection(eNextChar);
	MoveSelection(ePrevChar);
	Redisplay(colRedisp);
	SCSCR_Free(pScrap);
	SetDirty(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::GetFonts(LOGFONTLIST& lfList)
{
	scTypeSpecList TSList;
	SCCOL_TSList(m_pColumn, TSList);

	int nNumItems = TSList.NumItems();
	for (int i = 0; i < nNumItems; i++)
	{ // Loop through all of the items and get each logical font
		LOGFONT lf = ((CAGSpec*)TSList[i].ptr())->m_Font;

		int nFonts = lfList.size();
		for (int j = 0; j < nFonts; j++)
		{ // Check to see if the logical font is already in the font list
			if (!lstrcmp(lf.lfFaceName, lfList[j].lfFaceName) &&
				lf.lfWeight == lfList[j].lfWeight &&
				!lf.lfItalic == !lfList[j].lfItalic)
				break;
		}

		// If it's not in the list, add it
		if (j >= nFonts)
			lfList.push_back(lf);
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::GetSelParaTSList(scTypeSpecList& tsList) const
{
	SCSEL_PARATSList(m_pSelection, tsList);
}

//////////////////////////////////////////////////////////////////////
void CAGText::GetSelTSList(scTypeSpecList& tsList) const
{
	if (m_pSelection)
		SCSEL_TSList(m_pSelection, tsList);
}

//////////////////////////////////////////////////////////////////////
eVertJust CAGText::GetVertJust() const
{
	if (!m_pColumn)
		return eVertTop;

	return m_pColumn->GetVertJust();
}

//////////////////////////////////////////////////////////////////////
bool CAGText::IsEmpty() const
{
	if (!m_pColumn)
		return true;

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return true;

	long lCount = 0;
	SCSTR_ChCount(pStream, lCount);
	return (lCount <= 0);
}

#define MIN_TEXTSTRING 4096
#define DELIMITER (128+' ')
static BYTE* m_pTextString;
static int m_iTextStringReadPos;
static int m_iTextStringLength;
static int m_iTextStringBufferLength;

/////////////////////////////////////////////////////////////////////////////
// Copy each word from the symbol into a single text string
static status GetWordsFromSymbol(UCS2** ppOutWord, UCS2* pInWord, scSelection* pSelection)
{
	if (!ppOutWord || !pInWord)
		return scUserAbort;
		
	// Convert each word from the Stonehand internal format (short's) to the app format (BYTE's)
	BYTE WordBuffer[MAX_PATH];
	BYTE* pTextString = WordBuffer;
	UCS2 c2 = NULL;
	int iLength = 0;
	while (c2 = *pInWord++)
	{
		*pTextString++ = (BYTE)CMctToAPP(c2);
		iLength++;
	}

	*pTextString++ = DELIMITER;
	iLength++;
	*pTextString++ = '\0';
	iLength++;

	if (!m_pTextString)
	{
		m_iTextStringBufferLength = MIN_TEXTSTRING;
		m_pTextString = (BYTE*)malloc(m_iTextStringBufferLength);
		if (!m_pTextString)
			return scUserAbort;
	}

	if (m_iTextStringLength + iLength >= m_iTextStringBufferLength)
	{
		m_iTextStringBufferLength += MIN_TEXTSTRING;
		m_pTextString = (BYTE*)realloc(m_pTextString, m_iTextStringBufferLength);
	}

	memcpy(m_pTextString + m_iTextStringLength, WordBuffer, iLength);
	m_iTextStringLength += (iLength - 1);

	return scNoAction; // no change is necessary
}

/////////////////////////////////////////////////////////////////////////////
// Copy each word from the text string back into the symbol (only if it has changed)
static status PutWordsBackIntoSymbol(UCS2** ppOutWord, UCS2* pInWord, scSelection* pSelection)
{
	if (!ppOutWord || !pInWord)
		return scUserAbort;
		
	// Convert each word in the text string from the app format (BYTE's) to the Stonehand format (short's)
	// Compare characters with the symbols's original word as we convert
	bool bChanged = false;
	static UCS2 WordBuffer[MAX_PATH];
	UCS2* pOut = WordBuffer;
	BYTE* pTextString = m_pTextString + m_iTextStringReadPos;
	BYTE c1 = NULL;
	while (c1 = *pTextString++)
	{
		m_iTextStringReadPos++;
		if (c1 == DELIMITER)
			break;
		UCS2 c2 = CMappToCT(c1);
		*pOut++ = c2;
		if (!(*pInWord))
			bChanged = true; // ran out of BYTE's to compare against
		else
		if (c2 != *pInWord++)
			bChanged = true; // did not compare
	}

	*pOut++ = '\0';

	if (*pInWord)
		bChanged = true; // still have BYTE's left

	if (bChanged)
	{
		*ppOutWord = WordBuffer;
		return scSuccess; // the word has been changed
	}

	*ppOutWord = NULL;
	return scNoAction; // no change is necessary
}

//////////////////////////////////////////////////////////////////////
CString CAGText::ExtractText()
{
	HANDLE hMemory = CopyText(false/*bCut*/);
	if (!hMemory)
		return "";

	void* pMemory = ::GlobalLock(hMemory);
	DWORD dwSize = ::GlobalSize(hMemory);

	CString strText;
	if (pMemory && dwSize > 0)
	{
		#define MAX_TEXTSTRING 4096
		BYTE TextString[MAX_TEXTSTRING];
		memcpy(TextString, pMemory, min(dwSize, MAX_TEXTSTRING-1));
		TextString[dwSize] = '\0';
		strText = TextString;
	}

	::GlobalUnlock(hMemory);
	::GlobalFree(hMemory);
	return strText;
}

//////////////////////////////////////////////////////////////////////
bool CAGText::SpellCheck(CSpell& Spell, DWORD* pdwWordsChecked, DWORD* pdwWordsChanged, DWORD* pdwErrorsDetected, bool bCheckSelection)
{
	if (m_pTextString)
	{
		free(m_pTextString);
		m_pTextString = NULL;
		m_iTextStringBufferLength = 0;
	}
	m_iTextStringLength = 0;
	m_iTextStringReadPos = 0;

	// Iterate over the words in the symbol just to gather them up into a text string
	scRedispList colRedisp;
	status stat = scUserAbort;
	if (bCheckSelection)
	{
		if (m_pSelection && !m_pSelection->IsSliverCursor())
			stat = SCSEL_Iter(m_pSelection, GetWordsFromSymbol, &colRedisp);
	}
	else
	{
		scStream* pStream = NULL;
		SCCOL_GetStream(m_pColumn, pStream);
		if (pStream)
			stat = SCSTR_Iter(pStream, GetWordsFromSymbol, &colRedisp);
	}

	// Make sure the string buffer is 25% larger for error correction
	int iMaxLength = m_iTextStringLength + (m_iTextStringLength/4);
	if (iMaxLength >= m_iTextStringBufferLength)
	{
		m_iTextStringBufferLength = iMaxLength;
		m_pTextString = (BYTE*)realloc(m_pTextString, m_iTextStringBufferLength);
	}

	bool bAborted = Spell.SpellCheck(m_pTextString, m_iTextStringLength, m_iTextStringBufferLength, pdwWordsChecked, pdwWordsChanged, pdwErrorsDetected);
	if (bAborted || !*pdwWordsChanged)
		return bAborted;

	// Edit operation coming up - undraw the selection before the edit
	ShowSelection(false);

	// Iterate over the words in the symbol a 2nd time to see if the spell checker changed them
	stat = scUserAbort;
	if (bCheckSelection)
	{
		if (m_pSelection && !m_pSelection->IsSliverCursor())
			stat = SCSEL_Iter(m_pSelection, PutWordsBackIntoSymbol, &colRedisp);
	}
	else
	{
		scStream* pStream = NULL;
		SCCOL_GetStream(m_pColumn, pStream);
		if (pStream)
			stat = SCSTR_Iter(pStream, PutWordsBackIntoSymbol, &colRedisp);
	}

	return (stat == scUserAbort); // bAborted
}

//////////////////////////////////////////////////////////////////////
bool CAGText::OnChar(UINT nChar)
{
	bool bSizeChange = false;
	if (!m_pSelection || LBUTTON)
		return bSizeChange;

	// Edit operation coming up - undraw the selection before the edit
	ShowSelection(false);

	scKeyRecord keyRec;

#ifdef CHAR_UNDO_UNTESTED //j
	if (nChar == UNDO_CHAR)
	{
		keyRec = keyRecUndo;

		if (pScrapUndo)
		{
			scRedispList colRedisp;
			TypeSpec& ts = m_pSelection->fMark.fPara->SpecAtOffset(m_pSelection->fMark.fOffset);
			SCSEL_PasteText(m_pSelection, pScrapUndo, ts, &colRedisp);
			Redisplay(colRedisp);
		}
	}
#endif CHAR_UNDO_UNTESTED //j

	keyRec.type() = (m_bOverstrike ? scKeyRecord::overstrike : scKeyRecord::insert);
	keyRec.keycode() = CMappToCT(nChar);
	TypeSpec NullSpec;
	keyRec.spec() = NullSpec;

#ifdef NOTUSED //j Add this code back in to make hard returns soft, and vice versa
	if (keyRec.keycode() == scParaSplit)
		keyRec.keycode() = scHardReturn;
	else
	if (keyRec.keycode() == scHardReturn)
		keyRec.keycode() = scParaSplit;
#endif NOTUSED //j

#ifdef CHAR_UNDO_UNTESTED //j
	if (pScrapUndo)
	{
		SCSCR_Free(pScrapUndo);
		pScrapUndo = NULL;
	}
	if (m_pSelection && !m_pSelection->IsSliverCursor())
		SCSEL_CopyText(m_pSelection, pScrapUndo);
#endif CHAR_UNDO_UNTESTED //j

	bool bOverflow = false;
	scRedispList colRedisp;
	SCSEL_InsertKeyRecords(m_pSelection, 1, &keyRec, &bOverflow, &colRedisp);
	Redisplay(colRedisp);
	SetDirty(true);

#ifdef CHAR_UNDO_UNTESTED //j
	// Copy the key record and invert it so we can perform text undo
	keyRecUndo = keyRec;
	keyRecUndo.Invert();
#endif CHAR_UNDO_UNTESTED //j

	int iMaxCount = 100;
	while (bOverflow)
	{
		if (--iMaxCount <= 0)
			break;

		// Handle the overflow by changing the size of the symbol
		int nLines = 0;
		SCCOL_GetNumberLines(m_pColumn, &nLines);
		RECT Rect = GetDestRect();
		int nHeight = Rect.bottom - Rect.top;
		if (!nLines || !nHeight)
			break;

		Rect.bottom += nHeight / nLines;
		SetDestRect(Rect);
		bSizeChange = true;

		// Check again
		SCCOL_CheckOverflow(m_pColumn, &bOverflow);
	}

	return bSizeChange;
}

//////////////////////////////////////////////////////////////////////
bool CAGText::OnKeyDown(UINT nChar)
{
	if (!m_pSelection || LBUTTON)
		return false;

	switch (nChar)
	{
		case VK_INSERT:
			m_bOverstrike = !m_bOverstrike;
			break;

		case VK_DELETE:
		{
			Delete();
			break;
		}

		case VK_LEFT:
		{
			if (SHIFT)
				ExtendSelection(CONTROL ? ePrevWord : ePrevChar);
			else
				MoveSelection(CONTROL ? ePrevWord : ePrevChar);
			break;
		}
		case VK_RIGHT:
		{
			if (SHIFT)
				ExtendSelection(CONTROL ? eNextWord : eNextChar);
			else
				MoveSelection(CONTROL ? eNextWord : eNextChar);
			break;
		}
		case VK_UP:
		{
			if (SHIFT)
				ExtendSelection(ePrevLine);
			else
				MoveSelection(ePrevLine);
			break;
		}
		case VK_DOWN:
			if (SHIFT)
				ExtendSelection(eNextLine);
			else
				MoveSelection(eNextLine);
			break;

		case VK_HOME:
		{
			if (SHIFT)
				ExtendSelection(CONTROL ? eBeginColumn : eStartLine);
			else
				MoveSelection(CONTROL ? eBeginColumn : eStartLine);
			break;
		}
		case VK_END:
		{
			if (SHIFT)
				ExtendSelection(CONTROL ? eEndColumn : eEndLine);
			else
				MoveSelection(CONTROL ? eEndColumn : eEndLine);
			break;
		}
		
		case VK_PRIOR:
		{
			if (SHIFT)
				ExtendSelection(eBeginColumn);
			else
				MoveSelection(eBeginColumn);
			break;
		}
		case VK_NEXT:
		{
			if (SHIFT)
				ExtendSelection(eEndColumn);
			else
				MoveSelection(eEndColumn);
			break;
		}
		default:
			return false; // don't update the user interface
	}

	return true; // update the user interface
}

//////////////////////////////////////////////////////////////////////
void CAGText::OnKeyUp(UINT nChar)
{
}

//////////////////////////////////////////////////////////////////////
void CAGText::OnLButtonDblClk(POINT Point)
{
	if (!m_pSelection)
		return;

	scMuPoint muPt(Point.x - m_DestRect.left, Point.y - m_DestRect.top);
	ShowSelection(false);
	SCCOL_SelectSpecial(m_pColumn, muPt, eWordSelect, m_pSelection);
	ShowSelection(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::OnLButtonDown(POINT Point)
{
	// m_pSelection will be set here
	scMuPoint muPt(Point.x - m_DestRect.left, Point.y - m_DestRect.top);

//j	if (m_bSelectionIsShown && !m_bAllowBlinkCaret && SHIFT)
	if (m_bSelectionIsShown && SHIFT)
		SCCOL_ExtendSelect(m_pColumn, muPt, DrawHilite, this, m_pSelection);
	else
	{
		ShowSelection(false);
		SCCOL_StartSelect(m_pColumn, muPt, DrawHilite, this, m_pSelection);
		m_bSelectionIsShown = true;
		m_bAllowBlinkCaret = true;
	}

	m_LastMousePoint = muPt;
}

//////////////////////////////////////////////////////////////////////
bool CAGText::OnLButtonUp(POINT Point)
{
	if (!m_pSelection)
		return false; // don't update the user interface

	m_LastMousePoint.Invalidate();
	return true; // update the user interface
}

//////////////////////////////////////////////////////////////////////
void CAGText::OnMouseMove(POINT Point)
{
	if (!m_pSelection || !LBUTTON)
		return;

	// m_LastMousePoint must first be set by an LButonDown
	if (m_LastMousePoint.x == kInvalMP && m_LastMousePoint.y == kInvalMP)
		return;

	scMuPoint muPt(Point.x - m_DestRect.left, Point.y - m_DestRect.top);

	if (muPt.x < 0)
		muPt.x = 0;
	if (muPt.x > WIDTH(m_DestRect))
		muPt.x = WIDTH(m_DestRect);
	if (muPt.y < 0)
		muPt.y = 0;
	if (muPt.y > HEIGHT(m_DestRect))
		muPt.y = HEIGHT(m_DestRect);

	if (m_LastMousePoint != muPt)
	{
		SCCOL_ExtendSelect(m_pColumn, muPt, DrawHilite, this, m_pSelection);
		m_LastMousePoint = muPt;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::ReadColumn(CAGDocIO* pInput)
{
	if (m_pColumn)
		SCCOL_Delete(m_pColumn, 0);

	m_TSList.RemoveAll();

	m_pColumn = NULL;
	m_pSelection = NULL;

	m_pDocIO = pInput;
	pInput->Read(&m_DestRect, sizeof(m_DestRect));

	int nNumItems = 0;
	pInput->Read(&nNumItems, sizeof(nNumItems));

	for (int i = 0; i < nNumItems; i++)
	{
		CAGSpec* pAGSpec = new CAGSpec(); 
		pAGSpec->Read(pInput);

		TypeSpec ts(pAGSpec);
		m_TSList.Set(i, ts);
	}

	scSet* enumTable = NULL;
	SCSET_InitRead(enumTable, 100);
	SCCOL_Read(this, m_pColumn, enumTable, this, ReadFunc);
	SCOBJ_PtrRestore((scTBObj*)m_pColumn, enumTable);
	SCSET_FiniRead(enumTable, 0);

	eVertJust vj = m_pColumn->GetVertJust();
	if ((vj < eVertCentered) || (vj > eVertBottom))
		m_pColumn->SetVertJust(eVertTop);

	m_pDocIO = NULL;
}

#define SPEC_MARKER 0x01

//////////////////////////////////////////////////////////////////////
static int FindSpecMarker(CString& strMarkerText, int iStartIndex)
{
	int iLength = strMarkerText.GetLength();
	if (iStartIndex < 0) iStartIndex = 0;
	for (int i = iStartIndex; i < iLength; i++)
	{
		if (strMarkerText.GetAt(i) == SPEC_MARKER)
			return i;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
static BYTE GetAt(const CString& strText, int iIndex)
{
	if (iIndex < 0 || iIndex > strText.GetLength())
		return 0;

	return strText.GetAt(iIndex);
}

//////////////////////////////////////////////////////////////////////
void CAGText::GetSpecStructArray(SpecArray& cSpecArray)
{
	CString strText = ExtractText();
	CString strMarkerText = strText;

	// Generate the spec array and mark the text with the spec start positions
	GetSpecArray(cSpecArray, strMarkerText);

	// Now insert any soft line breaks to the text
	long nLines = 0;
	BOOL bMoreText = false;
	scLineInfoList cLineArray;
	SCCOL_LinePositions(m_pColumn, &cLineArray, nLines, bMoreText);

	int iCharOffset = 0;
	long lLastLineEnd = 0;
	for (int i = 0; i < nLines; i++)
	{
		scLineInfo lineInfo;
		cLineArray.GetDataAt(i, (ElementPtr)&lineInfo);
		const scTextline* pTextLine = lineInfo.fLineID;
		long lCharCount = pTextLine->GetCharCount();
		long lLineStart = pTextLine->GetStartOffset();
		if (lLineStart == 0) // If the line offsets start over, offset everything
			iCharOffset = lLastLineEnd;
		lLineStart += iCharOffset;
		lLastLineEnd = lLineStart + lCharCount;
		long lNextChar = lLastLineEnd;
		long lLastChar = lLastLineEnd - 1;
		bool bHardBreak = (GetAt(strText, lLastChar) == '\n');
		bool bExtraBreak = (GetAt(strText, lNextChar) == '\r');
		if (bExtraBreak)
		{
			lCharCount++;
			iCharOffset++;
			lLastLineEnd++;
		}

		if (bHardBreak || bExtraBreak || !lCharCount)
			continue;

		// No soft breaks necessary on the last line
		int iLength = strText.GetLength();
		if ((i == nLines-1) && (iLength == lLastLineEnd) || (i >= nLines))
			break;

		// A soft break is necessary; insert it into the text string
		CString strSoftBreak = "%_SLB_%";
		int iOffset = strSoftBreak.GetLength();
		int iNewLength = strText.Insert(lLastLineEnd, strSoftBreak);
		ATLASSERT(iNewLength == iLength + iOffset);
		iLength = strMarkerText.GetLength();
		iNewLength = strMarkerText.Insert(lLastLineEnd, strSoftBreak);
		ATLASSERT(iNewLength == iLength + iOffset);
		iCharOffset += iOffset;
		lLastLineEnd += iOffset;
	}

	// Now break apart the text on spec boundaries and put the pieces into the spec array
	int iStartIndex = FindSpecMarker(strMarkerText, 0/*iStartIndex*/);
	if (iStartIndex < 0)
		iStartIndex = 0; // Should never happen

	int nSpecs = cSpecArray.GetSize();
	for (i = 0; i < nSpecs; i++)
	{
		int iNextIndex = FindSpecMarker(strMarkerText, iStartIndex + 1);
		if (iNextIndex < 0)
			iNextIndex = strMarkerText.GetLength();

		// Extract the spec text
		CString strSpecText = strText.Mid(iStartIndex, iNextIndex - iStartIndex);
		strSpecText.Replace('\r', '\n');
		cSpecArray[i].strTxt = strSpecText;
		iStartIndex = iNextIndex;
	}
}

//////////////////////////////////////////////////////////////////////
static eTSJust GetParaHorzJust(scSpecLocList& pslist, int iPara)
{
	long iSize = pslist.NumItems();
	if (iPara < 0 || iPara >= iSize)
		return eFlushLeft;

	scSpecLocation& SpecLocation = pslist[iPara];
	scParaOffset& offset = SpecLocation.offset();
	TypeSpec& ts = SpecLocation.spec();
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	if (!pSpec)
		return eFlushLeft;

	return pSpec->m_HorzJust;
}

//////////////////////////////////////////////////////////////////////
void CAGText::GetSpecArray(SpecArray& cSpecArray, CString& strMarkerText)
{
	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return;

	scSpecLocList pslist(pStream);
	SCSEL_PARATSList(m_pSelection, pslist);

	scSpecLocList cslist(pStream);
	SCSEL_CHTSList(m_pSelection, cslist);

	// Loop thru all the specs, make copies of the unique ones, and add them to the passed array
	CAGSpec* pSpecLastAdded = NULL;
	int iBigOffset = 0;
	int iLastParaOffset = 0;
	int iLastCharOffset = 0;
	int nSpecs = cslist.NumItems();
	for (int i = 0; i < nSpecs; i++)
	{
		scSpecLocation& SpecLocation = cslist[i];
		scParaOffset& offset = SpecLocation.offset();
		TypeSpec& ts = SpecLocation.spec();
		CAGSpec* pSpec = (CAGSpec*)ts.ptr();

		// We compute the character offset utilizing the paragraph offset
		if (iLastParaOffset != offset.fParaOffset)
			iBigOffset += iLastCharOffset + 1;
		iLastParaOffset = offset.fParaOffset;
		iLastCharOffset = offset.fCharOffset;
		int iCharOffset = iLastCharOffset + iBigOffset;
		
		if (!pSpec)
			continue;

		eTSJust HorzJust = GetParaHorzJust(pslist, offset.fParaOffset);
		bool bSpecMatch = (!pSpecLastAdded ? false : (*pSpecLastAdded == *pSpec));
		if (bSpecMatch && pSpecLastAdded && pSpecLastAdded->m_HorzJust == HorzJust)
			continue;

		// We found a unique spec, so add a copy to the list
		SPECSTRUCT cSpec;
		cSpec.pSpec = new CAGSpec(*pSpec);
		cSpec.pSpec->m_HorzJust = HorzJust;

		// Place a mark on the text character where the spec starts so we can find it later
		if (!strMarkerText.IsEmpty())
		{
			if (iCharOffset >= 0 && iCharOffset < strMarkerText.GetLength())
				strMarkerText.SetAt(iCharOffset, SPEC_MARKER);
		}

		cSpecArray.Add(cSpec);
		pSpecLastAdded = cSpec.pSpec;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::SpecChanger(FNSPECCALLBACK fnSpecCallback, CAGSpec* pNewSpecTemplate)
{
	if (!fnSpecCallback)
		return;

	if (!m_pSelection)
		return;

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return;

	bool bCaretShowing = (m_pSelection && m_pSelection->IsSliverCursor());
	if (bCaretShowing)
	{
		m_bAutoSelect = true;
		SaveSelection();
		SelectAll(false/*bShow*/);
	}
	else
	{
		// Edit operation coming up - undraw the selection before the edit
		ShowSelection(false);
	}

	scSpecLocList cslist(pStream);
	SCSEL_CHTSList(m_pSelection, cslist);

	for (int i = 0; i < cslist.NumItems(); i++)
	{
		scSpecLocation& SpecLocation = cslist[i];
		scParaOffset& offset = SpecLocation.offset();
		TypeSpec& ts = SpecLocation.spec();
		CAGSpec* pSpec = (CAGSpec*)ts.ptr();
		if (pSpec)
		{
			CAGSpec* pNewSpec = new CAGSpec(*pSpec);
			bool bOK = fnSpecCallback(pNewSpec, pNewSpecTemplate);
			if (bOK)
				ts = pNewSpec;
			else
				delete pNewSpec;
		}
	}

	scRedispList colRedisp;
	SCSTR_CHTSListSet(pStream, cslist, &colRedisp);
	Redisplay(colRedisp);
	SetDirty(true);
		
	if (m_bAutoSelect)
	{
		RestoreSelection();
		m_bAutoSelect = false;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetFillType(FillType Fill)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_FillType = Fill;
	SpecChanger(CAGSpec::CallbackChangeFillType, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetFillColor(COLORREF Color)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_FillColor = Color;
	SpecChanger(CAGSpec::CallbackChangeFillColor, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetFillColor2(COLORREF Color)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_FillColor2 = Color;
	SpecChanger(CAGSpec::CallbackChangeFillColor2, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetLineWidth(int LineWidth)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_LineWidth = LineWidth;
	SpecChanger(CAGSpec::CallbackChangeLineWidth, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetLineColor(COLORREF Color)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_LineColor = Color;
	SpecChanger(CAGSpec::CallbackChangeLineColor, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetTextSize(int TextSize, double fScale)
{
	CAGSpec NewSpecTemplate;
	int iUnits = (TextSize ? TextSize : dtoi(fScale * abs(NewSpecTemplate.m_Font.lfHeight)));
	NewSpecTemplate.m_Font.lfHeight = -iUnits;
	SpecChanger(CAGSpec::CallbackChangeTextSize, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetTypeface(const LOGFONT& Font)
{
	CAGSpec NewSpecTemplate;
	lstrcpy(NewSpecTemplate.m_Font.lfFaceName, Font.lfFaceName);
	NewSpecTemplate.m_Font.lfWeight = Font.lfWeight;
	NewSpecTemplate.m_Font.lfItalic = (Font.lfItalic != 0);
	NewSpecTemplate.m_Font.lfCharSet = Font.lfCharSet;
	NewSpecTemplate.m_Font.lfOutPrecision = Font.lfOutPrecision;
	NewSpecTemplate.m_Font.lfClipPrecision = Font.lfClipPrecision;
	NewSpecTemplate.m_Font.lfQuality = Font.lfQuality;
	NewSpecTemplate.m_Font.lfPitchAndFamily = Font.lfPitchAndFamily;
	SpecChanger(CAGSpec::CallbackChangeTypeface, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetEmphasis(bool bBold, bool bItalic, bool bUnderline)
{
	CAGSpec NewSpecTemplate;
	NewSpecTemplate.m_Font.lfWeight = (bBold ? FW_BOLD : FW_NORMAL);
	NewSpecTemplate.m_Font.lfItalic = bItalic;
	NewSpecTemplate.m_Font.lfUnderline = bUnderline;
	SpecChanger(CAGSpec::CallbackChangeEmphasis, &NewSpecTemplate);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetHorzJust(eTSJust HorzJust)
{
	if (!m_pSelection)
		return;

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return;

	bool bCaretShowing = (m_pSelection && m_pSelection->IsSliverCursor());
	if (bCaretShowing)
	{
		m_bAutoSelect = true;
		SaveSelection();
		SelectAll(false/*bShow*/);
	}
	else
	{
		// Edit operation coming up - undraw the selection before the edit
		ShowSelection(false);
	}

	scSpecLocList cslist(pStream);
	SCSEL_PARATSList(m_pSelection, cslist);

	for (int i = 0; i < cslist.NumItems(); i++)
	{
		scSpecLocation& SpecLocation = cslist[i];
		scParaOffset& offset = SpecLocation.offset();
		TypeSpec& ts = SpecLocation.spec();
		CAGSpec* pSpec = (CAGSpec*)ts.ptr();
		if (pSpec)
		{
			CAGSpec* pNewSpec = new CAGSpec(*pSpec);
			pNewSpec->m_HorzJust = HorzJust;
			ts = pNewSpec;
		}
	}

	scRedispList colRedisp;
	SCSTR_PARATSListSet(pStream, cslist, &colRedisp);
	Redisplay(colRedisp);
	SetDirty(true);

	if (m_bAutoSelect)
	{
		RestoreSelection();
		m_bAutoSelect = false;
	}
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetVertJust(eVertJust VertJust)
{
	// Edit operation coming up - undraw the selection before the edit
	ShowSelection(false);

	SCCOL_FlowJustify(m_pColumn, VertJust);
	scRedispList colRedisp;
	SCFS_Recompose(m_pColumn, &colRedisp);
	Redisplay(colRedisp);
	SetDirty(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::SetText(const char* pText, int nSpecs, CAGSpec** pAGSpecs, const int* pTextOffsets)
{
	stTextImportExport* pAppText = NULL;
	SCAPPTXT_Alloc(pAppText);
	if (!pAppText)
		return;

	int nChars = lstrlen(pText);
	bool bStartPara = true;
	for (int i = 0; i < nSpecs; i++)
	{
		CAGSpec* pAGSpec = pAGSpecs[i];
		TypeSpec ts(pAGSpec);

		if (bStartPara)
		{
			pAppText->StartPara(ts);
			bStartPara = false;
		}

		int nTextBegin = pTextOffsets[i];
		int nTextEnd = (i + 1 < nSpecs ? pTextOffsets[i + 1] : nChars);

		while ((nTextEnd - nTextBegin) > 0)
		{
			int nPutCount = 0;
			for (int j = nTextBegin; j < nTextEnd; j++)
			{
				if (pText[j] == '\n')
				{
					bStartPara = true;
					break;
				}
				else
					nPutCount++;
			}

			if (nPutCount)
			{
				pAppText->PutString((const uchar*)(pText + nTextBegin), nPutCount, ts);
				nTextBegin += nPutCount;
			}

			if (bStartPara)
			{
				pAppText->StartPara(ts);
				bStartPara = false;
				nTextBegin++; // Caused by a '\n', so skip over it too
			}
		}
	}

	// Edit operation coming up - undraw the selection before the edit
	ShowSelection(false);

	pAppText->reset();
	SCFS_PasteAPPText(m_pColumn, *pAppText, 0);
	SCAPPTXT_Delete(pAppText);
}

//////////////////////////////////////////////////////////////////////
void CAGText::BlinkCaret()
{
	if (LBUTTON)
		return;

	if (!m_bAllowBlinkCaret)
		return;

	if (!m_pSelection)
		return;

	if (!m_pSelection->IsSliverCursor())
		return;

	SCSEL_Hilite(m_pSelection, DrawHilite);
	m_bSelectionIsShown = !m_bSelectionIsShown;
}

//////////////////////////////////////////////////////////////////////
void CAGText::ShowSelectionEx(HDC hDC, bool bCaretOnly, bool bShow, bool bDraw)
{
	if (bCaretOnly)
	{
		bool bCaretShowing = (m_pSelection && m_pSelection->IsSliverCursor());
		if (!bCaretShowing)
			return;
	}

	m_hPaintDC = hDC;
	ShowSelection(bShow);
	m_hPaintDC = NULL;
}

//////////////////////////////////////////////////////////////////////
void CAGText::ShowSelection(bool bShow, bool bDraw)
{
	if (!m_pSelection)
		return;

	// Temporarily stop the blinking caret
	m_bAllowBlinkCaret = false;

	if (bDraw && m_bSelectionIsShown != bShow)
		SCSEL_Hilite(m_pSelection, DrawHilite);

	m_bSelectionIsShown = bShow;
	m_bAllowBlinkCaret = bShow;
}

//////////////////////////////////////////////////////////////////////
void CAGText::ExtendSelection(eSelectMove movement)
{
	ShowSelection(false);
	SCSEL_Extend(m_pSelection, movement);
	UpdateSelection();
	ShowSelection(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::MoveSelection(eSelectMove movement)
{
	ShowSelection(false);
	SCSEL_Move(m_pSelection, movement);
	UpdateSelection();
	ShowSelection(true);
}

//////////////////////////////////////////////////////////////////////
void CAGText::UpdateSelection()
{
	scStreamLocation Mark, Point;
	SCSEL_Decompose2(m_pSelection, Mark, Point);

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return;

	SCSEL_Restore(pStream, Mark, Point, m_pSelection, false);
}

//////////////////////////////////////////////////////////////////////
void CAGText::WriteColumn(CAGDocIO* pOutput)
{
	if (!m_pColumn)
		return;

	m_pDocIO = pOutput;
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return;

	SCTSL_Alloc(m_pWriteTSList);
	SCSTR_ParaTSList(pStream, *m_pWriteTSList);
	SCSTR_TSList(pStream, *m_pWriteTSList);

	int nNumItems = m_pWriteTSList->NumItems();
	pOutput->Write(&nNumItems, sizeof(nNumItems));

	for (int i = 0; i < nNumItems; i++)
	{
		((CAGSpec*)((*m_pWriteTSList)[i].ptr()))->Write(pOutput);
	}

	long enumerate = 0;
	SCTB_ZeroEnumeration();
	SCOBJ_Enumerate((scTBObj*)m_pColumn, enumerate);
	SCCOL_Write(m_pColumn, this, WriteFunc);

	SCTSL_Delete(m_pWriteTSList);
	m_pWriteTSList = NULL;
	m_pDocIO = NULL;
}

//////////////////////////////////////////////////////////////////////
static long ReadFunc(APPCtxPtr pTextCtx, void* ptr, long size)
{
	CAGText* pText = (CAGText*)pTextCtx;
	CAGDocIO* pDocIO = pText->GetDocIO();
	if (pDocIO)
		pDocIO->Read(ptr, size);
	return size;
}

//////////////////////////////////////////////////////////////////////
static long WriteFunc(APPCtxPtr pTextCtx, void* ptr, long size)
{
	CAGText* pText = (CAGText*)pTextCtx;
	CAGDocIO* pDocIO = pText->GetDocIO();
	if (pDocIO)
		pDocIO->Write(ptr, size);
	return size;
}

//////////////////////////////////////////////////////////////////////
// Implementation of the Text processing callback
scTicks APPEventAvail(scProcType proctype)
{
	return 5000; // Text processing can take as much time as it wants
}

//////////////////////////////////////////////////////////////////////
void APPDrawStartLine(APPDrwCtx pTextCtx, MicroPoint xOrg, MicroPoint yOrg, const scXRect& inkExtents)
{
	CAGText* pText = (CAGText*)pTextCtx;
}

//////////////////////////////////////////////////////////////////////
void APPDrawEndLine(APPDrwCtx pTextCtx)
{
	CAGText* pText = (CAGText*)pTextCtx;
}

//////////////////////////////////////////////////////////////////////
void APPDrawRule(const scMuPoint& start, const scMuPoint& end, const scGlyphInfo& gi, APPDrwCtx pTextCtx)
{
	CAGText* pText = (CAGText*)pTextCtx;
}

//////////////////////////////////////////////////////////////////////
status APPDrawContext(APPColumn pTextCol, const scColumn* pColumn, APPDrwCtx& pTextCtx)
{
	CAGText* pText = (CAGText*)pTextCtx;
	pTextCtx = pTextCol;
	return scSuccess;
}

//////////////////////////////////////////////////////////////////////
APPSpec* APPDiskIDToPointer(APPCtxPtr pTextCtx, long index, stDiskidClass objclass)
{
	CAGText* pText = (CAGText*)pTextCtx;
	switch (objclass)
	{
		case diskidColumn:
			return (APPSpec*)pText;

		case diskidTypespec:
		{
			if (index <= 0)
				return NULL;

			const scTypeSpecList& TSList = pText->GetTSList();
			CAGSpec* pAGSpec = (CAGSpec*)(TSList[index - 1].ptr());
			return pAGSpec;
		}

		default:
			break;
	}

	return (APPSpec*)-1;
}

//////////////////////////////////////////////////////////////////////
long APPPointerToDiskID(APPCtxPtr pTextCtx, void* obj, stDiskidClass objclass)
{
	CAGText* pText = (CAGText*)pTextCtx;
	switch (objclass)
	{
		case diskidTypespec:
		{
			if (!obj)
				return 0;

			scTypeSpecList* pWriteTSList = pText->GetWriteTSList();
			if (!pWriteTSList)
				return 0;

			int nNumItems = pWriteTSList->NumItems();
			int nItem = -1;
			for (int i = 0; i < nNumItems; i++)
			{
				if ((*pWriteTSList)[i].ptr() == obj)
				{
					nItem = i + 1;
					break;
				}
			}
			return nItem;
		}

		default:
			break;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
void APPDrawString(APPDrwCtx pTextCtx, const scGlyphArray* glyphArray, short num, MicroPoint xOrg, MicroPoint yOrg, const scGlyphInfo& gi)
{
	CAGText* pText = (CAGText*)pTextCtx;
	if (!pText)
		return;

	CAGDC* pDC = pText->GetDC();
	if (!pDC)
		return;

	TCHAR* pString = new TCHAR[num+1];
	if (!pString)
		return;

	int* pWidths = new int[num];
	if (!pWidths)
	{
		delete [] pString;
		return;
	}

	for (int i = 0; i < num; i++)
	{
		pString[i] = LOBYTE(glyphArray->fGlyphID);
		pWidths[i] = glyphArray->hEscapement;
		glyphArray++;
	}

	pString[i] = '\0';

	CAGSpec* pSpec = (CAGSpec*)gi.typespec.ptr();
	RECT DestRect = pText->GetDestRect();
	int x = DestRect.left + xOrg;
	int y = DestRect.top + yOrg;

	pSpec->DrawText(pDC, pString, pWidths, x, y);

	delete [] pString;
	delete [] pWidths;
}

//////////////////////////////////////////////////////////////////////
GlyphSize FIgetDEVEscapement(const TypeSpec& fp, UCS2 ch)
{
	CAGSpec* pSpec = (CAGSpec*)fp.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(pSpec->m_Font, false/*bTransform*/);

	TCHAR tch = (TCHAR)ch;
	SIZE sizeExtent;
	TextIC.GetTextExtent(&tch, 1, &sizeExtent);
	return sizeExtent.cx;
}

//////////////////////////////////////////////////////////////////////
GlyphSize FIgetDEVKern(const TypeSpec& fp, UCS2 ch1, UCS2 ch2)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////
// It appears that these ink extents settings are only used by the toolbox
// for computing repaint rectangles and have nothing to do with composition
void FIgetDEVFontExtents(const TypeSpec& fp, scXRect& maxFont)
{
	CAGSpec* pSpec = (CAGSpec*)fp.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(pSpec->m_Font, false/*bTransform*/);
	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);

	int iMinA = 0;
	int iMaxW = 0;
	TextIC.GetABCWidths(tm.tmFirstChar, tm.tmLastChar, &iMinA, NULL, NULL, &iMaxW);

	maxFont.x1 = iMinA;
	maxFont.x2 = iMaxW;
	maxFont.y1 = tm.tmAscent;
	maxFont.y2 = -(tm.tmDescent + tm.tmExternalLeading + tm.tmInternalLeading);
}

//////////////////////////////////////////////////////////////////////
status TSTabInfo(TypeSpec& ps, TypeSpec& ts, scTabInfo& tabInfo, MicroPoint xPos, MicroPoint, long)
{
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	int nTabSize = one_inch / 4;
	tabInfo.xPosition = xPos + (nTabSize - (xPos % nTabSize));
	tabInfo.fillChar = ' ';
	tabInfo.fillCharAlign = eFCNormalAlign;
	tabInfo.tabAlign = eTBNoAlign;
	if (pSpec)
	{
		eTSJust hj = pSpec->m_HorzJust;
		if (hj == eFlushLeft)	tabInfo.tabAlign = eTBLeftAlign;	else
		if (hj == eFlushRight)	tabInfo.tabAlign = eTBRightAlign;	else
		if (hj == eRagCentered)	tabInfo.tabAlign = eTBCenterAlign;
	}

	return scSuccess;
}

//////////////////////////////////////////////////////////////////////
status TSfillCharInfo(TypeSpec& ts, UCS2& ch, eFCAlignment& , MicroPoint, MicroPoint, long)
{
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	return scSuccess;
}

//////////////////////////////////////////////////////////////////////
status TSGetStyle(TypeSpec& ts, scStyle& st)
{
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	if (!pSpec)
	{
		scStyle style("Times New Roman", 15);
		st = style;
		return scSuccess;
	}

	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(pSpec->m_Font, false/*bTransform*/);
	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);
	SIZE sizeSpace;
	TextIC.GetTextExtent(" ", 1, &sizeSpace);

	scStyle style(pSpec->m_Font.lfFaceName, tm.tmHeight);
	style.SetRag(pSpec->m_HorzJust);
	style.SetOptWord(sizeSpace.cx);
	style.SetMinWord(sizeSpace.cx);
	style.SetMaxWord(sizeSpace.cx);
	int iLineLead = (!pSpec->m_sLineLeadPct ? 0 : dtoi(((double)tm.tmHeight * pSpec->m_sLineLeadPct) / 1000.0));
	style.SetLineLead(iLineLead);
	style.SmallCapCorrection();

//j	bool bItalic = (pSpec->m_Font.lfItalic != 0);
//j	if (bItalic)
//j	{
//j		ATLTRACE("italic!\n");
//j		style.SetLeftBlockIndent(sizeSpace.cx);
//j		style.SetRightBlockIndent(sizeSpace.cx);
//j	}

//	STYLE PROPERTY REFERENCE

//	// CHARACTER LEVEL PROPERTIES
//	SetUnique( uint32 val ) //0;
//	style.SetFont( APPFont font ); //font; font from font catalog
//	style.SetEmphasis( eEmphasisType etype, BOOL allBits = false ); //enormal; font emphasis
//	style.SetRender( APPRender ar ) //0; any additional font rendering attributes
//	style.SetColor( APPColor pTextCol ) //0;
//	style.SetPtSize( GlyphSize ptsize ) //ptsize; vertical size
//	style.SetSetSize( GlyphSize setsize ) //ptsize; horizontal size
//	style.SetRotation( scAngle rotation ) //0; rotation about glyph center
//	style.SetHorzBaseline( eFntBaseline b ) //kRomanBaseline; horz. baseline type
//	style.SetVertBaseline( eFntBaseline b ) //kCenterBaseline; vertical baseline type
//	style.SetBaseline( GlyphSize baseline ) //0; movement from baseline

//	// MULTIPLE CHARACTER PROPERTIES
//	style.SetKern( BOOL kern ) //false;
//	style.SetMinLSP( GlyphSize minlsp ) //0; minimum letter spacing
//	style.SetOptLSP( GlyphSize optlsp ) //0; optimum letter spacing including tracking
//	style.SetMaxLSP( GlyphSize maxlsp ) //0; maximum letter spacing
//	style.SetMinWord( GlyphSize minword ) //ptsize/4; minimum word space
//	style.SetOptWord( GlyphSize optword ) //ptsize/2; optimum word space
//	style.SetMaxWord( GlyphSize maxword ) //ptsize; maximum letter space

//	// LINE LEVEL PROPERTIES
//	style.SetMinMeasure( MicroPoint mm ) //2 picas; minimum measure of the line
//	style.SetLeftHang( TenThousandth mm ) //0; percent to hang punc characters on left
//	style.SetRightHang( TenThousandth mm ) //0; percent to hang punc characters on right
//	style.SetLeftBlockIndent( MicroPoint lbi ) //0; block indent on left
//	style.SetRightBlockIndent( MicroPoint rbi ) //0; block indent on right
//	style.SetDecimalChar( UCS2 ch ) //'.'; decimal char to look for in tabs

//	// MULTIPLE LINE PROPERTIES
//	style.SetLineLead( int LineLead ) //0; line leading
//	style.SetHyphenate( BOOL hyphenate ) //false; hyphenation
//	style.SetCaseHyphs( BOOL casehyphs ) //false; hyphenate upper case
//	style.SetAcronymHyphs( BOOL acrhyphs ) //false; hyphenate acronyms
//	style.SetMaxConsHyphs( short mxh ) //2; max number of consecutive hyphens
//	style.SetHyphChar( UCS2 hyphchar ) //'-'; hyphenation character
//	style.SetHyphLanguage( APPLanguage hl ) //1; hyphenation language
//	style.SetPreHyph( short prehyph ) //2; glyphs before hyphen
//	style.SetPostHyph( short posthyph ) //2; glyphs after hyphen
//	style.SetMaxWordHyph( short maxword ) //4; size of word to hyphenate

//	// PARAGRAPH LEVEL PROPERTIES
//	style.SetBreakLang( eTSCompLang bl ) //eCompRoman; break language
//	style.SetAutoWrap( BOOL autowrap ) //true; automatic wrap
//	style.SetRag( eTSJust rag ) //0; rag type			
//	style.SetRagZone( MicroPoint ragzone ) //0; rag zone at the end of line
//	style.SetDiffRagZone( MicroPoint drz ) //0; rag difference (every line end must differ this much from previous line end)
//	style.SetIndentAmount( MicroPoint iamt ) //0; paragraph line indent amount
//	style.SetLinesToIndent( short lti ) //0; how many paragraph lines to indent
//	style.SetBulletChar( UCS2 ch ) //-;
//	style.SetBulletFontFamily( APPFont font ) //font; 
//	style.SetBulletEmphasis( eEmphasisType etype, BOOL allBits = false );  //0;
//	style.SetBulletIndent( MicroPoint indent ) //2 picas;
//	style.SetBulletRender( APPRender r ) //0; any additional font rendering attributes
//	style.SetBulletColor( APPColor c ) //0;
//	style.SetNumbered( BOOL tf ) //false; if true numbering (bullets) is on
//	style.SetNoBreak( BOOL tf ) //false; do not break this paragraph across a column
//	style.SetKeepWithNext( BOOL tf ) //false; keep this with the next paragraph
//	style.SetSpaceAbove( MicroPoint abovelead ) //0; space above paragraph
//	style.SetLinesBefore( short lb ) //0; min lines before a column break
//	style.SetLinesAfter( short lb ) //0; min lines after a column break
//	style.SetTab( const scTabInfo& ti, int index )

	st = style;

	return scSuccess;
}

//////////////////////////////////////////////////////////////////////
// Returns the distance from the top of the textbox to the first baseline
MicroPoint CSfirstLinePosition(APPColumn pTextCol, TypeSpec ts)
{
	CAGText* pText = (CAGText*)pTextCol;
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(pSpec->m_Font, false/*bTransform*/);
	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);
	return tm.tmAscent;
}

//////////////////////////////////////////////////////////////////////
// Returns the distance from the bottom of the textbox to the last baseline
MicroPoint CSlastLinePosition(APPColumn pTextCol, TypeSpec ts)
{
	CAGText* pText = (CAGText*)pTextCol;
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetFont(pSpec->m_Font, false/*bTransform*/);
	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);
	return tm.tmDescent;
}

//////////////////////////////////////////////////////////////////////
// Border to inset text from shape applied to column -- default is 0
// the spec is the first encountered in the column.
MicroPoint	CSrunaroundBorder(APPColumn pTextCol, TypeSpec ts)
{
	CAGText* pText = (CAGText*)pTextCol;
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	return 0;
}

//////////////////////////////////////////////////////////////////////
USTR ustring[] =
{
	{ L"", 0 },
	{ L"DATE", 4 },
	{ L"PAGE", 4 },
	{ L"FOOTNOTE", 8 },
	{ L"COLUMN X", 8 },	
	{ L"", 0 }	
};

class clFieldImp : public clField
{
public:
	clFieldImp( uint8 id )
	{
		id_ =  id;
	};
	virtual uint8 id() const
	{
		return id_;
	};
	virtual void release()
	{
		delete this;
	};
	virtual void content(USTR& ustr, APPColumn pTextCol, TypeSpec& ts)
	{
		CAGText* pText = (CAGText*)pTextCol;
		CAGSpec* pSpec = (CAGSpec*)ts.ptr();
		if ( id_ < 5 )
			ustr = ustring[id_];
		else
			ustr = ustring[0];
	}

private:
	uint8 id_;
};

//////////////////////////////////////////////////////////////////////
clField& APPCreateField(scStream* str, uint8 id)
{
	clField* field = new clFieldImp(id);
	return *field;
}

