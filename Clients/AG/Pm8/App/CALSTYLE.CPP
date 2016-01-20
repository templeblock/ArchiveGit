// calstyle.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"

//#include "system.h"
//#include "calinfo.h"
#include "calstyle.h"

#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalendarStyleDlg dialog

CCalendarStyleDlg::CCalendarStyleDlg(CALENDAR_INFO_REF info, CPmwDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CCalendarStyleDlg::IDD, pParent)
{
	m_document = pDoc;
	m_type = (CALENDAR_TYPE)info.style.type;
	//{{AFX_DATA_INIT(CCalendarStyleDlg)
	//}}AFX_DATA_INIT
	vars_from_info(info);
}

void CCalendarStyleDlg::vars_from_info(CALENDAR_INFO_REF info)
{
	m_shadow = info.style.shadow_style;			// -1;
	m_lines = info.style.line_style;				// -1;
	m_spelling = info.style.weekday_style;		// -1;
	m_background = info.style.label_style;		// -1;
	m_outline = info.style.outline_style;		// FALSE;
	m_color = info.style.color;
}

void CCalendarStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarStyleDlg)
	DDX_CBIndex(pDX, IDC_CALSTYLE_SHADOW, m_shadow);
	DDX_Radio(pDX, IDC_CALSTYLE_NO_LINES, m_lines);
	DDX_Radio(pDX, IDC_CALSTYLE_INITIALS, m_spelling);
	DDX_Radio(pDX, IDC_CALSTYLE_TEXT_ONLY, m_background);
	DDX_Check(pDX, IDC_CALSTYLE_OUTLINE, m_outline);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCalendarStyleDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CCalendarStyleDlg)
	ON_BN_CLICKED(IDC_CALSTYLE_DEFAULT, OnCalstyleDefault)
	ON_BN_CLICKED(IDC_CALSTYLE_COLOR, OnCalstyleColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarStyleDlg message handlers

BOOL CCalendarStyleDlg::get_exit_info(CALENDAR_INFO_REF info)
{
	BOOL changed = FALSE;
	if (info.style.shadow_style != m_shadow)
	{
		info.style.shadow_style = (CAL_SHADOW_STYLE)m_shadow;
		changed = TRUE;
	}
	if (info.style.line_style != m_lines)
	{
		info.style.line_style = (CAL_LINE_STYLE)m_lines;
		changed = TRUE;
	}
	if (info.style.weekday_style != m_spelling)
	{
		info.style.weekday_style = (WEEKDAY_STYLE)m_spelling;
		changed = TRUE;
	}
	if (info.style.label_style != m_background)
	{
		info.style.label_style = (CAL_LABEL_STYLE)m_background;
		changed = TRUE;
	}
	if (info.style.outline_style != m_outline)
	{
		info.style.outline_style = (CAL_OUTLINE_STYLE)m_outline;
		changed = TRUE;
	}
	if (info.style.color != m_color)
	{
		info.style.color = m_color;
		changed = TRUE;
	}
	return changed;
}

/*
// Set the dialog entries to default.
*/

void CCalendarStyleDlg::OnCalstyleDefault()
{
	CALENDAR_INFO info;

	info.set_default_style(m_type);

	vars_from_info(info);

	UpdateData(FALSE);
}

/*
// Choose the color for the calendar.
*/

void CCalendarStyleDlg::OnCalstyleColor()
{
	m_document->choose_color(m_color);
}

BOOL CCalendarStyleDlg::OnInitDialog()
{
	return CPmwDialog::OnInitDialog();
}
