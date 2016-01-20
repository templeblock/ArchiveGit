/*
// $Header: /PM8/App/DOCPICT.CPP 1     3/03/99 6:05p Gbeddow $
//
// Document picture/background routines.
//
// $Log: /PM8/App/DOCPICT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 42    2/26/99 8:45p Rgrenfel
// Fixed assumptions on usage of GraphicObjects.
// 
// 41    2/25/99 3:27p Lwilson
// Changed component creation to use a pointer to a CCreationData class or
// variant for in place of an RComponentDocument pointer.
// 
// 40    2/20/99 8:59p Psasse
// Support for vertical banners
// 
// 39    2/04/99 5:58p Rgrenfel
// Fixed a bug with the Art Gallery that was causing the system to crash
// after pressing cancel on a CD request.
// 
// 38    1/27/99 1:40p Lwilson
// Updated handle_new_object to use the objects refresh bounds, instead of
// bounds for initial update.
// 
// 37    1/11/99 1:32p Gbeddow
// support for reading/writing Print Shop square, row & column vector
// graphics in Pretzel collection files
// 
// 36    12/17/98 4:32p Rgrenfel
// Use original GraphicObject when CGM files are read
// 
// 35    12/17/98 8:57a Rgrenfel
// Changed the name for getting an image document to a generic GetDocument
// call.
// 
// 34    12/16/98 6:21p Rgrenfel
// Changed application to use the Image Component in place of the Graphic
// Object
// 
// 33    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 32    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 31    9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 30    9/14/98 12:16p Jayn
// 
// 29    9/14/98 9:20a Mwilson
// added entry to error log for missing graphic if using the print
// projects tool
// 
// 28    8/26/98 5:40p Hforman
// fix bug in free_import_picture_name()
// 
// 27    8/25/98 6:35p Hforman
// made create_chosen_picture() work with no filename
// 
// 26    8/10/98 4:02p Hforman
// fix aspect ratio if image was cropped at creation
// 
// 25    7/30/98 2:18p Jayn
// Kills any hints before complaining about missing pictures.
// 
// 24    7/22/98 3:11p Dennis
// Added PNG file type to handle_missing_graphic
// 
// 23    7/22/98 12:28p Rlovejoy
// Make composite path from graphic filename.
// 
// 22    7/18/98 12:08p Jayn
// 
// 21    7/14/98 5:01p Rlovejoy
// Close the hiddent PMO document.
// 
// 20    7/07/98 4:12p Jayn
// Needed to make full path for non-collection name in ExtractFile().
// 
// 19    6/24/98 7:20p Psasse
// correctly extracting *.pmo and *.dtt files
// 
// 18    6/01/98 1:23p Mwilson
// changed embed_all_pictures to embed_pictures with a flag for embedding
// all.  
// 
// 17    5/21/98 3:03p Dennis
// Removed obsolete code
// 
// 16    5/21/98 1:56p Dennis
// Added Art Collections to search by driven by CSearchArtCollections
// object.
// 
// 15    5/19/98 8:51a Fredf
// Changes .GIF to .PNG
// 
// 14    5/01/98 3:17p Jayn
// 
// 13    4/23/98 3:28p Dennis
// Project checker now looks in AG1ART collection for missing art.
// 
// 12    4/16/98 6:53p Hforman
// new Open dialog
// 
// 11    3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 10    3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
// 9     2/10/98 5:24p Hforman
// paste ".pmo" objects into center of page in create_chosen_picture(),
// 
// 8     2/05/98 11:03a Jayn
// Added lookup in TEMPART.
// 
// 7     2/04/98 4:51p Hforman
// add flag to PostPaste()
// 
// 6     1/30/98 7:21p Hforman
// 
// 5     1/29/98 8:10p Hforman
// support for ".pmo" files
// 
// 4     1/13/98 6:07p Hforman
// add parent param to ChoosePicture()
// 
// 3     12/12/97 2:55p Johno
// Moved resize stuff from CPmwView::OnPhotoEnhancer
// to CPmwDoc::DoReplacePicture.
// 
// 2     11/19/97 2:03p Jstoner
// multi-calendar mods to calendar cell functions
// 
//    Rev 1.0   14 Aug 1997 15:20:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:12   Fred
// Initial revision.
// 
//    Rev 1.30   01 Aug 1997 13:37:42   Fred
//  
// 
//    Rev 1.29   30 Jul 1997 10:43:44   Jay
// Increased refresh bounds for line. More flow frame cases.
// 
//    Rev 1.28   30 Jul 1997 09:06:28   Jay
// Added od() for missing graphic when left missing.
// 
//    Rev 1.27   02 Jul 1997 14:25:48   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.26   02 Jul 1997 10:03:22   Jay
// Passes graphic parameters through to Open From Disk
// 
//    Rev 1.25   02 Jul 1997 09:30:44   Jay
// Address book choose picture now works
// 
//    Rev 1.24   02 Jul 1997 09:03:30   Jay
// Modal picture selection
// 
//    Rev 1.23   27 Jun 1997 16:04:56   Jay
// Photo CD
// 
//    Rev 1.22   12 Jun 1997 17:09:58   Jay
// Try remapping missing TIF to CGM
// 
//    Rev 1.21   28 May 1997 09:17:16   Jay
// Catalog support. Better transparent previews.
// 
//    Rev 1.20   14 May 1997 14:06:36   Jay
// Doesn't clip a new picture if it has an absolute position.
// 
//    Rev 1.19   12 May 1997 18:08:32   Jay
// Browser drag & drop
// 
//    Rev 1.18   09 May 1997 15:41:00   Jay
// Faster collection lookup. Eliminated unused code.
// 
//    Rev 1.17   06 May 1997 10:30:06   Jay
// New browser stuff.
// 
//    Rev 1.16   30 Apr 1997 16:34:06   Jay
// Fixed a looping condition.
// 
//    Rev 1.15   25 Apr 1997 12:49:08   Jay
// Tries to find missing graphics in the PMG4ART collection.
// No longer uses picture map (should be removed).
// 
//    Rev 1.14   09 Apr 1997 17:14:06   Jay
// PhotoEnhancer
// 
//    Rev 1.13   03 Apr 1997 14:31:14   Jay
// Resource cleanup
// 
//    Rev 1.12   28 Mar 1997 17:10:44   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.11   05 Nov 1996 15:18:16   johno
// Moved strings to resource DLL
// 
//    Rev 1.10   01 Nov 1996 17:12:20   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.9   28 Oct 1996 09:48:44   johno
// Moved strings to resource file
// 
//    Rev 1.8   25 Aug 1996 13:24:38   Jay
// Friendly names for pictures
// 
//    Rev 1.7   01 Aug 1996 17:45:34   Jay
// Better handling panels, pages, and undo.
// 
//    Rev 1.6   29 Jul 1996 10:14:48   Jay
// Picture color -> picture attributes
// 
//    Rev 1.5   23 Jul 1996 12:54:30   Jay
// Removed background stuff.
// 
//    Rev 1.4   18 Jul 1996 11:11:24   Jay
// csDrive -> dsDir; BUG!
// 
//    Rev 1.3   16 Jul 1996 11:26:32   Jay
// Linked frames with cut/copy/paste/duplicate
// 
//    Rev 1.2   24 May 1996 16:11:54   Fred
// TRACEx
// 
//    Rev 1.1   23 Apr 1996 08:10:06   Jay
// Changes to the way graphics, fonts, etc are accessed from Document Record.
// 
//    Rev 1.0   14 Mar 1996 13:36:36   Jay
// Initial revision.
// 
//    Rev 1.2   13 Mar 1996 14:50:20   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.1   06 Mar 1996 10:28:20   Jay
// Changes from 2.1
// 
//    Rev 2.31   27 Feb 1996 16:50:02   JAY
// New filename translation system supporting subfiles.
// 
//    Rev 2.30   14 Feb 1996 16:19:00   JAY
// Fixed a bug with illegal CD names and picture mapping.
// 
//    Rev 2.29   02 Feb 1996 13:07:32   JAY
//  
// 
//    Rev 2.28   24 Jan 1996 10:04:00   JAY
// New string management in Configuration
// 
//    Rev 2.27   26 Dec 1995 10:30:18   FRED
// Project structure changes for mac port
// 
//    Rev 2.26   20 Dec 1995 09:56:34   JAY
// 32-bit fixes
// 
//    Rev 2.25   29 Sep 1995 16:00:58   JAY
// More drag/drop/paste/OLE stuff
// 
//    Rev 2.24   13 Sep 1995 11:48:30   JAY
// New 32-bit changes
// 
//    Rev 2.23   06 Sep 1995 10:17:22   JAY
// Now sets the correct sound context for picture/background attributes.
// 
//    Rev 2.22   05 Sep 1995 14:41:52   JAY
// Busy cursor around graphic creation.
// 
//    Rev 2.21   01 Sep 1995 12:36:16   JAY
// Now does special stuff for Photo CDs.
// 
//    Rev 2.20   31 Aug 1995 07:39:00   JAY
// Photo CD dialog support
// 
//    Rev 2.19   23 Aug 1995 12:33:58   JAY
// Now uses CStrings instead of stack buffers.
// 
//    Rev 2.18   21 Aug 1995 18:42:36   FRED
// Modeless picture album
// 
//    Rev 2.17   20 Aug 1995 15:39:30   FRED
// Rotate object control
// 
//    Rev 2.16   18 Aug 1995 15:07:20   JAY
// Doesn't attempt to embed a picture that has an image.
// 
//    Rev 2.15   11 Aug 1995 09:38:28   JAY
// Fixed/extended duplicate.
// 
//    Rev 2.14   10 Aug 1995 07:18:54   JAY
// Move PICTURES.MAP file to RUNTIME.
// 
//    Rev 2.13   08 Aug 1995 17:33:24   JAY
// Now toggles picture off before replacing it since the replacement can
// change the select handles.
// 
//    Rev 2.12   17 Jul 1995 15:10:14   JAY
// Added a read buffer to the picture map read.
// 
//    Rev 2.11   17 Jul 1995 08:29:28   JAY
// Fixed the mapping stuff.
// PICTURE.MAP must be all upper-case for this to work.
// 
//    Rev 2.10   16 Jul 1995 18:55:44   FRED
// Album Enhancements
// 
//    Rev 2.9   15 Jul 1995 17:19:24   FRED
// Open picture from disk
// 
//    Rev 2.8   14 Jul 1995 17:24:30   JAY
// Picture mapping.
// 
//    Rev 2.7   13 Jul 1995 13:39:20   JAY
// Only sets "landscape" for backgrounds if the aspect warrants it.
// 
//    Rev 2.6   10 Jul 1995 16:26:28   JAY
// More useful stuff for save project.
// 
//    Rev 2.5   26 Jun 1995 16:58:42   JAY
//  
// 
//    Rev 2.4   06 Jun 1995 16:54:52   JAY
// Project previews in the album.
// 
//    Rev 2.3   09 Mar 1995 08:37:34   JAY
// More undo.
// 
//    Rev 2.2   15 Feb 1995 17:15:56   JAY
// Undo
// 
//    Rev 2.1   08 Feb 1995 13:36:18   JAY
// Reverted. New series.
// 
//    Rev 1.6   09 Jan 1995 17:01:04   JAY
// Cut & paste, etc.
// 
//    Rev 1.5   04 Aug 1994 07:35:06   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   18 Jul 1994 10:06:36   JAY
// Converts "illegal" characters in missing graphic filenames to "_"s.
// 
//    Rev 1.3   14 Jun 1994 17:01:18   JAY
// Embedded pictures support.
// 
//    Rev 1.2   31 May 1994 12:47:08   JAY
// Fixed some LPSTR/LPCSTR stuff.
// 
//    Rev 1.1   26 May 1994 13:46:10   JAY
// Moved virtual file list searching code to path manager.
// virtual file name component can now be late-binding.
// 
//    Rev 1.0   23 May 1994 10:50:38   JAY
// Initial revision.
*/

