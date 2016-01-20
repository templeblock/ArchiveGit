//****************************************************************************
//
// File Name:	ProjectQSLDlg.cpp
//
// Project:		Print Shop Launcher
//
// Author:		Lance Wilson
//
// Description: Implementation of QSL dialogs.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	             500 Redwood Blvd.
//               Novato, CA 94948
//	             (415) 382-4400
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/PSLaunchDLL/ProjectQSLDlg.cpp                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
#include "LaunchResource.h"
ASSERTNAME

#include <direct.h>

#include "ProjectQSLDlg.h"
#include "CommonTypes.h"
#include "PSLaunchDLL.h"

// PressWriter support
#include "Enums.h"
#include "FileFinder.h"

// Framework support
#include "CollectionManager.h"
#include "Printer.h"

using RPrinter::kPortrait;
using RPrinter::kLandscape;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_CD_SEARCH_DRIVES	50

/////////////////////////////////////////////////////////////////////////////
// CProjectQSLDlg dialog

const int kNumRows = 2;
const int kNumCols = 2;
const int kNumItems = kNumRows * kNumCols;

// wildcard filenames for all size and orientation combinations
static const char	*sizeOrientWildCards[] = 
{
	"?A*.QSL",				// 8-1/2 x 11 - Portrait
	"?B*.QSL",				// 8-1/2 x 11 - Landscape
	"?C*.QSL",				// 8-1/2 x 14 - Portrait
	"?D*.QSL",				// 8-1/2 x 14 - Landscape
	"?E*.QSL",				//         A4 - Portrait
	"?F*.QSL",				//         A4 - Landscape
};


CProjectQSLDlg::CProjectQSLDlg(CWnd* pParent /*=NULL*/)
	: CProjectDialog(CProjectQSLDlg::IDD, pParent)
	, m_nListIndex( -1 )
{
	//{{AFX_DATA_INIT(CProjectQSLDlg)
	m_nOrientation = kPortrait;
	m_nPaperSize = PAPER_LETTER;  // TODO: eDefaultPaper;
	m_fSampleGraphics = TRUE;
	m_fSampleText = TRUE;
	//}}AFX_DATA_INIT
}

CProjectQSLDlg::~CProjectQSLDlg()
{
	RCollectionCDLockManager::TheCollectionCDLockManager().UnlockAllDrives( );
}

void CProjectQSLDlg::DoDataExchange(CDataExchange* pDX)
{
	CProjectDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectQSLDlg)
	DDX_Control(pDX, IDC_LIST1, m_cQSLGrid);
	DDX_CBIndex(pDX, IDC_ORIENT, m_nOrientation);
	DDX_CBIndex(pDX, IDC_PAPER, m_nPaperSize);
	DDX_Check(pDX, IDC_SAMPLEGRAPHICS, m_fSampleGraphics);
	DDX_Check(pDX, IDC_SAMPLETEXT, m_fSampleText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectQSLDlg, CProjectDialog)
	//{{AFX_MSG_MAP(CProjectQSLDlg)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_PAPER, OnSelchangePaper)
	ON_CBN_SELCHANGE(IDC_ORIENT, OnSelchangeOrient)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// @mfunc <c CBPubApp> get a list of CDROM
//         drive letters in system.
//
int CProjectQSLDlg::GetCDROMDrives(
		char *szBuff) 
{
	int		nDrives = 0;
	char	szDrives[202];
	char	szRoot[5];

	// obtain drive letters for all drives in system (50 max)
	DWORD nSize = ::GetLogicalDriveStrings(200, szDrives);
	
	if (nSize > 0)
	{
		DWORD i = 0;
		char *srcptr = szDrives;
		
		// examine all drives
		while (i < nSize)
		{
			// get next drive
			char *dstptr = szRoot;
			while (*srcptr != '\0')
			{
				*dstptr++ = *srcptr++;
				i++;
			}
			*dstptr = *srcptr++;
			i++;

			if (*szRoot != '\0')
			{
				// determine drive type
				UINT nType = ::GetDriveType(szRoot);

				if (nType == DRIVE_CDROM)
				{
					// add drive letter to output buffer
					if (nDrives < 10)
						*szBuff++ = *szRoot;
					nDrives++;
				}
			}
		}
	}
	// terminate buffer
	*szBuff = '\0';

	return nDrives;
}

