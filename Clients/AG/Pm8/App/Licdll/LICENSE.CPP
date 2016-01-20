// Licensee.cpp : Defines the initialization routines for the DLL.
//

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/LICENSE.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/31/98 1:14p Dennis
// Changed pending orders message
// 
// 2     8/20/98 4:13p Jayn
// LICENSE.DAT filename now sent from the app.
// Now supports exclusion filters.
// 
//    Rev 1.0   14 Aug 1997 15:36:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:50   Fred
// Initial revision.
// 
//    Rev 1.4   13 Jun 1997 15:16:52   Fred
// Fixed accumulating product list bug
// 
//    Rev 1.3   13 Jun 1997 11:30:58   Jay
// Initialize now returns error codes
// 
//    Rev 1.2   11 Jun 1997 11:10:42   Jay
// Changes
// 
//    Rev 1.1   10 Jun 1997 15:43:42   Fred
// Cleanup
// 
//    Rev 1.0   10 Jun 1997 14:53:06   Fred
// Initial revision.
// 
//    Rev 1.2   21 May 1997 11:57:04   johno
// Changes for 16 bit
// 
//    Rev 1.1   20 May 1997 16:11:18   johno
// Changes for PrintMaster integration
// 
// 
//    Rev 1.0   20 May 1997 10:42:00   johno
// Initial revision.
//
//   Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/


#include "stdafx.h"  
#pragma hdrstop
#include "license.h"

#include "bmpctl.h"
#include "cwmfctl.h"
#include "cprogbar.h"
#include "util.h"
#include "msregini.h"
#include "salestax.h"
#include "cinstall.h"
#include "fstream.h"

#include "upgdlg.h"

IMPLEMENT_SERIAL(CEncryptedFileInfoValue,CObject,1);

IMPLEMENT_SERIAL(CEncryptedFileInfoTable,CMapStringToOb,1);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

CString g_cstrCodeToMark;//global tells which product code to pre-mark for user to order.
                  //should probably encapsulate, but I ran out of time.

/////////////////////////////////////////////////////////////////////////////
// CLicenseApp

BEGIN_MESSAGE_MAP(CLicenseApp, CWinApp)
   //{{AFX_MSG_MAP(CLicenseApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//DLL entry points
LIC_ENTRYPOINT Initialize(LPCSTR szDllPath, LPCSTR szLicensePath, LPCSTR szOrderFile, LPCSTR szMsregIniFile, LPCSTR szFilters)
{
   return GetApp()->Initialize(szDllPath,szLicensePath,szOrderFile,szMsregIniFile, szFilters);
}

STR_ENTRYPOINT GetLastDllError()
{
   return (char FAR *)GetApp()->GetLastDllError();
}


LIC_ENTRYPOINT IsLicensedByCode(char FAR * szCode)
{  
   return GetApp()->IsLicensedByCode(szCode);
}

LIC_ENTRYPOINT IsLicensed(char FAR * szFileName,structEncryptedFileInfo * pinfo)
{  
   return GetApp()->IsLicensed(szFileName,pinfo);
}


extern "C" LIC_ENTRYPOINT License(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, char far * szCode)
{
   g_cstrCodeToMark = szCode;
   return GetApp()->Upgrade(hWnd, pNotify,nProductLevel,dwPPN);
}

BOOL_ENTRYPOINT CanClose()
{  
   return GetApp()->CanClose();
}


VOID_ENTRYPOINT BuildDataFile(BOOL bReallyBuild)
{
   GetApp()->BuildDataFile(bReallyBuild); 
}




/////////////////////////////////////////////////////////////////////////////
// CLicenseApp construction

CLicenseApp::CLicenseApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance

   m_pEventNotification = NULL;

   m_pInternationalInformationDll = NULL;
   m_pInternationalData = NULL;
   m_pEncryptedFileInfoTable = NULL;
}

CLicenseApp::~CLicenseApp()
{  
   if(m_pEncryptedFileInfoTable){
      delete m_pEncryptedFileInfoTable;
      m_pEncryptedFileInfoTable = NULL;
   }
   if(m_pInternationalInformationDll){
      delete m_pInternationalInformationDll;
      m_pInternationalInformationDll = NULL;
   }     
}