#include "stdafx.h"
#include <ctype.h>

#include "pmw.h"
#include "pmwdoc.h"
#include "system.h"

#include "posdlg.h"
#include "OpenDlgs.h"
#include "pictattr.h"
#include "utils.h"
#include "util.h"

#include "calobj.h"
#include "grafobj.h"
#include "backobj.h"
#include "frameobj.h"
#include "dateobj.h"
#include "CompObj.h"
#include "CreateData.h"

#include "compfn.h"					// Support for CompositeFileName's
#include "cntritem.h"

#include "commands.h"
#include "ccommand.h"
#include "pmwini.h"
#include "pmwcfg.h"
#include "browser.h"
#include "clip.h"		// for PasteObjectList()
#include "srchart.h"	// For SearchArtCollections define.

// Framework Support
ASSERTNAME
#include "ComponentDocument.h"

/////////////////////////////////////////////////////////////////////////////
// More CPmwDoc stuff

static BOOL FindPhotoCD(CString& csOutPath)
{
/*
// Scan the CD drives for a CD with the look of a Photo CD.
*/
	IDriveManager* pDriveManager = GetGlobalDriveManager();
	IPathManager* pPathManager = GetGlobalPathManager();

	static char cbImageDir[] = "x:\\PHOTO_CD\\IMAGES";

	BOOL fFound = FALSE;
	char cFirstCD = 0;

	for (char cDrive = 'A'; cDrive < 'Z'; cDrive++)
	{
		if (pDriveManager->GetDriveType(cDrive) == DT_CDROM)
		{
		// Remember the first CD drive if necessary.
			if (cFirstCD == 0)
			{
				cFirstCD = cDrive;
			}
		// See if the directory structure exists that we want.
			cbImageDir[0] = cDrive;
			CString csName = cbImageDir;
			csName += "\\IMG0001.PCD";

			if (pPathManager->FileExists(csName, FALSE))
			{
				fFound = TRUE;
				break;
			}
		}
	}

	if (fFound)
	{
		csOutPath = cbImageDir;
	}

	return fFound;
}

