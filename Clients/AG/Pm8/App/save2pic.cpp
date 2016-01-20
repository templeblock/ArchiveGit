//
// $Workfile: save2pic.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/save2pic.cpp $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 27    9/23/98 10:28a Jayn
// Converted CollectionManager to COM-style object.
// 
// 26    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 25    4/13/98 2:34p Hforman
// using Common Dialog, added categories, etc.
// 
// 24    2/11/98 5:10p Hforman
// moved SavePMObjectsToFile() to dib2file.cpp
// 
// 23    1/29/98 8:05p Hforman
// saving ".pmo" files, other cleanup
// 
// 22    1/12/98 1:46p Johno
// Added Keywords edit box
// 
// 21    1/09/98 5:26p Johno
// Changes for "Save in Art Gallery" option with "Save as Graphic File".
// 
// 20    12/09/97 4:04p Johno
// 
// 19    12/05/97 4:24p Johno
// Minor changes
// 
// 18    12/04/97 5:15p Johno
// 
// 17    12/04/97 5:14p Johno
// Moved filter string stuff here from dialog
// 
// 16    12/04/97 12:14p Johno
// Dialog filter string
// 
// 15    12/03/97 4:52p Johno
// Cleaned up
// 
// 14    11/25/97 10:39a Johno
// Went back to derived from CStorageFile
// 
// 13    11/21/97 5:21p Johno
// General update
// 
// 12    11/20/97 10:37a Johno
// Use CPmwDoc::CDIBInfo::PrepareData
// 
// 11    11/19/97 2:38p Johno
// Re - sync to Source Safe
// 
// 10    11/19/97 2:35p Johno
// Changed Create() to CreateGraphicFile()
// 
// 9     11/14/97 11:58a Johno
// 
// 8     11/10/97 2:52p Johno
// Split Save2pic.cpp into:
// save2pic.cpp, svpicdlg.cpp, and svpicdlg.h
// 
// 7     11/07/97 5:27p Johno
// 2 sets of output sizes - selected objects and all objects
// enum for graphic type
// 
// 6     11/07/97 3:19p Johno
// Dialog size options
// 
// 5     11/06/97 5:50p Johno
// 
// 4     11/06/97 5:48p Johno
// Makes fixed size GIF!
// 
// 3     11/05/97 5:29p Johno
// CFileDialog derived custom dialog
// 
// 2     11/05/97 2:08p Johno
// 
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "svpicdlg.h"
#include "dib2file.h"
#include "util.h"

void CPmwView::DoFileSaveAsGraphic() 
{
	BOOL     fRet = FALSE;
   CPmwDoc  *pDoc = GetDocument ();

   CString  Filter;
   Filter.LoadString (IDS_SAVE_AS_GRAPHIC_FILT);
	// this array must match the order in the above string!
	SaveGraphicType graphicTypes[] =
		{ None, Pmo, Bmp8, Bmp24, Jpeg, Png, Gif, Pcx, Tiff, WMeta };

   CString InitialDir = pDoc->GetPathManager()->ExpandPath("[[P]]");
   CString defExt = "*.pmo"; // This must match the first selection in the filter string

   CFileSaveAsGraphicDlg fileDlg(pDoc->something_selected(), Filter, defExt, InitialDir, this);

   PBOX WorldAll, WorldSel;
	pDoc->get_panel_world (&WorldAll, pDoc->get_current_panel());
   if (pDoc->GetSelectBound (&WorldSel) != TRUE)
      WorldSel = WorldAll;

   CDC *pDC = GetDC();
   if (!pDC)
	{
		return;	// just bail...
	}

	int LogPixX = pDC->GetDeviceCaps (LOGPIXELSX);
	int LogPixY = pDC->GetDeviceCaps (LOGPIXELSY);
   fileDlg.SetDimensions(&WorldAll, &WorldSel, LogPixX, LogPixY);
   ReleaseDC (pDC);

   if (fileDlg.DoModal () == IDOK)
   {
      CString  Filename = fileDlg.GetPathName ();

		CPoint cpDims;
		cpDims.x = fileDlg.m_Width;
		cpDims.y = fileDlg.m_Height;

      enum SaveGraphicType gt;
		gt = graphicTypes[fileDlg.GetGraphicTypeIndex ()];
      ASSERT ((gt > None) && (gt < Invalid));         

		BOOL fSelected = fileDlg.IsSaveSelected();

      ERRORCODE error = ERRORCODE_Memory;
		if (gt == Pmo)
		{
			// save as "PrintMaster Objects" file
			error = SavePMObjectsToFile(Filename, pDoc, fSelected);
		}
		else
		{
			CDibToGraphicFile pDibToGraphicFile(Filename);
			pDibToGraphicFile.CreateGraphicFile (pDoc, gt, cpDims, fSelected, NULL, LogPixX);
			error = pDibToGraphicFile.GetErrorCode();
		}

      if (error != ERRORCODE_None)
         AfxMessageBox (IDS_ERROR_SAVING);
      else
      {
         if (fileDlg.AddToGallery() == TRUE)
         {
            CPMWCollection* pCollection = GetGlobalCollectionManager()->GetUserCollection(CPMWCollection::typeArt);
            if (pCollection != NULL)
            {
               int nResult = pCollection->ImportItem(Filename, NULL, NULL, fileDlg.m_strCategory, FALSE);
               if (nResult != ERRORCODE_None)
                  AfxMessageBox (IDS_ERROR_SAVING);
            }
         }
      }
   }
}

