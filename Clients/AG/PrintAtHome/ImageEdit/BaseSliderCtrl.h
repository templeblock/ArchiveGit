#pragma once

class CBaseSliderCtrl : public CSliderCtrl
{
public:
	CBaseSliderCtrl();
	virtual ~CBaseSliderCtrl();

protected:
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void DrawChannel(CDC* pDC, LPNMCUSTOMDRAW lpcd);

protected:
	DECLARE_DYNAMIC(CBaseSliderCtrl)
	DECLARE_MESSAGE_MAP()

private:
	bool m_bRedraw;
};
