//****************************************************************************
//
// File Name:  CollectionGridBase.h
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Definition class for keyboard supported collection grid controls
//
// Portability: Windows Specific
//
// Developed by:   Broderbund Software
//	                500 Redwood Blvd.
//                 Novato, CA 94948
//	                (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/CollectionGridBase.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:04p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _COLLECTIONGRIDBASE_H_
#define _COLLECTIONGRIDBASE_H_

#include "WinGridCtrl.h"
#include "CollectionSearcher.h"

class RCollectionGridBase : public RGridCtrlBase
{
public:
	

								RCollectionGridBase( UINT uiStyles = kAddSpacing ) ;
	virtual					~RCollectionGridBase() ;

	virtual void			SetData( RSearchResultArray* pResultArray ) ;

protected:

	virtual void			CharToItem( UINT nKey, UINT nIndex ) ; 

	// Generated message map functions
	//{{AFX_MSG(RCollectionGridBase)
	afx_msg void			OnDestroy( );
	afx_msg void			OnTimer( UINT  nIDEvent );
	afx_msg LRESULT OnLbGetText( UINT wParam, LPTSTR lParam );
	afx_msg LRESULT OnLbGetTextLen( UINT wParam, LONG lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:

	RSearchResultArray*	m_pResultArray ;

private:

	CString					m_strBuffer ;
	UINT						m_uiTimer ;

} ;

#endif // _COLLECTIONGRIDBASE_H_