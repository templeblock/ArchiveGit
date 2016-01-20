// fontdlg.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPMWFontDialog dialog

class CPMWFontDialog : public CPmwDialog
{
// Construction
public:
	CPMWFontDialog(CPmwDoc *pDoc, CTextStyle& Style, WORD wSoundID = 0, CWnd* pParent = NULL);	// standard constructor

	CPmwDoc *m_pDoc;
	PMGFontServer* m_font_server;

// Original Data
	int		m_original_size;
	int		m_original_face;
	int		m_original_outline;
	int		m_original_shadow;
	int		m_original_style;
	int		m_original_pattern;
	COLOR		m_original_color;
	int		m_nOriginalBold;
	int		m_nOriginalItalic;
// Dialog Data
	//{{AFX_DATA(CPMWFontDialog)
	enum { IDD = IDD_CHOOSEFONT };
	CComboBox	m_size_combo;
	CListBox	m_face_list;
	CButton	m_preview;
	int		m_face;
	int		m_outline;
	int		m_shadow;
	int		m_style;
	int		m_pattern;
	//}}AFX_DATA
	COLOR		m_color;
	CString	m_pointsize;

	RECT		m_full_preview_rect;			/* Extent of control (SCREEN) */
	RECT		m_preview_rect;				/* Extent we preview into (CLIENT) */
	BOOL		m_previewing;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPMWFontDialog)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnFontChanged();
	afx_msg void OnDblclkFontSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	VOID preview_font(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	BOOL get_exit_info(CTextStyle& Style);
};
