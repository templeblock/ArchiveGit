#pragma once


class CListBoxECard : public CWindowImpl<CListBoxECard, CListViewCtrl>,
					  public CCustomDraw<CListBoxECard>
{
// Construction
public:
	CListBoxECard();
	virtual ~CListBoxECard();


	// Operations
public:
                             
    //DECLARE_WND_SUPERCLASS(_T("ECARDLISTVIEW"), GetWndClassName())

	BEGIN_MSG_MAP(CListBoxECard)
		CHAIN_MSG_MAP(CCustomDraw<CListBoxECard>)
    END_MSG_MAP()

// Implementation
public:

	// Overridables


protected:

};