BOOL CLicenseApp::CanClose()
{
   if(GetApp()->Offer()->CountProductFlags(PRODUCT_ITEM_Selected) != 0)
   {
#pragma message("change caption here.")
      if(MessageBox(NULL,"You have selected products to order.\nDo you want to order them now?","Pending Orders",MB_ICONQUESTION|MB_YESNO) == IDYES){
         return FALSE;
      }
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLicenseApp object

CLicenseApp theApp;

BOOL CLicenseApp::InitInstance() 
{
   if (!CWinApp::InitInstance())
   {
      return FALSE;
   }

   CBitmapControl::RegisterMyClass();
   CWindowsMetaFileControl::RegisterMyClass();
   CProgressBar::RegisterMyClass();
   
/*
// Set default dialog background to be gray.
*/

   SetDialogBkColor();
   
/*
// Compute the working directory.
*/

   if (!SetupWorkingDirectory())
   {
      return FALSE;
   }

   return TRUE;

}

LICENSERESULT CLicenseApp::SaveDatabase()
{
   CFile cfDictionary;
   CString csPath = m_csDllPath;

      csPath += INFO_TABLE_NAME;
      if(!cfDictionary.Open( csPath, CFile::modeWrite|CFile::modeCreate)){
         m_csError = "ERROR, This file is missing: ";
         m_csError += INFO_TABLE_NAME;
         return LICENSERESULT_Missing_Critical_File; 
      }
      

   CArchive archive(&cfDictionary,CArchive::store);
   m_pEncryptedFileInfoTable->Serialize(archive);
   archive.Close();
   cfDictionary.Close();

   return LICENSERESULT_Success;

}

LICENSERESULT CLicenseApp::InitializeDatabase()
{
   CFile cfDictionary;
   CString csPath = m_csDllPath;

   if(!m_pEncryptedFileInfoTable){//allocate it and open, and read it.
      m_pEncryptedFileInfoTable = new CEncryptedFileInfoTable(); 

      csPath += INFO_TABLE_NAME;
      if(!cfDictionary.Open( csPath, CFile::modeRead)){
         m_csError = "ERROR, This file is missing: ";
         m_csError += INFO_TABLE_NAME;
         return LICENSERESULT_Missing_Critical_File; 
      }

      CArchive archive(&cfDictionary,CArchive::load);
      m_pEncryptedFileInfoTable->Serialize(archive);
      archive.Close();
      cfDictionary.Close();

   }
   return LICENSERESULT_Success;

}

void CLicenseApp::UpdateLicenseFlags()
{
   m_Offer.UpdateLicenseFlags(&m_InstallationData);
}

/*
// Figure out our "working" directory. It's the one we were loaded from.
*/

BOOL CLicenseApp::SetupWorkingDirectory(void)
{
#if 0//because now we're using m_csPath which is passe into the DLL.
   BOOL fResult = FALSE;

   CString csModuleFileName;

   m_csWorkingDirectory.Empty();

   TRY
   {
   /*
   // Setup the working directory.
   */

      fResult = (::GetModuleFileName(AfxGetInstanceHandle(), csModuleFileName.GetBuffer(_MAX_PATH+1), _MAX_PATH+1-1) != 0);
      csModuleFileName.ReleaseBuffer();

      if (fResult)
      {
         Util::SplitPath(csModuleFileName, &m_csWorkingDirectory, NULL);
         fResult = TRUE;
      }
   }
   END_TRY

   return fResult;
#endif
   return TRUE;
}




                            
void CEncryptedFileInfoValue::Serialize(CArchive& ar)
{
   CObject::Serialize(ar);
   if(ar.IsStoring()){
      ar.Write(info.m_szCode,sizeof info.m_szCode);
      ar << info.m_lFileOffset;
      ar << info.m_lFileLen;
      ar.Write(&info.m_iMaxRuns,sizeof info.m_iMaxRuns);
      ar.Write(&info.m_iNumRuns,sizeof info.m_iNumRuns);
   }else{
      ar.Read(info.m_szCode,sizeof info.m_szCode);
      ar >> info.m_lFileOffset;
      ar >> info.m_lFileLen;
      ar.Read(&info.m_iMaxRuns,sizeof info.m_iMaxRuns);
      ar.Read(&info.m_iNumRuns,sizeof info.m_iNumRuns);
   }     
}


CEncryptedFileInfoTable::~CEncryptedFileInfoTable()
{
   CString key;
   CEncryptedFileInfoValue * value;

   POSITION pos = GetStartPosition();
   while( pos != NULL ){
      GetNextAssoc(pos, key,(CObject*&)value);
      if(value){
         delete value;
         value = NULL;
      }
   }

}



void CEncryptedFileInfoTable::Serialize(CArchive& ar)
{
   CString key;
   CEncryptedFileInfoValue * value;
   int cnt;

   if(ar.IsStoring()){
      cnt = GetCount();
      ar.Write(&cnt,sizeof cnt);
      POSITION pos = GetStartPosition();
      while( pos != NULL ){
         GetNextAssoc(pos, key,(CObject*&)value);
         ar << key;
         ar << value;
      }
   }else{
      ar.Read(&cnt,sizeof cnt);
      for(int i = 0; i < cnt; ++i){
         ar >> key;
         value = new CEncryptedFileInfoValue();
         ar >> value;
         SetAt(key,value);
      }
   }

}

void CLicenseApp::BumpRunCnt(char FAR * pszFileName,structEncryptedFileInfo * info)
{

   info->m_iNumRuns++;

   CEncryptedFileInfoValue * FileInfo = new CEncryptedFileInfoValue(info->m_szCode,info->m_lFileOffset,info->m_lFileLen, info->m_iMaxRuns, info->m_iNumRuns);
   m_pEncryptedFileInfoTable->SetAt(pszFileName,FileInfo);
   if(SaveDatabase() != LICENSERESULT_Success)
      AfxMessageBox("Could not update database file");

}

LICENSERESULT CLicenseApp::GetEncryptionInfo(char FAR * szFileName,structEncryptedFileInfo * pinfo)
{
   CEncryptedFileInfoValue * info;
   LICENSERESULT retval;

   retval = InitializeDatabase();
   if(retval != LICENSERESULT_Success)
      return retval;
   
   if(m_pEncryptedFileInfoTable->Lookup(szFileName,(CObject*&)info)){
      strcpy(pinfo->m_szCode,(const char *)info->Code());
      pinfo->m_lFileOffset = info->FileOffset();
      pinfo->m_lFileLen = info->FileLen();
      pinfo->m_iMaxRuns = info->MaxRuns();
      pinfo->m_iNumRuns = info->NumRuns();
      return   LICENSERESULT_Success;
   }else{
      return LICENSERESULT_NA;
   }

}


LICENSERESULT CLicenseApp::Initialize(LPCSTR szDllPath, LPCSTR szLicenseDat, LPCSTR szOrderFile, LPCSTR szMsregIniFile, LPCSTR szFilters)
{
#ifndef WIN32
	//only allow this dll to run once.
   static BOOL bFirstTime = TRUE;
   if(!bFirstTime)
   {
      return LICENSERESULT_AlreadyInUse;	//instance is already running.  calling app should terminate this first.
   }
	bFirstTime = FALSE;
#endif

   m_csDllPath = szDllPath;
   m_csOrderFile = szOrderFile;
   m_csMsregIniFile = szMsregIniFile;
	if (szFilters != NULL)
	{
		// Break out the filters
		Util::FillArrayFromDelimitedString(szFilters, ", ", m_csaFilters);
	}

	//
   // Setup the correct INI file names.
   //

   if (!SetupIniFiles())
   {
      return LICENSERESULT_Error;
   }

/*
// Get the "application name" from MSREG.INI (SetupIniFile must have been called).
*/
   if (!SetupApplicationName())
   {
      return LICENSERESULT_InvalidMsregIni;
   }

   // Allocate the object used for computing international information.
   TRY
   {
      m_pInternationalInformationDll = new CInternationalInformationDll(m_csDllPath);
   }
   END_TRY

   if (m_pInternationalInformationDll == NULL)
   {
      AfxMessageBox("Can't load international information library (INTLxx.DLL).");
      return LICENSERESULT_IntlDLLNotFound;
   }

   // We can now get the sales tax information. Requires international data
   // and ini files to be set up.
   SalesTaxList.Init();

/*
// Read LICENSE.DAT. This just gets us a time-stamp, etc. we can
// use to generate LRCs. It also gets us the products.
*/

   TRY
   {
      m_InstallationData.Get(szLicenseDat);
   }
   END_TRY

	return LICENSERESULT_Success;
}


const char FAR * CLicenseApp::GetLastDllError()
{
   return m_csError;
}


/*LIC_ENTRYPOINT GetEncryptionInfo(char * szFileName,structEncryptedFileInfo * pinfo)
{
   return GetApp()->GetEncryptionInfo(szFileName, pinfo);
} */



void CLicenseApp::BuildDataFile(BOOL bReallyBuild)
{

   CFile cfDictionary,cfData,os,is;
   char FAR * buf1;
   char FAR * buf2;
   char FAR * pszCurrentSection;
   char FAR * pszCurrentEntry;      
   long lFileOffset = NULL;
   long lFileLen =NULL;
   DWORD retval;
   CString csIniFile;
   char buf[100];
   char buf3[50];
   char buf4[20];
   int iMaxRuns;
   BOOL bContinue;
   unsigned int i = 0;
   BOOL bAlreadyFoundRuns;
   
   buf1 = (char FAR *)malloc(INI_BUF_SIZE);
   buf2 = (char FAR *)malloc(INI_BUF_SIZE);  

   CString csPath = m_csDllPath;
   csPath += INFO_TABLE_NAME;

   AfxMessageBox("Any files that are added to the ms.dat file via the catbuild.ini will be deleted.\nBack-up those files now.");

   CFileException Error;

   TRY
   {
      cfDictionary.Open( csPath, CFile::modeCreate | CFile::modeWrite,&Error);
      csPath = m_csDllPath;
      csPath += ENCRYPTED_FILE_NAME;
      os.Open(csPath,CFile::modeCreate|CFile::typeBinary|CFile::modeWrite,&Error);

   }
   CATCH( CFileException, e)
   {
         strcpy(buf,"ERROR, Could not open one of these files: ");
          strcat(buf,csPath);
         AfxMessageBox(buf);
   }
   END_CATCH

// CArchive oa(&os,CArchive::store);
   if(bReallyBuild){
      strcpy(buf3,MINDSCAPE_NOTICE);
      os.Write(buf3,strlen(buf3));
      lFileOffset += strlen(buf3);
   }
   CEncryptedFileInfoTable EncryptedFileInfoTable; 

   //gets all setion names.
#pragma message("gets catbuild.ini here.")
   csIniFile = m_csOrderFile;
   csIniFile += "catbuild.ini";
   retval = GetPrivateProfileSectionNames(buf1,INI_BUF_SIZE,csIniFile);
   if(!strlen(buf1)){
      AfxMessageBox("ERROR: Could not open catbuild.ini or there are no sections in catbuild.ini");

   }
   if(retval >= INI_BUF_SIZE - 1)
      AfxMessageBox("ERROR: greater than 32K entries in catbuild.ini");
   pszCurrentSection = buf1;
   while(strcmp(pszCurrentSection,"")){//the end of the data is marked by an empty string.
      iMaxRuns = 0;
      csIniFile = m_csOrderFile;
      csIniFile += "catbuild.ini";
      i = 1;
      bAlreadyFoundRuns = FALSE;
      while(GetPrivateProfileString(pszCurrentSection,itoa(i,buf,10),"",buf2,INI_BUF_SIZE,csIniFile)){
         pszCurrentEntry = buf2;
         bContinue = TRUE;
         if(bReallyBuild){
            //see if the group is specified with a maximum number of runs.
            if(!bAlreadyFoundRuns && GetPrivateProfileString(pszCurrentSection,"FREE_RUNS","",buf4,20,csIniFile)){
               iMaxRuns = strtol(buf4,NULL,10);
               bContinue = FALSE;
               bAlreadyFoundRuns = TRUE;
            }else{
               i++;
               if(!is.Open(pszCurrentEntry,CFile::modeRead)){//open file that will get stuffed into the .dat file
                  strcpy(buf,"ERROR, Could not open input file: ");
                  strcat(buf,pszCurrentEntry);
                  AfxMessageBox(buf);
                  bContinue = FALSE;
               }else{
                  lFileLen  = is.GetLength();
            }  }
         }

         if(bContinue){
            CEncryptedFileInfoValue * FileInfo = new CEncryptedFileInfoValue(pszCurrentSection,lFileOffset,lFileLen, iMaxRuns, 0);

            //the key is the file name; the value contains the product code, the position/length in the .dat file.
            strupr(pszCurrentEntry);
            pszCurrentEntry += 2;//remove the drive specifier from the key.
            EncryptedFileInfoTable.SetAt(pszCurrentEntry,FileInfo);
            //actually remove the file from its directory and linnearly add it to the ms.dat file.
            if(bReallyBuild){
               //add the input to the .dat file.
               HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE,lFileLen);
               if(!hmem){
                   AfxMessageBox("Could not allocate memory for input file");
               }
               void FAR * buf = GlobalLock(hmem);
               is.ReadHuge(buf,lFileLen);
               os.WriteHuge(buf,lFileLen);
               GlobalUnlock(hmem);
               GlobalFree(hmem);
               is.Close();       
               is.Remove(pszCurrentEntry);
               lFileOffset += lFileLen;
            }
         }
         pszCurrentEntry += strlen(pszCurrentEntry) + 1;
      }
      pszCurrentSection += strlen(pszCurrentSection) + 1;
   }

   CArchive archive( &cfDictionary, CArchive::store);
   EncryptedFileInfoTable.Serialize(archive);
   archive.Close();
   cfDictionary.Close();

// oa.Close();
   os.Close();
   
   free(buf1);
   free(buf2);

//   return NULL;

}


LICENSERESULT CLicenseApp::IsLicensedByCode(char FAR * szCode)
{
   CInstallInfo Info;

   if(Info.m_pInstallationData->ProductIsLicensed(szCode))
      return LICENSERESULT_Success;
   else
      return LICENSERESULT_Not_Licensed;

}


LICENSERESULT CLicenseApp::IsLicensed(char FAR * szFileName,structEncryptedFileInfo * pinfo)
{

   CInstallInfo Info;
   LICENSERESULT retval;

   retval = GetEncryptionInfo(szFileName,pinfo);
   if(retval == LICENSERESULT_Success){//else return the error code.
      if(Info.m_pInstallationData->ProductIsLicensed(pinfo->m_szCode)){
         retval = LICENSERESULT_Success;
      }else{
         if(pinfo->m_iNumRuns >= pinfo->m_iMaxRuns){
            retval = LICENSERESULT_Not_Licensed;
         }else{
            BumpRunCnt(szFileName,pinfo);
            retval = LICENSERESULT_Success;
         }
      }
   }
   return retval;
}


LICENSERESULT CLicenseApp::Upgrade(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN)
{
   /*****************************************************************************
   * For the moment, we are limiting the use of this DLL to one user at a time. *
   * It could be extended for multiple simultaneous clients in the future.      *
   *****************************************************************************/

   /*
   // Lock temporary maps.
   */

   AfxLockTempMaps();

   /*
   // Init the offer.
   */

   TRY
   {
      GetApp()->InitOffer(nProductLevel, dwPPN);
   }
   CATCH(CFileException,e )
   {
       m_csError = "An entry in the following .ini file was not found: ";
       m_csError += e->m_cause;
       return LICENSERESULT_Missing_Critical_File;
    }

    END_CATCH


   /*
   // Plug in the event notification function.
   */

   GetApp()->SetEventNotification(pNotify);

   /*
   // Default result code.
   */

   GetApp()->SetUpgradeResult(LICENSERESULT_Not_Licensed);

   /*
   // Do the upgrade dialog.
   */


   GetApp()->UpdateLicenseFlags();

   CUpgradeDialog Dialog(CWnd::FromHandle(hWnd));
   Dialog.DoModal();

   /*
   // Free any international data that has been allocate
   */
   GetApp()->FreeInternationalData();

   /*
   // Unlock temporary maps.
   */

   AfxUnlockTempMaps();

   /*
   // And go home...
   */

   return GetApp()->GetUpgradeResult();
}

/*
// Initialize the offer.
*/

void CLicenseApp::InitOffer(int nProductLevel, DWORD dwPPN)
{
	if (m_Offer.OfferCode().IsEmpty())
	{
		m_Offer.Init(m_csOrderFile);//, nProductLevel, dwPPN);
	   m_InstallationData.OfferCode(m_Offer.OfferCode());
	}
}

/*
// Set up the INI file name member to point to the correct file.
// Also, setup the other necessary INI files.
*/

BOOL CLicenseApp::SetupIniFiles(void)
{
   CString csIniName;
   CString csWindowsDirectory;
   BOOL fResult = FALSE;

   TRY
   {
   /*
   // Set the MSREG.INI file name.
   */
      m_MsRegIniFile.Name(m_csMsregIniFile);

   /*
   // Set the MSREGUSR.INI file name.
   */

      GetWindowsDirectory(csWindowsDirectory.GetBuffer(256), 256-1);
      csWindowsDirectory.ReleaseBuffer();

      Util::ConstructPath(csIniName, csWindowsDirectory, "MSREGUSR.INI");
      m_UserIniFile.Name(csIniName);

      fResult = TRUE;
   }
   END_TRY

   if (!fResult)
   {
      AfxMessageBox("Memory error initializing INI file names.");
   }

   return fResult;
}

BOOL CLicenseApp::SetupApplicationName(void)
{
   m_csApplicationName = m_MsRegIniFile.GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Application, "");
   m_csParentPartNumber = m_MsRegIniFile.GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber, "");
   return !m_csApplicationName.IsEmpty() && !m_csParentPartNumber.IsEmpty();

}

