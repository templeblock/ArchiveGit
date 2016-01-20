//****************************************************************************
//
// File Name:	ProjectInfo.h
//
// Project:		Print Shop Launcher
//
// Author:		Lance Wilson
//
// Description: Definition class for project info.
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
//  $Logfile:: /PM8/PSLaunchDLL/ProjectInfo.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:26p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef _PROJECTINFO_H_
#define _PROJECTINFO_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "LaunchInterfaces.h"
#include "DocDefs.h"

class CProjectInfo
{
public:
				CProjectInfo();

	BOOL		GetProjectApp(EAppType& eAppType);

public:

	short		eProjectMethod;
	short		nProjectType;
	short		nProjectFormat;
	short		nPaperType;

	// PressWriter initialization
	SBDocDescription sDocDesc;

	// QSL Data
	QSLData			 sQSLData;
};

#endif // _PROJECTINFO_H_
