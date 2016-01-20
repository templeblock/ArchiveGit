/* (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from:
              MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

*****************************************************************************

                                dlgsize.h

*****************************************************************************
*/

#ifndef _DLGOBJS_H
#define _DLGOBJS_H

#include "ppmodal.h"


#define  	ALIGN_NUM_OBJECTS 	3
#define 	ICON_HEIGHT 		32
#define 	ICON_WIDTH			32
#define 	NUM_GROUPS 			128

class CObjAlgnDlg : public CPPViewModalDlg
{
public:
    CObjAlgnDlg(CServerView *pView, LPALIGNOBJECTS_PARMS lpData, CWnd* pParentWnd = NULL );

  	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	void DoReset();
	void AlignIcons();
	BOOL IsPrevGroup( WORD wGroupID, LPWORD wGroupIDs );
	void SetGroupList( WORD wGroupID, LPWORD wGroupIDs );
	void InitGroupList( LPWORD wGroupIDs );
	BOOL IsGroupLocked(WORD wGroupID);

	POINT InitPos[ALIGN_NUM_OBJECTS];

	LPALIGNOBJECTS_PARMS		lpAlignParms;
private:
};

class CObjPosDlg : public CPPViewModalDlg
{
public:
    CObjPosDlg(CServerView *pView, LPPOSOBJECTS_PARMS lpData, CWnd* pParentWnd = NULL );

  	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	LPPOSOBJECTS_PARMS		lpPosParms;
	int						Units;
	void InitSpin(LFIXED fSetX, LFIXED fSetY,int flag);
private:
};
#endif _DLGOBJS_H