int CPmwDoc::ChooseExternalPicture(LPCSTR pszLastName, RectPageObjectPtr pLastPicture, LPCSTR pszDefaultPath, BOOL fPhotoCD /*=FALSE*/)
{
	BOOL fGotName = FALSE;
   CString csLastName;

/* Handle the last object. */

	if (pLastPicture != NULL)
	{
		const CString *pstrFileName = pLastPicture->GetOriginalDataPath();
		if (pstrFileName != NULL)
		{
			csLastName = GetPathManager()->ExpandPath(*pstrFileName, TRUE);
			if (!csLastName.IsEmpty())
			{
				fGotName = TRUE;
			}
		}
//		DB_RECORD_NUMBER g_record = pLastPicture->get_graphic_record();
//		GraphicPtr graphic;
//
//		if ((graphic = (GraphicPtr)pLastPicture->get_database()->get_record(g_record, NULL, RECORD_TYPE_Graphic)) != NULL)
//		{
//		/* Set the last picture name to this so we'll be in the dialog. */
//			csLastName = GetPathManager()->ExpandPath(graphic->m_csFileName, TRUE);
//
//			graphic->release();
//			fGotName = TRUE;
//		}
	}

	{
		CPmwApp *pApp = GET_PMWAPP();

		CString csDrive;
		CString csDir;
		CString csFname;
		CString csExt;
		CString csFullDir;
		CString csFullName;

		LPSTR drive;
		LPSTR dir;
		LPSTR fname;
		LPSTR ext;
		LPSTR full_dir;
		LPSTR full_name;

		TRY
		{
#ifndef _MAC
			//TODO:MAC drive and ext not supported
			drive = csDrive.GetBuffer(_MAX_DRIVE);
			ext = csExt.GetBuffer(_MAX_EXT);
#endif
			dir = csDir.GetBuffer(_MAX_DIR);
			fname = csFname.GetBuffer(_MAX_FNAME);
			full_dir = csFullDir.GetBuffer(_MAX_PATH);
			full_name = csFullName.GetBuffer(_MAX_PATH);
		}
		CATCH_ALL(e)
		{
			return ERROR;
		}
		END_CATCH_ALL

		if (fGotName)
		{
			strcpy(full_name, csLastName);
		}
		else
		{
		/* Handle the global last name. */

			CString csName = pszLastName;
			if (csName.IsEmpty())
			{
				csName = GET_PMWAPP()->get_last_picture_name();
			}

			if (csName.IsEmpty())
			{
				strcpy(full_dir, GetPathManager()->ExpandPath(pszDefaultPath));
				*full_name = '\0';
			}
			else
			{
				strcpy(full_name, GetPathManager()->ExpandPath(csName, TRUE));
				fGotName = TRUE;
			}
		}

		CString csLastName = full_name;

		if (fGotName)
		{
#ifndef _MAC
			// TODO:MAC drive and ext not supported -- splitpath not supported
			_splitpath(full_name, drive, dir, fname, ext);
#endif
			sprintf(full_dir, "%s%s", drive, dir);
			sprintf(full_name, "%s%s", fname, ext);
		}

		CString filters;
		GetConfiguration()->PictureFilter(filters);

	/* If this is a Photo CD load, do our special stuff. */

      if (fPhotoCD)
		{
		/*
		// If we have a valid Photo CD name, and the file still exists, then
		// we're all set. Otherwise, we must prompt for the user to enter
		// a CD.
		*/
			if (type_of_graphic_extension(csLastName) != GRAPHIC_TYPE_PCD
				 || !GetPathManager()->FileExists(csLastName, FALSE))
			{
				CString csPhotoCD;
				if (FindPhotoCD(csPhotoCD))
				{
				/* We found a Photo CD. */
					strcpy(full_dir, csPhotoCD);
					full_name[0] = '\0';
				}
				else
				{
				/*
				// Put up a dialog asking the user to enter their CD.
				*/
					if (AfxMessageBox(IDS_PUT_IN_PHOTO_CD, MB_OKCANCEL | MB_ICONQUESTION)
											 != IDOK)
					{
						return IDCANCEL;
					}

					if (FindPhotoCD(csPhotoCD))
					{
					/* We found a Photo CD. */
						strcpy(full_dir, csPhotoCD);
						full_name[0] = '\0';
					}
					else
					{
					/* We didn't find a Photo CD. Go to the first CD drive. */
						// Well, the first CD drive may be empty. Then, the user
						// would get an error message. Maybe we should leave well
						// enough alone. Let's tell the user what has happened...
						AfxMessageBox(IDS_NO_PHOTO_CD_FOUND, MB_OK | MB_ICONEXCLAMATION);
					}
				}
			}
		}

		// Create the dialog.
		COpenPictureDlg picdlg(full_name, full_dir, filters,
									  CWnd::GetSafeOwner(NULL));

		SHORT ret;
		if ((ret = picdlg.DoModal()) == IDOK)
		{
			SetImportPictureName(GetPathManager()->MakeCompositePath(picdlg.GetPathName()));

			gcs.proof = TRUE;
			gcs.embed_source = picdlg.m_fEmbedPicture;
			// Set the "last picture" name in the app.
			GET_PMWAPP()->set_last_picture_name(gcs.m_csFileName);
		}
		return ret;
	}
}

/*
// Choose a picture.
*/

int CPmwDoc::ChoosePicture(LPCSTR last_name /*=NULL*/, RectPageObjectPtr last_picture /*=NULL*/, CWnd* pParent /*=NULL*/)
{
	CArtBrowserDialog Browser(CArtBrowserDialog::TYPE_ModalPicture, pParent);
	Browser.m_pDoc = this;
   Browser.m_pszLastPicture = last_name;
ASSERT( 0 );	// put in the last picture, but not as a graphic object.
//	Browser.m_pLastPicture = last_picture;

	return Browser.DoModal();
}

