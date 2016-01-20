// prntrnge.h : header file
//

#include "tilesbtn.h"
#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintRange dialog

class CPrintRange : public CPmwDialog
{
// Construction
public:
	CPrintRange(CPmwDoc *pDoc, double scale, CWnd* pParent = NULL);
	~CPrintRange();

// Dialog Data
	//{{AFX_DATA(CPrintRange)
	enum { IDD = IDD_PRINT_RANGE };
	//}}AFX_DATA

	CPmwDoc *m_pDoc;
	double m_scale;
	TileInfo m_tile_info;

	LPBYTE m_tiles;
	SHORT m_tiles_chosen;
	SHORT m_total_tiles;

	LPBYTE m_local_tiles;

	CTilesButton m_preview;
	RECT m_preview_r;

/* Tile sizes. These are scaled by 10 to maintain some precision. */

	SHORT m_tile_x;
	SHORT m_tile_y;
	SHORT m_first_tile_x;
	SHORT m_first_tile_y;

	BYTE m_draw_value;
	ORIENTATION m_orientation;

// Implementation
protected:
	VOID init_preview();
	BOOL point_to_tile(POINT p, PPOINT tile, int *index);
	VOID update_tile(POINT p, int index, HDC in_hdc = 0);
	VOID set_tile(POINT p, int index);
	VOID preview_tiles(LPDRAWITEMSTRUCT lpDrawItemStruct);
	VOID draw_tiles(SHORT value, HDC hdc = NULL);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CPrintRange)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickedTilesAll();
	afx_msg void OnClickedTilesNone();
	//}}AFX_MSG
	afx_msg LRESULT on_child_button_down(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT on_child_mouse_move(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
};
