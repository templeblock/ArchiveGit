//****************************************************************************
//
// File Name:	ProjectInfo.h
//
// Project:		Print Shop Launcher
//
// Author:		Lance Wilson
//
// Description: Implementation of project info class.
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
//  $Logfile:: /PM8/PSLaunchDLL/ProjectInfo.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "stdafx.h"
#include "LaunchResource.h"
#include "ProjectInfo.h"

CProjectInfo::CProjectInfo()
	: eProjectMethod( 0 )
	, nProjectType( -1 )
	, nProjectFormat( -1 )
	, nPaperType( -1 )
{
}

BOOL CProjectInfo::GetProjectApp(EAppType& eAppType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// Map project type to application
	//
	if (nProjectType >= kLeapFrogProjectStart)
		eAppType = kAppWebSiteDesigner;
	else if (nProjectType >= kPressWriterProjectStart)
		eAppType = kAppPressWriter;
	else if (nProjectType >= 0)
		eAppType = kAppPrintShop;
	else
		return FALSE;

	return TRUE;
}

