#pragma once

#include "AGSym.h"

class CAGDC;
class CAGSymCalendar;

struct FONTSIZE : tagSIZE
{
	LONG tmDescent;
};

struct CALSTYLES
{
	enum YEARSTYLES
	{
		YR_UPPERCASE = 0,
		YR_LOWERCASE = 1,
		YR_ABBR_UPPERCASE = 2,
		YR_ABBR_LOWERCASE = 3,
		YR_UPPERCASE_YYYY = 4,
		YR_LOWERCASE_YYYY = 5,
		YR_ABBR_UPPERCASE_YYYY = 6,
		YR_ABBR_LOWERCASE_YYYY = 7
	};

	enum WEEKSTYLES
	{
		WK_UPPERCASE = 0,
		WK_LOWERCASE = 1,
		WK_ABBR_UPPERCASE = 2,
		WK_ABBR_LOWERCASE = 3,
		WK_SINGLE_UPPER = 4,
		WK_SINGLE_LOWER = 5,
		WK_SINGLE_MIXED = 6
	};

	enum TITLE_SIZES
	{
		TITLESIZE_1 = 10,
		TITLESIZE_2 = 15,
		TITLESIZE_3 = 20,
		TITLESIZE_4 = 25,
		TITLESIZE_5 = 30,
		TITLESIZE_6 = 35,
		TITLESIZE_7 = 40,
		TITLESIZE_8 = 45,
		TITLESIZE_9 = 50,
		TITLESIZE_10 = 55,
		TITLESIZE_11 = 60,

		TITLESIZE_MIN = 10,
		TITLESIZE_MAX = 60,
		TITLESIZE_INCR = 5
	};

	enum WEEK_SIZES
	{
		WEEKSIZE_1 = 05,
		WEEKSIZE_2 = 10,
		WEEKSIZE_3 = 15,
		WEEKSIZE_4 = 20,

		WEEKSIZE_MIN = 05,
		WEEKSIZE_MAX = 20,
		WEEKSIZE_INCR = 5
	};

	enum WEEK_START
	{
		SUNDAY_START = 0,
		MONDAY_START = 1
	};
};

class CCalSupport 
{
public:
	CCalSupport(CAGSymCalendar *pSym, CAGDC& dc);
	~CCalSupport();

	void Draw();
	void DrawBackgroundRectangle(const RECT& Rect, COLORREF FillColor);
	void DrawRectangle(const RECT& Rect, COLORREF LineColor, int LineWidth, COLORREF FillColor, const CString& strText, const CAGSpec& Spec, int VertJust, int nLineWidth, int nLineHeight);

	static const CString YearStr(const COleDateTime& Date, UINT yrStyle);
	static const CString WeekStr(int iDay, UINT iStyle);

protected:
	int GetDaysInMonth();
	int GetFirstDayOfMonth();

	CString GetTitleStr();
	CString GetWeekStr(int iDay);

protected:
	CAGDC& m_dc;
	CAGSymCalendar* m_pCalSym;
};