ERRORCODE CPmwDoc::create_chosen_picture(PBOX_PTR view_pbox, PPNT* pOrigin /*=NULL*/)
{
	PMGDatabasePtr database = get_database();
	ERRORCODE error;
	BOOL fExtracted = FALSE;

	BeginWaitCursor();

	// See what kind of graphic type we have.
	// If there's no filename, there better be a DIB in the gcs!

	GRAPHIC_TYPE graphicType;
	if (gcs.m_csFileName.IsEmpty())
		graphicType = GRAPHIC_TYPE_BMP;
	else
		graphicType = type_of_art_extension(gcs.m_csFileName);

	RComponentDocument *pCompDoc;

	switch (graphicType)
	{
		case GRAPHIC_TYPE_PMO:
		{
			CString csFileName = gcs.m_csFileName;
			fExtracted = ExtractFile(csFileName);

			CPmwDoc* pTempDoc = GET_PMWAPP()->OpenHiddenDocument(csFileName);
			if (pTempDoc)
			{
				// paste objects into center of document
				ObjectList* pList = pTempDoc->object_list();

				PPNT dimsPmo = pTempDoc->get_dimensions();
				PPNT dimsDoc = get_dimensions();
				PCOORD x0 = ((dimsDoc.x / 2) - (dimsPmo.x / 2));
				PCOORD y0 = ((dimsDoc.y / 2) - (dimsPmo.y / 2));

				PBOX pbxWorld;
				pbxWorld.x0 = -x0;
				pbxWorld.x1 = -x0 + dimsDoc.x;
				pbxWorld.y0 = -y0;
				pbxWorld.y1 = -y0 + dimsDoc.y;

				PrePaste();
				if ((error = PasteObjectList(this, pList, pbxWorld)) == ERRORCODE_None)
					PostPaste(FALSE);
				else
					AbortPaste();

				if(fExtracted)
				{
					TRY
					{
					   CFile::Remove(csFileName);
					}
					END_TRY
				}

				// Close the hidden doc
				pTempDoc->OnCloseDocument();
			}
			else
				error = ERRORCODE_Fail;
			break;
		}
		case GRAPHIC_TYPE_DPP:
		{
			// Serif Draw Plus file.
			// Convert to a Serif object.
			CPmwCntrItem* pItem = NULL;
			CString csFileName;
			TRY
			{
				csFileName = gcs.m_csFileName;
				fExtracted = ExtractFile(csFileName);

				// We need to create an OLE object to contain this.
				// Create new item connected to this document.
				pItem = new CPmwCntrItem(this);
				ASSERT_VALID(pItem);

				// Initialize the item using the file name passed.
				if (pItem->CreateFromFile(csFileName))
				{
					deselect_all();

					if ((error = FinishOleObjectCreate(pItem, NULL, OBJECT_TYPE_Serif)) != ERRORCODE_None)
					{
						AfxThrowMemoryException();  // any exception will do
					}
				}
				else
				{
					error = ERRORCODE_Fail;
				}
				
				if(fExtracted)
				{
					TRY
					{
					   CFile::Remove(csFileName);
					}
					END_TRY
				}

			}
			CATCH_ALL(e)
			{
				error = ERRORCODE_Fail;
			}
			END_CATCH_ALL

			if (error != ERRORCODE_None)
			{
				delete pItem;
			}
			break;
		}
		case GRAPHIC_TYPE_CGM:
#if 0
		{
			// Support CGM graphics through the original Graphic Object for now.
			// We might eventually be able to support these in the image component
			// in the future by porting the built in CGM support or by using the
			// LeadTools CGM filter.
			GraphicObjectPtr object;
			if ((object = database->create_graphic_object(&gcs)) == NULL)
			{
				error = database->last_creation_error();
			}
			else
			{
				handle_new_object(object, view_pbox, TRUE, pOrigin);
				error = ERRORCODE_None;
			}
			break;
		}
#endif
#if 0 
		// This is no longer necessary, as the component object
		// can now be created with a GRAPHIC_CREATE_STRUCT.
		
		case GRAPHIC_TYPE_PS_SQUARE:
		case GRAPHIC_TYPE_PS_ROW:
		case GRAPHIC_TYPE_PS_COLUMN:
			// PrintShop square, row & column vector graphics
			pCompDoc = ComponentObject::CreateGraphicDocument( &gcs, graphicType );
			if (pCompDoc != NULL)
			{
				error = create_new_component(pCompDoc, view_pbox, pOrigin);
			}
			else
			{
				error = ERRORCODE_DoesNotExist;
			}
			break;
		default:
		{
			// Create the graphic.
			pCompDoc = ComponentObject::CreateImageDocument( &gcs );
			if (pCompDoc != NULL)
			{
				error = create_new_component(pCompDoc, view_pbox, pOrigin);
			}
			else
			{
				error = ERRORCODE_DoesNotExist;
			}
		}
#else
		default:
		{
			CGraphicData cData( &gcs );
			error = create_new_component( &cData, view_pbox, pOrigin );
		}
#endif
	}
	EndWaitCursor();

	// Put up a message about LZW if needed.
	if (error == ERRORCODE_LZWNotSupported)
	{
		// Complain!
		GetConfiguration()->SupportsLZW();
	}
	return error;
}

void CPmwDoc::handle_new_object(RectPageObjectPtr object, PBOX_PTR view_pbox /*=NULL*/, BOOL fAppend /*=TRUE*/, PPNT* pAlternateOrigin /*=NULL*/)
{
	PMGDatabasePtr database = get_database();

/* Set the panel. */

	object->set_panel(get_current_panel());

/*
// Get the dimensions of the new graphic.
*/

	PPNT dims;

	if (!object->original_dims(&dims))
	{
		dims.x = dims.y = PAGE_RESOLUTION;
	}
	else
	{
		// image may have been cropped, so scale appropriately
		CDoubleRect& visibleRect = object->GetVisibleRect();
		dims.x = PCOORD((double)dims.x * visibleRect.Width());
		dims.y = PCOORD((double)dims.y * visibleRect.Height());
	}

/* Make the bound be centered and this big. */

	PBOX world, bound;

	get_panel_world(&world);

	if (view_pbox != NULL)
	{
		IntersectBox(&world, &world, view_pbox);
	}

	if (pAlternateOrigin == NULL)
	{
		if ((bound.x0 = (world.x0 + world.x1 - dims.x)/2) < world.x0)
		{
			bound.x0 = world.x0;
		}
		if ((bound.y0 = (world.y0 + world.y1 - dims.y)/2) < world.y0)
		{
			bound.y0 = world.y0;
		}
	}
	else
	{
		bound.x0 = pAlternateOrigin->x;
		bound.y0 = pAlternateOrigin->y;
	}
	bound.x1 = bound.x0 + dims.x;
	bound.y1 = bound.y0 + dims.y;

	if (pAlternateOrigin == NULL)
	{
		/* Make sure we stay within the world if possible. */

		if (bound.x1 > world.x1 && get_project_type() != PROJECT_TYPE_Banner)
		{
			bound.y1 = bound.y0
					+ scale_pcoord(world.x1 - bound.x0, dims.y, dims.x);
			bound.x1 = world.x1;
		}

		if (bound.y1 > world.y1 && get_project_type() != PROJECT_TYPE_Banner)
		{
			bound.x1 = bound.x0
					+ scale_pcoord(world.y1 - bound.y0, dims.x, dims.y);
			bound.y1 = world.y1;
		}
	}

/* Set the bound. */

	object->set_bound(bound);
	object->calc();

	if (fAppend)
	{
	/* Add us to the document... */
	
		append_object(object);
	
	/* ...and make us show up. */
	
		object->get_refresh_bound( &bound, NULL );
		add_refresh(&bound);
	
		deselect_all();
		select_object(object);
	}
}

