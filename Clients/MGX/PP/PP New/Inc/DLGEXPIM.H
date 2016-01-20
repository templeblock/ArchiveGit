// dlgexpim.h : header file
// CExpandImageDlg Dialog
#include "ppmodal.h"


class CExpandImageDlg : public CPPViewModalDlg
{

public:
    
    enum { IDD = IDD_EXPAND};
    CExpandImageDlg(CServerView *pView,CWnd* pParent = NULL);   
	VOID DoExpandInits();
	VOID DoExpandControls(ITEMID id, UINT codeNotify );
	LFIXED GetWidth() const {return m_Width;}
	LFIXED GetTop() const {return m_Top;}
	LFIXED GetHeight() const {return m_Height;}
	LFIXED GetLeft() const {return m_Left;}
	COLORINFO GetFillColor() const {return m_FillColor;}
//	EXPAND_PARMS* GetParmsPtr() {return &m_Parms;}


protected:
///	virtual void DoDataExchange(CDataExchange* pDX);    
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal (LPVOID lpVoid=NULL);
	virtual LPVOID PostDoModal ();
	virtual void afx_msg OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	
	
private:
	LFIXED m_Left;
	LFIXED m_Right;
	LFIXED m_Top;
	LFIXED m_Bottom;
	LFIXED m_Width;
	LFIXED m_Height;
	LFIXED m_OrigWidth;
	LFIXED m_OrigHeight;
	WORD m_Units;
	WORD m_SaveUnits;
	COLORINFO m_FillColor;
	WORD m_Resolution;
	WORD m_CropResolution;
	WORD m_OrigResolution;
	int m_Depth;
	RECT m_Crop;
//	EXPAND_PARMS m_Parms;
DECLARE_MESSAGE_MAP()
};
