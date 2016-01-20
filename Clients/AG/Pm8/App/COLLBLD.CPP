/*
// $Workfile: COLLBLD.CPP $
// $Revision: 2 $
// $Date: 3/08/99 3:36p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/COLLBLD.CPP $
// 
// 2     3/08/99 3:36p Gbeddow
// support for asking at start of an online collection build whether to
// compress the online data files
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 31    2/24/99 1:28p Gbeddow
// for online collection building, mirror the directory structure of the
// input files for the downloadable files, per Alexei Kulberg
// 
// 30    2/09/99 1:48p Gbeddow
// Support in collection building for creating online collection GIF and
// JPEG thumbnail files, and related changes, per spec from Alexei
// 
// 29    1/28/99 4:47p Mwilson
// added special colleciton builders
// 
// 28    1/28/99 2:00p Gbeddow
// collection building support for graphic type identifier used in current
// spreadsheets ("GR"), and eliminated the "no product specified" warning
// per Mari Collings and Corey Day
// 
// 27    1/25/99 1:20p Gbeddow
// support for the latest spec'd list of graphic types and initial support
// for a new web art browser dialog
// 
// 26    12/18/98 4:52p Gbeddow
// support for writing grapic types into art collection files
// 
// 25    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 24    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 23    10/02/98 7:10p Jayn
// More content manager changes
// 
// 22    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 21    8/11/98 1:50p Dennis
// Spreadsheet headers can now be case insensitive.  Changes for Crop
// field
// 
// 20    8/10/98 6:39p Hforman
// init m_bCropCopyright to FALSE
// 
// 19    8/10/98 4:08p Dennis
// Changes to fix Preprocessor Util
// 
// 18    8/10/98 3:06p Hforman
// add CropCopyright code
// 
// 17    7/21/98 11:53a Cboggio
// made last change work properly!
// 
// 16    6/03/98 12:28p Cboggio
// Added check to see if the Capitalize function should be used in the
// category building. This locale-dependent.
// 
// 15    6/03/98 9:51a Fredf
// More changes for address book and merge names.
// 
// 14    5/29/98 8:50p Fredf
// Support for download items with subdirectories.
// 
// 13    5/25/98 4:43p Fredf
// Support for building online collections.
// 
// 12    5/19/98 11:32a Dennis
// Made StringIsEmpty() a public static method.  DoBuild() now takes
// parameter to indicate if it should run silent.
// 
// 11    5/13/98 3:32p Dennis
// Added trace for current line being processed.
// 
// 10    4/24/98 7:41a Jayn
// Builder changes to support "inclusion column" and "external file".
// 
// 9     4/20/98 9:09a Jayn
// Cached thumbnail support.
// 
// 8     4/20/98 8:39a Dennis
// Output perfect build message to LOG file.
// 
// 7     4/15/98 1:19p Dennis
// Enhanced progress bar logic to exclude empty spreadsheet lines.
// 
// 5     4/14/98 10:04a Dennis
// a) Removed MEDIA and STYLE as required fields.
// b) Added FREE SAMPLE, HIDE ITEM, and THUMBNAIL fields.
// c) Removed obsolete thumbnail code.
// d) Removed Media and Style.
// e) Added Custom Flags.
// f) Updated GetData() with AddSubdirectory changes from PMG4 source.
// 
// 4     1/29/98 9:54a Dennis
// Renamed Progress Control
// 
// 3     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 2     12/08/97 2:38p Dennis
// Added error handling for missing columns.  Added progress bar with
// cancel.  Disabled main window while building.
// 
//    Rev 1.0   14 Aug 1997 15:19:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:40   Fred
// Initial revision.
// 
//    Rev 1.24   16 Jul 1997 13:40:40   dennisp
// Added error statistics to build.
// 
//    Rev 1.23   10 Jul 1997 14:06:46   dennisp
// Added PVCS header
// 
// 
*/

#include "stdafx.h"
#include "resource.h"
#include "collbld.h"
#include "file.h"
#include "progbar.h"    // For progress bar
#include "bmpfile.h"
#include "colldrw.h"		// PrintMaster collection drawing object.

// Do not include these files for Preprocessor utility.
#ifdef NEW_THUMBNAILS
#include "cdeflate.h"
#endif

#ifdef LOCALIZE
#include "clocale.h"
#endif

static const char * s_szPerfectBuild = "Perfect Build! Congratulations!\n";


#ifdef WIN32
// Browser builder class members
CCollectionBuilder::CCollectionBuilder(IPathManager* pPathManager,
													const CString &strINIFileName,
													const CString &strSection, 
													const CString &strSpreadSheetFileName,
													const CString* pstrCollectionPath /*=NULL*/,
													BOOL bOnline /*=FALSE*/,
													BOOL bCompressOnlineDataFiles /*=TRUE*/)
			: m_trFileSpreadSheet(strSpreadSheetFileName)
   {

		m_pPathManager = pPathManager;
		m_pPathManager->AddRef();

      ASSERT(!strINIFileName.IsEmpty());
      ASSERT(!strSpreadSheetFileName.IsEmpty());

      m_nCurLine = 1;
      m_strSpreadSheetFileName = strSpreadSheetFileName;
      Util::SplitPath(m_strSpreadSheetFileName, &m_strBuildPath, NULL);

		CString csCollectionPath;
		if (pstrCollectionPath != NULL)
		{
			csCollectionPath = *pstrCollectionPath;
		}
		else
		{
			csCollectionPath = m_strBuildPath;
		}

		m_bOnline = bOnline;
		if (m_bOnline)
		{
			// If this is an "online" collection, then build it in the
			// online subdirectory. The "offline" version goes into the
			// normal folder. We want to have two versions of the collection,
			// online and offline, because we'll be shipping both. The online
			// version will be on the online gallery server and the offline
			// version can be bought on a CD for those who don't want to
			// (or can't) get to the online version.

			Util::AppendBackslashToPath(csCollectionPath);
			csCollectionPath += "Online";
			Util::MakeDirectory(csCollectionPath);
		}
		m_bCompressOnlineDataFiles = bCompressOnlineDataFiles;

		ASSERT(!csCollectionPath.IsEmpty());

      m_pContentData = NULL;

		CPMCollectionDraw* pCollectionDraw = new CPMCollectionDraw;
		pCollectionDraw->Init(m_pPathManager);

		// Initialize the collection.
      m_nInitRetVal = m_Collection.Init(m_pPathManager,
													 (ICollectionDraw*)pCollectionDraw->GetInterface(&IID_ICollectionDraw),
													 strINIFileName,
													 strSection,
													 CPMWCollection::originGeneric,
													 CPMWCollection::modeCreate | CPMWCollection::modeReadWrite,
													 csCollectionPath,
													 m_bOnline);

		// Release the creation reference on the collection draw.
		// The collection should have its own reference.
		pCollectionDraw->ExternalRelease();

		if (m_bOnline)
		{
			// Initialize the strings used by the "online" build.
			// By convention, these paths always have a backslash
			// on the end.
			m_strOnlineCollectionName = m_Collection.GetBaseName();

			// Collection file path (should already exist.)
			m_strOnlinePath = m_Collection.GetCollectionPath();
			Util::AppendBackslashToPath(m_strOnlinePath);
			Util::MakeDirectory(m_strOnlinePath);

			// Root directory for FTP files.
			m_strOnlineFtpPath = m_strOnlinePath;
			m_strOnlineFtpPath += "Ftp\\";
			m_strOnlineFtpPath += m_strOnlineCollectionName;
			m_strOnlineFtpPath += "\\";
			Util::MakeDirectory(m_strOnlineFtpPath);

			// Root directory for SQL files.
			m_strOnlineSqlPath = m_strOnlinePath;
			m_strOnlineSqlPath += "Sql\\";
			Util::MakeDirectory(m_strOnlineSqlPath);
		}
   }

