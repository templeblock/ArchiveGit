/*
// $Workfile: WHCalend.h $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/WHCalend.h $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 2:47p Hforman
// name change
// 
// 6     9/14/98 12:06p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     6/02/98 2:31p Rlovejoy
// Added ModifyPreviewName().
// 
// 4     6/01/98 11:28p Hforman
// change CWHCalendarPicturePage baseclass to CWHelpPropertyPage
// 
// 3     5/31/98 3:30p Rlovejoy
// Offset categories from 1.
// 
// 2     5/28/98 2:10p Rlovejoy
// Alpha calendars!
// 
// 1     5/26/98 3:05p Rlovejoy
// Initial creation.
// 
//    
*/

#ifndef _WHCALEND_H_
#define _WHCALEND_H_

#include "WHlpWiz.h"
#include "WHlpCmn.h"
#include "WHlpView.h"
#include "calinfo.h"

// Calendar
class CWCalendarPageContainer : public CWHelpPageContainer
{
	enum {
		IDe_CALENDAR_CALENDAR_TYPE = 1,
		IDe_CALENDAR_ENTER_TIME,
		IDe_CALENDAR_PICTURE_LOC,
		IDe_CALENDAR_CATEGORY,
		IDe_CALENDAR_STYLE,
		IDe_CALENDAR_HERE_IT_IS,
		IDe_CALENDAR_HELP
	};

	// Running calendar attributes
   CALENDAR_INFO m_CalendarInfo;
	CAL_PICTURE_STYLE m_PictureStyle;
	short m_nNumberOfCalendars;

// Construction
public:
	CWCalendarPageContainer(CWHelpWizPropertySheet* pSheet);
	virtual ~CWCalendarPageContainer();

// Attributes:
	void GetCalendarInfo(CALENDAR_INFO_REF CalendarInfo, short& nNumberOfCalendars)
	{
		CalendarInfo = m_CalendarInfo;
		nNumberOfCalendars = m_nNumberOfCalendars;
	}
	void SetCalendarInfo(CALENDAR_INFO_REF CalendarInfo, short nNumberOfCalendars)
	{
		m_CalendarInfo = CalendarInfo;
		m_nNumberOfCalendars = nNumberOfCalendars;
	}
	CALENDAR_TYPE GetTimeType()
	{
		return (CALENDAR_TYPE)m_CalendarInfo.style.type;
	}
	void SetTimeType(CALENDAR_TYPE TimeType)
	{
		m_CalendarInfo.style.type = TimeType;
	}
	CAL_PICTURE_STYLE GetPictureStyle()
	{
		return m_PictureStyle;
	}
	void SetPictureStyle(CAL_PICTURE_STYLE PictureStyle)
	{
		m_PictureStyle = PictureStyle;
	}
	

// Implementation
public:
	virtual void RetrieveDlgInfo(UINT nIDContainerItem, void* stDlgInfo);
	virtual void StoreUserInfo(UINT nIDContainerItem, int nChoice);
	virtual void ModifyPreviewName(UINT nIDContainerItem, CString& csName);
};


/////////////////////////////////////////////////////////////////////////////
// CWHCalendarWhenPage dialog

class CWHCalendarWhenPage : public CWHelpPropertyPage
{
private:
	VOID enable_child(UINT id, BOOL enable);
	VOID update_week_selection();
	void ComputeNow();

// Construction
public:
	CWHCalendarWhenPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CCalendarWhenPage)
	enum { IDD = IDD_WH_CALENDAR_WHEN };
	CSpinButtonCtrl	m_ctlSpinNum;
	int		m_month;
	int		m_year;
	int		m_week;
	int		m_numMonths;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWHelpCommonPropertyPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	CALENDAR_INFO			m_ChangingCalendarInfo;
	CALENDAR_TYPE        m_type;

// Implementation
protected:
	void Afx_Data_Init();
	void vars_from_info();

	// Generated message map functions
	//{{AFX_MSG(CCalendarWhenPage)
	afx_msg void OnCalwhenNow();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedType();
	afx_msg void OnMonthChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWHCalendarPicturePage dialog

class CWHCalendarPicturePage : public CWHelpPropertyPage
{
// Construction
public:
	CWHCalendarPicturePage(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWHCalendarPicturePage)
	enum { IDD = IDD_WH_CALENDAR_PICTURE };
	int		m_iAbove;
	int		m_iPicture;
	//}}AFX_DATA
	short m_picture_style;	
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWHCalendarPicturePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int FindBranchChoice();
	CAL_PICTURE_STYLE FindStyleChoice();

	// Generated message map functions
	//{{AFX_MSG(CWHCalendarPicturePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnPictureAbove();
	afx_msg void OnPictureNo();
	afx_msg void OnPictureSide();
	afx_msg void OnPictureYes();
	afx_msg void OnPictureSame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	VOID enable_child(UINT id, BOOL enable);
	void FixControls();
private:
	short	m_type;
	short m_orientation;
	void	UpdateCBitmap();
};

/////////////////////////////////////////////////////////////////////////////
// CWCalendarView dialog

class CWCalendarView : public CWView
{
// Construction
public:
	CWCalendarView(CWHelpPageContainer* pContainer, UINT nIDContainerItem);

// Dialog Data
	//{{AFX_DATA(CWCalendarView)
	//}}AFX_DATA

public:
   // incremented for each WH_TMPx file
   static int m_nTmpFileNumber;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWCalendarView)
	public:
	virtual BOOL OnSetActive();
	//}}AFX_VIRTUAL	


protected:

	// Generated message map functions
	//{{AFX_MSG(CWCalendarView)
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _WHCALEND_H_