VOID CPmwDoc::choose_calendar_picture()
{
	//CalendarObjectPtr calobj;

	/* Get the calendar object. */
	CalendarObjectPtr calobj = NULL;
	PMGPageObjectPtr object = NULL;
	if ((object = selected_objects()) != NULL
		&& object->type() == OBJECT_TYPE_CalendarDate)
	{
		calobj = (CalendarObjectPtr)object->get_container();
	}
	
	if (calobj == NULL)
		return;
	

/* Choose a picture for this date. */

	if (ChoosePicture(NULL, calobj->get_date_graphic()) == IDOK)
	{
		GraphicObjectPtr object;

	/* We need to create a graphic to put in the calendar. */

		if ((object = create_graphic_object(&gcs)) != NULL)
		{
		/* Setup the undo/redo command for the date picture. */

			CCmdCalendarDate* pCommand = new CCmdCalendarDate(IDCmd_DatePicture);

			if (!pCommand->Before(this, calobj, get_date_selected()))
			{
				delete pCommand;
				pCommand = NULL;
			}

		/* Set the date's picture. */

			calobj->make_date_graphic(object, -1, FALSE);
			refresh_object(calobj->get_date_object(-1));
			SetModifiedFlag();

		/* Finish the undo/redo command for the date picture. */

			if (pCommand != NULL)
			{
				if (pCommand->After())
				{
					AddCommand(pCommand, FALSE);
				}
				else
				{
					delete pCommand;
				}
			}
		}
	}
}

VOID CPmwDoc::replace_picture()
{
	RectPageObjectPtr pObject = (RectPageObjectPtr)selected_objects();

	if (pObject != NULL && ((pObject->type() == OBJECT_TYPE_Graphic) || (pObject->type() == OBJECT_TYPE_Component)) )
	{
		if (ChoosePicture(NULL, pObject) == IDOK)
		{
			DoReplacePicture(pObject, &gcs, TRUE, IDCmd_ReplacePicture);
		}
	}
}

void CPmwDoc::DoReplacePicture(RectPageObject* , GRAPHIC_CREATE_STRUCT* , BOOL , UINT )
// void CPmwDoc::DoReplacePicture(GraphicObject* pObject, GRAPHIC_CREATE_STRUCT* pGCS, BOOL fResize, UINT uIDCmd)
{
	ASSERT( 0 );	// We should no longer use this call.
#if 0
   // JRO Save this stuff so we can detect later
   // if the picture dimensions have changed
   GraphicRecord* pGraphic;
   BOOL BoolOrg   = FALSE;
   PPNT           DimsOrg;
   PBOX           boundOld;
   if ((pGraphic = pObject->LockGraphicRecord()) != NULL)
   {
      BoolOrg = pObject->original_dims(&DimsOrg); 
      boundOld = ((GraphicObject*)pObject)->get_unrotated_bound();
      pGraphic->release();
   }
   
   PMGDatabasePtr database = pObject->get_database();
	DB_RECORD_NUMBER new_g_record, old_g_record = pObject->get_graphic_record();

	// Create the new graphic record.
	if ((new_g_record = database->new_graphic_record(pGCS)) != 0)
	{
	/* Create the undo/redo command for this action. */

		CCmdChangeSelects* pCommand = new CCmdChangeSelects(uIDCmd);
		if (!pCommand->Before(GetCurrentView()))
		{
			delete pCommand;
			pCommand = NULL;
		}

	/* Toggle the object off. */

		toggle_object(pObject);
		refresh_object(pObject);

	/* Set the new graphic record. */

		pObject->set_graphic_record(new_g_record);

	/* And free the old one. */

		database->free_graphic_record(old_g_record);

		if (fResize)
		{
			if (pObject->get_layout() <= 0)
			{
            // JRO If we got the original dims successfully...
            if (BoolOrg == TRUE)
            {
               if ((pGraphic = pObject->LockGraphicRecord()) != NULL)
               {
                  PPNT           DimsNew;
                  // JRO If we get the new original dims successfully...
                  if (pObject->original_dims(&DimsNew) == TRUE)
                  {
                     // JRO if the dimensions are changed, 
                     if ((DimsNew.x != DimsOrg.x) || (DimsNew.y != DimsOrg.y))
                     {
                        // Get the old bound sizes
                        PCOORD x_size = boundOld.Width();
                        PCOORD y_size = boundOld.Height();
                        // JRO Find the center of the old bound
                        PCOORD BoundXCenterOld = boundOld.CenterX();
                        PCOORD BoundYCenterOld = boundOld.CenterY();

                        PBOX boundNew;
                        // JRO Scale the size by the new dimensions
	                     x_size = MulDiv(x_size, DimsNew.x, DimsOrg.x);
                        y_size = MulDiv(y_size, DimsNew.y, DimsOrg.y);
                        // JRO Create a new bound thing
                        boundNew.x0 = BoundXCenterOld - (x_size / 2);
	                     boundNew.x1 = boundNew.x0 + x_size;
	                     boundNew.y0 = BoundYCenterOld - (y_size / 2);
	                     boundNew.y1 = boundNew.y0 + y_size;;
                        // JRO Set the new bound
                        pObject->set_unrotated_bound(boundNew);
                     }
                  }

                  pGraphic->release();
               }
            }
/*
			// We need to manually size this.

				PBOX bound = pObject->get_bound();
				PCOORD x_size, y_size;

			// Compute original sizes.

				x_size = bound.x1 - bound.x0;
				y_size = bound.y1 - bound.y0;

				if (x_size < y_size)
				{
					x_size = y_size;
				}
				else
				{
					y_size = x_size;
				}

			// Compute new size.

				PPNT dims;

				if (pObject->original_dims(&dims))
				{
					if (dims.x > dims.y)
					{
						y_size = scale_pcoord(x_size, dims.y, dims.x);
					}
					else
					{
						x_size = scale_pcoord(y_size, dims.x, dims.y);
					}
				}

			// Compute new location.

				bound.x0 = (bound.x0 + bound.x1 - x_size)/2;
				bound.y0 = (bound.y0 + bound.y1 - y_size)/2;
				bound.x1 = bound.x0 + x_size;
				bound.y1 = bound.y0 + y_size;

				pObject->set_bound(bound);
*/
			}
		}

		pObject->changed_image();

		calc_object(pObject);

		toggle_object(pObject);
		refresh_object(pObject);

	/*
	// Finish up the undo/redo command.
	*/

		if (pCommand->After())
		{
			AddCommand(pCommand, FALSE);
		}
		else
		{
			delete pCommand;
		}
	}
#endif
}