CCollectionBuilder::~CCollectionBuilder()
   {
		if (m_pPathManager != NULL)
		{
			m_pPathManager->Release();
		}
      ReleaseData();
   }

int CCollectionBuilder::DoBuild(CWnd *pParent, BOOL bValidateOnly, BOOL bRunSilent)
   {
      int                  nRetVal = ERRORCODE_None, nMsgBoxRetVal;
      BOOL                 bCreated, bFatalError = FALSE;
      MSG                  msg;
      CPMWProgressBar      progressDialog;

      m_cmspContentFileNames.RemoveAll();
      m_cmspCategoryNames.RemoveAll();
      m_BuildStats.Empty();

      m_bValidateOnly = bValidateOnly;

      if(m_nInitRetVal != ERRORCODE_None)
         {
            switch(m_nInitRetVal)
               {
                  case ERRORCODE_NoINIFile:
                     AfxMessageBox("Init. Failed - Missing .INI File");
                     break;
                  case ERRORCODE_InvalidINIEntry:
                     AfxMessageBox("Init. Failed - Invalid INI Entry");
                     break;
                  case ERRORCODE_StopListIOError:
                     AfxMessageBox("Init. Failed - Missing Stoplist file");
                     break;
                  default:
                     AfxMessageBox("Builder Init. Failed");
                     break;
               }
            return ERRORCODE_Fail;
         }
      m_strFileNamePrefix = m_Collection.GetDBFileNamePrefix();
      m_BuildType = m_Collection.GetType();
      m_pParent = pParent;

      TRY
         {
            m_trFileSpreadSheet.set_read_buffer(4096);

            m_nNonEmptySpreadSheetLineCount = GetSpreadSheetLineCount();
            if(m_nNonEmptySpreadSheetLineCount == 0)
            {
               AfxMessageBox("Error opening spreadsheet");
               return ERRORCODE_Open;
            }
      
            // Open databases
            nRetVal = OpenAll();
            if(nRetVal != ERRORCODE_None)
               return nRetVal;

            // Create Progress Bar control
            bCreated = progressDialog.Create(IDD_PROGRESS_BAR, m_pParent);
            if(!bCreated)
               {
                  AfxMessageBox("Failed to Create Progress Bar");
                  return ERRORCODE_Fail;
               }
            AfxGetMainWnd()->EnableWindow(FALSE);
            progressDialog.EnableWindow(TRUE);     // Re-enable progress dialog
            progressDialog.SetWindowText("Building Collection...");

            CProgressCtrl *pProgressBar = (CProgressCtrl *)progressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
            pProgressBar->SetRange(1, m_nNonEmptySpreadSheetLineCount);
            pProgressBar->SetStep(1);

            if(ReadHeaderLines() != ERRORCODE_None)
               {
                  CloseAll();
                  AfxMessageBox("Error occurred while reading spreadsheet header\nCheck log file");
                  AfxGetMainWnd()->EnableWindow(TRUE);
                  return ERRORCODE_Fail;
               }

				if (m_bOnline)
				{
					TRY
					{
						CString csSql;

						// SQL Installation Script

						m_stdioSqlInstallFile.WriteString("Declare @Collection varchar(255)\n");
						csSql.Format("Select @Collection = '%s'\n\n", (LPCSTR)m_strOnlineCollectionName);
						m_stdioSqlInstallFile.WriteString(csSql);
						m_stdioSqlInstallFile.WriteString(
							"Declare @CollectionID Integer\n"
							"Declare @CollectionFileID Integer\n"
							"Declare @CollectionIconID Integer\n"
							"Declare @CollectionBrochureID Integer\n"
							"Declare @ItemID Integer\n"
							"\n"
							"INSERT MindscapePGS.dbo.DownloadItem\n"
							"VALUES (@Collection+' Collection', 'Coll/'+@Collection+'.', '', '', 'F', 'E', 'C', '', '', '')\n"
							"Select @CollectionFileID = ItemID from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Collection'\n"
							"\n"
							"INSERT MindscapePGS.dbo.DownloadItem\n"
							"VALUES (@Collection+' Icon', 'Coll/'+@Collection+'.ico', '', '', 'F', 'E', 'I', '', '', '')\n"
							"Select @CollectionIconID = ItemID from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Icon'\n"
							"\n"
							"INSERT MindscapePGS.dbo.DownloadItem\n"
							"VALUES (@Collection+' Brochure', 'Ads/'+@Collection+'._bm', '', '', 'F', 'E', 'A', '', '', '')\n"
							"Select @CollectionBrochureID = ItemID from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Brochure'\n"
							"\n"
							"INSERT MindscapePGS.dbo.Collection\n"
							"VALUES (@Collection, 'us-english', @Collection, 0.0, @CollectionFileID, @CollectionIconID, @CollectionBrochureID, 'E', '', '', '')\n"
							"Select @CollectionID = CollectionID from MindscapePGS.dbo.Collection where Description=@Collection\n"
							"\n"
							"Declare @Description varchar(255)\n"
							"Declare @FileName varchar(255)\n"
							"Declare @FriendlyName varchar(255)\n"
							"Declare @MajorCategory varchar(255)\n"
							"Declare @MinorCategory varchar(255)\n"
							"Declare @Keywords varchar(255)\n"
							"Declare @Tone varchar(255)\n"
							"Declare @GraphicType varchar(255)\n"
							"Declare @FileType varchar(255)\n"
							"Declare InstallCursor CURSOR FOR SELECT * FROM MindscapePGS.dbo.InstallList\n"
							"OPEN InstallCursor\n"
//							"FETCH NEXT FROM InstallCursor INTO @Description, @FileName\n"
							"FETCH NEXT FROM InstallCursor INTO @Description, @FileName, @FriendlyName, @MajorCategory, @MinorCategory, @Keywords, @Tone, @GraphicType, @FileType\n"
							"WHILE (@@fetch_status <> -1) BEGIN\n"
							"\tIF (@@fetch_status <> -2) BEGIN\n"
							"\t\tINSERT MindscapePGS.dbo.DownloadItem\n"
//							"\t\tVALUES (@Description, @FileName, @Collection, @Collection, 'F', 'E', 'G', @Category, @Subcategory, @Keywords)\n"
							"\t\tVALUES (@Description, @FileName, @Collection, @Collection, 'F', 'E', 'G', @FriendlyName, @MajorCategory, @MinorCategory, @Keywords, @Tone, @GraphicType, @FileType)\n"
							"\t\tSelect @ItemID = ItemID from MindscapePGS.dbo.DownloadItem where Description=@Description\n"
							"\t\tINSERT MindscapePGS.dbo.Collection_Graphics\n"
							"\t\tVALUES (@CollectionID, @ItemID, @Description)\n"
							"\tEND\n"
//							"\tFETCH NEXT FROM InstallCursor INTO @Description, @FileName\n"
							"\tFETCH NEXT FROM InstallCursor INTO @Description, @FileName, @FriendlyName, @MajorCategory, @MinorCategory, @Keywords, @Tone, @GraphicType, @FileType\n"
							"END\n"
							"CLOSE InstallCursor\n"
							"DEALLOCATE InstallCursor\n");

						// SQL Deinstallation Script

						m_stdioSqlDeleteFile.WriteString("Declare @Collection varchar(255)\n");
						csSql.Format("Select @Collection = '%s'\n\n", (LPCSTR)m_strOnlineCollectionName);
						m_stdioSqlDeleteFile.WriteString(csSql);

						m_stdioSqlDeleteFile.WriteString(
							"Declare @CollectionID Integer\n"
							"Select @CollectionID = CollectionID from MindscapePGS.dbo.Collection where Description=@Collection\n"
							"\n"
							"Delete from MindscapePGS.dbo.Collection_Graphics where CollectionID=@CollectionID\n"
							"\n"
							"Delete from MindscapePGS.dbo.Collection where CollectionID=@CollectionID\n"
							"\n"
							"Delete from MindscapePGS.dbo.Customer_Downloads From MindscapePGS.dbo.DownloadItem\n"
							"where MindscapePGS.dbo.Customer_Downloads.DownloadID = MindscapePGS.dbo.DownloadItem.ItemID\n"
							"and MindscapePGS.dbo.DownloadItem.LoginID=@Collection\n"
							"\n"
							"Delete from MindscapePGS.dbo.DownloadItem where LoginID=@Collection\n"
							"\n"
							"Delete from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Collection'\n"
							"Delete from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Icon'\n"
							"Delete from MindscapePGS.dbo.DownloadItem where Description=@Collection+' Brochure'\n");
					}
					CATCH_ALL(e)
					{
						ReportError("Can't write SQL string", CCollectionBuilderStats::typeError);
					}
					END_CATCH_ALL
				}

            // Process lines until end of file or serious error
            do
               {
                  TRACE("Processing Line %05i: \n",m_nCurLine);

                  nRetVal = ProcessLine();
                  m_nCurLine++;
                  // Empty spreadsheet lines are not reflected in progress bar
                  if(!m_bLineIsEmpty)
                     pProgressBar->StepIt();

                  if(nRetVal == ERRORCODE_Exists)
                     {
                        CString strError;

                        strError.Empty();
                        strError.Format("Line %05i: FATAL ERROR-DUPLICATE CONTENT %s\n",
                           m_nCurLine-1, m_strFullDataFileName.GetBuffer(1));
                        ReportError(strError, CCollectionBuilderStats::typeError);
                        if(!bRunSilent)
                        {
                          nMsgBoxRetVal = AfxMessageBox(
                           "Found Duplicate Content Item\nContent is unusable\nProceed with build to identify errors?\n", MB_YESNO);
                          if(nMsgBoxRetVal == IDYES)
                              nRetVal = ERRORCODE_None;
                        }
                        else
                          nRetVal = ERRORCODE_None;
                        bFatalError = TRUE;
                     }
                  else if(nRetVal == ERRORCODE_DataSequenceError)
                     {
                        // If an error occurred and it's a category that's 
                        // out of order, then no data associated with this
                        // item was added
                        CString  strError;
                        strError.Empty();
                        strError.Format("Line %05i: Category out of order %s\n",
                           m_nCurLine-1, m_strCategory);
                        ReportError(strError);
                        nRetVal = ERRORCODE_None;              // Lie, error on LOG file
                     }
                  else if(nRetVal != ERRORCODE_None && nRetVal != ERRORCODE_Done)
                     {
                        // All Other errors, abort build!
                        CString  strError;
                        strError.Empty();
                        strError.Format("Line %05i: Add Error %i occurred - Build Aborted\n",
                           m_nCurLine-1, nRetVal);
                        ReportError(strError, CCollectionBuilderStats::typeError);
                     }
               	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
               		// Pump the message through.
               		AfxGetApp()->PumpMessage();
               }
            while (nRetVal == ERRORCODE_None && !progressDialog.CancelHit());
            progressDialog.DestroyWindow();
         }
      CATCH_ALL(exception)
         {
            CString  strError;
            strError.Format("Exception occurred in builder at Line %05i: Build Aborted",
               m_nCurLine-1);
            AfxMessageBox(strError);
         }
      END_CATCH_ALL

      if(bFatalError)
         nRetVal = ERRORCODE_Exists;   // Same data item was attempted to be added twice
      else if(nRetVal == ERRORCODE_Done)    
         nRetVal = ERRORCODE_None;     // Normal completion
      else if(progressDialog.CancelHit())
         nRetVal = ERRORCODE_Abort;    // User abort

      if(m_BuildStats.GetErrorCount() == 0 &&
         m_BuildStats.GetWarningCount() == 0 &&
         m_BuildStats.GetNoneCount() > 0)
            m_stdioLogFile.WriteString(s_szPerfectBuild);
      // Closes/cleanup
      CloseAll();
      if(bValidateOnly)
         {
            AfxMessageBox("Preprocessing Completed!");
         }
      else if(nRetVal == ERRORCODE_None)
         {
            if(m_BuildStats.GetErrorCount() == 0 &&
               m_BuildStats.GetWarningCount() == 0 &&
               m_BuildStats.GetNoneCount() > 0)
                  AfxMessageBox(s_szPerfectBuild);
            else
                  AfxMessageBox("Build Completed!");
         }
      else
         {
            switch(nRetVal)
               {
                  case ERRORCODE_Exists:
                     AfxMessageBox("Encountered a FATAL ERROR during build, content is unusable");
                     break;
                  case ERRORCODE_Abort:
                     AfxMessageBox("Build Cancelled by user request!");
                     break;
                  default:
                     AfxMessageBox("Build Aborted");
                     break;
               }
         }

      CString  strStats;
      strStats.Format("ERRORS: %li  WARNINGS: %li  ITEMS ADDED: %li\n",
         m_BuildStats.GetErrorCount(), m_BuildStats.GetWarningCount(),
         m_BuildStats.GetNoneCount());
      AfxMessageBox(strStats);

      AfxGetMainWnd()->EnableWindow(TRUE);
      return nRetVal;
   }

