/*
// $Workfile: ADONINST.CPP $Revision:   1.0  $
// $Date: 3/03/99 6:02p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/ADONINST.CPP $
// 
// 1     3/03/99 6:02p Gbeddow
// 
// 2     9/21/98 5:25p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:18:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:06   Fred
// Initial revision.
// 
//    Rev 1.7   11 Aug 1997 16:51:46   johno
// Split DoAddonInstall into DoAddonInstall and copyfiles
// 
//    Rev 1.6   08 Aug 1997 14:55:16   johno
// Allow abort during copy
// 
//    Rev 1.5   08 Aug 1997 10:11:44   johno
// 
//    Rev 1.4   07 Aug 1997 17:29:38   johno
// Changes for updating, allowing file rename
// 
//    Rev 1.3   30 Jul 1997 16:53:14   johno
// Added 2nd progress bar
// 
//    Rev 1.2   19 Jun 1997 18:08:52   johno
// 
//    Rev 1.1   19 Jun 1997 11:21:18   johno
// 
//    Rev 1.0   18 Jun 1997 16:38:52   johno
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "aprogres.h"
#include "pmaodlg.h"
#include "pmwcfg.h"
#include "util.h"
#include "file.h"
#include "pmwdlg.h"

#ifdef WIN32
#include <..\src\afximpl.h>         // for AfxGetSafeOwner()
#else
#include <auxdata.h>       // for _AfxGetSafeOwner()
#endif

//#define  JTRACE_ON
#include "jtrace.h"
/*
#define  RELEASEBUILDSUCKS
#ifdef   RELEASEBUILDSUCKS
#define  MARK(s) AfxMessageBox (s);
#else
#define  MARK(s)(void)0
#endif
*/
ERRORCODE 
addon_copy_file(StorageDevicePtr source_file, StorageDevicePtr dest_file, CAddonProgressDialog *pDlg)
{
	ERRORCODE error;
	ST_DEV_POSITION size, progress;

/* Initialize the destination file. */

	if ((error = dest_file->initialize()) == ERRORCODE_None)
	{
	/* Allocate some memory to use as a buffer. */

		if ((error = source_file->length(&size)) == ERRORCODE_None && size != 0)
		{
         if (pDlg != NULL)
            pDlg->SetMinMaxPos2 (0, size, 0);

			LPVOID buffer = NULL;

		/* Make sure we don't try to allocate too much. */

			if (size > MAX_ALLOC_SIZE)
			{
				size = MAX_ALLOC_SIZE;
			}

			TRY
			{
				do
				{
					TRY
					{
						buffer = (LPVOID)(new char[(int)size]);
					}
					CATCH_ALL(e)
					{
						buffer = NULL;
					}
					END_CATCH_ALL

					if (buffer != NULL)
					{
						break;
					}
					size >>= 1;
				} while (size != 0);

				// Check the result of the allocation.

				if (buffer == NULL)
				{
					error = ERRORCODE_Memory;
				}
				else
				{
				/* We got some memory. */
				/* Start at the front of each file. */

					source_file->seek(0L, ST_DEV_SEEK_SET);
					dest_file->seek(0L, ST_DEV_SEEK_SET);

				/* Loop reading and writing until we can read no more. */

					do
					{
						ST_DEV_IO_SIZE amount_read;

						if ((error = source_file->read(buffer, (ST_DEV_IO_SIZE)size, &amount_read)) == ERRORCODE_None)
						{
						/* See if we read anything. */

							if (amount_read == 0)
							{
							/* All done! */
								break;
							}

						/* Write the data we read. */

							error = dest_file->write(buffer, amount_read);
						}
                  
                  if (pDlg != NULL)
                  {
                     if (pDlg->CheckForAbort () == TRUE)
                        error = ERRORCODE_Abort;

                     source_file->tell (&progress);
                     pDlg->SetPos2 (progress);
                     pDlg->ShowProgress ();
                  }
               } while (error == ERRORCODE_None);
				}
			}
			CATCH_ALL(e)
			{
   			delete [] (char*)buffer;
            THROW_LAST();
			}
			END_CATCH_ALL

			// Free the buffer we allocated.
			delete [] (char*)buffer;
		}
	}
	return error;
}

void
SplitMultipleFilename (LPCSTR in, CString &out1, CString &out2)
{
   int   i, j, l;
   
   l = strlen (in);
   for (i = l - 1; i >= 0; i--)
   {
      if (in [i] == '>')
         break;
   }

   if (i < 0)                                      // Not a multi name
   {
      out1 += in;
      out2 += in;
   }
   else
   {
      for (j = 0; j < i; j++)
            out1 += in [j];                  // Append 2nd name to output

      for (j = i + 1; j < l; j ++)   
            out2 += in [j];                  // Append 2nd name to output
   }
}

