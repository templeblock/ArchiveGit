// caldoc.h : header file
//

#include "calendar.h"
#include "frameobj.h"
#include "calobj.h"

class CCmdCalendarDate;

/////////////////////////////////////////////////////////////////////////////
// CCalendarDoc document

class CCalendarDoc : public CPmwDoc
{
	DECLARE_SERIAL(CCalendarDoc)
	INHERIT(CCalendarDoc, CPmwDoc)
private:
	void AdjustPageSize ();
	PBOX get_text_bound(DWORD dwPage);
public:
	void RecalcPageSize ();
	void CalcCalendarSize (PBOX_PTR);
	PBOX GetPicturePageBound(DWORD dwPage);
	void GetPageDims(PPNT& bottom, PPNT& top);
	CFrameObject * CreateFrame(PBOX_PTR bound, LPSTR text, ALIGN_TYPE align, VERT_ALIGN_TYPE valign, SHORT panel, CTextStyle* style);
	virtual VOID size_to_paper(VOID);
	virtual void SizeCurrentPage(PPNT OldSize, PPNT NewSize);

	FLAGS CalcPagePrintSourceBox(PBOX_PTR box, short page);
	void FlipPage(PBOX& world, FLAGS Flags);
	BOOL CreateYearlyCalendar (CALENDAR_CREATE_STRUCT* ccs, ObjectList* ol, CDialog * pDlg = NULL, BOOL bSwap = FALSE);
	CalendarObject * find_calendar_object(DWORD dwPage);
	virtual void GetObjectPageDims(PBOX * pBox, PageObject * pObject);
	void GetPreviewBound(PBOX* pBound);
	void GetPanelBoundForPreview(PBOX* pBound);
protected:
	void ClipBlankPanels(PBOX* pBound);
	CCalendarDoc();			// protected constructor used by dynamic creation


// Attributes
public:

	static ProjectInfo StandardInfo;

// Operations
public:
	
	virtual CalendarObjectPtr get_current_calendar(BOOL use_global = FALSE);

	virtual PMGPageObject* CopyObject(PMGPageObjectPtr pObject, ObjectListPtr pObjects);
	virtual PMGPageObject* PasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate = TRUE);

	virtual PMGPageObject* PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate = TRUE);
	virtual void PrePaste(void);
	virtual void PostPaste(void);
	virtual void AbortPaste(void);

	// Move to next or previous object.
	virtual void SelectNextObject(void);
	virtual BOOL CanSelectNextObject(void);
	virtual void SelectPreviousObject(void);
	virtual BOOL CanSelectPreviousObject(void);

	void ChangeDate(int nCount);
	short GetPictureStyle ();
	void SetPictureStyle(short picture_style);

	//short GetWeeksInMonth (short year, short month);

public:
	virtual BOOL CanPageNext(void);
	virtual void DoPageNext(void);
	short NumberOfPrinterPages(void);	
	void GoToFirstPage();

public:
	void ChangeCalendarDate(CALENDAR_INFO info);
	void ChangeLabelFontStyle(CTextStyle style);
	void ChangeNumberFontStyle(const CTextStyle& style);
	void ReplaceCalendarInfo(CALENDAR_INFO & info, CalendarObjectPtr pObject);

// Implementation
protected:
	virtual ~CCalendarDoc();
	virtual	BOOL OnNewDocument();
	virtual	BOOL OnOpenDocument(const char *pszPathName);

	void	CreateCalendarInfoRecord();
	void	GetCalendarInfoRecord();
	void	EnsureCalendarInfoRecord();		// For loaded documents.

	// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Pending command during paste, etc.
	CCmdCalendarDate* m_pPendingCommand;
private:
	short m_picture_style;
	short m_number_of_calendars;

};