BOOL CLicenseApp::UserNeedsTax(void)
{
   return UserIniFile().GetInteger(MSREGUSRINI_Section_CustomerInfo,
                                   MSREGUSRINI_CustomerInfo_NeedsTax,
                                   FALSE);
}

void CLicenseApp::UserNeedsTax(BOOL fNeedsTax)
{
   UserIniFile().WriteInteger(MSREGUSRINI_Section_CustomerInfo,
                              MSREGUSRINI_CustomerInfo_NeedsTax,
                              fNeedsTax);
}

/*
// Notify the user (of this DLL) of some event.
*/

void CLicenseApp::EventNotify(int nEvent, HWND hWnd, int nID, DWORD dwData /*=0*/)
{
   if (m_pEventNotification != NULL)
   {
      m_pEventNotification(nEvent, hWnd, nID, dwData);
   }
}

void CLicenseApp::WinHelp(DWORD dwData, UINT nCmd /*=HELP_CONTEXT*/)
{
   EventNotify(LICENSE_EVENT_Help, NULL, nCmd, dwData);
}

/*
// Set the event notification function.
*/

void CLicenseApp::SetEventNotification(EVENT_NOTIFY_FUNCTION pEventNotification)
{
   m_pEventNotification = pEventNotification;
}

// Make a phone number.

