/*	$Header: /PM8/App/TimePieceDlg.h 1     3/03/99 6:12p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a timepiece graphic.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/App/TimePieceDlg.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 1     2/01/99 11:43a Rgrenfel
// New timepieces dialog for the creation of custom time pieces.
*/

#ifndef TIMEPIECEDLG_H
#define TIMEPIECEDLG_H

#include "GenericGroupedObjectDialog.h"
#include "DateTime.h"
#include "ComponentAttributes.h"

#include "CollectionManager.h"
#include "CollectionSearcher.h"
#include "CustomGraphicSearch.h"

typedef RCollectionSearcher<RTimepieceGraphicSearchCollection> RTimepieceCollectionSearcher;

class RDateTimeInterface;
class RTimepiecePlacementInterface;
class RHeadlineInterface;

class RTimepieceDialog : public RGenericGroupedObjectDialog
{
// Member functions	
public:
	// Construction
												RTimepieceDialog(RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent);
												~RTimepieceDialog();

	// Implementation
private:
	// Updates the time in an existing timepiece
	void										UpdateTime();
	
	// Updates the timepiece group object (both the headline and the graphic).
	// Called when the user selects a new graphic from the list box.
	void										UpdateTimepieceComponent();

protected:
	// Overrides
	virtual void							DoDataExchange(CDataExchange* pDX);
	virtual BOOL							OnInitDialog();
	virtual const YComponentType&		GetComponentType() const;
	virtual void							WriteUIContextData(RUIContextData& contextData) const;
	virtual RUIContextData*				ReadUIContextData();

	// Implementation
	afx_msg void							OnTimeChange();
	afx_msg void							OnUpdateHour();
	afx_msg void							OnUpdateMinute();
	afx_msg void							OnGraphicChange();
	afx_msg void							OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

// Data members
private:   
	// Timepiece data
	RTime										m_rTime;
	RMBCString								m_rGraphicName;
	RMBCString								m_rTimepieceFont;

	// The headline component view	
	RComponentView*						m_pGraphicView;
	RComponentView*						m_pHeadlineView;

	// The interfaces to the timepiece data
	RDateTimeInterface*					m_pDateTimeInterface;		
	RHeadlineInterface*					m_pHeadlineInterface;	

	// Component attributes used by the preview
	RComponentAttributes					m_rGraphicAttributes;
	RComponentAttributes					m_rHeadlineAttributes;

	// The search results
	// NOTE 4/11/97 GKB We need to declare these as members because the RTimepieceCollectionSearcher
	// depends on the existence of the RCollectionArray and the RSearchResultArray depends on the
	// existence of the RTimepieceCollectionSearcher.
	RCollectionArray						m_rCollectionArray;
	RTimepieceCollectionSearcher		m_rSearcher;
	RSearchResultArray*					m_pSearchResultArray;
	
	// A timer used in making text updates perform more smoothly
	UINT										m_uTimerID;

	// The dialog's controls
	CEdit										m_cEditHour;
	CEdit										m_cEditMinute;
	CSpinButtonCtrl						m_cSpinHour;
	CSpinButtonCtrl						m_cSpinMinute;
};

#endif // TIMEPIECEDLG_H