void CPmwDoc::SetImportPictureName(LPCSTR pszFileName, LPCSTR pszFriendlyName /*=NULL*/)
{
	free_import_picture_name();
	if (pszFileName != NULL)
	{
		gcs.m_csFileName = pszFileName;
	}
	if (pszFriendlyName != NULL)
	{
		gcs.m_csFriendlyName = pszFriendlyName;
	}
}

void CPmwDoc::free_import_picture_name(void)
{
	gcs.m_csFileName.Empty();
	gcs.m_csFriendlyName.Empty();
}

/*
// Handle a missing graphic in a document.
*/

BOOL CPmwDoc::handle_missing_graphic(VOIDPTR data, GraphicPtr graphic, GRAPHIC_CREATE_STRUCT_PTR gcs, int16 *index)
{
	CPmwDoc *pDoc = (CPmwDoc *)data;
	IPathManager* pPathManager = pDoc->GetPathManager();
	ICollectionManager* pCollectionManager = pDoc->GetCollectionManager();

	CStringArray *pPaths = GET_PMWAPP()->GetPictureSearchPaths();
#if 0
	char buffer[_MAX_PATH*2+50];			// Useful buffer...
#else
	CString csBuffer;
	LPSTR buffer;
	TRY
	{
		buffer = csBuffer.GetBuffer(_MAX_PATH*2+50);
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
#endif

/* Get the current graphic file name. */

	CompositeFileName cfn(graphic->m_csFileName);
	LPCSTR in_name = cfn.get_file_name();

/* In case we succeed... */

	gcs->proof = TRUE;

/* Extract just the name part. */

	CString csInDir;
	CString csInBaseName;
	Util::SplitPath(in_name, &csInDir, &csInBaseName);

//	od("Base picture name is %s\r\n", (LPCSTR)csInBaseName);

/* See what the index is. See if we have any paths to search. */

	while (*index < pPaths->GetSize())
	{
		CString path = pPaths->GetAt((*index)++);

	/*
	// Build the new picture name. We try to validate it here so that the
	// picture won't have its name changed (permanently) to something bogus.
	*/

		strcpy(buffer, path);
		strcat(buffer, csInBaseName);

		if (!pPathManager->FileExists(buffer))
		{
			continue;
		}

	/*
	// We have a new path.
	*/

		gcs->m_csFileName = pPathManager->MakeCompositePath(buffer);

		return TRUE;
	}

	// If the path is not a "collection" path, do a lookup in the collection
//	if (path_manager.path_binding_type(cfn.get_dynamic_name(TRUE)) != PBT_CollectionItem)
	{
		// 
		// First, lookup the graphic in the standard picture collection.
		//
		CSearchArtCollections* pSearchCollections = GET_PMWAPP()->GetSearchCollections();
		if (pSearchCollections != NULL)
		{
			for (int i = 0; i < pSearchCollections->GetCount(); i++)
			{
				LPCSTR pszArtCollection = pSearchCollections->GetBaseName(i);
				int nSubdirs = pSearchCollections->GetSubDirCount(i);

				// Preserve old behavior.
				CString csSearchName = csInBaseName;
				if (nSubdirs != 0)
				{
					CString csPath;
					Util::SplitPath(cfn.get_file_name(), &csPath, NULL);
					while (nSubdirs-- != 0 && !csPath.IsEmpty())
					{
						Util::RemoveBackslashFromPath(csPath);
						// Split off the last subdirectory from the path.
						CString csSubDir, csNewPath;
						Util::SplitPath(csPath, &csNewPath, &csSubDir);
						// Add this path on to the front of the base name.
						CString csOldName = csSearchName;
						Util::ConstructPath(csSearchName, csSubDir, csOldName);
						csPath = csNewPath;
					}
				}

				CPMWCollection* pCollection = pCollectionManager->FindCollection(pszArtCollection);
				if (pCollection != NULL)
				{
					DWORD dwItemNumber;
					if (pCollection->FindItem(csSearchName, &dwItemNumber) == ERRORCODE_None)
					{
						CString csNewFile;
						csNewFile.Format("[[G:%s]]\\%s", pszArtCollection, csSearchName);
						gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
						return TRUE;
					}

					// See if the graphic has a TIF extension.
					// If so, try changing it to JPG and doing a lookup.
					CString csPath, csName, csExtension;
					Util::SplitPath(csSearchName, NULL, &csPath, &csName, &csExtension);

					if (csExtension.CompareNoCase(".TIF") == 0
					 || csExtension.CompareNoCase(".GIF") == 0
					 || csExtension.CompareNoCase(".PSD") == 0
					 || csExtension.CompareNoCase(".BMP") == 0
					 || csExtension.CompareNoCase(".PNG") == 0)
					{
						// Build the new name.
						CString csNewName;
						Util::ConstructPath(csNewName, "", csPath, csName, ".JPG");

						// Do the lookup as a JPG.
						DWORD dwItemNumber;
						if (pCollection->FindItem(csNewName, &dwItemNumber) == ERRORCODE_None)
						{
							// We found it! Use this name.
							CString csNewFile;
							csNewFile.Format("[[G:%s]]\\%s", pszArtCollection, csNewName);
							gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
							return TRUE;
						}

						// Now, try it as CGM (some got converted to CGM).
						Util::ConstructPath(csNewName, "", csPath, csName, ".CGM");

						// Do the lookup.
						if (pCollection->FindItem(csNewName, &dwItemNumber) == ERRORCODE_None)
						{
							// We found it! Use this name.
							CString csNewFile;
							csNewFile.Format("[[G:%s]]\\%s", pszArtCollection, csNewName);
							gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
							return TRUE;
						}

						// Now, try it as BMP (some got converted to BMP).
						Util::ConstructPath(csNewName, "", csPath, csName, ".BMP");

						// Do the lookup.
						if (pCollection->FindItem(csNewName, &dwItemNumber) == ERRORCODE_None)
						{
							// We found it! Use this name.
							CString csNewFile;
							csNewFile.Format("[[G:%s]]\\%s", pszArtCollection, csNewName);
							gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
							return TRUE;
						}

						// Now, try it as PNG (some got converted to PNG).
						Util::ConstructPath(csNewName, "", csPath, csName, ".PNG");

						// Do the lookup.
						if (pCollection->FindItem(csNewName, &dwItemNumber) == ERRORCODE_None)
						{
							// We found it! Use this name.
							CString csNewFile;
							csNewFile.Format("[[G:%s]]\\%s", pszArtCollection, csNewName);
							gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
							return TRUE;
						}
					}
				}
			}
		}

		//
		// Now, do a lookup in the catalog.
		//

		LPCSTR pszCatCollection = "CAT4ART";
		CPMWCollection* pCollection = pCollectionManager->FindCollection(pszCatCollection);
		if (pCollection != NULL)
		{
			// Catalog graphics have a portion of the subdirectory attached
			// to allow unique identification. We need to build a name with that
			// piece and do a lookup. The trick: TOTEM graphics have the name
			// constructed differently since the old TOTEM structure had extra
			// subdirectory levels and a non-unique final directory name. So,
			// TOTEMs have two levels of subdirectory stripped off.

			// Determine if this is a TOTEM graphic or not.
			int nLevels = (csInDir.Find("TOTEM") == -1) ? 1 : 3;

			// Grab that part of the path. If we ever go empty, then this
			// is not a catalog graphic.
			CString csDir = csInDir;
			CString csSubDir, csRest;

			// Extract the appropriate subdirectory level.
			while (nLevels-- > 0 && !csDir.IsEmpty())
			{
				// Make sure we do not have a backslash (looks like a dir).
				Util::RemoveBackslashFromPath(csDir);
				// Split the path.
            Util::SplitPath(csDir, &csRest, &csSubDir);
				csDir = csRest;
			}

			if (!csDir.IsEmpty() && !csSubDir.IsEmpty())
			{
				// Build the name to look for.
				CString csSearchName;
				Util::ConstructPath(csSearchName, csSubDir, csInBaseName);

				// Lookup the name.
				DWORD dwItemNumber;
				if (pCollection->FindItem(csSearchName, &dwItemNumber) == ERRORCODE_None)
				{
					CString csNewFile;
					csNewFile.Format("[[G:%s]]\\%s", pszCatCollection, csSearchName);
					gcs->m_csFileName = pPathManager->MakeCompositePath(csNewFile);
					return TRUE;
				}
			}
		}
	}

/*
// Make sure we don't try to refresh the document while this is going on.
// This replacement may be happening in the middle of a refresh
// (possibly re-entering draw_objects() during another refresh), or
// draw_objects() may need to find a missing picture (re-entering here)
// Either way, it wouldn't be good.
*/

	if (pDoc != NULL && !(pDoc->m_wDocumentFlags & (FLAG_AlbumDocument|FLAG_ClipDocument)))
	{
		pDoc->refreshes_suspended = TRUE;

		char name_buffer[_MAX_PATH];

		LPCSTR vname = cfn.get_virtual_name(TRUE);

	/* Bind the name (in case it's a binding name). */

		strcpy(name_buffer, pPathManager->ExpandPath(vname));

		if (graphic->m_csName.IsEmpty())
		{
		/*
		// Picture files which were originally there have a name.
		// This one does not. It must have been a bad choice.
		*/
			// JRO
/*
         wsprintf(buffer, "A picture file is unreadable:\n\n%s\n\n"
									"Would you like to replace it?",
									(LPCSTR)name_buffer);
*/
         wsprintf(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_PICT_BAD),
									(LPCSTR)name_buffer);
		}
		else
		{
/*
			wsprintf(buffer, "A picture is missing or unreadable:\n\n\"%s\"\n(file %s)\n\n"
							"Would you like to replace it?",
						graphic->name, (LPCSTR)name_buffer);
*/
			wsprintf(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_PICT_MISSING),
						(LPCSTR)graphic->m_csName, (LPCSTR)name_buffer);
		}

		GET_PMWAPP()->RemoveHelpfulHint();

		//if we are using the debug tool to print multiple projects we don't want to stop execution
		//with a message box. just go ahead and print the placeholder.
		if(GET_PMWAPP()->m_bPrintProjects == FALSE)
		{
			if (AfxMessageBox(buffer, MB_YESNO) == IDYES)
			{
			/* They want to choose a replacement. */

				if (pDoc->ChoosePicture(graphic->m_csFileName) == IDOK)
				{
					pDoc->refreshes_suspended = FALSE;
					pDoc->SetModifiedFlag();

				/* Steal the file name from the doc's gcs. */

					*gcs = pDoc->gcs;

				/*
				// They chose a replacement name. Since this is apparently a "good"
				// directory to look, remember the path if it isn't in the list.
				// (You're thinking "it can't be because otherwise the picture would
				// have been found without user intervention". This is true only if
				// the user picked a picture with the same name. They may have picked
				// a picture with a different name in one of the default search paths.)
				*/

					CompositeFileName cfn(gcs->m_csFileName);

					LPCSTR fn = cfn.get_file_name();

					LPCSTR base_name = get_base_name(fn);

					if (base_name != fn)
					{
					/*
					// This, unfortunately, doesn't allow searching in the base
					// directory. We may need to change this.
					*/

						int len = base_name - fn;

						strncpy(buffer, fn, len);
						buffer[len] = '\0';

						GET_PMWAPP()->UpdatePicturePaths(buffer);
					}

					return TRUE;
				}
			}
		}
		else
		{
			// Record that something bad took place.
			FILE* fp = fopen(pPathManager->ExpandPath("misspict.log"), "a+");
			if (fp != NULL)
			{
				fprintf(fp, "Missing picture '%s' in '%s'\n", (LPCSTR)in_name, (LPCSTR)(pDoc->m_strPathName));
				fclose(fp);
			}		
		}

		pDoc->refreshes_suspended = FALSE;
	}

	od("Leaving picture MISSING: %s\r\n", (LPCSTR)in_name);

