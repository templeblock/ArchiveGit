/*
// $Workfile: CINSTALL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CINSTALL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:34   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:04   JAY
// Initial revision.
// 
//    Rev 1.15   28 Nov 1995 17:14:06   JAY
// 
//    Rev 1.14   27 Feb 1995 11:13:18   FRED
// More multi-level object stuff. Added type flags to CProductItem
// 
//    Rev 1.13   24 Feb 1995 17:19:30   FRED
// Added PF1 and CDL products
// 
//    Rev 1.12   04 Jan 1995 11:30:26   JAY
// Selective installation.
// 
//    Rev 1.11   21 Dec 1994 17:13:38   JAY
//  
// 
//    Rev 1.10   21 Dec 1994 14:57:52   JAY
//  
// 
//    Rev 1.9   16 Dec 1994 12:00:08   JAY
// Shows space required in product list.
// 
//    Rev 1.8   15 Dec 1994 13:11:18   JAY
//  
// 
//    Rev 1.7   14 Dec 1994 17:22:04   JAY
// New product scheme
// 
//    Rev 1.6   13 Dec 1994 18:28:52   FRED
//  
// 
//    Rev 1.5   06 Dec 1994 14:34:16   JAY
// Added Install congrats dialog.
// 
//    Rev 1.4   05 Dec 1994 11:38:12   JAY
//  
// 
//    Rev 1.3   17 Nov 1994 14:30:20   JAY
//  
// 
//    Rev 1.2   10 Nov 1994 15:00:04   JAY
//  
// 
//    Rev 1.1   10 Nov 1994 13:09:56   JAY
// Expanded the functionality of CInstallInfo.
// 
//    Rev 1.0   08 Nov 1994 15:53:08   JAY
// Initial revision.
*/

class CInstallInfo;

#ifndef __CINSTALL_H__
#define __CINSTALL_H__

class CInstallationData;

class CInstallInfo
{
public:
	CInstallInfo();
	virtual ~CInstallInfo();

	/*
	// Get a license request code for the selected products.
	*/

	void GetLRCFromProducts(CString& csLRC);

	/*
	// Single-pass method (like phone orders). Immediately license the
	// products which are selected.
	*/

	void LicenseSelectedProducts(void);

	/*
	// Two-pass method (like mail orders). Remember for now, then license later.
	*/

	void OrderSelectedProducts(const CString& csLRC);		// Remember...
	void LicenseProducts(const CString& csLAC);				// ...license

	/*
	// Setup the info about the products which are selected.
	*/

	void SetupProducts(CProductItems* pItems = NULL);

	// The installation data listing which products have been licensed.
	CInstallationData* m_pInstallationData;

	// Available/selected product information.
	COfferInformation* m_pOffer;

protected:
	void DoLicenseProducts(LPCSTR pszLRC);
	void LicenseProductItems(CProductItems* pItems, LPCSTR pszLRC);
	BOOL LicenseProducts(CProductItems* pItems, const CString& csLRC);
};

#endif					// #ifndef __CINSTALL_H__