int CCollectionBuilder::GetSpreadSheetLineCount()
   {
      int   nLineCount = 0;

      // Seek to top of file
      m_trFileSpreadSheet.seek(0, ST_DEV_SEEK_SET);
      while(m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE) == ERRORCODE_None)
         {
            // Don't count it if empty
            if(!StringIsEmpty(m_chLine))
               nLineCount++;
         }
      // Seek back to top of file
      m_trFileSpreadSheet.seek(0, ST_DEV_SEEK_SET);
      return nLineCount;
   }

// Checks if passed string is empty
BOOL CCollectionBuilder::StringIsEmpty(LPCSTR szString)
   {
      BOOL  bIsEmpty = TRUE;
      int   i;

      for(i=0;szString[i] && bIsEmpty;i++)
         {
            if(szString[i] != ' ' && szString[i] != '\t')
               {
                  bIsEmpty = FALSE;
                  continue;
               }
         }
      return bIsEmpty;
   }

int CCollectionBuilder::OpenAll()
   {
      CCIndexFileException indexException;
      CString              strLogFileName;

		strLogFileName = m_strFileNamePrefix;
      strLogFileName += ".LOG";

      // Create LOG file
      if (!m_stdioLogFile.Open(strLogFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			CString	csError;
			csError.Format("Failed to open LOG file %s.\nCheck if file is in use.\nBuild has been aborted", strLogFileName);
			AfxMessageBox(csError);
         return ERRORCODE_Open;
		}

		if (m_bOnline)
		{
			// Create SQL installation file.
			CString strSqlInstallFileName = m_strOnlineSqlPath;
			strSqlInstallFileName += "\\Install.sql";
			if (!m_stdioSqlInstallFile.Open(strSqlInstallFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
			{
				CString	csError;
				csError.Format("Failed to open SQL installation file %s.\nCheck if file is in use.\nBuild has been aborted", strSqlInstallFileName);
				AfxMessageBox(csError);
				return ERRORCODE_Open;
			}

			// Create SQL installation list.
			CString strSqlInstallListName = m_strOnlineSqlPath;
			strSqlInstallListName += "\\Install.txt";
			if (!m_stdioSqlInstallList.Open(strSqlInstallListName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
			{
				CString	csError;
				csError.Format("Failed to open SQL installation list file %s.\nCheck if file is in use.\nBuild has been aborted", strSqlInstallListName);
				AfxMessageBox(csError);
				return ERRORCODE_Open;
			}

			// Create SQL deinstallation file.
			CString strSqlDeleteFileName = m_strOnlineSqlPath;
			strSqlDeleteFileName += "\\Delete.sql";
			if (!m_stdioSqlDeleteFile.Open(strSqlDeleteFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
			{
				CString	csError;
				csError.Format("Failed to open SQL deinstallation file %s.\nCheck if file is in use.\nBuild has been aborted", strSqlDeleteFileName);
				AfxMessageBox(csError);
				return ERRORCODE_Open;
			}
		}

      return ERRORCODE_None;
   }

int CCollectionBuilder::CloseAll()
   {
      // Close LOG file
      m_stdioLogFile.Close();

      return ERRORCODE_None;
   }

int CCollectionBuilder::ReadHeaderLines()
   {
		// Extract the fields for this line.
      while(m_nCurLine <= NUM_SPREADSHEET_HEADER_LINES)
         {
      		if (m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE) != ERRORCODE_None)
               return ERRORCODE_Read;

            m_csaHeaderFields[m_nCurLine-1].SetNames(m_chLine, '\t'); // Set delimiter to Tabs
            if(m_nCurLine == 1)
            {
               if(m_csaHeaderFields[m_nCurLine-1].ValidateFields(&m_stdioLogFile) !=
                  ERRORCODE_None)
                  {
                     AfxMessageBox("Error Occurred in Validating Header Fields\nCheck Log File");
                     return ERRORCODE_Fail;
                  }
            }
            m_nCurLine++;
         }
		// If we have an inclusion field, try to find the column now
		CString csInclusionField = m_Collection.GetInclusionField();
		if (csInclusionField.IsEmpty())
			m_nInclusionColumn = -1;		// No inclusion column.
		else if ((m_nInclusionColumn = m_csaHeaderFields[0].FindName(csInclusionField), CMlsStringArray::IgnoreCase) != -1)
			{
				// We found the inclusion field.
				if (m_nInclusionColumn > m_csaHeaderFields[0].GetLastUsedColumn())
					{
						m_csaHeaderFields[0].SetLastUsedColumn(m_nInclusionColumn);
					}
			}
		else
			{
				// Inclusion field is missing.
				CString csError;
				csError.Format("Can't find inclusion field: %s\n", (LPCSTR)csInclusionField);
				ReportError(csError);
				AfxMessageBox("Error Occurred in Validating Header Fields\nCheck Log File");
				return ERRORCODE_Fail;
			}
      return ERRORCODE_None;
   }

// Main logic for processing a spreadsheet line
int CCollectionBuilder::ProcessLine()
   {
      int      i, nRetVal;
      CString  strError;

      m_cdItem.Empty();
      m_strProductCode.Empty();
      m_strThumbnailFileName.Empty();
      m_bFreeSample = m_bHideItem = FALSE;
		m_bCropCopyright = FALSE;
		m_GraphicTypeFlags = CCategoryDataItem::cfNone;
      m_bLineIsEmpty = FALSE;

		int nMode = (m_BuildType == CPMWCollection::typeSentiments)
								? TRF_ignore_eol_in_brackets
								: 0;		// We need brackets in other builds.

      if (m_trFileSpreadSheet.read_line(m_chLine, MAX_SPREADSHEET_LINE, nMode) != ERRORCODE_None)
			return ERRORCODE_Done;

		// Excel puts quotes around things with commas, and doubles up quotes in things
		// with quotes. We don't want the extra quotes in the collection, so we have to
		// strip them out.
		char szBuffer[sizeof(m_chLine)];
		int nBufferIndex = 0;
		for (i = 0; ; i++)
		{
			if (m_chLine[i] != '"')
			{
				szBuffer[nBufferIndex++] = m_chLine[i];
				if (m_chLine[i] == '\0')
				{
					break;
				}
			}
			else
			{
				// Current character is a quote. Eliminate single quotes,
				// collapse double quotes to a single quote. Note that
				// "single" and "double" refer to the number of quote
				// characters not the single and double quote characters
				// (' and ").

				if (m_chLine[i+1] == '"')
				{
					szBuffer[nBufferIndex++] = '"';
					i++;
				}
			}
		}

		// Copy the result back to the original line.
		strcpy(m_chLine, szBuffer);

      // Check if empty
      m_bLineIsEmpty = FALSE;
      if(StringIsEmpty(m_chLine))
         {
            // Skip it if empty
            m_bLineIsEmpty = TRUE;
            return ERRORCODE_None;
         }

      m_strFullDataFileName = m_strBuildPath;
      // Remove trailing backslash
		Util::RemoveBackslashFromPath(m_strFullDataFileName);
		int nInitialPathSize = m_strFullDataFileName.GetLength();

		// Extract the fields for this line.
      m_csaFields.SetNames(m_chLine, '\t'); // Set delimiter to Tabs

      // Check for incomplete fields entered in spreadsheet 09/08/97-DGP
      if(m_csaFields.GetSize() < m_csaHeaderFields[0].GetLastUsedColumn())
         {
            strError.Empty();
            strError.Format("Line %05i: COLUMN COUNT MISMATCH - Line has missing fields\n",
               m_nCurLine);
            ReportError(strError);

            return ERRORCODE_None;
         }

		// If we have an inclusion column, see if we want to include this line.
		if (m_nInclusionColumn != -1
				&& atoi(m_csaFields.GetAt(m_nInclusionColumn)) == 0)
			{
				// Skip this line.
				return ERRORCODE_None;
			}

      StandardizeFields();

		TRACE(">>> Processing line: %s <<<\n", (LPCSTR)m_strFileName);

      if(ValidateFields() != ERRORCODE_None)
         return ERRORCODE_None;   // Lie!, log file will contain errors

      if(m_bValidateOnly)
         {
            nRetVal = CheckForDuplicateContent();
            if(nRetVal != ERRORCODE_None)
               return nRetVal;
            return CheckForCategoryOrderError();
         }

      if(AddKeywords() != ERRORCODE_None)
      {
         AfxMessageBox("Error occurred while adding Keywords");
         return ERRORCODE_Fail;
      }

      if(AddItems() != ERRORCODE_None)
      {
         AfxMessageBox("Error occurred while adding Items");
         return ERRORCODE_Fail;
      }

#if 0 // GCB 2/24/99 - mirror directory structure of input files, per Alexei Kulberg
      if(GetData() != ERRORCODE_None)
      {
         AfxMessageBox("Error occurred while adding Content");
         return ERRORCODE_Fail;
      }

      // Set external thumbnail file to use (if any)
      m_CollectionData.m_strThumbnailFileName = m_strThumbnailFileName;

		// Set the relative path to all that is not in the base name.
		m_CollectionData.m_strRelativeFileName = m_strFullDataFileName.Mid(nInitialPathSize);
		m_CollectionData.m_strFullFileName = m_strFullDataFileName;

      // Add item to category tree and item list
      m_CollectionData.m_CategoryData.Empty();
#else
      // Set external thumbnail file to use (if any)
      m_CollectionData.m_strThumbnailFileName = m_strThumbnailFileName;

		// Set the relative path to all that is not in the base name.
		m_CollectionData.m_strRelativeFileName = m_strFullDataFileName.Mid(nInitialPathSize);
		m_CollectionData.m_strFullFileName = m_strFullDataFileName;

      // Add item to category tree and item list
      m_CollectionData.m_CategoryData.Empty();

      if(GetData() != ERRORCODE_None)
      {
         AfxMessageBox("Error occurred while adding Content");
         return ERRORCODE_Fail;
      }
#endif

      // Set item tone
      m_cdItem.SetTone((CCategoryDataItem::Tones) m_nTone);

      // Set item custom flag
      m_cdItem.SetCustomFlag(CCategoryDataItem::cfNone);
      if(m_bFreeSample)
         m_cdItem.SetCustomFlag(CCategoryDataItem::cfFreeSample);
      if(m_bHideItem)
         m_cdItem.SetCustomFlag(CCategoryDataItem::cfHideItem);
      if(m_bCropCopyright)
         m_cdItem.SetCustomFlag(CCategoryDataItem::cfCropCopyright);

		// Set item graphic type sub-field in the custom flag
		m_cdItem.SetGraphicType(m_GraphicTypeFlags);

      m_cdItem.SetProductFlags(m_shProductFlags);

      // DGP - Get Product Code and Flags
      m_CollectionData.m_CategoryData.Set(m_strCategory, m_strProductCode,
         &m_cdItem, m_shProductFlags);
      nRetVal = m_Collection.Add(&m_CollectionData);
      if(nRetVal == ERRORCODE_None)
         m_BuildStats.IncrementNoneCount();
      return nRetVal;
   }

int CCollectionBuilder::ValidateFields()
   {
      int      nStartColumn, nEndColumn;
      CString  strError;
      CString  strCategoryField, strField;

      m_nTone = 0;
      m_bValidationError=FALSE;
      m_strCategory.Empty();
		m_strExternalFile.Empty();
      for(CCollHeaderArray::FieldTypes curField=CCollHeaderArray::SuperCategory;
         curField < CCollHeaderArray::ListEnd;
         curField++)
      {
         // Skip unused fields
         switch(m_BuildType)
         {
            case CPMWCollection::typeArt:
               if(curField == CCollHeaderArray::SuperCategory ||
                  curField == CCollHeaderArray::Front ||
                  curField == CCollHeaderArray::Inside)
                     continue;
               break;
            case CPMWCollection::typeProjects:
               if(curField == CCollHeaderArray::Front ||
                  curField == CCollHeaderArray::Inside)
                     continue;
               break;
            case CPMWCollection::typeSentiments:
               if(curField == CCollHeaderArray::SuperCategory)
                     continue;
               break;
         }
         nStartColumn = m_csaHeaderFields[0].GetStartColumn(curField);
			if (nStartColumn == -1)
				continue;				// Optional field not found.
         nEndColumn   = m_csaHeaderFields[0].GetEndColumn(curField);

         ASSERT(nStartColumn < m_csaFields.GetSize());
         ASSERT(nEndColumn < m_csaFields.GetSize());
         if(nStartColumn >= m_csaFields.GetSize() || nEndColumn >= m_csaFields.GetSize())
         {
            AfxMessageBox("Internal Error");
            return ERRORCODE_IntError;
         }

         // Skip check for empty Tone Thumbnail, and Custom Flags fields
         // Tone, Custom Flags, and Product are checked when they are 
         // converted to values
         // All others should not be empty.
         if(m_csaFields.GetAt(nStartColumn).IsEmpty() &&
            !(curField == CCollHeaderArray::ProductCode ||
              curField == CCollHeaderArray::ProductFlags ||
              curField == CCollHeaderArray::FreeSample ||
              curField == CCollHeaderArray::HideItem ||
              curField == CCollHeaderArray::CropCopyright ||
              curField == CCollHeaderArray::Thumbnail ||
              curField == CCollHeaderArray::ExternalFile ||
              curField == CCollHeaderArray::CropCopyright ||
              curField == CCollHeaderArray::GraphicType ||
              (curField == CCollHeaderArray::SuperCategory &&
                 m_Collection.GetType() == CPMWCollection::typeProjects) ||
              ( (curField == CCollHeaderArray::Front ||
                 curField == CCollHeaderArray::Inside ) &&
                 m_Collection.GetType() == CPMWCollection::typeSentiments) ||
              curField == CCollHeaderArray::Directory ||
              curField == CCollHeaderArray::Tone))
         {
            strError.Empty();
            strError.Format("Line %05i: Empty Field %s\n",
               m_nCurLine, m_csaHeaderFields[0].GetName(curField));
            ReportError(strError);
            m_bValidationError = TRUE;
         }
         // Special case checks
         switch(curField)
            {
               case CCollHeaderArray::SuperCategory:
               case CCollHeaderArray::Category:
               case CCollHeaderArray::SubCategory:
                  // First category field has no delimiter
                  if(m_BuildType == CPMWCollection::typeProjects)
                     {
                        if(curField != CCollHeaderArray::SuperCategory)
                           m_strCategory += "|";   // delimiter
                     }
                  else
                     {
                        if(curField != CCollHeaderArray::Category)
                           m_strCategory += "|";   // delimiter
                     }

                  // if category has leading space, flag it as an error
                  strCategoryField = m_csaFields.GetAt(nStartColumn);
                  if(!strCategoryField.IsEmpty() && strCategoryField[0] == ' ')
                     {
                        m_bValidationError = TRUE;
                        strError.Empty();
                        strError.Format("Line %05i: Category has a leading space\n",
                           m_nCurLine);
                        ReportError(strError);
                     }
                  m_strCategory += m_csaFields.GetAt(nStartColumn);
                  // GetNameSize() returns sizeof category name including NULL
                  if((m_csaFields.GetAt(nStartColumn).GetLength()+1) > CCategoryEntry::GetNameSize())
                  {
                     m_bValidationError = TRUE;
                     strError.Empty();
                     strError.Format("Line %05i: Category too long \"%s\"\n",
                        m_nCurLine, m_csaFields.GetAt(nStartColumn).GetBuffer(1));
                     ReportError(strError);
                  }
                  break;
               case CCollHeaderArray::Tone:
                  // RangeToFlagValue will trap errors
                  m_nTone = RangeToFlagValue(nStartColumn, nEndColumn);
                  break;
               case CCollHeaderArray::Directory:
                  m_strFullDataFileName += m_csaFields.GetAt(nStartColumn);
                  m_strFullDataFileName += "\\";
                  m_strFullContentPath = m_strFullDataFileName;
                  break;
               case CCollHeaderArray::FileName:
                  m_strFullDataFileName += m_csaFields.GetAt(nStartColumn);
                  break;
               case CCollHeaderArray::ProductCode:
                  m_strProductCode = m_csaFields.GetAt(nStartColumn);
                  if((m_strProductCode.GetLength()+1) >
                     CCategoryEntry::GetProductCodeSize())
                     {
                        m_bValidationError = TRUE;
                        strError.Empty();
                        strError.Format("Line %05i: Product Code too long [%s]\n",
                           m_nCurLine, m_strProductCode);
                        ReportError(strError);
                     }
                  break;
               case CCollHeaderArray::ProductFlags:
                  // BuildFlags will trap errors
                  m_shProductFlags = (USHORT) BuildFlags(nStartColumn, nEndColumn);
                  break;
               case CCollHeaderArray::Front:
                  m_strSentiment = m_csaFields.GetAt(nStartColumn);
                  break;
               case CCollHeaderArray::Inside:
                  m_strSentiment += '|';              // Delimiter
                  m_strSentiment += m_csaFields.GetAt(nStartColumn);
                  break;
               case CCollHeaderArray::FreeSample:
                  strField = m_csaFields.GetAt(nStartColumn);
                  if(atoi(strField) > 0)
                     m_bFreeSample = TRUE;
                  break;
               case CCollHeaderArray::HideItem:
                  strField = m_csaFields.GetAt(nStartColumn);
                  if(atoi(strField) > 0)
                     m_bHideItem = TRUE;
                  break;
               case CCollHeaderArray::CropCopyright:
                  strField = m_csaFields.GetAt(nStartColumn);
                  if(atoi(strField) > 0)
                     m_bCropCopyright = TRUE;
                  break;
               case CCollHeaderArray::Thumbnail:
					{
                  CString strThumbnailFileName = m_csaFields.GetAt(nStartColumn);
                  Util::Trim(strThumbnailFileName);
                  if(!strThumbnailFileName.IsEmpty())
                     {
                        m_strThumbnailFileName = m_strFullContentPath;
                        m_strThumbnailFileName += strThumbnailFileName;
                        if(Util::FileExists(m_strThumbnailFileName))
                           {
                              // Check Bitmap info in file
                              CBitmapFile cbfThumbnail;

                              CBitmapFile::ErrorCode  ecBitmap;

                              ecBitmap = cbfThumbnail.ReadHeaders(m_strThumbnailFileName);
                              if(ecBitmap == CBitmapFile::ecNone)
                                 {
                                    const BITMAPINFOHEADER *pbmpHeader;
            
                                    pbmpHeader = cbfThumbnail.GetInfoHeader();

                                    if(pbmpHeader->biBitCount != 24)
                                       {
                                          m_bValidationError = TRUE;
                                          strError.Empty();
                                          strError.Format("Line %05i: Thumbnail File is not a 24 bit bitmap [%s]\n",
                                             m_nCurLine, m_strThumbnailFileName);
                                          ReportError(strError);
                                       }
                                 }
                              else
                                 {
                                    m_bValidationError = TRUE;
                                    strError.Empty();
                                    strError.Format("Line %05i: Error Opening Thumbnail File [%s]\n",
                                       m_nCurLine, m_strThumbnailFileName);
                                    ReportError(strError);
                                 }
                           }
                        else
                           {
                              m_bValidationError = TRUE;
                              strError.Empty();
                              strError.Format("Line %05i: Missing Thumbnail File [%s]\n",
                                 m_nCurLine, m_strThumbnailFileName);
                              ReportError(strError);
                           }
                     }
                  break;
					}
               case CCollHeaderArray::ExternalFile:
						m_strExternalFile = m_csaFields.GetAt(nStartColumn);
                  break;
					case CCollHeaderArray::GraphicType:
						// SetGraphicTypeFlags will trap errors
						m_GraphicTypeFlags = SetGraphicTypeFlags(nStartColumn, nEndColumn);
						break;
            }
      }
      if(m_BuildType != CPMWCollection::typeSentiments && _access(m_strFullDataFileName.GetBuffer(1), 4) != 0)
         {
            m_bValidationError = TRUE;
            strError.Empty();
            strError.Format("Line %05i: File %s missing\n",
               m_nCurLine, m_strFullDataFileName.GetBuffer(1));
            ReportError(strError);
         }
      if(m_bValidationError)
            return ERRORCODE_Fail;    // Warnings/Errors in LOG File
      m_strFullDataFileName.MakeUpper();

      return ERRORCODE_None;      
   }

int CCollectionBuilder::StandardizeFields()
   {
      int                                    nStartColumn, nEndColumn;
      CCollHeaderArray::FieldTypes    curField;

      if(m_BuildType == CPMWCollection::typeProjects)
         curField=CCollHeaderArray::SuperCategory;
      else
         curField=CCollHeaderArray::Category;
      for( ;curField < CCollHeaderArray::ListEnd; curField++)
      {
         nStartColumn = m_csaHeaderFields[0].GetStartColumn(curField);
			if (nStartColumn != -1)
			{
				nEndColumn   = m_csaHeaderFields[0].GetEndColumn(curField);

				// Remove leading and trailing spaces from field
				Util::Trim(m_csaFields[curField]);

				// Special case checks
				switch(curField)
					{
						case CCollHeaderArray::SuperCategory:
						case CCollHeaderArray::Category:
						case CCollHeaderArray::SubCategory:
#ifdef LOCALIZE
							if (CurrentLocale.UseCapitalizeInCategoryBuild())
#endif
								Util::Capitalize(m_csaFields[nStartColumn], FALSE);
							break;
	//             case CCollHeaderArray::FriendlyName:
						case CCollHeaderArray::Keywords:
							Util::Capitalize(m_csaFields[nStartColumn]);
							break;
						case CCollHeaderArray::Directory:
							m_csaFields[nStartColumn].MakeUpper();
							break;
						case CCollHeaderArray::FileName:
							m_csaFields[nStartColumn].MakeUpper();
							m_strFileName = m_csaFields[nStartColumn];
							break;
					}
			}
      }
      return ERRORCODE_None;      
   }

int CCollectionBuilder::AddKeywords()
   {
      int               nKeywordsColumn;

      nKeywordsColumn = m_csaHeaderFields[0].GetStartColumn(CCollHeaderArray::Keywords);
      m_CollectionData.m_strKeywords = m_csaFields.GetAt(nKeywordsColumn);
      return ERRORCODE_None;
   }

// This routine simulates adding content for preprocessing in order
// to detect duplicate content filenames in spreadsheet
int CCollectionBuilder::CheckForDuplicateContent()
   {
      LPVOID         pVoid;

      if(m_cmspContentFileNames.Lookup(m_strFullDataFileName, (LPVOID) pVoid) == 0)
         {
            m_cmspContentFileNames[m_strFullDataFileName] = 0;
            return ERRORCODE_None;
         }

      return ERRORCODE_Exists;
   }

int CCollectionBuilder::CheckForCategoryOrderError()
   {
      LPVOID         pVoid;
      DWORD          dwMatchedLine, dwCount;

      if(m_cmspCategoryNames.Lookup(m_strCategory, (LPVOID) pVoid) == 0)
         {
            dwCount = m_cmspCategoryNames.GetCount()+1;
            m_cmspCategoryNames[m_strCategory] = (LPVOID) ((DWORD)dwCount);
            return ERRORCODE_None;
         }
      dwMatchedLine = (DWORD) pVoid;
      if((DWORD) m_cmspCategoryNames.GetCount() == dwMatchedLine)
         return ERRORCODE_None;

     return ERRORCODE_DataSequenceError;
   }

int CCollectionBuilder::GetData()
   {
      CFile          cfileData;
      DWORD          dwFileLength, dwBytesRead;
		CString			csFileName = m_strFileName;
		BOOL				bExternal = !m_strExternalFile.IsEmpty();

		if (bExternal)
		{
			// Use the external name.
			csFileName = m_strExternalFile;
		}
		else
		{
			// We want to have internal data.
			// Handle the name mangling for the "add subdirectory" case.
			int nAddSubdirectory = m_Collection.GetAddSubdirectory();
			if (nAddSubdirectory != 0)
			{
				// Split off the path from the base name.
				CString csPath;
				Util::SplitPath(m_strFullDataFileName, &csPath, NULL);

#if 0 // GCB 2/24/99 - mirror directory structure of input files, per Alexei Kulberg
				while (nAddSubdirectory-- != 0 && !csPath.IsEmpty())
				{
					Util::RemoveBackslashFromPath(csPath);
					// Split off the last subdirectory from the path.
					CString csSubDir, csNewPath;
					Util::SplitPath(csPath, &csNewPath, &csSubDir);
					// Add this path on to the front of the base name.
					CString csOldName = csFileName;
					Util::ConstructPath(csFileName, csSubDir, csOldName);
					csPath = csNewPath;
				}
#else
				csFileName = m_CollectionData.m_strRelativeFileName;
				if (csFileName[0] == '\\')
					csFileName = csFileName.Right( csFileName.GetLength() - 1 );
#endif
			}

			// If the collection is being built for the Online Gallery,
			// then we prepend the special string "[[I:collection]]\"
			// to the front of the path name. No data for the item will
			// be stored in the .CDT file, and PrintMaster will go out
			// to the Online Gallery to download the data from the online
			// collection specified in the [[I:...]].
			if (m_bOnline)
			{
				CString csInternetHandle;
				csInternetHandle.Format("[[I:%s]]\\", m_strOnlineCollectionName);
				csFileName = csInternetHandle+csFileName;
			}
		}
		TRACE("Name of %s: %s\n", (LPCSTR)m_strFullDataFileName, (LPCSTR)csFileName);

      if(m_BuildType == CPMWCollection::typeSentiments)
         {
            m_CollectionData.m_ContentData.m_DataHeader.Set(m_strSentiment.GetLength()+1,
               m_strSentiment.GetLength()+1, m_nCurLine);
            m_CollectionData.m_ContentData.Attach(m_strSentiment.GetBuffer(1),
               m_strSentiment.GetLength()+1);
            m_CollectionData.m_strContentFileName = csFileName;
// FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
//            m_CollectionData.m_dwContentTimeStamp = 0;
				m_CollectionData.m_dwUnused = 0;
            return ERRORCODE_None;
         }

      ReleaseData();

		// If the file is external, don't add it to the .CDT file.
		// The "External File" mechanism was originally added to
		// build online collections. Now, for various reasons, online
		// building is done differently. However, external files might
		// be useful for other things, so the machanism has been left
		// in place.
		if (!bExternal)
		{
			// Read in the file data.
			if(cfileData.Open(m_strFullDataFileName, CFile::modeRead) == 0)
				return ERRORCODE_Open;
			dwFileLength = cfileData.GetLength();
			m_pContentData = new char [dwFileLength];
			if(!m_pContentData)
				return ERRORCODE_Memory;

			dwBytesRead = cfileData.ReadHuge(m_pContentData, dwFileLength);
			if(dwBytesRead != dwFileLength)
				return ERRORCODE_Read;

			if (!m_bOnline)
			{
				// Normal case, add the data to the collection.
				m_CollectionData.m_ContentData.m_DataHeader.Set(dwFileLength, dwFileLength, m_nCurLine);
				m_CollectionData.m_ContentData.Attach(m_pContentData, dwFileLength);
//		FF: DWORD m_dwContentTimeStamp was phased out starting in PM 5.0
//				m_CollectionData.m_dwContentTimeStamp = Util::ComputeTimeStamp(m_strFullDataFileName);
			}
			else
			{
				// For an online collection we write out the data to a file which can
				// be FTP'ed over to the online gallery server. If this file should be
				// compressed we'll attempt to compress it first.
// No Online collection processing for Preprocessor utility
#ifdef NEW_THUMBNAILS
				LPBYTE pUncompressedData = (LPBYTE)m_pContentData;
				DWORD dwUncompressedSize = dwFileLength;

		      LPBYTE pCompressedData = NULL;
				DWORD dwCompressedSize = 0;

				// Set up to write out the uncompressed data.
				CCompressInfo CompressInfo;
				CompressInfo.Set(dwUncompressedSize, dwUncompressedSize, 0);
				LPBYTE pWriteData = pUncompressedData;
				DWORD dwWriteSize = dwUncompressedSize;

				if (m_bCompressOnlineDataFiles
				 && (m_Collection.DataIsCompressable(m_strFullDataFileName))
				 && (pUncompressedData != NULL)
				 && (dwUncompressedSize > 0))
				{
			      CDeflate Deflator;

					if (Deflator.Compress(
							pUncompressedData,
							dwUncompressedSize,
							&pCompressedData,
							&dwCompressedSize))
					{
						// We have data. If the size got smaller, use it.
						if (dwCompressedSize < dwUncompressedSize)
						{
							// Set up to write out the compressed data.
							CompressInfo.Set(dwUncompressedSize, dwCompressedSize, 2);
							pWriteData = pCompressedData;
							dwWriteSize = dwCompressedSize;
						}
					}
				}

				// Write out the data.
				ASSERT(pWriteData != NULL);
				ASSERT(dwWriteSize > 0);

				if ((pWriteData == NULL) || (dwWriteSize == 0))
				{
					CString csMessage;
					csMessage.Format("Empty online file data for %s\n", m_strFullDataFileName);
					ReportError(csMessage, CCollectionBuilderStats::typeError);
					return ERRORCODE_Memory;
				}

				int nIndex = csFileName.Find('\\');
				CString csDownloadName = csFileName.Mid(nIndex+1);

				CString csFtpFile = m_strOnlineFtpPath;
				csFtpFile += csDownloadName;

				CString csDirectory;
				Util::SplitPath(csFtpFile, &csDirectory, NULL);
				if (!Util::DirectoryExists(csDirectory))
				{
					if (!Util::MakeDirectory(csDirectory))
					{
						return ERRORCODE_Write;
					}
				}

				TRACE("Write %ld bytes to %s\n", dwWriteSize, (LPCSTR)csFtpFile);

				TRY
				{
					CFile cfOutput(csFtpFile, CFile::modeCreate|CFile::modeWrite);

					if (m_bCompressOnlineDataFiles)
						cfOutput.Write(&CompressInfo, sizeof(CompressInfo));
					cfOutput.Write(pWriteData, dwWriteSize);
					cfOutput.Close();
				}
				CATCH_ALL(e)
				{
					CString csMessage;
					csMessage.Format("Can't write FTP data for %s", (LPCSTR)csFtpFile);
					ReportError(csMessage, CCollectionBuilderStats::typeError);
				}
				END_CATCH_ALL

				// Free any data allocated by the deflator (it checks for NULL).
		      CDeflate::FreeData(pCompressedData);

				TRY
				{
					CString csID = csDownloadName;
					Util::NormalizeContentServerID(csID);

					CString csDownloadFileName = m_strOnlineCollectionName+"\\"+csDownloadName;
					Util::FileNameToURLForm(csDownloadFileName);

					CString csSql;
					// GCB 1/21/99 - support for additional online collection SQL installation text file fields
					CString csFriendlyName( m_CollectionData.m_strFriendlyName );
					CString csMajorCategory( MajorCategoryFromBoth( m_strCategory ) );
					CString csMinorCategory( MinorCategoryFromBoth( m_strCategory ) );
					CString csKeywords( m_CollectionData.m_strKeywords );
					CString csTone( ToneEnumToString( (CCategoryDataItem::Tones)m_nTone ) );
					CString csGraphicType( GraphicTypeEnumToString( m_GraphicTypeFlags ) );

					int index = csDownloadFileName.ReverseFind( '.' );
					if (index < 0)
						index = -1;
					CString csFileType( csDownloadFileName.Right( csDownloadFileName.GetLength() - index - 1 ) );

					csSql.Format("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
									 (LPCSTR)csID,
									 (LPCSTR)csDownloadFileName,
									 (LPCSTR)csFriendlyName,
									 (LPCSTR)csMajorCategory,
									 (LPCSTR)csMinorCategory,
									 (LPCSTR)csKeywords,
									 (LPCSTR)csTone,
									 (LPCSTR)csGraphicType,
									 (LPCSTR)csFileType);
					m_stdioSqlInstallList.WriteString(csSql);
				}
				CATCH_ALL(e)
				{
					ReportError("Can't write SQL string", CCollectionBuilderStats::typeError);
				}
				END_CATCH_ALL
#endif
         }
		}

		// Fill in the rest of the fields.
		m_CollectionData.m_strContentFileName = csFileName;
      m_CollectionData.m_dwUnused = 0;

      return ERRORCODE_None;
   }

CString CCollectionBuilder::MajorCategoryFromBoth( CString csMajorAndMinorCategory )
{
	int index = csMajorAndMinorCategory.Find( '|' );
	if (index < 0)
		index = csMajorAndMinorCategory.GetLength();
	return csMajorAndMinorCategory.Left( index );
}

CString CCollectionBuilder::MinorCategoryFromBoth( CString csMajorAndMinorCategory )
{
	int index = csMajorAndMinorCategory.Find( '|' );
	if (index < 0)
		index = -1;
	return csMajorAndMinorCategory.Right( csMajorAndMinorCategory.GetLength() - index - 1 );
}

char* CCollectionBuilder::ToneEnumToString( CCategoryDataItem::Tones tone )
{
	switch(tone)
	{
	case CCategoryDataItem::toneNone: return "None";
	case CCategoryDataItem::toneHumorous: return "Humorous";
	case CCategoryDataItem::toneSentimental: return "Sentimental";
	case CCategoryDataItem::toneTraditional: return "Traditional";
	case CCategoryDataItem::toneContemporary: return "Contemporary";
	case CCategoryDataItem::toneSpiritual: return "Spiritual";
	default:
		CString strError;
		strError.Format( "Line %05i: Invalid selection in TONE\n", m_nCurLine );
		ReportError( strError );
		m_bValidationError = TRUE;
	}
	return NULL;
}

char* CCollectionBuilder::GraphicTypeEnumToString( CCategoryDataItem::CustomFlags graphicType )
{
	switch(graphicType)
	{
	case CCategoryDataItem::cfNone:return "None";
	case CCategoryDataItem::cfColorGraphic: return "ColorGraphic";
	case CCategoryDataItem::cfBWGraphic: return "BWGraphic";
	case CCategoryDataItem::cfPhoto: return "Photo";
	case CCategoryDataItem::cfClipPhoto: return "ClipPhoto";
	case CCategoryDataItem::cfFineArt: return "FineArt";
	default:
		CString strError;
		strError.Format( "Line %05i: Invalid selection in GRAPHIC TYPE\n", m_nCurLine );
		ReportError( strError );
		m_bValidationError = TRUE;
	}
	return NULL;
}

void CCollectionBuilder::ReleaseData()
   {
      if(m_pContentData)
         delete m_pContentData;
      m_pContentData = NULL;
   }

int CCollectionBuilder::AddItems()
   {
      int               nColumn;
      CString           strFriendlyName;


      nColumn = m_csaHeaderFields[0].GetStartColumn(CCollHeaderArray::FriendlyName);
      m_CollectionData.m_strFriendlyName = m_csaFields.GetAt(nColumn);
      return ERRORCODE_None;
   }

DWORD CCollectionBuilder::RangeToFlagValue(int nStartColumn, int nEndColumn)
   {
      int      nCurField, nNumSet=0;
      DWORD    dwValue = 0;
      BOOL     bOn;

      for(nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++)
         {
            bOn = atoi(m_csaFields.GetAt(nCurField));
            if(bOn)
               {
                  dwValue |= (1 << (nCurField-nStartColumn));
                  nNumSet++;
               }
         }
      if(nNumSet != 1)
         {
            CString  strError;

            strError.Empty();
            strError.Format("Line %05i: ", m_nCurLine);
            if(nNumSet > 1)
               strError += "More than one selection for TONE";
            else
               strError += "No selection in TONE";
            strError += "\n";
            ReportError(strError);
            m_bValidationError = TRUE;
         }
      return dwValue;
   }

DWORD CCollectionBuilder::BuildFlags(int nStartColumn, int nEndColumn)
   {
      int      nCurField, nNumSet=0;
      DWORD    dwValue = 0;
      BOOL     bOn;

      for(nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++)
         {
            bOn = atoi(m_csaFields.GetAt(nCurField));
            if(bOn)
               {
                  dwValue |= (1 << (nCurField-nStartColumn));
                  nNumSet++;
               }
         }
#if 0 // GCB 1/28/99 relax error checking per Mari Collings and Corey Day
      if((nNumSet == 0) && (m_nInclusionColumn == -1))
         {
            CString  strError;

            strError.Empty();
            strError.Format("Line %05i: No Product Specified\n", m_nCurLine);
            ReportError(strError);
            // Don't error out on this for now
//            m_bValidationError = TRUE;
         }
#endif // 0
      if((nNumSet != 0) && (m_nInclusionColumn != -1))
         {
            CString  strError;

            strError.Empty();
            strError.Format("Line %05i: Product Specified with inclusion column\n", m_nCurLine);
            ReportError(strError);
            // Don't error out on this for now
//            m_bValidationError = TRUE;
         }
      return dwValue;
   }

CCategoryDataItem::CustomFlags CCollectionBuilder::SetGraphicTypeFlags(int nStartColumn, int nEndColumn)
	{
		int										nCurField, nNumSet=0;
		DWORD										dwValue = 0;
		BOOL										bOn;

		for(nCurField = nStartColumn;nCurField <= nEndColumn;nCurField++)
		{
			bOn = atoi(m_csaFields.GetAt(nCurField));
			if(bOn)
			{
				// graphic type uses bits 11-15 out of bits 0..15
				dwValue |= (1 << (nCurField-nStartColumn+11));
				nNumSet++;
			}
		}
		if(nNumSet > 1)
		{
			CString  strError;

			strError.Empty();
			strError.Format("Line %05i: ", m_nCurLine);
			strError += "More than one selection for GRAPHIC TYPE";
			strError += "\n";
			ReportError(strError);
			// REVIEW GCB 1/20/99 multiple graphic type selections are currently supported in the
			// collection file format but probably won't be in the graphic server database
			// so consider this a "fatal" error; may want to consider changing the collection file
			// format to save space in the file (use mutually exclusive values instead of bit fields)
			m_bValidationError = TRUE;
		}
		return (CCategoryDataItem::CustomFlags)dwValue;
	}

void CCollectionBuilder::ReportError(const CString &strError, CCollectionBuilderStats::StatisticTypes statType)
   {
      m_stdioLogFile.WriteString(strError);
      m_stdioLogFile.Flush();    // Flush buffers 
	  m_BuildStats.IncrementCount(statType);
   }

int CCollectionBuilder::DumpCategoryTree()
   {
      int                        nRetVal;
      CCategoryDatabase          *pCategoryFile;
      CCategoryData              categoryData;
      CString                    strIniFile = m_Collection.GetIniFile(), strSection=m_Collection.GetSection();

      nRetVal = m_Collection.Init(m_pPathManager,
											 NULL,
											 strIniFile,
											 strSection,
											 CPMWCollection::originGeneric,
											 CPMWCollection::modeRead);
      if(nRetVal != ERRORCODE_None)
         {
            switch(nRetVal)
               {
                  case ERRORCODE_NoINIFile:
                     AfxMessageBox("Init. Failed - Missing .INI File");
                     break;
                  case ERRORCODE_InvalidINIEntry:
                     AfxMessageBox("Init. Failed - Invalid INI Entry");
                     break;
                  case ERRORCODE_StopListIOError:
                     AfxMessageBox("Init. Failed - Missing Stoplist file");
                     break;
                  default:
                     AfxMessageBox("Builder Init. Failed");
                     break;
               }
            return ERRORCODE_Fail;
         }
      TRY
         {
            // Open databases
            m_strFileNamePrefix = m_Collection.GetDBFileNamePrefix();
            m_stdioLogFile.Open(m_strFileNamePrefix + ".DMP",
               CFile::modeCreate | CFile::modeWrite | CFile::typeText);
            nRetVal = m_Collection.NewCategoryStream(&pCategoryFile);
            if(nRetVal != ERRORCODE_None)
            {
               AfxMessageBox("Unable to allocate Category stream");
               return nRetVal;
            }
            pCategoryFile->GetDir()->Dump(m_stdioLogFile);
         }
      CATCH_ALL(exception)
         {
            AfxMessageBox("Exception occurred in builder!");
         }
      END_CATCH_ALL

      // Closes/cleanup
      m_stdioLogFile.Close();
      m_Collection.ReleaseCategoryStream(pCategoryFile);
      return ERRORCODE_None;
   }
#endif


