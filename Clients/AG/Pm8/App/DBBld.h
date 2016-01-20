//////////////////////////////////////////////////////////////////////////////
// $DBBld: /PMW/DBBLD.H 2     5/31/98 5:19p Hforman $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/DBBld.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 8     2/25/99 3:43p Lwu
// added selected glow state to buttons
// 
// 7     2/22/99 4:32p Lwu
// added restore btn to the enum
// 
// 6     2/17/99 4:27p Lwu
// Added a few columns to handle btnType and other control types
// 
// 5     2/16/99 5:28p Mwilson
// added tile flag
// 
// 4     2/10/99 11:33a Mwilson
// added ctrl id
// 
// 3     2/05/99 11:29a Mwilson
// added classes for new collection handling for CreataCard
// 
// 2     1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 1     1/27/99 3:41p Mwilson
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __DBBLD_H__
#define __DBBLD_H__

#include <string.h>  // for memset()
#include <ctype.h>

#include "error.h"
#include "sortarry.h"
#include "compdb.h"
#include "BaseBld.h"
#include "ssHeaderArray.h"

#pragma pack(1)   // Pack all structures used in this file

class CPCompDBCollBuilder : public CPBaseCollBuilder
{
public:

	CPCompDBCollBuilder(	const CString &csSpreadSheetFilePath,
								const CString &csCollectionPath,
								const CString &csCollectionName);

	~CPCompDBCollBuilder();

	virtual int DoBuild();
protected:

	virtual int   ProcessLine();
	virtual int	ValidateFields();
	virtual int OpenAll();
   virtual int	CloseAll();
	virtual CString GetCollectionName(){return m_csCollectionName;}

	void	GetResolution(int nStartColumn, int nEndColumn);
	CPUIImageInfo::ImageType GetImageTypeFromFlag(int nStartColumn, int nEndColumn);


   CPUIImageInfo::ImageType	m_nType;
	CSize					m_csizeResolution;
	CString				m_csCollectionName;

	StorageFile			m_storageFile;
	CCIndexFile			m_cindexFile;
	CSortedDatabaseObject		m_componentDB;
	CCIndexIndexList	m_cindexList;

};

class CPControlInfo : public CObject
{
public:
	DECLARE_SERIAL(CPControlInfo);

	CPControlInfo();
	void Serialize(CArchive &ar);
	int		nSize;
	int		nControlType;
	int		nBtnType;
	CPoint	cpPosition;
	CSize		czSize;
	CString	csBmpUp;
	CString	csBmpDown;
	CString	csBmpGlow;
	CString	csBmpSelGlow;
	CString	csBmpDisabled;
	CString	csBmpDefault;
	CString	csCaption;
	CString	csTooltip;
	CString	csFontName;
	BOOL		bTiled;
	int		nFontSize;
	int		nCtrlID;
	inline const CPControlInfo& operator=( const CPControlInfo& rhs )
						{
							nSize = rhs.nSize;
							nControlType = rhs.nControlType;
							nBtnType = rhs.nBtnType;
							cpPosition = rhs.cpPosition;
							czSize = rhs.czSize;
							csBmpUp = rhs.csBmpUp;
							csBmpDown = rhs.csBmpDown;
							csBmpGlow = rhs.csBmpGlow;
							csBmpDisabled = rhs.csBmpDisabled;
							csBmpDefault = rhs.csBmpDefault;
							csCaption = rhs.csCaption;
							csTooltip = rhs.csTooltip;
							bTiled = rhs.bTiled;
							csFontName = rhs.csFontName;
							nFontSize = rhs.nFontSize;
							nCtrlID = rhs.nCtrlID;
							return *this;
						}

};

class CPCompPage : public CObject
{
public:
	
	DECLARE_SERIAL(CPCompPage);


	CPCompPage();
	~CPCompPage();
	void Serialize(CArchive& ar);

	//init functions
	void SetPageName(CString csPageName){m_csPageName = csPageName;}
	void AddCtrlInfo(CPControlInfo* pInfo){m_listCtrls.AddTail(pInfo);}
	void Reset();
	
	//accessor functions
	CString GetPageName(){return m_csPageName;}
	CTypedPtrList<CObList, CPControlInfo*>* GetControls(){return &m_listCtrls;}
protected:
	CString				m_csPageName;
	CTypedPtrList<CObList, CPControlInfo*> m_listCtrls;

};

class CPCompPageCollBuilder : public CPBaseCollBuilder
{
public:

	CPCompPageCollBuilder(	const CString &csSpreadSheetFilePath,
								const CString &csCollectionPath,
								const CString &csCollectionName);

	~CPCompPageCollBuilder();

	enum ControlType
	{
		BKGND = 0,
		BUTTON,
		RADIO,
		CHECKBOX,
		STATIC,
		LISTBOX,
		COMBOBOX,
		BORDER,
		CORNER,
		TITLEBAR,
		ListEnd
	};

	enum ButtonType
	{
			STDBTN = 0,
			SYSCLOSEBTN,
			SYSMINBTN,
			SYSMAXBTN,
			SYSRESTOREBTN,
			SCROLLUPBTN,
			SCROLLDOWNBTN,
			SCROLLTHUMBBTN
	};

	virtual int DoBuild();
protected:

	virtual int   ProcessLine();
	virtual int	ValidateFields();
	virtual int OpenAll();
   virtual int	CloseAll();
	virtual CString GetCollectionName(){return m_csCollectionName;}

	ControlType GetControlTypeFromFlag(int nStartColumn, int nEndColumn);
	int			AddItem();
	void			AddCtrlID(CString& csCtrlName);


	CString				m_csCollectionName;

	BOOL					m_bNewPage;
	BOOL					m_bFirstPage;

	StorageFile			m_storageFile;
	CCIndexFile			m_cindexFile;
	CSortedDatabaseObject		m_componentDB;
	CCIndexIndexList	m_cindexList;
	int					m_nID;
	CStdioFile			m_fileIDs;
	
	CPCompPage m_compPage;

};

inline CPCompPageCollBuilder::ControlType operator++(CPCompPageCollBuilder::ControlType &refType,int)
{
   return refType = (CPCompPageCollBuilder::ControlType)(refType+1);
}

#pragma pack()   // back to default packing


#endif