#if 0
   // Record that something bad took place.
	FILE* fp = fopen(pPathManager->ExpandPath("misspict.log"), "a+");
	if (fp != NULL)
	{
		fprintf(fp, "Missing picture '%s' in '%s'\n", (LPCSTR)in_name, (LPCSTR)(pDoc->m_strPathName));
		fclose(fp);
	}
#endif

	return FALSE;
}

/*
// Attach a background to the list.
*/

VOID CPmwDoc::attach_background(PMGPageObjectPtr object)
{
	if (page_objects != NULL)
	{
		page_objects->prepend(object);
		SetModifiedFlag();
	}
}

/*
// Find the background if one exists.
*/

BackgroundObjectPtr CPmwDoc::find_background(VOID)
{
	PMGPageObjectPtr object;

	for (object = objects(); object != NULL; object = (PMGPageObjectPtr)object->next_object())
	{
		if (object->type() == OBJECT_TYPE_Background)
		{
			break;
		}
	}
	return (BackgroundObjectPtr)object;
}

PRIVATE BOOL
duplicate_func(PBOX_PTR box, VOIDPTR data)
{
	((RectPageObjectPtr)data)->set_bound(*box);
	return TRUE;
}

BOOL CPmwDoc::duplicate_picture()
{
	CPositionDialog pdlg(this, TRUE);

	if (pdlg.DoModal() == IDOK)
	{
		if (pdlg.m_layout_number == 0)
		{
		/* Do a single duplicate. */
			return FALSE;
		}
		else
		{
		/* Picked a multiple layout. */

			PMGDatabasePtr database = get_database();
			PMGPageObjectPtr object = selected_list;

		/* Create a group to hold these objects. */

			GroupObjectPtr group;

			if ((group = database->create_group_object()) == NULL)
			{
			/* Skip it. */
				return TRUE;
			}

		/* Make sure we set the panel. */

			group->set_panel(current_panel);

		/* The world is the panel. */

			PBOX world;

			get_panel_world(&world);

		/* Get the object original dims. */

			PPNT dims;
			POINT size;
			PMGDatabase* pDatabase = get_database();

			if (object->original_dims(&dims))
			{
				size.x = (SHORT)dims.x;
				size.y = (SHORT)dims.y;
			}
			else
			{
				size.x = size.y = 0;
			}

			// We append the group now so that any text flows that happen
			// within this duplicate will be able to find the objects.
			append_object(group);

			LayoutEntryPtr entry = (LayoutEntryPtr)pdlg.m_layouts.element_in_list(pdlg.m_layout_number);
      	LayoutElementPtr element;
      
			if (entry != NULL)
			{
				for (element = (LayoutElementPtr)entry->get_elements()->first_element();
							element != NULL;
							element = (LayoutElementPtr)element->next_element())
				{
					CMapPtrToPtr Map;
					PMGPageObjectPtr new_object;

				/* Duplicate the object. */

					if ((new_object = (PMGPageObjectPtr)object->duplicate()) == NULL)
					{
						break;
					}

					PMGDatabase::MapCopiedFrames(object, new_object, Map);

				/* Generate its bound. */

					element->generate_rects(world, size, 0, duplicate_func, new_object);

				/* It has no layout. */

					new_object->set_new_layout(MANUAL_LAYOUT);

				/* Add it to the new group. */

					group->object_list()->append(new_object);

					// Now link up the copied frames (after the append).
					pDatabase->LinkCopiedFrames(pDatabase, Map, TRUE);
				}

			/* Establish the bounds for the group. */

				group->establish();
				group->calc();

				deselect_all();

			/* Do the standard add for the group. */

				refresh_object(group);
				select_object(group);

				MarkFramesBelow(group);
				UpdateMarkedFrames(TRUE);

			/* Add a create event for this. */

				AddCreateCommand(IDCmd_Duplicate);
			}
		}
	}

/* Handled here! */

	return TRUE;
}

