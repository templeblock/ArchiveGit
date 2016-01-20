#include "stdafx.h"
#include "CalSupport.h"
#include "AGDC.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

static LPCSTR arrDayOfWeek[][7] = {
	{"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"},
	{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
	{"SUN", "MON", "TUE", "WED", "THUR", "FRI", "SAT"},
	{"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"},
	{"S", "M", "T", "W", "T", "F", "S"},
	{"s", "m", "t", "w", "t", "f", "s"},
	{"Su", "M", "Tu", "W", "Th", "F", "Sa"}
};

//////////////////////////////////////////////////////////////////////
CCalSupport::CCalSupport(CAGSymCalendar *pSym, CAGDC& dc) :
	m_dc(dc)
{
	m_dc = dc;
	m_pCalSym = pSym;
}

//////////////////////////////////////////////////////////////////////
CCalSupport::~CCalSupport()
{
}

//////////////////////////////////////////////////////////////////////
void CCalSupport::Draw()
{
	COLORREF LineColor = m_pCalSym->m_LineColor;
	bool bInvisible = (LineColor == CLR_NONE);
	int LineWidth = m_pCalSym->m_LineWidth;
	if (bInvisible)
		LineWidth = LT_None;

	CAGMatrix LineInverseMatrix = m_dc.GetSymbolMatrix();
	double fAngle = LineInverseMatrix.GetAngle();
	LineInverseMatrix.Rotate(-fAngle, -fAngle);
	LineInverseMatrix = LineInverseMatrix.Inverse();

	int nLineSize = (LineWidth == LT_Hairline ? 0 : LineWidth);
	int nLineWidth = dtoi(LineInverseMatrix.TransformWidth(nLineSize));
	int nLineHeight = dtoi(LineInverseMatrix.TransformHeight(nLineSize));

	RECT SymbolRect = m_pCalSym->m_DestRect;
	int iSymbolHeight = HEIGHT(SymbolRect);
	int iTitleHeight = (iSymbolHeight * m_pCalSym->m_uCalStyles.TitleSize) / 100;
	int iWeekHeight  = (iSymbolHeight * m_pCalSym->m_uCalStyles.DayOfWkSize) / 100;
	int iDatesHeight = (iSymbolHeight - iTitleHeight - iWeekHeight);
	int iDateHeight = ((iDatesHeight - nLineHeight) / 6);
	int iDateWidth = (WIDTH(SymbolRect) - nLineWidth) / 7;

	// Fill the 3 big background areas
	RECT TitleRect = SymbolRect;
	TitleRect.bottom = TitleRect.top + iTitleHeight;
	
	RECT WeekRect = SymbolRect;
	WeekRect.top = TitleRect.bottom;
	WeekRect.bottom = WeekRect.top + iWeekHeight;

	RECT DateRect = SymbolRect;
	DateRect.top = WeekRect.bottom;
	DateRect.bottom = DateRect.top + iDatesHeight;

	COLORREF TitleFillColor = (m_pCalSym->m_FillColor == CLR_DEFAULT ? CLR_NONE : m_pCalSym->m_FillColor);
	COLORREF WeekFillColor = (m_pCalSym->m_FillColor2 == CLR_DEFAULT ? CLR_NONE : m_pCalSym->m_FillColor2);
	COLORREF DateFillColor = (m_pCalSym->m_FillColor3 == CLR_DEFAULT ? CLR_NONE : m_pCalSym->m_FillColor3);

	CAGMatrix Matrix = m_dc.GetSymbolToDeviceMatrix();
	DrawBackgroundRectangle(TitleRect, Matrix, TitleFillColor);
	DrawBackgroundRectangle(WeekRect, Matrix, WeekFillColor);
	DrawBackgroundRectangle(DateRect, Matrix, DateFillColor);

	// Compute the Title rect
	TitleRect = SymbolRect;
	TitleRect.bottom = TitleRect.top + iTitleHeight + nLineHeight;

	// Draw the Title box and text
	m_pCalSym->m_SpecMonth.m_HorzJust = (eTSJust)m_pCalSym->m_uCalStyles1.TitleHorzJust;
	DrawRectangle(TitleRect, Matrix, LineColor, LineWidth, CLR_NONE, GetTitleStr(), m_pCalSym->m_SpecMonth, m_pCalSym->m_uCalStyles1.TitleVertJust, nLineWidth, nLineHeight);

	// Compute the Week rect
	WeekRect.left = SymbolRect.left;
	WeekRect.right = WeekRect.left + iDateWidth + nLineWidth;
	WeekRect.top = TitleRect.bottom - nLineHeight;
	WeekRect.bottom = WeekRect.top + iWeekHeight + nLineHeight;

	// Draw the Week boxes and text
	m_pCalSym->m_SpecWeek.m_HorzJust = (eTSJust)m_pCalSym->m_uCalStyles1.WeekHorzJust;

	bool bSunStart = (m_pCalSym->m_uCalStyles.WeekStart == CALSTYLES::SUNDAY_START);
	for (int i=0; i<7; i++)
	{
		int iDay = (bSunStart ? i : i+1);
		DrawRectangle(WeekRect, Matrix, LineColor, LineWidth, CLR_NONE, GetWeekStr(iDay), m_pCalSym->m_SpecWeek, m_pCalSym->m_uCalStyles1.WeekVertJust, nLineWidth, nLineHeight);

		// Update the Week rect
		WeekRect.left += iDateWidth;
		WeekRect.right = WeekRect.left + iDateWidth + nLineWidth;
		if (i == 5)
			WeekRect.right = SymbolRect.right;
	}

	// Compute the Date rect
	DateRect.left = 0;
	DateRect.right = 0;
	DateRect.top = WeekRect.bottom - nLineHeight;
	DateRect.bottom = DateRect.top + iDateHeight + nLineHeight;

	// Draw the Date boxes and text
	m_pCalSym->m_SpecDays.m_HorzJust = (eTSJust)m_pCalSym->m_uCalStyles1.DateHorzJust;

	int nDaysInMonth = GetDaysInMonth();
	int iDay = 2 - GetFirstDayOfMonth(); 
	for (int y=0; y<6; y++)
	{
		DateRect.left = SymbolRect.left;
		DateRect.right = DateRect.left + iDateWidth + nLineWidth;

		bool bRowEmpty = true;
		for (int x=0; x<7; x++)
		{
			bool bDraw = false;
			CString strText;
			if (iDay <= nDaysInMonth)
			{
				bDraw = true;
				if (iDay >= 1)
				{
					strText = String("%d", iDay);
					bRowEmpty = false;
				}
			}
			else
			{
				bool bHideUnusedBoxes = (m_pCalSym->m_uCalStyles.HideBoxes == 1); 
				bool bHideAllBoxes = (m_pCalSym->m_uCalStyles.HideAllBoxes == 1);
				bool bHideEmptyRows = (m_pCalSym->m_uCalStyles.HideAllBoxes == 0);
				bool bHide = bHideUnusedBoxes && (bHideAllBoxes || (bHideEmptyRows && bRowEmpty));
				bDraw = !bHide;
			}

			if (bDraw)
				DrawRectangle(DateRect, Matrix, LineColor, LineWidth, CLR_NONE, strText, m_pCalSym->m_SpecDays, m_pCalSym->m_uCalStyles1.DateVertJust, nLineWidth, nLineHeight);
			iDay++;

			// Update the Date rect
			DateRect.left += iDateWidth;
			DateRect.right = DateRect.left + iDateWidth + nLineWidth;
			if (x == 5)
				DateRect.right = SymbolRect.right;
		}

		// Update the Date rect
		DateRect.top += iDateHeight;
		DateRect.bottom = DateRect.top + iDateHeight + nLineHeight;
		if (y == 4)
			DateRect.bottom = SymbolRect.bottom;
	}
}

//////////////////////////////////////////////////////////////////////
void CCalSupport::DrawBackgroundRectangle(const RECT& Rect, const CAGMatrix& Matrix, COLORREF FillColor)
{
	POINT pPoints[4];
	Matrix.TransformRectToPolygon(Rect, pPoints);

	HRGN hClipRegion = m_dc.SaveClipRegion();
	m_dc.SetClipRectangle(pPoints);

	// Draw the interior
	if (FillColor != CLR_NONE)
		m_dc.FillRectangle(pPoints, FillColor);

	m_dc.RestoreClipRegion(hClipRegion);
}


//////////////////////////////////////////////////////////////////////
void CCalSupport::DrawRectangle(const RECT& Rect, const CAGMatrix& Matrix, COLORREF LineColor, int LineWidth, COLORREF FillColor, const CString& strText, const CAGSpec& Spec, int VertJust, int nLineWidth, int nLineHeight)
{
//j	#define RND (int)(255 * ((double)rand() / RAND_MAX))
//j	LineColor = RGB(RND, RND, RND);

	POINT pPoints[4];
	Matrix.TransformRectToPolygon(Rect, pPoints);

	HRGN hClipRegion = m_dc.SaveClipRegion();
	m_dc.SetClipRectangle(pPoints);

	// Draw the interior
	if (FillColor != CLR_NONE)
		m_dc.FillRectangle(pPoints, FillColor);

	// Draw the text
	if (!strText.IsEmpty())
	{
		CRect RectDeflated = Rect;
		RectDeflated.DeflateRect(nLineWidth, nLineHeight);
		Spec.DrawTextAligned(&m_dc, strText, NULL, RectDeflated, (eVertJust)VertJust);
	}

	// Draw the outline (non-hairline)
	if (LineWidth >= LT_Normal)
	{
		int iLineWidth = dtoi(m_dc.GetViewToDeviceMatrix().TransformDistance(2*LineWidth+1));
		m_dc.DrawRectangle(pPoints, LineColor, iLineWidth, CLR_NONE);
	}

	m_dc.RestoreClipRegion(hClipRegion);

	// Draw the outline (hairline)
	if (LineWidth == LT_Hairline)
		m_dc.DrawRectangle(pPoints, LineColor, LT_Hairline, CLR_NONE);
}

//////////////////////////////////////////////////////////////////////
int CCalSupport::GetDaysInMonth()
{
	int iYear = m_pCalSym->m_iYear;
	int iNextMonth = m_pCalSym->m_iMonth + 1;
	if (iNextMonth == 13)
	{
		iNextMonth = 1;
		iYear++;
	}
	COleDateTime Date(iYear, iNextMonth, 1, 0,0,0); // The first day of next month
	Date -= COleDateTimeSpan(1,0,0,0); // 1 day
	return Date.GetDay();
}

//////////////////////////////////////////////////////////////////////
int CCalSupport::GetFirstDayOfMonth()
{
	COleDateTime Date(m_pCalSym->m_iYear, m_pCalSym->m_iMonth, 1, 0,0,0); // The first day of this month
	int iFirstDayOfMonth = Date.GetDayOfWeek(); // Sunday = 1

	bool bSundayStart = (m_pCalSym->m_uCalStyles.WeekStart == CALSTYLES::SUNDAY_START);
	if (!bSundayStart)
	{
		if (--iFirstDayOfMonth == 0)
			iFirstDayOfMonth = 7;
	}

	return iFirstDayOfMonth;
}

//////////////////////////////////////////////////////////////////////
const CString CCalSupport::WeekStr(int iDay, UINT iStyle)
{
	return arrDayOfWeek[iStyle][iDay == 7 ? 0 : iDay];
}

//////////////////////////////////////////////////////////////////////
CString CCalSupport::GetWeekStr(int iDay)
{
	int iStyle = m_pCalSym->m_uCalStyles.WeekStyle;
	return WeekStr(iDay, iStyle);
}

//////////////////////////////////////////////////////////////////////
const CString CCalSupport::YearStr(const COleDateTime & date, UINT yrStyle)
{
	CString strFormat;

	switch (yrStyle)
	{
		case CALSTYLES::YR_UPPERCASE: 
		case CALSTYLES::YR_LOWERCASE:
			strFormat = "%B";
			break;
		case CALSTYLES::YR_ABBR_UPPERCASE: 
		case CALSTYLES::YR_ABBR_LOWERCASE:
			strFormat = "%b";
			break;
		case CALSTYLES::YR_UPPERCASE_YYYY:
		case CALSTYLES::YR_LOWERCASE_YYYY:
			strFormat = "%B %Y";
			break;
		case CALSTYLES::YR_ABBR_UPPERCASE_YYYY:
		case CALSTYLES::YR_ABBR_LOWERCASE_YYYY:
			strFormat = "%b %Y";
			break;
		default:
			strFormat = "%B";
	}
	strFormat = date.Format(strFormat);
	if ( (yrStyle == CALSTYLES::YR_ABBR_UPPERCASE) || 
		 (yrStyle == CALSTYLES::YR_ABBR_UPPERCASE_YYYY) || 
		 (yrStyle == CALSTYLES::YR_UPPERCASE) ||
		 (yrStyle == CALSTYLES::YR_UPPERCASE_YYYY))
		strFormat.MakeUpper();

	return strFormat;
}

//////////////////////////////////////////////////////////////////////
CString CCalSupport::GetTitleStr()
{
	COleDateTime Date(m_pCalSym->m_iYear, m_pCalSym->m_iMonth, 1, 0,0,0);
	return CCalSupport::YearStr(Date, m_pCalSym->m_uCalStyles.YearStyle);
}
