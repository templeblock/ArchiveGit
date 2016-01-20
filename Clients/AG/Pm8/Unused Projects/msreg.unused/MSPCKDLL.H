/*
// $Workfile: MSPCKDLL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSPCKDLL.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.9   23 Oct 1996 01:59:56   FRED
// International Catalog and Instant Upgrade structures
// 
//    Rev 1.8   24 Sep 1996 09:51:58   FRED
// Added SystemInformation to registration packet
// 
//    Rev 1.7   10 Jul 1996 12:47:04   FRED
// International registration fields.
// 
//    Rev 1.6   11 Mar 1996 16:31:04   FRED
// New registration packet
// 
//    Rev 1.5   04 Jan 1996 12:03:40   FRED
// Removed collections from Product Order Request packets.
// 
// Added Product Information Request Packets.
// 
// 
//    Rev 1.4   21 Dec 1995 15:55:24   FRED
// Product Orders
// 
//    Rev 1.3   12 Dec 1995 17:56:56   FRED
// ParentPartNumber
// 
//    Rev 1.2   12 Dec 1995 13:29:20   FRED
// Product version
// 
//    Rev 1.1   05 Dec 1995 12:23:18   FRED
// Catalog orders
// 
//    Rev 1.0   04 Dec 1995 07:52:56   FRED
// Initial revision.
// 
//    Rev 1.1   28 Nov 1995 16:22:28   FRED
// Added Email
// 
//    Rev 1.0   01 Nov 1995 08:32:24   FRED
// Initial revision.
*/ 

// MSPCKDLL.h : main header file for the MSPCKDLL DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CMspckDllApp;

#ifndef __MSPCKDLL_H__
#define __MSPCKDLL_H__

#include "resource.h"
#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// CMspckDllApp
// See MSPCKDLL.cpp for the implementation of this class
//

class CMspckDllApp : public CWinApp
{
public:
	CMspckDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMspckDllApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMspckDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*
// A little helper function to get the app.
*/

#define GetApp()		((CMspckDllApp*)AfxGetApp())

/*
// Request types and current versions.
*/

enum
{
	REQUESTPACKETTYPE_Registration				=	1,
	REQUESTPACKETVERSION_Registration			=	1,
	
	REQUESTPACKETTYPE_CatalogOrder				=	2,
	REQUESTPACKETVERSION_CatalogOrder			=	1,
	
	REQUESTPACKETTYPE_ProductOrder				=	3,
	REQUESTPACKETVERSION_ProductOrder			=	1,
	
	REQUESTPACKETTYPE_ProductInformation		=	4,
	REQUESTPACKETVERSION_ProductInformation	=	1
};

/*
// Client Structure filled in with registration request information.
*/

#pragma pack(1)

struct RegistrationRequestInformation
{
	WORD RequestType;
	WORD RequestVersion;
	
	BYTE ProductName[80+1];
	BYTE ProductVersion[64+1];
	BYTE ParentPartNumber[6+1];
	BYTE LastName[20+1];
	BYTE FirstName[15+1];
	BYTE Company[40+1];
	BYTE Title[35+1];
	BYTE Address1[30+1];
	BYTE Address2[30+1];
	BYTE City[20+1];
	BYTE State[15+1];
	BYTE Zip[15+1];
	BYTE Country[50+1];
	BYTE Phone[24+1];
	BYTE Email[255+1];
	BYTE OfferCode[20+1];
	BYTE SystemInformation[80+1];
};
	
#pragma pack()

/*
// Client Structure filled in CD Catalog order request information.
*/

#pragma pack(1)

struct CatalogOrderRequestInformation
{
	WORD RequestType;
	WORD RequestVersion;
	
	BYTE ProductName[80+1];
	BYTE ProductVersion[64+1];
	BYTE ParentPartNumber[6+1];
	BYTE LastName[20+1];
	BYTE FirstName[15+1];
	BYTE Company[40+1];
	BYTE Title[35+1];
	BYTE Address1[30+1];
	BYTE Address2[30+1];
	BYTE City[20+1];
	BYTE State[15+1];
	BYTE Zip[15+1];
	BYTE Country[50+1];
	BYTE Phone[24+1];
	BYTE Email[255+1];
	BYTE PaymentMethod[3+1];		// VSA | MC | AMX | DIS
	BYTE CardNumber[19+1];
	BYTE ExpirationDate[10+1];
	BYTE OfferCode[10+1];			// e.g., MLS4
	BYTE RequestCode[3+1];			// *** ONLY USED IN CATALOG ORDERS ***
	DWORD Tax;							// D.CC * 100
	DWORD Shipping;
	DWORD Total;
	WORD LineItemCount;				// number of line item structures that follow
		
	// Line Item Structures follow.
};
	
struct CatalogOrderRequestLineItemInformation
{
	WORD LineItemNumber;				// for validation
	BYTE Category[3+1];				// e.g., MGL
	WORD Quantity;
	DWORD Cost;							// total line item cost (unit cost*quantity)
	WORD CollectionCount;			// number of collection names that follow
		
	// Collection names follow. This is series of pascal strings
	// placed end-to-end.
};
	
#pragma pack()

/*
// Client Structure filled in CD Product Catalog order request information.
*/

#pragma pack(1)

typedef CatalogOrderRequestInformation ProductOrderRequestInformation;
	
struct ProductOrderRequestLineItemInformation
{
	WORD LineItemNumber;				// for validation
	BYTE ParentPartNumber[6+1];	// six digit parent part number
	WORD Quantity;
	DWORD Cost;							// total line item cost (unit cost*quantity)
	DWORD Flags;						// 32-bit flags value for line item
											// 0x00000001 - information only
};

#pragma pack()

/*
// Client Structure filled in CD Product Catalog information request information.
*/

#pragma pack(1)

typedef CatalogOrderRequestInformation ProductInformationRequestInformation;
	
struct ProductInformationRequestLineItemInformation
{
	WORD LineItemNumber;				// for validation
	BYTE ParentPartNumber[6+1];	// six digit parent part number
	WORD Quantity;
	DWORD Cost;							// total line item cost (unit cost*quantity)
	DWORD Flags;						// 32-bit flags value for line item
											// 0x00000001 - information only
};

#pragma pack()

/////////////////////////////////////////////////////////////////////////////

#endif