CString CLicenseApp::MakePhoneNumber(CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, BOOL fIncludePrefix /*=FALSE*/)
{
   CString csNumber;
   TRY
   {
      if (fIncludePrefix)
      {
         csNumber += GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_Prefix, DEFAULT_PREFIX);
         int nLength = csNumber.GetLength();
         if (nLength > 0 && csNumber[nLength-1] != ',')
         {
            csNumber += ',';
         }
      }
         
      csNumber += pPhoneNumber->m_szPrefix;

      // Process "Dial 1" option if 
      if ((pPhoneNumber->m_fUnitedStatesLocal)
       && (pInternationalData->m_fUnitedStatesLocal))
      {
         if (UserIniFile().GetInteger(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_DialOne, DEFAULT_DIAL1))
         {
            csNumber += "1-";
         }
      }

      csNumber += pPhoneNumber->m_szNumber;
   }
   END_TRY

   return csNumber;
}

// Make a mailing address string.

CString CLicenseApp::MakeMailAddress(CInternationalData* pInternationalData, CInternationalData::MailAddress* pMailAddress)
{
   CString csAddress;
   CNameList csAddressList;
   TRY
   {
      csAddressList.SetNames(pMailAddress->m_szAddress);
      for (int nLine = 0; nLine < csAddressList.GetSize(); nLine++)
      {
         if (!csAddress.IsEmpty())
         {
            csAddress += '\n';
         }
         csAddress += csAddressList.ElementAt(nLine);
      }
   }
   END_TRY

   return csAddress;
}

