//==========================================================================//
// AGText.cpp - CAGText and CAGSpec classes
//==========================================================================//
#include "stdafx.h"
#include "AGText.h"

#include "scappint.h"
#include "sccallbk.h"
#include "scstyle.h"
#include "scselect.h"
#include "scapptex.h"
#include "sccolumn.h"
#include "scparagr.h"

#ifdef _AFX
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] =__FILE__;
#define new DEBUG_NEW
#endif
#endif


static long ReadFunc(APPCtxPtr pText, void* ptr, long size);
static long WriteFunc(APPCtxPtr pText, void* ptr, long size);


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSpec::CAGSpec()
{
	memset(&m_Font, 0, sizeof(m_Font));
	m_Color = 0;
	m_HorzJust = eRagRight;
	m_nAboveParaLeading = 0;
	m_nBelowParaLeading = 0;
	m_nLineLeading = 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSpec::CAGSpec(const LOGFONT& Font, COLORREF Color, eTSJust HorzJust, int nAboveParaLeading, int nBelowParaLeading, int nLineLeading)
{
	m_Font = Font;
	m_Color = Color;
	m_HorzJust = HorzJust;
	m_nAboveParaLeading = nAboveParaLeading;
	m_nBelowParaLeading = nBelowParaLeading;
	m_nLineLeading = nLineLeading;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGSpec::CAGSpec(const CAGSpec& Spec)
{
	m_Font = Spec.m_Font;
	m_Color = Spec.m_Color;
	m_HorzJust = Spec.m_HorzJust;
	m_nAboveParaLeading = Spec.m_nAboveParaLeading;
	m_nBelowParaLeading = Spec.m_nBelowParaLeading;
	m_nLineLeading = Spec.m_nLineLeading;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSpec::operator= (const CAGSpec& Spec)
{
	m_Font = Spec.m_Font;
	m_Color = Spec.m_Color;
	m_HorzJust = Spec.m_HorzJust;
	m_nAboveParaLeading = Spec.m_nAboveParaLeading;
	m_nBelowParaLeading = Spec.m_nBelowParaLeading;
	m_nLineLeading = Spec.m_nLineLeading;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
int CAGSpec::operator== (const CAGSpec& Spec) const
{
	return (m_Font == Spec.m_Font &&
			m_Color == Spec.m_Color &&
			m_HorzJust == Spec.m_HorzJust &&
			m_nAboveParaLeading == Spec.m_nAboveParaLeading &&
			m_nBelowParaLeading == Spec.m_nBelowParaLeading &&
			m_nLineLeading == Spec.m_nLineLeading);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSpec::Read(CAGDocIO* pInput)
{
	pInput->Read(&m_Font, sizeof(m_Font));
	pInput->Read(&m_Color, sizeof(m_Color));
	pInput->Read(&m_HorzJust, sizeof(m_HorzJust));
	pInput->Read(&m_nAboveParaLeading, sizeof(m_nAboveParaLeading));
	pInput->Read(&m_nBelowParaLeading, sizeof(m_nBelowParaLeading));
	pInput->Read(&m_nLineLeading, sizeof(m_nLineLeading));
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGSpec::Write(CAGDocIO* pOutput) const
{
	pOutput->Write(&m_Font, sizeof(m_Font));
	pOutput->Write(&m_Color, sizeof(m_Color));
	pOutput->Write(&m_HorzJust, sizeof(m_HorzJust));
	pOutput->Write(&m_nAboveParaLeading, sizeof(m_nAboveParaLeading));
	pOutput->Write(&m_nBelowParaLeading, sizeof(m_nBelowParaLeading));
	pOutput->Write(&m_nLineLeading, sizeof(m_nLineLeading));
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGText::CAGText()
{
	::SetRect(&m_DestRect, 0, 0, 0, 0);
	m_pColumn = NULL;
	m_pSelection = NULL;
	m_pdc = NULL;
	m_bSelection = false;
	m_bShiftKeyDown = false;
	m_bControlKeyDown = false;
	m_bLButtonDown = false;
	m_bOverstrike = false;
	m_pNextSpec = NULL;
	m_pWriteTSList = NULL;
	m_pDocIO = NULL;
	m_pWriteTSList = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
CAGText::~CAGText()
{
	if (m_pColumn)
		SCCOL_Delete(m_pColumn, 0);

	if (m_pNextSpec)
		delete m_pNextSpec;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::BlinkCursor()
{
	if (m_pSelection && m_pSelection->IsSliverCursor() && !m_bLButtonDown)
		ShowSelection(!m_bSelection);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::ComputeRedisplay(scRedispList& colRedisp)
{
	scColRedisplay colRect;

	for (int limit = 0; limit < colRedisp.GetNumItems(); limit++)
	{
		colRedisp.GetDataAt(limit, (ElementPtr)&colRect);

		if (colRect.fHasDamage)
		{
			scXRect dRect(colRect.fDamageRect);

			RECT r =
			{
				m_DestRect.left + dRect.x1, m_DestRect.top + dRect.y1,
				m_DestRect.left + dRect.x2, m_DestRect.top + dRect.y2
			};

			POINT Pts[4];
			Pts[0].x = Pts[1].x = r.left;
			Pts[2].x = Pts[3].x = r.right;
			Pts[0].y = Pts[3].y = r.top;
			Pts[1].y = Pts[2].y = r.bottom;
			m_pdc->LPtoDP(Pts, 4);

			r.left = r.top = LONG_MAX;
			r.right = r.bottom = LONG_MIN;
			for (int i = 0; i < 4; i++)
			{
				if (Pts[i].x < r.left)
					r.left = Pts[i].x;
				if (Pts[i].x > r.right)
					r.right = Pts[i].x;
				if (Pts[i].y < r.top)
					r.top = Pts[i].y;
				if (Pts[i].y > r.bottom)
					r.bottom = Pts[i].y;
			}

			::InvalidateRect(m_pdc->GetWnd(), &r, false);
		}
		
		if (colRect.fHasRepaint)
		{
			scXRect rRect(colRect.fRepaintRect);

			RECT r =
			{
				m_DestRect.left + rRect.x1, m_DestRect.top + rRect.y1,
				m_DestRect.left + rRect.x2, m_DestRect.top + rRect.y2
			};

			POINT Pts[4];
			Pts[0].x = Pts[1].x = r.left;
			Pts[2].x = Pts[3].x = r.right;
			Pts[0].y = Pts[3].y = r.top;
			Pts[1].y = Pts[2].y = r.bottom;
			m_pdc->LPtoDP(Pts, 4);

			r.left = r.top = LONG_MAX;
			r.right = r.bottom = LONG_MIN;
			for (int i = 0; i < 4; i++)
			{
				if (Pts[i].x < r.left)
					r.left = Pts[i].x;
				if (Pts[i].x > r.right)
					r.right = Pts[i].x;
				if (Pts[i].y < r.top)
					r.top = Pts[i].y;
				if (Pts[i].y > r.bottom)
					r.bottom = Pts[i].y;
			}

			::InvalidateRect(m_pdc->GetWnd(), &r, false);
		}

		if (colRect.fImmediateRedisplay)
		{
			scXRect iRect(colRect.fImmediateArea.fImmediateRect);
			SCCOL_UpdateLine(colRect.fColumnID, colRect.fImmediateArea, this);
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::Create(const RECT& DestRect)
{
	m_DestRect = DestRect;

	SCCOL_New(this, m_pColumn, WIDTH(m_DestRect), HEIGHT(m_DestRect));
	SCFS_Recompose(m_pColumn, 0);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::DrawColumn(CAGDC& dc)
{
	CAGDC* pSaveDC = m_pdc;
	m_pdc = &dc;

	scXRect ClipRect(0, 0, WIDTH(m_DestRect), HEIGHT(m_DestRect));
	SCCOL_Update(m_pColumn, ClipRect, this);

	m_pdc = pSaveDC;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::DrawSelection(CAGDC& dc)
{
	if (!m_pSelection || !m_bSelection)
		return;

	CAGDC* pSaveDC = m_pdc;
	m_pdc = &dc;

	SCSEL_Hilite(m_pSelection, APPDrawRect);

	m_pdc = pSaveDC;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::Edit(CAGDC* pdc, int x, int y, bool bClick)
{
	m_pdc = pdc;
	ShowSelection(false);

	if (IsEmpty())
	{
		TypeSpec ts(m_pNextSpec);
		SCCOL_InitialSelect(m_pColumn, ts, m_pSelection);
		m_pNextSpec = NULL;
		ShowSelection(true);
	}
	else
	{
		scMuPoint pt(x - m_DestRect.left, y - m_DestRect.top);
		SCCOL_StartSelect(m_pColumn, pt, APPDrawRect, this, m_pSelection);
		m_bSelection = true;
	}

	m_bShiftKeyDown = false;
	m_bControlKeyDown = false;
	m_bOverstrike = false;
	m_bLButtonDown = bClick;
	m_LastPt.Invalidate();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::EndEdit()
{
	ShowSelection(false);
	m_pdc = NULL;
	m_pSelection = NULL;
	m_bShiftKeyDown = false;
	m_bControlKeyDown = false;
	m_bLButtonDown = false;
	m_bOverstrike = false;

	if (m_pNextSpec)
	{
		delete m_pNextSpec;
		m_pNextSpec = NULL;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::ExtendSelection(eSelectMove movement)
{
	ShowSelection(false);
	SCSEL_Extend(m_pSelection, movement);
	UpdateSelection();
	ShowSelection(true);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::GetFonts(LOGFONTARRAY& lfArray)
{
	scTypeSpecList TSList;
	SCCOL_TSList(m_pColumn, TSList);

	int nNumItems = TSList.NumItems();
	for (int i = 0; i < nNumItems; i++)
	{
		LOGFONT lf = ((CAGSpec*)TSList[i].ptr())->m_Font;

		int nFonts = lfArray.size();
		for (int j = 0; j < nFonts; j++)
		{
			if (!lstrcmp(lf.lfFaceName, lfArray[j].lfFaceName) && lf.lfWeight == lfArray[j].lfWeight && lf.lfItalic == lfArray[j].lfItalic)
				break;
		}

		if (j >= nFonts)
			lfArray.push_back(lf);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::GetSelParaTSList(scTypeSpecList& tsList) const
{
	SCSEL_PARATSList(m_pSelection, tsList);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::GetSelTSList(scTypeSpecList& tsList) const
{
	if (m_pNextSpec)
	{
		TypeSpec ts(new CAGSpec(*m_pNextSpec));
		tsList.Insert(ts);
	}
	else
	if (m_pSelection)
		SCSEL_TSList(m_pSelection, tsList);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
eVertJust CAGText::GetVertJust() const
{
	if (m_pColumn)
		return m_pColumn->GetVertJust();

	return eVertTop;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
bool CAGText::IsEmpty() const
{
	if (!m_pColumn)
		return true;

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);
	if (!pStream)
		return true;

	long lChCount = 0;
	SCSTR_ChCount(pStream, lChCount);
	return !lChCount;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::MoveSelection(eSelectMove movement)
{
	ShowSelection(false);
	SCSEL_Move(m_pSelection, movement);
	UpdateSelection();
	ShowSelection(true);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnChar(UINT nChar)
{
	if (!m_pSelection || m_bLButtonDown)
		return;

	scRedispList colRedisp;
	scKeyRecord keyRec;

	keyRec.type() = m_bOverstrike ? scKeyRecord::overstrike : scKeyRecord::insert;
	keyRec.keycode() = CMappToCT(nChar);
	if (m_pNextSpec)
	{
		TypeSpec Spec(m_pNextSpec);
		keyRec.spec() = Spec;
		m_pNextSpec = NULL;
	}
	else
	{
		TypeSpec NullSpec;
		keyRec.spec() = NullSpec;
	}

	if (keyRec.keycode() == scParaSplit)
		keyRec.keycode() = scHardReturn;
	else
	if (keyRec.keycode() == scHardReturn)
		keyRec.keycode() = scParaSplit;

	ShowSelection(false);
	SCSEL_InsertKeyRecords(m_pSelection, 1, &keyRec, &colRedisp);
	ComputeRedisplay(colRedisp);
	ShowSelection(true);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnKeyDown(UINT nChar)
{
	if (!m_pSelection || m_bLButtonDown)
		return;

	switch (nChar)
	{
		case VK_CONTROL:
			m_bControlKeyDown = true;
			break;

		case VK_SHIFT:
			m_bShiftKeyDown = true;
			break;

		case VK_INSERT:
			m_bOverstrike = !m_bOverstrike;
			break;

		case VK_DELETE:
		{
			scRedispList colRedisp;
			scKeyRecord keyRec;

			keyRec.keycode() = (UCS2) scForwardDelete;
			ShowSelection(false);
			SCSEL_InsertKeyRecords(m_pSelection, 1, &keyRec, &colRedisp);
			ComputeRedisplay(colRedisp);
			ShowSelection(true);
			break;
		}

		case VK_LEFT:
			if (m_bShiftKeyDown)
				ExtendSelection(m_bControlKeyDown ? ePrevWord : ePrevChar);
			else
				MoveSelection(m_bControlKeyDown ? ePrevWord : ePrevChar);
			break;

		case VK_RIGHT:
			if (m_bShiftKeyDown)
				ExtendSelection(m_bControlKeyDown ? eNextWord : eNextChar);
			else
				MoveSelection(m_bControlKeyDown ? eNextWord : eNextChar);
			break;

		case VK_UP:
			if (m_bShiftKeyDown)
				ExtendSelection(ePrevLine);
			else
				MoveSelection(ePrevLine);
			break;

		case VK_DOWN:
			if (m_bShiftKeyDown)
				ExtendSelection(eNextLine);
			else
				MoveSelection(eNextLine);
			break;

		case VK_HOME:
			if (m_bShiftKeyDown)
				ExtendSelection(m_bControlKeyDown ? eBeginColumn : eStartLine);
			else
				MoveSelection(m_bControlKeyDown ? eBeginColumn : eStartLine);
			break;

		case VK_END:
			if (m_bShiftKeyDown)
				ExtendSelection(m_bControlKeyDown ? eEndColumn : eEndLine);
			else
				MoveSelection(m_bControlKeyDown ? eEndColumn : eEndLine);
			break;

		case VK_PRIOR:
			if (m_bShiftKeyDown)
				ExtendSelection(eBeginColumn);
			else
				MoveSelection(eBeginColumn);
			break;

		case VK_NEXT:
			if (m_bShiftKeyDown)
				ExtendSelection(eEndColumn);
			else
				MoveSelection(eEndColumn);
			break;

		default:
			break;
	}

	if (m_pNextSpec && !(*m_pSelection == m_SelNextSpec))
	{
		delete m_pNextSpec;
		m_pNextSpec = NULL;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnKeyUp(UINT nChar)
{
	switch (nChar)
	{
		case VK_CONTROL:
			m_bControlKeyDown = false;
			break;

		case VK_SHIFT:
			m_bShiftKeyDown = false;
			break;

		default:
			break;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnLButtonDblClk(POINT Point)
{
	if (!m_pSelection)
		return;

	scMuPoint muPt(Point.x - m_DestRect.left, Point.y - m_DestRect.top);
	ShowSelection(false);
	SCCOL_SelectSpecial(m_pColumn, muPt, eWordSelect, m_pSelection);
	ShowSelection(true);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnLButtonDown(POINT Point, bool bShift)
{
	if (!m_pSelection)
		return;

	ShowSelection(false);
	scMuPoint muPt(Point.x - m_DestRect.left, Point.y - m_DestRect.top);
	SCCOL_StartSelect(m_pColumn, muPt, APPDrawRect, this, m_pSelection);
	m_bSelection = true;
	m_bLButtonDown = true;
	m_LastPt.Invalidate();

	if (m_pNextSpec && !(*m_pSelection == m_SelNextSpec))
	{
		delete m_pNextSpec;
		m_pNextSpec = NULL;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnLButtonUp(POINT Point)
{
	if (!m_pSelection)
		return;

	m_bLButtonDown = false;
	if (m_pSelection && !m_pSelection->IsSliverCursor())
		m_bSelection = true;

	if (m_pNextSpec && !(*m_pSelection == m_SelNextSpec))
	{
		delete m_pNextSpec;
		m_pNextSpec = NULL;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::OnMouseMove(POINT Point)
{
	if (!m_pSelection || !m_bLButtonDown)
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

	if (m_LastPt != muPt)
	{
		SCCOL_ExtendSelect(m_pColumn, muPt, APPDrawRect, this, m_pSelection);
		m_LastPt = muPt;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::ReadColumn(CAGDocIO* pInput)
{
	if (m_pColumn)
		SCCOL_Delete(m_pColumn, 0);
	if (m_pNextSpec)
	{
		delete m_pNextSpec;
		m_pNextSpec = NULL;
	}

	m_TSList.RemoveAll();

	m_pColumn = NULL;
	m_pSelection = NULL;
	m_pdc = NULL;
	m_bShiftKeyDown = false;
	m_bControlKeyDown = false;
	m_bLButtonDown = false;
	m_bOverstrike = false;

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

	scSet* enumTable;
	SCSET_InitRead(enumTable, 100);
	SCCOL_Read(this, m_pColumn, enumTable, this, ReadFunc);
	SCOBJ_PtrRestore((scTBObj*)m_pColumn, enumTable);
	SCSET_FiniRead(enumTable, 0);

	m_pDocIO = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetColor(COLORREF Color)
{
	if (!m_pSelection)
		return;

	if (m_pSelection->IsSliverCursor())
	{
		if (!m_pNextSpec)
		{
			TypeSpec ts = m_pSelection->fMark.fPara->SpecAtOffset(m_pSelection->fMark.fOffset);
			m_pNextSpec = new CAGSpec(*((CAGSpec*)ts.ptr()));
			m_SelNextSpec = *m_pSelection;
		}

		m_pNextSpec->m_Color = Color;
	}
	else
	{
		ShowSelection(false);

		scStream* fStream = NULL;
		SCCOL_GetStream(m_pColumn, fStream);
		scSpecLocList redocsl(fStream);
		SCSEL_CHTSList(m_pSelection, redocsl);

		for (int i = 0; i < redocsl.NumItems(); i++)
		{
			if (redocsl[i].spec().ptr())
			{
				CAGSpec* pNewSpec = new CAGSpec(*((CAGSpec*)redocsl[i].spec().ptr()));
				pNewSpec->m_Color = Color;
				redocsl[i].spec() = pNewSpec;
			}
		}

		scRedispList colRedisp;
		SCSTR_CHTSListSet(fStream, redocsl, &colRedisp);
		ComputeRedisplay(colRedisp);
		ShowSelection(true);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetFont(const LOGFONT& Font)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetHorzJust(eTSJust HorzJust)
{
	scStream* fStream = NULL;
	SCCOL_GetStream(m_pColumn, fStream);
	scSpecLocList csl(fStream);

	SCSEL_PARATSList(m_pSelection, csl);

	for (int i = 0; i < csl.NumItems(); i++)
	{
		if (csl[i].spec().ptr())
		{
			CAGSpec* pNewSpec = new CAGSpec(*((CAGSpec*)csl[i].spec().ptr()));
			pNewSpec->m_HorzJust = HorzJust;
			csl[i].spec() = pNewSpec;
		}
	}
	scRedispList colRedisp;
	SCSTR_PARATSListSet(fStream, csl, &colRedisp);
	ComputeRedisplay(colRedisp);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetNextSpec(const CAGSpec& Spec)
{
	if (m_pNextSpec)
		delete m_pNextSpec;
	m_pNextSpec = new CAGSpec(Spec);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetPtSize(int nPtSize)
{
	if (m_pSelection)
	{
		if (m_pSelection->IsSliverCursor())
		{
			if (!m_pNextSpec)
			{
				TypeSpec ts = m_pSelection->fMark.fPara->SpecAtOffset(m_pSelection->fMark.fOffset );
				m_pNextSpec = new CAGSpec(*((CAGSpec*)ts.ptr()));
				m_SelNextSpec = *m_pSelection;
			}
			m_pNextSpec->m_Font.lfHeight = -nPtSize * APP_RESOLUTION / 72;
		}
		else
		{
			ShowSelection(false);

			scStream* fStream = NULL;
			SCCOL_GetStream(m_pColumn, fStream);
			scSpecLocList redocsl(fStream);
			SCSEL_CHTSList(m_pSelection, redocsl);

			for (int i = 0; i < redocsl.NumItems(); i++ )
			{
				if (redocsl[i].spec().ptr())
				{
					CAGSpec* pNewSpec = new CAGSpec(*((CAGSpec*)redocsl[i].spec().ptr()));
					pNewSpec->m_Font.lfHeight = -nPtSize * APP_RESOLUTION / 72;
					redocsl[i].spec() = pNewSpec;
				}
			}
			scRedispList colRedisp;
			SCSTR_CHTSListSet(fStream, redocsl, &colRedisp);
			ComputeRedisplay(colRedisp);
			ShowSelection(true);
		}
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetText(const char* pText, int nChars, int nSpecs, const CAGSpec* pAGSpecs, const int* pSpecOffsets)
{
	bool bStartNewPara = true;

	stTextImportExport &AppText = stTextImportExport::MakeTextImportExport();
	CAGSpec* pAGSpec = NULL;

	for (int i = 0; i < nSpecs; i++)
	{
		int nLen;
		if ((i + 1) < nSpecs)
			nLen = pSpecOffsets[i + 1] - pSpecOffsets[i];
		else
			nLen = nChars - pSpecOffsets[i];

		if (nLen)
		{
			pAGSpec = new CAGSpec(pAGSpecs[i]);
			TypeSpec ts(pAGSpec);

			if (bStartNewPara)
			{
				AppText.StartPara(ts);
				bStartNewPara = false;
			}

			for (int j = 0; j < nLen; j++)
			{
				if (pText[j] == 0x12)
				{
					if (bStartNewPara)
						AppText.StartPara(ts);

					if (j > 0)
						AppText.PutString((const uchar*)pText, j, ts);

					bStartNewPara = true;
					nLen -= (j + 1);
					pText += (j + 1);
					j = -1;
				}
			}

			if (nLen)
			{
				if (bStartNewPara)
				{
					AppText.StartPara(ts);
					bStartNewPara = false;
				}

				AppText.PutString((const uchar*)pText, nLen, ts);
				pText += nLen;
			}
		}
	}

	AppText.reset();
	SCFS_PasteAPPText(m_pColumn, AppText, 0);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetTypeface(const LOGFONT& Font)
{
	if (!m_pSelection)
		return;

	if (m_pSelection->IsSliverCursor())
	{
		if (!m_pNextSpec)
		{
			TypeSpec ts = m_pSelection->fMark.fPara->SpecAtOffset(m_pSelection->fMark.fOffset);
			m_pNextSpec = new CAGSpec(*((CAGSpec*)ts.ptr()));
			m_SelNextSpec = *m_pSelection;
		}

		lstrcpy(m_pNextSpec->m_Font.lfFaceName, Font.lfFaceName);
		m_pNextSpec->m_Font.lfWeight = Font.lfWeight;
		m_pNextSpec->m_Font.lfItalic = (Font.lfItalic != 0);
		m_pNextSpec->m_Font.lfCharSet = Font.lfCharSet;
		m_pNextSpec->m_Font.lfOutPrecision = Font.lfOutPrecision;
		m_pNextSpec->m_Font.lfClipPrecision = Font.lfClipPrecision;
		m_pNextSpec->m_Font.lfQuality = Font.lfQuality;
		m_pNextSpec->m_Font.lfPitchAndFamily = Font.lfPitchAndFamily;
	}
	else
	{
		ShowSelection(false);

		scStream* fStream = NULL;
		SCCOL_GetStream(m_pColumn, fStream);
		scSpecLocList redocsl(fStream);
		SCSEL_CHTSList(m_pSelection, redocsl);

		for (int i = 0; i < redocsl.NumItems(); i++ )
		{
			if (redocsl[i].spec().ptr())
			{
				CAGSpec* pNewSpec = new CAGSpec(*((CAGSpec*)redocsl[i].spec().ptr()));
				lstrcpy(pNewSpec->m_Font.lfFaceName, Font.lfFaceName);
				pNewSpec->m_Font.lfWeight = Font.lfWeight;
				pNewSpec->m_Font.lfItalic = (Font.lfItalic != 0);
				pNewSpec->m_Font.lfCharSet = Font.lfCharSet;
				pNewSpec->m_Font.lfOutPrecision = Font.lfOutPrecision;
				pNewSpec->m_Font.lfClipPrecision = Font.lfClipPrecision;
				pNewSpec->m_Font.lfQuality = Font.lfQuality;
				pNewSpec->m_Font.lfPitchAndFamily = Font.lfPitchAndFamily;
				redocsl[i].spec() = pNewSpec;
			}
		}

		scRedispList colRedisp;
		SCSTR_CHTSListSet(fStream, redocsl, &colRedisp);
		ComputeRedisplay(colRedisp);
		ShowSelection(true);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetUnderline(bool bUnderline)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::SetVertJust(eVertJust VertJust)
{
	ShowSelection(false);

	SCCOL_FlowJustify(m_pColumn, VertJust);
	SCFS_Recompose(m_pColumn, 0);

	if (m_pSelection)
	{
		RECT r = m_DestRect;
		m_pdc->LPtoDP(&r);
		::InvalidateRect(m_pdc->GetWnd(), &r, false);
	}

	ShowSelection(true);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::ShowSelection(bool bShow)
{
	if (!m_pSelection)
		return;

	if (bShow != m_bSelection)
	{
		SCSEL_Hilite(m_pSelection, APPDrawRect);
		m_bSelection = bShow;
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::UpdateSelection()
{
	scStreamLocation Mark, Point;
	SCSEL_Decompose2(m_pSelection, Mark, Point);

	scStream* streamID = NULL;
	SCCOL_GetStream(m_pColumn, streamID);
	SCSEL_Restore(streamID, Mark, Point, m_pSelection, false);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void CAGText::WriteColumn(CAGDocIO* pOutput)
{
	if (!m_pColumn)
		return;

	m_pDocIO = pOutput;
	pOutput->Write(&m_DestRect, sizeof(m_DestRect));

	scStream* pStream = NULL;
	SCCOL_GetStream(m_pColumn, pStream);

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
	m_pDocIO = NULL;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static long ReadFunc(APPCtxPtr pText, void* ptr, long size)
{
	CAGDocIO* pDocIO = pText->GetDocIO();
	if (pDocIO)
		pDocIO->Read(ptr, size);
	return size;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static long WriteFunc(APPCtxPtr pText, void* ptr, long size)
{
	CAGDocIO* pDocIO = pText->GetDocIO();
	if (pDocIO)
		pDocIO->Write(ptr, size);
	return size;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
scTicks SCTickCount()
{
	return GetTickCount();
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
scTicks APPEventAvail(scProcType proctype)
{
	switch (proctype)
	{
		case scDrawProc:
			return 1000;

		case scReformatProc:
//			if (IsKeyStrokes())
//				return 0;
			return 20;
	}
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void* APPDiskIDToPointer(APPCtxPtr pText, long index, stDiskidClass objclass)
{
	switch (objclass)
	{
		case diskidColumn:
			return pText;

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

	return (void*)-1;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
long APPPointerToDiskID(APPCtxPtr pText, void* obj, stDiskidClass objclass)
{
	switch (objclass)
	{
		case diskidTypespec:
		{
			if (!obj)
				return 0;

			scTypeSpecList* pWriteTSList = pText->GetWriteTSList();
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


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
Bool APPRecomposeColumn(APPColumn)
{
	return true;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void APPDrawStartLine(APPDrwCtx pText, MicroPoint xOrg, MicroPoint yOrg, const scXRect& inkExtents)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void APPDrawEndLine(APPDrwCtx pText)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void APPDrawString(APPDrwCtx pText, const scGlyphArray* glyphArray, short num, MicroPoint xOrg, MicroPoint yOrg, const scGlyphInfo& gi)
{
	TCHAR* pString = new TCHAR [num];
	int* pWidths = new int [num];

	for (int i = 0; i < num; i++, glyphArray++)
	{
		pString[i] = LOBYTE(glyphArray->fGlyphID);
		pWidths[i] = glyphArray->hEscapement;
	}

	CAGSpec* pSpec = (CAGSpec*)gi.typespec.ptr();
	CAGDC* pDC = pText->GetDC();

	pDC->SetFont(pSpec->m_Font);
	pDC->SetTextColor(pSpec->m_Color);

	RECT DestRect = pText->GetDestRect();
	pDC->ExtTextOut(DestRect.left + xOrg, DestRect.top + yOrg,
					0, NULL, pString, num, pWidths);

	delete [] pString;
	delete [] pWidths;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void APPDrawRect(const scXRect& xrect, APPDrwCtx pText, Bool sliverCursor)
{
	CAGDC* pDC = pText->GetDC();
	if (!pDC)
		return;

	RECT DestRect = pText->GetDestRect();
	RECT r;

	r.left = max((DestRect.left + xrect.x1), DestRect.left);
	r.top = max((DestRect.top + xrect.y1), DestRect.top);
	r.right = min((DestRect.left + xrect.x2), DestRect.right);
	r.bottom = min((DestRect.top + xrect.y2), DestRect.bottom);

	if (WIDTH(r) >= 0 && HEIGHT(r) >= 0)
	{
		if (!WIDTH(r))
		{
			POINT ptFrom = { r.left, r.top };
			POINT ptTo = { r.left, r.bottom };
			pDC->InvertLine(ptFrom, ptTo);
		}
		else
			pDC->InvertRect(r);
	}
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
status APPDrawContext(APPColumn appID, const scColumn* colid, APPDrwCtx& pText)
{
	pText = appID;
	return scSuccess;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void APPDrawRule(const scMuPoint& start, const scMuPoint& end, const scGlyphInfo& gi, APPDrwCtx pText)
{
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
RLU FIgetRLUEscapement(const scFontRender& fp, UCS2 ch)
{
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
RLU FIgetRLUKern(const scFontRender& fp, UCS2 ch1, UCS2 ch2)
{
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
scRLURect& FIgetRLUExtents(const scFontRender& fp, UCS2 ch, scRLURect& chBox)
{
	return chBox;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void FIgetRLUFontExtents(const scFontRender& fp, RLU& capHite, RLU& xHite, RLU& ascenderHite, RLU& descenderDepth, scRLURect& maxFont)
{
	capHite = 0;
	xHite = 0;
	ascenderHite = 0;
	descenderDepth = 0;

	maxFont.rluLeft = 0;
	maxFont.rluTop = 0;
	maxFont.rluRight = 0;
	maxFont.rluBottom = 0;

	CAGSpec* pSpec = (CAGSpec*)fp.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetTransformMode(false);
	TextIC.SetFont(pSpec->m_Font);

	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);

	capHite = (short)(scRoundMP((REAL)(tm.tmAscent - tm.tmInternalLeading) * scBaseRLUsystem / tm.tmHeight));
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
GlyphSize FIgetDEVEscapement(const scFontRender& fp, UCS2 ch)
{
	CAGSpec* pSpec = (CAGSpec*)fp.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetTransformMode(false);
	TextIC.SetFont(pSpec->m_Font);

	TCHAR temp = (TCHAR)ch;
	SIZE sizeExtent;
	TextIC.GetTextExtent(&temp, 1, &sizeExtent);

	return sizeExtent.cx;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
MicroPoint FIgetDEVEscapement(const scFontRender& , UCS2, MicroPoint suggestedWidth)
{
	return suggestedWidth;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
GlyphSize FIgetDEVKern(const scFontRender& fp, UCS2 ch1, UCS2 ch2)
{
	return 0;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
scXRect& FIgetDEVExtents(const scFontRender& fp, UCS2 ch, scXRect& chBox)
{
	return chBox;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
void FIgetDEVFontExtents(const scFontRender& fp, MicroPoint& capHite, MicroPoint& xHite, MicroPoint& ascenderHite, MicroPoint& descenderDepth, scXRect& maxFont)
{
	CAGSpec* pSpec = (CAGSpec*)fp.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetTransformMode(false);
	TextIC.SetFont(pSpec->m_Font);

	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);
	int nMinAWidth = TextIC.GetMinAWidth(tm.tmFirstChar, tm.tmLastChar);

	capHite = tm.tmAscent;
	xHite = 0;	// Not used
	ascenderHite = tm.tmAscent;
	descenderDepth = -tm.tmDescent;

	maxFont.x1 = nMinAWidth;
	maxFont.y1 = tm.tmAscent;
	maxFont.x2 = tm.tmMaxCharWidth;
	maxFont.y2 = -(tm.tmDescent + tm.tmExternalLeading);
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
Bool HYFWord(const UCS2* word, short* hyfArray)
{
	return false;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
status TSTabInfo(TypeSpec& ps, TypeSpec& ts,scTabInfo& tabInfo, MicroPoint xPos, MicroPoint, long)
{
	int nTabSize = one_inch / 4;
	tabInfo.xPosition = xPos + (nTabSize -(xPos % nTabSize));
	tabInfo.tabAlign = eTBLeftAlign;
	tabInfo.fillChar = ' ';
	tabInfo.fillCharAlign = eFCNormalAlign;

	return scSuccess;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
status TSfillCharInfo(TypeSpec& , UCS2& , eFCAlignment& , MicroPoint, MicroPoint, long)
{
	return scSuccess;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
status TSGetStyle(TypeSpec& ts, scStyle& st)
{
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	if (pSpec)
	{
		CAGIC TextIC("DISPLAY");
		TextIC.SetTransformMode(false);
		TextIC.SetFont(pSpec->m_Font);

		TEXTMETRIC tm;
		TextIC.GetTextMetrics(&tm);

		SIZE sizeSpace;
		TextIC.GetTextExtent(" ", 1, &sizeSpace);

		scStyle style(pSpec->m_Font.lfFaceName, tm.tmHeight);
		style.SetRag(pSpec->m_HorzJust);

		if (!pSpec->m_nLineLeading)
		{
			style.SetLead(0);
			style.SetMaxLead(0);
		}
		else
		{
			style.SetLead(pSpec->m_nLineLeading - tm.tmHeight);
			style.SetMaxLead(pSpec->m_nLineLeading - tm.tmHeight);
		}

		if (!pSpec->m_nAboveParaLeading)
		{
			style.SetMaxSpaceAbove(0);
			style.SetSpaceAbove(0);
		}
		else
		{
			style.SetMaxSpaceAbove(pSpec->m_nAboveParaLeading - tm.tmHeight);
			style.SetSpaceAbove(pSpec->m_nAboveParaLeading - tm.tmHeight);
		}

		style.SetMaxSpaceBelow(0);
		style.SetSpaceBelow(0);

		style.SetAbsoluteLead(0);

		style.SetOptWord(sizeSpace.cx);
		style.SetMinWord(sizeSpace.cx);
		style.SetMaxWord(sizeSpace.cx);

		style.SmallCapCorrection();
		st = style;
	}
	else
	{
		scStyle style("Times New Roman", 15);
		st = style;
	}
	return scSuccess;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
Bool TSdropCap(TypeSpec& pspec, TypeSpec& charspec, DCPosition& dcpos, UCS2 dropchar)
{
	return false;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
MicroPoint CSfirstLinePosition(APPColumn pText, TypeSpec ts)
{
	CAGSpec* pSpec = (CAGSpec*)ts.ptr();
	CAGIC TextIC("DISPLAY");
	TextIC.SetTransformMode(false);
	TextIC.SetFont(pSpec->m_Font);

	TEXTMETRIC tm;
	TextIC.GetTextMetrics(&tm);

	if (pText->GetVertJust() == eVertCentered)
		return tm.tmHeight + tm.tmExternalLeading;
	else
		return tm.tmAscent;
}


//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
MicroPoint CSlastLinePosition(APPColumn pText, TypeSpec ts)
{
	if (pText->GetColumn()->GetVertJust() == eVertBottom)
	{
		CAGSpec* pSpec = (CAGSpec*)ts.ptr();
		CAGIC TextIC("DISPLAY");
		TextIC.SetTransformMode(false);
		TextIC.SetFont(pSpec->m_Font);

		TEXTMETRIC tm;
		TextIC.GetTextMetrics(&tm);

		return tm.tmDescent + tm.tmExternalLeading;
	}
	return 0;
}


stUnivChar	ucnull[]	= { '\0' };
stUnivChar	ucdate[]	= { 'D', 'A', 'T', 'E', '\0' };
stUnivChar	ucpage[]	= { 'P', 'A', 'G', 'E', '\0' };
stUnivChar	ucfootnote[]	= { 'F', 'O', 'O', 'T', 'N', 'O', 'T', 'E', '\0' };
stUnivChar	uccolumn[]	= { 'C', 'O', 'L', 'U', 'M', 'N', ' ', 'X', '\0' };
					  
stUnivString ustring[] =
{
	{ ucnull, 0 },
	{ ucdate, 4 },
	{ ucpage, 4 },
	{ ucfootnote, 8 },
	{ uccolumn, 8 },	
	{ ucnull, 0 }	
};


class clFieldImp : public clField
{
public:
	clFieldImp( uint8 id )
		{ id_ =  id; }
							
	virtual uint8 id() const;
	virtual void release();
	virtual void content(stUnivString&, APPColumn, TypeSpec&);

private:
	uint8	id_;
};


clField& clField::createField(scStream* str, uint8 id)
{
	clField* field = new clFieldImp(id);
	return *field;
}


uint8 clFieldImp::id() const
{
	return id_;
}


void clFieldImp::release()
{
	delete this;
}


void clFieldImp::content(stUnivString& ustr, APPColumn col, TypeSpec& )
{
	if ( id_ < 5 )
	{
		ustr = ustring[id_];
		if ( id_ == 4 )
		{
//			stUnivChar* ptr = (stUnivChar*)ustr.ptr;
//			ptr[7]	= col->num() + '0';
		}
	}
	else
		ustr = ustring[0];
}