/*
// Choose attributes for a picture.
*/

VOID CPmwDoc::choose_picture_attributes(GraphicObjectPtr object)
{
	CPictureAttrDlg padlg(object);

	if (padlg.DoModal() == IDOK)
	{
		BOOL changed = FALSE;

	/*
	// Set up the undo/redo command.
	*/

		CCmdChangeObject* pCommand = new CCmdChangeObject(IDCmd_PictureAttributes);

		if (!pCommand->Before(GetCurrentView(), object))
		{
			delete pCommand;
			pCommand = NULL;
		}

		if (padlg.m_outline)
		{
			FLAGS old_flags = object->get_graphic_flags();

			if (padlg.m_coloring_book)
			{
				object->add_graphic_flags(GRAPHIC_FLAG_coloring_book);
			}
			else
			{
				object->remove_graphic_flags(GRAPHIC_FLAG_coloring_book);
			}
			changed = (old_flags != object->get_graphic_flags());
		}
		else
		{
			if (padlg.m_render_method != object->get_render_method())
			{
				changed = TRUE;
				object->set_render_method((RENDER_METHOD)padlg.m_render_method);
			}
		}

		if (object->get_bcolor() != padlg.m_ChosenColor)
		{
			changed = TRUE;
			object->set_bcolor(padlg.m_ChosenColor);
		}

		if (changed)
		{
			object->changed_image();
			refresh_object(object);
		}

	/*
	// Finish up the undo/redo command.
	*/

		if (pCommand != NULL)
		{
			if (changed && pCommand->After())
			{
				AddCommand(pCommand, FALSE);
			}
			else
			{
				delete pCommand;
			}
		}
	}
}

void CPmwDoc::embed_pictures(BOOL bExternalOnly/*= FALSE*/)
{
	DWORD dwCount = document->NumberOfGraphics();

/* Run through all graphics in the array. */
 
	if (dwCount != 0)
	{
		BeginWaitCursor();
		for (DWORD i = 0; i < dwCount; i++)
		{
			DB_RECORD_NUMBER record = document->GetGraphic(i);
			GraphicPtr graphic;

			if ((graphic = (GraphicPtr)mapping.database->get_record(record, NULL, RECORD_TYPE_Graphic)) != NULL)
			{
				if (graphic->record.source == 0 && graphic->record.image == 0)
				{
					if(bExternalOnly)
					{
						CString csGraphicName = graphic->m_csFileName;
						if(csGraphicName.Left(3).CompareNoCase("[[G") != 0)
							graphic->embed_source();

					}
					else
						graphic->embed_source();
				}
				graphic->release();
			}
		}
		EndWaitCursor();
	}
}

BOOL CPmwDoc::ExtractFile(CString& csFileName)
{
   BOOL fCopied = FALSE;
	IPathManager* pPathManager = GetPathManager();

   if (pPathManager->GetPathBindingType(csFileName) == PBT_CollectionItem)
   {
      CString csDestPath;
      CString csName;
      CString csExtension;

	  //BeginWaitCursor();

      ReadOnlyFile Source(csFileName);

      Util::SplitPath(csFileName, NULL, NULL, &csName, &csExtension);
      csDestPath = GET_PMWAPP()->GetTransferDirectory();
      Util::AppendBackslashToPath(csDestPath);
      csDestPath += csName;
      csDestPath += csExtension;

      // We have to copy the data out of the content database to its
      // own file in order to open it.
      StorageFile Dest(csDestPath);

      if (copy_file(&Source, &Dest) == ERRORCODE_None)
      {
         Dest.flush();
         csFileName = pPathManager->ExpandPath(Dest.get_name());
			fCopied = TRUE;
      }

	  //EndWaitCursor();
   }
	else
	{
		csFileName = pPathManager->LocatePath(csFileName);
	}

   return fCopied;
}