//#define PREPARE_UNINSTALL

ERRORCODE
CopyFiles (LPCSTR pKeys, CIniFile &INIFile, CString &PathSource, CString &PathDestination, CAddonProgressDialog *pDlg)
{
   DWORD       NumFiles, NumCopied;
   LPCSTR      pSection;
   ERRORCODE   error = ERRORCODE_Fail;

   if (pDlg != NULL)
   {
      for (NumFiles = 0, pSection = pKeys; *pSection != '\0'; NumFiles++, pSection += strlen(pSection)+1)
      {
      }

      pDlg->SetMinMaxPos (0, NumFiles, 0);
      CString t;
      t.LoadString (IDS_COPYING);
      pDlg->ShowAction (t);
      pDlg->ShowProgress ();
   }

#ifdef PREPARE_UNINSTALL
   // Create an INI file at destination for uninstall
   CString PathUninstallINI = PathDestination;
   Util::AppendBackslashToPath (PathUninstallINI);
   // Get the name from ADDON.INI
   PathUninstallINI += INIFile.GetString ("UNINSTALL", "FileName", "uninst.ini");
   CIniFile UninstallINI (PathUninstallINI);
#endif

   for (NumCopied = 0, pSection = pKeys; *pSection != '\0'; pSection += strlen(pSection) + 1, NumCopied ++)
   {
      CString FileNameDestination = PathDestination;

      CString FileNameSource = PathSource;
JTRACE1("Got section: %s\n", pSection);
      CString FileName = INIFile.GetString ("INSTALLFILES", pSection);
      if (FileName.IsEmpty ())
      {
#ifdef _DEBUG                  
         AfxMessageBox ("Installation failed (file name)");
#endif
         break;
      }

      if (pDlg != NULL)
         pDlg->ShowItem (FileName);

      SplitMultipleFilename (FileName, FileNameSource, FileNameDestination);

JTRACE2("Copying %s to %s\n", FileNameSource, FileNameDestination);

      ReadOnlyFile   *pSourceFile = NULL;
      StorageFile    *pDestFile = NULL;

      TRY
      {   
         pSourceFile = new ReadOnlyFile (FileNameSource);
         pDestFile = new StorageFile (FileNameDestination);
         error = addon_copy_file (pSourceFile, pDestFile, pDlg);
      }
      CATCH_ALL(e)
	   {
		   error = ERRORCODE_Fail;
      }
	   END_CATCH_ALL
   
      if (pSourceFile != NULL)
         delete pSourceFile;
      if (pDestFile != NULL)
         delete pDestFile;

      if (error != ERRORCODE_None)
      {
#ifdef _DEBUG                  
         AfxMessageBox ("Installation failed (copy)");
#endif
         break;
      }

      if ((pDlg != NULL) && (pDlg->CheckForAbort () == TRUE))
      {
#ifdef _DEBUG                  
         AfxMessageBox ("Installation Aborted");
#endif
         error = ERRORCODE_Abort;
         break;
      }
#ifdef PREPARE_UNINSTALL
      else
      {
         // Write full destination paths to uninstall INI
         CString name;
         Util::FormatString (name, "FILE%d", NumCopied);
         UninstallINI.WriteString ("Files", name, FileNameDestination);
      }
#endif

      pDlg->SetPos (NumCopied);
      pDlg->Breathe ();
   }
   
   if (error != ERRORCODE_None)
   { 
      for (pSection = pKeys; *pSection != '\0'; pSection += strlen(pSection)+1)
      {
         TRY
         {
            CString del = PathDestination;
            CString FileName = INIFile.GetString ("INSTALLFILES", pSection);
            del += FileName;
            CFile::Remove (del);
         }
         CATCH_ALL(e)
			{
         }
			END_CATCH_ALL
      }
#ifdef PREPARE_UNINSTALL
      TRY
      {
         CFile::Remove (PathUninstallINI);
      }
      CATCH_ALL(e)
		{
      }
		END_CATCH_ALL
#endif
   }

   return error;
}

//#define ADDON_UPDATE

#ifdef ADDON_UPDATE

