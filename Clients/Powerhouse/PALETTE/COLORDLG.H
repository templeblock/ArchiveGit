// colordlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog

class CColorDlg : public CDialog
{
// Construction
public:
    CColorDlg(CWnd* pParent = NULL);    // standard constructor
    ~CColorDlg();

// Dialog Data
    //{{AFX_DATA(CColorDlg)
    enum { IDD = IDD_COLOR };
    //}}AFX_DATA
    int m_red;
    int m_green;
    int m_blue;

// Implementation
public:
    void PaintColor();
private:
    CScrollBar* m_psbRed;
    CScrollBar* m_psbGreen;
    CScrollBar* m_psbBlue;
    CDIBPal* m_pPal;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Generated message map functions
    //{{AFX_MSG(CColorDlg)
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnClickedDelete();
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

