// prntsize.h : header file
//

#include "pmwdlg.h"
#include "cfixed.h"

#define	PRINT_SIZE_SHEETS_LIMIT			(4)
#define	PRINT_SIZE_INCHES_PRECISION	(3)
#define	PRINT_SIZE_INCHES_LIMIT			(6+1+PRINT_SIZE_INCHES_PRECISION)
#define	PRINT_SIZE_PERCENT_PRECISION	(1)
#define	PRINT_SIZE_PERCENT_LIMIT		(6+1+PRINT_SIZE_PERCENT_PRECISION)
#define	PRINT_SIZE_INCHES_MAX			(340.0)
#define	PRINT_SIZE_INCHES_MIN			(0.5)

enum PRINTSIZECHECKRESULT
{
	PRINT_SIZE_CHECK_UNCHANGED,
	PRINT_SIZE_CHECK_CHANGED,
	PRINT_SIZE_CHECK_ERROR
};

/////////////////////////////////////////////////////////////////////////////
// CPrintSize dialog

class CPrintSize : public CPmwDialog
{
// Construction
public:
	CPrintSize(CPmwDoc *pDoc, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrintSize)
	enum { IDD = IDD_PRINT_SIZE };
	CButton	m_preview;
	double	m_inches_wide;
	double	m_inches_tall;
	int		m_sheets_wide;
	int		m_sheets_tall;
	double	m_scale;
	int		m_sheets_wide_rounded_down;
	int		m_sheets_tall_rounded_down;
	double	m_min_inches_wide;
	double	m_max_inches_wide;
	double	m_min_inches_tall;
	double	m_max_inches_tall;
	int		m_min_sheets_wide;
	int		m_max_sheets_wide;
	int		m_min_sheets_tall;
	int		m_max_sheets_tall;
	double	m_min_scale;
	double	m_max_scale;
	//}}AFX_DATA
	
	CPmwDoc *m_pDoc;
	
	PPNT m_doc_dims;
	BOOL m_changed;

	CFixedRect m_PaperMargins;
	CFixedPoint m_Size;
	CFixedPoint m_TileOverlap;
	CFixedPoint m_Tile;

	UINT m_last_active_item_id;
	BOOL m_last_active_item_changed;

	UINT m_active_item_id;
	BOOL m_active_item_changed;

	BOOL m_ignore_change_focus;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();

	VOID compute_x_tiles_from_size(VOID);
	VOID compute_y_tiles_from_size(VOID);
	VOID compute_x_scale(VOID);
	VOID compute_y_scale(VOID);
	VOID update_min_max_values(VOID);
	VOID update_size_from_scale(VOID);
	VOID init_tile_sizes(VOID);

	VOID init_preview(VOID);

	VOID update_all_but(UINT this_id);
	void limit_control(UINT id, SHORT digits);
	BOOL ChangeFocus(UINT wID);
	PRINTSIZECHECKRESULT CheckLastActiveItem(void);
	PRINTSIZECHECKRESULT CheckInt(UINT id, int &value, int min, int max);
	PRINTSIZECHECKRESULT CheckDouble(UINT id, double &value, double min, double max, SHORT precision);
	
	// Generated message map functions
	//{{AFX_MSG(CPrintSize)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusInchesWide();
	afx_msg void OnSetfocusInchesTall();
	afx_msg void OnSetfocusSheetsWide();
	afx_msg void OnSetfocusSheetsTall();
	afx_msg void OnSetfocusPrintScale();
	afx_msg void OnSetfocusSecret1();
	afx_msg void OnSetfocusSecret2();
	afx_msg void OnClickedSizeShow();
	afx_msg void OnChangeAnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
extern VOID compute_tile_dimensions(CPmwDoc *pDoc, double scale,
				PCOORD *width, PCOORD *height,
 				PCOORD *first_width, PCOORD *first_height);

extern VOID draw_print_preview(LPDRAWITEMSTRUCT lpDrawItemStruct,
				CPmwDoc *pDoc,
				SHORT sheets_wide,
				SHORT sheets_tall,
				double scale);

extern VOID size_print_preview(PPNT doc_dims, CWnd *dialog, CWnd *preview);