BOOL
DoAddonUpdate (CWnd *pWnd, CIniFile &INI, LPCSTR pmwdir)
{
   LPSTR pKeys = NULL, pSection;

   if ((pKeys = INI.GetSectionKeys ("UPDATE")) == NULL)
      return TRUE;
      
   ERRORCODE   error = ERRORCODE_Fail; 
   DWORD       NumFiles = 1, NumCopied = 0;
   
   CAddonProgressDialog  ProgressBar (IDD_ADDON_INSTALL_PROGRESS, pWnd);
     
   TRY
   {   
      CString     cwdr = GetGlobalPathManager()->GetWorkingDirectory();
      
      for (NumFiles = 0, pSection = pKeys; *pSection != '\0'; NumFiles++, pSection += strlen(pSection) + 1)
      {
      }
      
      ProgressBar.SetMinMaxPos (0, NumFiles, 0);
      CString t;
      t.LoadString (IDS_COPYING);
      ProgressBar.ShowAction (t);
      ProgressBar.ShowProgress ();
      
      for (NumCopied = 0, pSection = pKeys; *pSection != '\0'; pSection += strlen(pSection) + 1, NumCopied ++)
      {
         CString  FileName, FileNameDest, FileNameSource;
         
         FileNameSource = cwdr;
         Util::RemoveBackslashFromPath (FileNameSource);
         FileName = INI.GetString ("UPDATE", pSection);
         
         FileNameDest = pmwdir;

         SplitMultipleFilename (FileName, FileNameSource, FileNameDest);

         ReadOnlyFile   *pSourceFile = NULL;
         StorageFile    *pDestFile = NULL;
         
         TRY
         {
            pSourceFile = new ReadOnlyFile (FileNameSource);
            pDestFile = new StorageFile (FileNameDest);
            error = addon_copy_file (pSourceFile, pDestFile, &ProgressBar);
			}
         CATCH_ALL(e)
			{
			   error = ERRORCODE_Fail;
         }
			END_CATCH_ALL
      
         if (pSourceFile != NULL)
            delete pSourceFile;
         if (pDestFile != NULL)
            delete pDestFile;

         if (error != ERRORCODE_None)
         {
#ifdef _DEBUG                  
            AfxMessageBox ("Upgrade failed (copy)");
#endif
            break;
         }
         
         ProgressBar.SetPos (NumCopied);
         ProgressBar.Breathe ();
      }
   }
   CATCH_ALL(e)
   {
   }
   END_CATCH_ALL

   ProgressBar.DestroyWindow ();
   
   if ((NumFiles == NumCopied) && (error == ERRORCODE_None))
      return TRUE;
   
   return FALSE;
}
#endif

void 
DoAddonInstall (CWnd *pWnd)
{
   LPSTR             pKeys = NULL;
   int               nResult = CAddonInstallDlg::FindPrintMasterDirectory();   
   ERRORCODE error = ERRORCODE_Fail; 

   CAddonInstallDlg  AddonInstallDlg (nResult, CAddonInstallDlg::s_csPmDirectory, pWnd);

   if (AddonInstallDlg.DoModal () == IDOK)
   {
      CString  INIFileName = GetConfiguration()->GetAddOnININame ();
      CIniFile INIFile (INIFileName);

      CAddonProgressDialog  ProgressBar (IDD_ADDON_INSTALL_PROGRESS, pWnd);
      
#ifdef PREPARE_UNINSTALL
      // Create an INI file at destination for uninstall
      CString PathUninstallINI = AddonInstallDlg.GetPmDirectory ();
      Util::AppendBackslashToPath (PathUninstallINI);
      // Get the name from ADDON.INI
      PathUninstallINI += INIFile.GetString ("UNINSTALL", "FileName", "uninst.ini");
      CIniFile UninstallINI (PathUninstallINI);
#endif
      CString PathSource;
      CString Temp = GetGlobalPathManager()->GetWorkingDirectory();
      Util::SplitPath (Temp,
                        &PathSource, // We want a  drive
                        NULL,       // No directory
                        NULL,       // No base name
                        NULL);      // No extension
      
      PathSource += INIFile.GetString ("INSTALLDIRS", "DirectorySource");

      if ((pKeys = INIFile.GetSectionKeys ("INSTALLFILES")) != NULL)
      {
         CString PathDestination = AddonInstallDlg.GetPmDirectory ();
         error = CopyFiles (pKeys, INIFile, PathSource, PathDestination, &ProgressBar);
         
         delete pKeys;
      }

#ifdef ADDON_UPDATE
      if ((error == ERRORCODE_None) && (DoAddonUpdate (pWnd, INIFile, AddonInstallDlg.GetPmDirectory ()) == TRUE))
#else
      if (error == ERRORCODE_None)
#endif
      {
         {
            CPmwDialog  DlgResult (IDD_ADDONINST_OK);
            DlgResult.DoModal ();
         }
      }
      else
      {
         if (error != ERRORCODE_Abort)
         {
            CPmwDialog  DlgResult (IDD_ADDONINST_FAIL);
            DlgResult.DoModal ();
         }
      }
      
      ProgressBar.DestroyWindow ();
   }
}