// Wrappers for CInternationalData methods.

BOOL CLicenseApp::GetInternationalData(WORD wCountryId, CInternationalData* pData)
{
   BOOL fResult = FALSE;
   if (m_pInternationalInformationDll != NULL)
   {
      fResult = m_pInternationalInformationDll->GetInternationalData(wCountryId, MsRegIniFile().Name(), pData);
   }
   return fResult;
}

BOOL CLicenseApp::FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
{
   BOOL fResult = FALSE;
   if (m_pInternationalInformationDll != NULL)
   {
      fResult = m_pInternationalInformationDll->FillCountryComboBox(hComboBox, wSelectCountryId);
   }
   return fResult;
}

BOOL CLicenseApp::GetCountryName(WORD wCountryId, CString& csCountryName)
{
   BOOL fResult = FALSE;
   if (m_pInternationalInformationDll != NULL)
   {
      TRY
      {
         fResult = m_pInternationalInformationDll->GetCountryName(wCountryId, csCountryName);
      }
      END_TRY
   }
   return fResult;
}

BOOL CLicenseApp::GetWebSite(WORD wCountryId, CString& csWebSite)
{
   BOOL fResult = FALSE;
   if (m_pInternationalInformationDll != NULL)
   {
      TRY
      {
         fResult = m_pInternationalInformationDll->GetWebSite(wCountryId, MsRegIniFile().Name(), csWebSite);
      }
      END_TRY
   }
   return fResult;
}

