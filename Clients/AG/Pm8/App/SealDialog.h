/*	$Header: /PM8/App/SealDialog.h 1     3/03/99 6:11p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a Seal compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/SealDialog.h $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     2/10/99 5:08p Rgrenfel
// Created a new DisplayGraphic call that takes the new and old component
// documents instead of performing a collection read.
// 
// 1     2/01/99 5:39p Rgrenfel
// Constructs a new Seal component object.
*/

#ifndef _SEALDIALOG_H_
#define _SEALDIALOG_H_	

#ifndef _RENAISSANCERESOURCE_H_
//	#include "RenaissanceResource.h"
#endif
class RStandaloneView;

#ifndef _COMMONGROUPEDOBJECTDIALOG_H_
	#include "CommonGroupedObjectDialog.h"
#endif

#ifndef _CUSTOMGRAPHICSEARCH_H_
	#include "CustomGraphicSearch.h"
#endif

#ifndef _COLLECTIONSEARCHER_H_
	#include "CollectionSearcher.h"
#endif

#ifndef _WINCOLORDLG_H_
	#include "WinColorDlg.h"
#endif


//	Define a context data version based on a marker field in the stream.
//	If we make sure we are using the correct
//	'version' of the stream then we know we can read and write our data
//	without interfering with other stream users (like derived dialogs)
const YContextDataVersion kContextDataVersion3 = _T("kContextDataVersion3");		//	The size before any version2 data is written.


class RSealDialog : public RCommonGroupedObjectDialog
{
// Construction
public:
	RSealDialog( RStandaloneDocument* pParentDocument,
					 RComponentDocument* pEditedComponent,
					 int nResourceId = RSealDialog::IDD );

	virtual ~RSealDialog();

// Dialog Data
	//{{AFX_DATA(RGenericGroupedObjectDialog)
	enum { IDD = IDD_SEAL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RGenericGroupedObjectDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void								CreateGraphicsList( RMBCString& rList );
	// In this dialog the effects list contains the outer graphics
	virtual void								CreateEffectsList( RMBCString& rList );
	virtual void								UpdatePreview( BOOLEAN fUpdateHeadline = TRUE, BOOLEAN fUpdateGraphic = TRUE );
	virtual void								DisplayGraphic( const SearchResult& rSelGraphic,  RComponentDocument*& pGraphicDoc, uLONG& uSelectedGraphicId );
	virtual void								DisplayGraphic( RComponentDocument *pNewGraphicDoc, RComponentDocument*& pGraphicDoc );
	virtual const YComponentType&			GetComponentType( ) const;
	virtual RUIContextData*		  			ReadUIContextData( ) ;
	virtual RUIContextData*		  			ReadVersion2UIContextData( RUIContextData& contextData );
	virtual void					  			WriteUIContextData( RUIContextData& contextData ) const;
	virtual void					  			WriteVersion2UIContextData( RUIContextData& contextData ) const;
	virtual RRealRect				  			GetDefaultHeadlineRect();
	virtual void					  			CheckForMonochromeGraphic( RComponentDocument*& pGraphicDoc );
	virtual void					  			ShowOrHideBullets( BOOLEAN fShowBullets );
	virtual void					  			SetupHeadlines();
	virtual void					  			OnButtonColorEdge();


	RCollectionSearcher<RSealInnerGraphicSearchCollection>*		m_pInnerGraphicCollectionSearcher;
	RCollectionSearcher<RSealOuterGraphicSearchCollection>*		m_pOuterGraphicCollectionSearcher;
	//CMap<int, int, SearchResult, SearchResult>						m_cOuterGraphicMap;										
	CArray<SearchResult, SearchResult&>									m_cOuterGraphicArray;

	RMBCString																	m_rBottomText;
	BOOLEAN																		m_fShowBullets;

	CEdit																			m_cBottomTextEditCtrl;
	RHeadlineInterface*														m_pBottomHeadlineInterface;
	RComponentView*															m_pBottomHeadlineView;
	RComponentView*															m_pLeftBulletView;
	RComponentView*															m_pRightBulletView;
	RGOHeadlineData															m_rBottomHeadlineData;

	RColor																		m_rColorEdgeGraphic;
	RWinColorBtn																m_cEdgeGraphicColorBtn;

	RColorSettingsInterface*												m_pOuterGraphicInterface;
	RComponentDocument*														m_pOuterGraphicDoc;
	uLONG																			m_uSelectedEdgeGraphicId;

	// Generated message map functions
	//{{AFX_MSG(RGenericGroupedObjectDialog)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelchangeListboxGraphics();
	virtual afx_msg void OnSelchangeListboxEffects();
	virtual afx_msg void	OnEditChangeBottomText();
	virtual afx_msg void	OnButtonShowBullets();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _SEALDIALOG_H_