/////////
// @mfunc <c CBPubapp> function finds the QuickStart Layout directory.
// 1. Look the hard drive where the application is
// 2. Look on the $Install From Root$ registry entry
// 3. Look on the local CD ROM Drives
//
// @rdesc CString containing the path (empty on unrecoverable failure)
//
CString CProjectQSLDlg::FindQuickStartPath( short eProject )
{
	CString strQSLPath;						// string to return the path in
	char currentDir[_MAX_PATH];				// current directory

	_getcwd( currentDir, _MAX_PATH );		// save off the directory we are in

	// First look for the QSL dir in the directory of the application
//	if( strQSLPath.IsEmpty() )				
	{
		CString strPath = ((CPSLaunchDLLApp *) AfxGetApp())->FindModulePath();
		CString strDirQSL;					// the QSL directory
		strDirQSL.LoadString( IDS_DIR_QSL );
		CString strDirProj;					// the QSL project directory
		strDirProj.LoadString( IDS_DIR_NEWSLETTER + eProject );
		strPath += strDirQSL + "\\" + strDirProj;

		if( _chdir( strPath ) != -1 )
			strQSLPath = strPath;
		_chdir( currentDir );
	}

#if 0
	// TODO: look of registery setting

	// if the QSL dir is still empty look in the install from root
	if( strQSLPath.IsEmpty() )
	{
		char buf[1024];								// build the QSL path
		if( GetRegistryEntryInstallFromRoot( buf, 1024 ) )// the root
		{
			CString strDirQSL;						
			strDirQSL.LoadString( IDS_CDDIR_QSL );// the QSL dir on the CD
			strDirQSL += "\\";
			strcat( buf, strDirQSL);
			CString strDirProj;						// the QSL project dir on the CD
			strDirProj.LoadString( ID_CDDIR_QSLBASE + eProject );
			strcat( buf, strDirProj);
			strcat( buf, "\\" );

			if( _chdir( buf ) != -1 )
				strQSLPath = buf;
			_chdir( currentDir );
		}
	}
#endif

	// if the QSL dir is still empty look on all local CD ROM drives.
	if( strQSLPath.IsEmpty() )
	{
		char szDrives[MAX_CD_SEARCH_DRIVES];
		int n = GetCDROMDrives(szDrives);
		if( n != 0 )
		{
			char *srcptr = szDrives;

			CString strFullPath;				// build the CD path
			strFullPath += (char)*srcptr;		// the CD drive 
			strFullPath	+= ":\\";		
			CString strDirQSL;						
			strDirQSL.LoadString( IDS_CDDIR_QSL );// the QSL dir on the CD
			strFullPath += strDirQSL + "\\";
			CString strDirProj;				 // the QSL project dir on the CD
			strDirProj.LoadString( IDS_CDDIR_QSLNEWSLETTER + eProject );
			strFullPath += strDirProj + "\\";

			while( *srcptr++ != '\0')
			{
				if( _chdir( strFullPath ) != -1 )
				{
					strQSLPath = strFullPath;
					break;
				}
				strFullPath.SetAt( 0, *srcptr );
			} 
		}
		_chdir( currentDir );
	}

    return strQSLPath;
}

BOOL CProjectQSLDlg::InitImageList(	int nListIndex )
{
	// get name of directory containing QuickStart Layout files
	CString strQSLPath = FindQuickStartPath( _ProjectInfo.nProjectType - kPressWriterProjectStart );
	BOOL fResult = FALSE ;

	// change to QuickStart Layout directory
	VERIFY (_chdir (strQSLPath) == 0);

	// remove any existing path names and descriptions
	m_cQSLGrid.EmptyCache() ;
	m_cQSLGrid.ResetContent() ;

	// get array of pathnames matching wildcarded path
	CBFileFinder fileFinder;
	CArray<CString, CString> arrPathNames ;

	if (fileFinder.GetPathnames( sizeOrientWildCards[nListIndex], &arrPathNames ))
	{
		// get count of path names in list
		int nNumThumbnails = arrPathNames.GetSize();

		// for each path name found...
		for (int i = 0; i < nNumThumbnails; i++)
		{
			QSLItemData itemData ;
			itemData.SetPathName( strQSLPath + "\\" + arrPathNames[i] );

			if (itemData.m_strDesc.IsEmpty())
			{
				// TODO: retrieve "No Decription" string from resources
				// itemData.m_strDesc.LoadString( IDS_NODESCRIPTION );
			}

			m_cQSLGrid.AddItem( itemData );
		} 

		fResult = TRUE;
	} 

	m_nListIndex = nListIndex;

	return fResult ;
}

/////////////////////////////////////////////////////////////////////////////
// CProjectQSLDlg message handlers

BOOL CProjectQSLDlg::OnInitDialog() 
{
	CProjectDialog::OnInitDialog();
	
	BeginWaitCursor();
	
	m_cQSLGrid.SizeCellsToClient( kNumRows, kNumCols );

	// set default orientation based on project type
	switch (_ProjectInfo.nProjectType - kPressWriterProjectStart)
	{
		case PROJECT_BROCHURE:
		case PROJECT_BOOKLET:
			m_nOrientation = kLandscape;
			break;
		default:
			m_nOrientation = kPortrait;
			break;
	}

	CString strQSLPath = FindQuickStartPath( _ProjectInfo.nProjectType - kPressWriterProjectStart );
	CString strDrive = strQSLPath.Left(3);

	// determine drive type
	UINT nType = ::GetDriveType(strDrive);

	if (nType == DRIVE_CDROM)
	{
		RCollectionCDLockManager::TheCollectionCDLockManager().LockDrive( strDrive[0] );
	}

	UpdateData (FALSE);

	int nListIndex = (m_nPaperSize * 2) + m_nOrientation;
	InitImageList( nListIndex );

	EndWaitCursor();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CProjectQSLDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = (HBRUSH) ::GetStockObject( WHITE_BRUSH );
	
	return hbr;
}

void CProjectQSLDlg::OnOK() 
{
	CProjectDialog::OnOK();

	QSLItemData itemData;
	m_cQSLGrid.GetItemData( m_cQSLGrid.GetCurSel(), itemData );
	lstrcpy( _ProjectInfo.sQSLData.lpszQSLName, itemData.m_strFile );

	_ProjectInfo.sQSLData.fSampleText = m_fSampleText;
	_ProjectInfo.sQSLData.fSampleGraphics = m_fSampleGraphics;
}

void CProjectQSLDlg::OnSelchangePaper() 
{
	// dialog values to member variables
	UpdateData (TRUE);


	// associate image list with list control
	int nListIndex = (m_nPaperSize * 2) + m_nOrientation;
	if (nListIndex == m_nListIndex)
		return;

	InitImageList( nListIndex );
//	m_listCtrl.Invalidate();
}

void CProjectQSLDlg::OnSelchangeOrient() 
{
	// dialog values to member variables
	UpdateData (TRUE);

	// associate image list with list control
	int nListIndex = (m_nPaperSize * 2) + m_nOrientation;
	if (nListIndex == m_nListIndex)
		return;

	InitImageList( nListIndex );
//	m_listCtrl.Invalidate();
}
