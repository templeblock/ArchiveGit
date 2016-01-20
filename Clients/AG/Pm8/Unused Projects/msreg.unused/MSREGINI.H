/*
// $Workfile: MSREGINI.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGINI.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.14   18 Jun 1997 08:49:28   Fred
// Merged in Sean's changes for NGI
// 
//	(don't have the MicroLogic PVCS config, so can't check into it!)
//	Rev 1.14	03 JUN 1997  20:27:00	SBS
//	Added DoRun() support keys (see MSREGDLL.CPP).
//
//    Rev 1.13   01 Oct 1996 10:39:54   FRED
//  
// 
//    Rev 1.12   26 Jun 1996 07:04:10   FRED
// International Registration
// 
//    Rev 1.11   22 Mar 1996 08:51:48   FRED
// Placeholders in MSREG.INI
// Mail registration form uses Configuration/Bonus from MSREG.INI
// 
//    Rev 1.10   11 Mar 1996 15:36:24   FRED
// Printed registration form
// 
//    Rev 1.9   29 Jan 1996 14:23:40   FRED
// Added key for auto run atom
// 
//    Rev 1.8   22 Dec 1995 09:38:50   JAY
// 
//    Rev 1.7   13 Dec 1995 12:35:20   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.6   12 Dec 1995 17:57:24   FRED
// ModemOnly and ParentPartNumber
// 
//    Rev 1.5   07 Dec 1995 14:05:40   FRED
// Added Version
// 
//    Rev 1.4   06 Dec 1995 09:52:36   JAY
// Added keys for "Mindscape" related text.
// 
//    Rev 1.3   28 Nov 1995 08:47:08   FRED
// Phone Switch
// 
//    Rev 1.2   22 Nov 1995 13:06:58   JAY
//  
// 
//    Rev 1.1   22 Nov 1995 10:26:18   FRED
// Merged in bonus changes from PMW version
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.3   20 Nov 1995 16:25:38   FRED
// Removed Title, added Email
// 
//    Rev 1.2   30 Oct 1995 12:12:22   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 15:35:52   FRED
//  
// 
//    Rev 1.0   26 Oct 1995 10:27:50   FRED
//  
*/ 

#ifndef __MSREGINI_H__
#define __MSREGINI_H__

////////////////////////////////////////////////////////////////////
// Special code used to enable bonus for modem registration.

#define	MODEM_REGISTRATION_CODE							"Modem"

////////////////////////////////////////////////////////////////////
// Calling number and default dialing options.

#define	DEFAULT_PREFIX										""
#define	DEFAULT_DIAL1										1

////////////////////////////////////////////////////////////////////
// MSREGUSR.INI

#define	MSREGINI_IniName									"MSREG.INI"

// Configuration section

#define	MSREGINI_Section_Configuration				"Configuration"
#define	MSREGINI_Configuration_Application			"Application"
#define	MSREGINI_Configuration_Version				"Version"
#define	MSREGINI_Configuration_ParentPartNumber	"ParentPartNumber"
#define	MSREGINI_Configuration_SkipCount				"SkipCount"
#define	MSREGINI_Configuration_RunsCount				"RunsCount"
#define	MSREGINI_Configuration_TimeInterval			"TimeInterval"
#define	MSREGINI_Configuration_Key						"Key"
#define	MSREGINI_Configuration_CDName					"CDName"
#define	MSREGINI_Configuration_DllPath				"DllPath"
#define	MSREGINI_Configuration_SkipAutoRunAtom		"SkipAutoRunAtom"
#define	MSREGINI_Configuration_Bonus					"Bonus"
#define	MSREGINI_Configuration_BonusShort			"BonusShort"
#define	MSREGINI_Configuration_BonusIconTitle		"BonusIconTitle"

////////////////////////////////////////////////////////////////////
// MSREGUSR.INI

#define	MSREGUSRINI_IniName								"MSREGUSR.INI"

// Dialing options section

#define	MSREGUSRINI_Section_DialingOptions			"DialingOptions"
#define	MSREGUSRINI_DialingOptions_Prefix			"Prefix"
#define	MSREGUSRINI_DialingOptions_DialOne			"DialOne"

// Customer information section

#define	MSREGUSRINI_Section_CustomerInfo				"CustomerInformation"
#define	MSREGUSRINI_CustomerInfo_CustomerNumber	"CustomerNumber"
#define	MSREGUSRINI_CustomerInfo_FirstName			"FirstName"
#define	MSREGUSRINI_CustomerInfo_LastName			"LastName"
#define	MSREGUSRINI_CustomerInfo_Company				"Company"
#define	MSREGUSRINI_CustomerInfo_Title				"Title"
#define	MSREGUSRINI_CustomerInfo_Address1			"Address1"
#define	MSREGUSRINI_CustomerInfo_Address2			"Address2"
#define	MSREGUSRINI_CustomerInfo_City					"City"
#define	MSREGUSRINI_CustomerInfo_State				"State"
#define	MSREGUSRINI_CustomerInfo_Zip					"Zip"
#define	MSREGUSRINI_CustomerInfo_Country				"Country"
#define	MSREGUSRINI_CustomerInfo_Phone				"Phone"
#define	MSREGUSRINI_CustomerInfo_Email				"Email"
#define	MSREGUSRINI_CustomerInfo_NeedsTax			"NeedsTax"
#define	MSREGUSRINI_CustomerInfo_CallingFrom		"CallingFrom"

// Application information section

#define	MSREGUSRINI_Application_Registered			"Registered"
#define	MSREGUSRINI_Application_RegistrationCode	"RegistrationCode"
#define	MSREGUSRINI_Application_SkipsLeft			"SkipsLeft"
#define	MSREGUSRINI_Application_LastTime				"LastTime"
#define	MSREGUSRINI_Application_RunsLeft			"RunsLeft"

#endif