BOOL CLicenseApp::GetTaxInformation(CString& csTaxInformation)
{
   BOOL fResult = FALSE;
   if (m_pInternationalInformationDll != NULL)
   {
      TRY
      {
         fResult = m_pInternationalInformationDll->GetTaxInformation(MsRegIniFile().Name(), csTaxInformation);
      }
      END_TRY
   }
   return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyControl

WNDPROC CReadOnlyControl::m_pfnSuperWndProc = NULL;

CReadOnlyControl::CReadOnlyControl()
{
}

CReadOnlyControl::~CReadOnlyControl()
{
}

BEGIN_MESSAGE_MAP(CReadOnlyControl, CWnd)
   //{{AFX_MSG_MAP(CReadOnlyControl)
   ON_WM_MOUSEACTIVATE()
   ON_WM_SETCURSOR()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyControl message handlers

int CReadOnlyControl::OnMouseActivate(CWnd* pDesktopWnd, UINT uHitTest, UINT uMessage)
{
   return MA_NOACTIVATEANDEAT;
}

BOOL CReadOnlyControl::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage)
{
   ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
   return TRUE;
}
                          
                          
#ifndef WIN32
DWORD GetPrivateProfileSectionNames(char FAR * buf,int size_buf,CString file_name) 
{                                          
   char FAR * buf2 = (char FAR *)malloc(size_buf); 
   _fmemset(buf2,'\0',size_buf);
   int cnt = 0;
   
   CFile file(file_name,CFile::modeRead);      
// CArchive ar(&file,CArchive::load);

   unsigned int len = file.ReadHuge(buf2,size_buf);
   for(unsigned int i = 0; i < len; ++i){
      if(buf2[i] == '['){
         for(unsigned int j = i; j < len; ++j){
            if(buf2[j] == ']'){
               strncpy(&buf[cnt],&buf2[i+1],j-i-1);
               cnt += j-i;
               buf[cnt-1] = '\0';
               break;
            }
         }
      }        
   } 
   free(buf2);           
   //ar.Close();
   file.Close();
   
   return cnt;
   
}   


DWORD GetPrivateProfileSection(char FAR * CurrentSection,char FAR * buf,int size_buf,CString file_name)
{
   _fmemset(buf,'\0',size_buf);

   return GetPrivateProfileString(CurrentSection,  NULL, "", buf, size_buf, file_name);
}  

#endif                        

