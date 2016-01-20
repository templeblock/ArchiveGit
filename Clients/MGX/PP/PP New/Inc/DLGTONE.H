// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//dlgstone.h

//////////////////////////////////////////  Classes defined in this file /////////////////////////////////
class CToneBalanceDlg;
class CToneBalanceOptionsDlg;
////////////////////////////////////////////////// Typedefs ///////////////////////////////////////////////
typedef struct _quarterstruct {
	LPDWORD lpHistos[5];	// histograms for each channel
	int iProbe;				// probing which marker
	int OriginalValue;  	// value of marker before probing
	WORD wChannel;			// channel currently editing
	LPMAP lpMap;			// Map for current channel
	int iHighlight;			// highlight
	int iShadow;			// midtone
	int iMidtone;			// actual value
	int MidtonePer;			// 'percent'(0-255) as range between iShadow & iMidtone 
}QUARTERSTRUCT, FAR * LPQUARTERSTRUCT;
/////////////////////////////////////////////// Class Definitions //////////////////////////////////////////
class CToneBalanceDlg : public CPPMapExDlg
{
private:
	BOOL 	m_bUseMidPer;
	int  	m_nHSacrifice;
	int  	m_nSSacrifice;

//	LPDWORD m_lpHistos[5];		//histograms for each channel
//	int 	m_nProbe;			//probing which marker
//	int		m_nOriginalValue;	//value of marker before probing
//	WORD	m_wChannel;			//channel currently editing
//	LPMAP	m_lpMap;			//Map for current channel
//	int		m_nHighlight;		//highlight
//	int 	m_nShadow;			//shadow
//	int		m_nMidtone;			//midtone
//	int 	m_nMidtonePer;		//'percent' (0-255) as range between m_nShadow and m_nMidtone

	TONE_PARMS m_Parms;
	LPQUARTERSTRUCT m_lpData;
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
	afx_msg void OnTimer( UINT  nIDEvent );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnDestroy();
	afx_msg void OnMapLoad();
	afx_msg void OnMapSave();
	afx_msg void OnMapOptions();
	afx_msg void OnReset();
	afx_msg void OnResetAll();
	afx_msg void OnPreview();
	afx_msg void OnAutoClip();

	VOID reset_channel(WORD wChannel);
	LPMAP tone_activate_channel(WORD wChannel );
	void change_mark(LPQUARTERSTRUCT lpData, int nMark, int val, int inval);
	VOID set_quarter_channel(WORD wChannel);
	void set_sacrifice(LPQUARTERSTRUCT lpData, WORD wChannel);
	void handle_tone_load(LPQUARTERSTRUCT lpData);
public:
	enum{IDD=IDD_QUARTERTONE};
	CToneBalanceDlg(CServerView* pView=NULL, CWnd* pParent=NULL);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);

	BOOL GetAutoPreviewPref() const {return m_bAutoPreview;}
	VOID SetAutoPreviewPref(BOOL bAutoPreview) {m_bAutoPreview = bAutoPreview;}
	BOOL GetPercentagesPref() const {return m_bPercentages;}
	VOID SetPercentagesPref(BOOL bPercentages) {m_bPercentages = bPercentages;}
	BOOL GetMidtonePercentagesPref() const {return m_bUseMidPer;}
	VOID SetMidtonePercentagesPref(BOOL bUseMidPer) {m_bUseMidPer = bUseMidPer;}
	int	 GetHSacrificePref() const {return m_nHSacrifice;}
	VOID SetHSacrificePref(int nHSacrifice) {m_nHSacrifice = nHSacrifice;}
	int  GetSSacrificePref() const {return m_nSSacrifice;}
	VOID SetSSacrificePref(int nSSacrifice) {m_nSSacrifice = nSSacrifice;}
	LPTONE_PARMS GetParmsPtr()  {return &m_Parms;}
DECLARE_MESSAGE_MAP()
DECLARE_DYNCREATE(CToneBalanceDlg)
};

class CToneBalanceOptionsDlg : public CPPModalDlg
{
private:
	BOOL m_bUseMidPer;
	long m_nHSacrifice;
	long m_nSSacrifice;
	BOOL m_bPercentages;
	BOOL m_bAutoPreview;
	CToneBalanceDlg* m_pParent;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreDoModal(LPVOID lpVoid = NULL);
	virtual LPVOID PostDoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void OnMidPercentages();
	void OnAutoPreview();
	void OnPercentages();
//	void OnHilightsSacrificed(NMHDR* pNMHDR, LRESULT* pResult);

public:
	enum{IDD=IDD_QUARTER_OPTIONS};
	CToneBalanceOptionsDlg(CWnd* pParent = NULL);

DECLARE_DYNCREATE(CToneBalanceOptionsDlg)
DECLARE_MESSAGE_MAP()
};


