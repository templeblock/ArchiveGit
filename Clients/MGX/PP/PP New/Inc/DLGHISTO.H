// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

//dlghisto.h

#define HISTOMARKS 	2
#define NUMHISTOS	5

class CHistogramDlg : public CPPMapExDlg
{
private:
	LPDWORD m_lpHistos[NUMHISTOS]; 		//histograms for each channel
	LONG 	m_lTotals[NUMHISTOS];		//totals for each histogram
	BOOL 	m_bHistoProbe;				//probing which marker
	int 	m_nOriginalValue;			//value of marker before probing
	WORD 	m_wChannel;					//channel currently examining

	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	virtual BOOL OnInitDialog();

	void set_histo_channel();
	void histo_change_mark(int nMark, int val);
	void histo_set_HMSvalues();
	void histo_browse(int x);
public:
	enum {IDD = IDD_HISTOGRAM};
	CHistogramDlg(CServerView* pView=NULL, CWnd* pParent=NULL);

DECLARE_MESSAGE_MAP()
DECLARE_DYNCREATE(CHistogramDlg)
};
