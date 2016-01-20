// dlgjpeg.h : header file for CJPEGOptionsDlg

#include <afxcmn.h>

#define JPG_LOWRES 0
#define JPG_MEDRES 1
#define JPG_HIRES  2
#define JPG_TICFREQ 5
#define JPG_MAXSLIDERPOS 100
#define JPG_MINSLIDERPOS 0
#define JPG_SLIDERPAGESIZE JPG_TICFREQ 

class CJPEGOptionsDlg : public CPPModalDlg
{
public:
	CJPEGOptionsDlg(CWnd* pParent = NULL); 
	virtual int DoModal();
	virtual BOOL PreDoModal(LPVOID lpData = NULL);
	virtual LPVOID PostDoModal();
	virtual void OnOK();

	enum { IDD = IDD_JPEGOPTIONS };
	CComboBox	m_SubsamplingCombo;
	CSliderCtrl	m_CompressionSlider;
	UINT		m_CompressionEdit;
	
	UINT m_SubsamplingPref;
	UINT m_CompressionPref;
	BOOL m_bAlwaysPromptPref;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCompressionEdit();
	afx_msg void OnSelchangeSubsamplingCombo();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPromptForOptions();
	DECLARE_MESSAGE_MAP()
};
