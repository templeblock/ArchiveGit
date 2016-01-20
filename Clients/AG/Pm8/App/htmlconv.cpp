/***************************************************************************
*  FILE:        HTMLCONV.CPP                                               *
*  SUMMARY:     Class for converting a PrintMaster document to HTML        *
*  AUTHOR:      Dennis Plotzke                                             *
//
// $Log: /PM8/App/htmlconv.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 88    2/04/99 5:57p Rgrenfel
// Fixed so that JPEG based objects always output for the web in JPEG
// format.
// 
// 87    2/04/99 11:55a Rgrenfel
// Fixed assertions which were causing the app to abort without the
// assertion dialogs every showing themselves.
// 
// 86    2/01/99 9:38a Johno
// Changes for paneled HTML
// 
// 85    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 84    1/27/99 1:48p Johno
// Compile update
// 
// 83    1/21/99 6:13p Johno
// Changes for links, hotspots, and text links!
// 
// 82    1/20/99 5:00p Johno
// Changes for paneled HTML
// 
// 81    1/14/99 2:24p Johno
// Rearranged code from pmw.doc
// 
// 80    1/13/99 6:01p Johno
// Moved some HTML stuff from HTMLCONV to PMWDOC
// 
// 79    1/13/99 10:47a Johno
// Changes for HTML projects
// 
// 78    1/07/99 6:12p Johno
// Preliminary "paneled HTML" for some projects
// Changes to eliminate some h file dependecies
// 
// 77    12/23/98 3:50p Johno
// Changes for projects as HTML
// 
// 76    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 75    9/04/98 3:53p Johno
// Changes for new CHTMLTagTableData
// 
// 74    7/21/98 4:55p Johno
// Improved title / author handling in CHTMLDocConverter::ConvertToHTML()
// 
// 73    7/20/98 7:08p Johno
// Skip left origin in CHTMLDocConverter::ConvertToHTML() if output is to
// be centered
// 
// 72    7/10/98 7:18p Johno
// Added code to CHTMLObject::Add(CHTMLObject*) - it was not copying
// hyperlinks from the object pointed to.
// 
// 71    7/10/98 10:29a Johno
// Changed copy_file sources to ReadOnlyFile so it works with write
// protected files.
// 
// 70    7/01/98 5:37p Johno
// Checks for enabled hyperlink in IsLink()
// 
// 69    6/15/98 5:50p Johno
// New CHTMLCompliantInfo::nctObjectTable flag
// 
// 68    6/10/98 3:25p Johno
// In CHTMLObject::Add(), use bound for clipping test, refresh bound for
// object positioning.
// 
// 67    6/05/98 12:23p Johno
// Changes to web page title, author
// 
// 66    6/04/98 5:17p Johno
// Probable fix for HTML position bug
// 
// 65    6/03/98 2:47p Johno
// Changes for "run design checker" menu option
// 
// 64    6/02/98 7:04p Johno
// Simplifications / changes / debugs to find HTML positioning bug
// 
// 63    6/02/98 12:00p Johno
// Tweaks
// 
// 62    6/01/98 12:38p Johno
// Bug fix: Was losing overlapped objects rect in some cases
// 
// 61    5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 60    5/30/98 6:27p Johno
// Changes for HTML warning rect
// 
// 59    5/30/98 4:23p Johno
// m_CompliantInfo now tracks the printmaster objects of HTML objects;
// Simpler, more object oriented over all.
// 
// 58    5/30/98 1:13p Johno
// Fixed memory leak with CWebPageStats::m_ComplianceGroups
// Good things happen when it is QUIET!
// 
// 57    5/28/98 6:37p Johno
// A background of TRANSPARENT_COLOR no longer shows up as black in the
// HTML file.
// 
// 56    5/28/98 4:58p Johno
// Changes for HTML compliance groups
// 
// 55    5/26/98 6:53p Johno
// 
// 54    5/26/98 1:58p Dennis
// Added call to GetConfiguration()->ReplaceText() so app. name can be
// substituted in string.
// 
// 53    5/22/98 3:44p Johno
// Changes for design checker
// 
// 52    5/20/98 6:14p Johno
// Text forced to graphic, or overlapping hot spots, not reported in
// design checker. Non overlapping hot spots reported as empty.
// 
// 51    5/20/98 12:12p Johno
// Changes for design checker
// 
// 50    5/14/98 4:47p Johno
// Code reduction
// 
// 49    5/13/98 4:32p Johno
// Changes for "center output" option, invisible hot spots
// 
// 48    5/11/98 12:05p Johno
// minor changes
// 
// 47    5/11/98 9:39a Johno
// Compile update
// 
// 46    5/07/98 5:54p Johno
// Allow new text - to - graphic override
// 
// 45    5/06/98 4:38p Johno
// Change for background color
// 
// 44    4/30/98 6:07p Johno
// Not much...
// 
// 43    4/29/98 7:20p Johno
// Changes for m_CompliantInfo, link colors
// 
// 42    4/29/98 10:05a Johno
// Some debug stuff
// 
// 41    4/23/98 3:25p Johno
// Changes for rotated text (as graphic with polygon image map)
// 
// 40    4/23/98 10:50a Johno
// Many changes for polygon image maps
// 
// 39    4/20/98 5:37p Johno
// Bug fix in NeedsMap(): Multiple graphics with at least one link need to
// use an image map, not a whole graphic link.
// 
// 38    4/20/98 4:39p Johno
// All non frame objects are now set to graphic objects.
// 
// 37    4/17/98 6:24p Johno
// Changes for design checker
// 
// 36    4/17/98 3:54p Johno
// Horribleness reduction to prepare for Design Checker.
// 
// 35    4/15/98 10:09a Johno
// Web page backgrounds can now be types other than JPG or GIF
// (they are converted to GIF)
// 
// 34    4/14/98 4:51p Johno
// Temporary fix for web page backgrounds - it only does JPEGs or GIFs
// 
// 33    4/10/98 1:44p Johno
// Fixed error handling in CopyLinkedExternalFiles()
// 
// 32    4/08/98 4:20p Johno
// Added CopyLinkedExternalFiles()
// 
// 31    4/08/98 9:51a Johno
// Fixed CHTMLObject::NeedsMap() - it now detects text objects that have
// one link and were converted to a graphic.
// 
// 30    4/06/98 11:40a Johno
// Added KillHTMLStrings()
// 
// 29    4/03/98 5:55p Johno
// Got rid of GetURL() (page links don't have 'em)
// Fixed IsLink()
// 
// 28    4/03/98 4:29p Johno
// Many changes, primarily for html document name generation
// 
// 27    4/01/98 12:10p Johno
// Changes for text links that are converted to graphics
// 
// 26    3/31/98 10:13a Johno
// Added TextLinksAsMaps for text - as - gif
// 
// 25    3/27/98 5:39p Johno
// Removed GetFullURL; Added GetHyperlinkData
// 
// 24    3/25/98 6:32p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 23    3/24/98 5:32p Johno
// Got rid of SetMapName; image maps will now have unique names.
// 
// 22    3/20/98 5:29p Johno
// Image map elements now contained within CHTMLObjects.
// Moved inlines to CPP file.
// Added NormalizeImageMaps
// 
// 21    3/20/98 2:01p Johno
// Simplified code, moved inlines to CPP
// Image maps almost work...
// 
// 20    3/17/98 5:19p Johno
// Compile update
// 
// 19    3/10/98 4:21p Johno
// Start log
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  1/20/98  DGP      Created                                               *
***************************************************************************/

#include "stdafx.h"
#include "resource.h"   // For progress bar
#include "error.h"
#include <math.h>       // for ceil() in GetDownloadSecondsTotalInt()
#include "docdib.h"     // For CDIBInfo
#include "htmlconv.h"

#include "utils.h"
#include "util.h"
#include "grafrec.h"
#include "gif.h"
#include "frameobj.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "progbar.h"
#include "grafobj.h"
#include "file.h"
#include "filemgr.h"
#include "paper.h"      // for CPaperInfo
#include "bkgobj.h"     // For CBackgroundObject
#include "imagemap.h"
#include "inifile.h"
#include "webdef.h"     // For SZ_PUBLISH_SECTION, etc. 
#include "pmwcfg.h"     // For GetConfiguration()

//#define  JTRACE_ON
#include "jtrace.h"

#define  ISROTATED(o) (o->get_rotation() == 0.0 ? FALSE : TRUE)

#ifdef _DEBUG
   //#define  DEBUG_COLORS
   //#define  DEBUG_REMARKS
#endif

#ifdef DEBUG_COLORS
   #define  SETCOLOR_DB(o,c)  (o.SetBackgroundColor(c))
#else
   #define  SETCOLOR_DB(o,c)  ((void)0)
#endif

#ifdef DEBUG_REMARKS
   #define  SETREMARK_DB(o,r) (o.AddRemark(r))
#else
   #define  SETREMARK_DB(o,r) ((void)0)
#endif
//////// 
// CHTMLObject CWebPageStats
void
CWebPageStats::CopyOf(CWebPageStats &ps)
{
   m_dwGraphicSize    = ps.m_dwGraphicSize;
   m_dwLinkedFileSize = ps.m_dwLinkedFileSize;
   m_dwHTMLSize       = ps.m_dwHTMLSize;

   EmptyGroups();
   for (int i = 0; i < ps.m_ComplianceGroups.GetSize(); i++)
   {
      AddCompliantInfoCopy(*ps.m_ComplianceGroups.GetAt(i));
   }
}

void
CWebPageStats::EmptyGroups(void)
{
   for (int i = 0; i < m_ComplianceGroups.GetSize(); i++)
   {
      delete m_ComplianceGroups.GetAt(i);
   }
   m_ComplianceGroups.RemoveAll();   
}

void
CWebPageStats::AddCompliantInfoCopy(CHTMLCompliantInfo &ci)
{
   CHTMLCompliantInfo *lci = new CHTMLCompliantInfo(ci);
   m_ComplianceGroups.Add(lci);
}
// CHTMLObject
int	CHTMLObject::sm_nXResolution = 0;
int	CHTMLObject::sm_nYResolution = 0;
BOOL	CHTMLObject::sm_bRetrievedResolution = FALSE;
PPNT	CHTMLObject::sm_ppntPageDimensions = {0,0};

int   CHTMLObject::sInstanceCount = 0;  
int   CHTMLObject::sMapName = 0;  

CHTMLObject::CHTMLObject()
{
   m_enumOutputType = outputtypeUndefined;
   m_crectImageArea.SetRectEmpty();
   m_bObjectClipped = FALSE;
   
   sInstanceCount ++;
   sMapName ++;
   mImageMapToUse = sMapName;
   WasText = FALSE;
   m_dwGraphicSize = m_dwLinkedFileSize = 0;
}

CHTMLObject::~CHTMLObject()
{
   sInstanceCount --;
   if (sInstanceCount < 1)
   {
      ASSERT(sInstanceCount == 0);
      sInstanceCount = 0;  // Just to be sure
      sMapName = 0;
   }
}

void
CHTMLObject::AddObjectData(PMGPageObject *pObject, CRect &crectDeviceObjectPosition)
{
   TRY
   {
      AddObjectPointer(pObject);
      HyperlinkData  HData;
      pObject->GetHyperlinkData(&HData);
      if (HData.IsValid() == TRUE)
      {
         PMObjectShapeData sd;
         pObject->GetShapeData(sd);

         AddImageMapInfo(HData, sd);
      }

      m_crectImageArea.UnionRect(m_crectImageArea, crectDeviceObjectPosition);
      // This will set / reset the CHTMLObject type
      SetTypeAndCompliance();
   }
   END_TRY
}

BOOL
CHTMLObject::NeedsMap(void)
{
   int   Size = mImageMapElementArray.GetSize();
   // If this is a graphic object, it used to be a text frame
   if ((Size == 1) && (WasText == TRUE))
      return TRUE;

   BOOL  AtLeastOneNonRectLink = FALSE;
   int LinkCount = 0;
   for (int i = 0; i < Size; i ++)
   {
      CImageMapElement* p = mImageMapElementArray.GetAt(i);
      if (p->IsURL() == TRUE)
      {
         LinkCount ++;

         if (p->IsPoly() == TRUE)
            AtLeastOneNonRectLink = TRUE;
      }
   }
   // a link with multiple objects
   if ((LinkCount > 0) && (NumPMObjects() > 1))
      return TRUE;
   // A rotated object
   if (AtLeastOneNonRectLink == TRUE)
      return TRUE;
   // More than one link
   if (LinkCount > 1 == TRUE)
      return TRUE;

   return FALSE;
}

BOOL
CHTMLObject::IsLink(void)
{
   if (NeedsMap() == TRUE)
      return FALSE;

   HyperlinkData hd;
   GetHyperlinkData(hd); 
   if (hd.IsValid())
	{
		return hd.IsEnabled();
	}
	return FALSE;
}

void
CHTMLObject::CopyLinkedExternalFiles(LPCSTR Dir, CFileManager *pFileManager)
{
   HyperlinkData hd;
   for (int i = 0; i < mImageMapElementArray.GetSize(); i ++)
   {
      CImageMapElement* p = mImageMapElementArray.GetAt(i);
      hd = p->GetHyperlinkData();
      if (hd.StringType == TYPE_FilePath)
      {
         CString  Temp, DestStr;
         Util::SplitPath(hd.FilePathOrURL, NULL, &Temp);
         DestStr = Dir;
         DestStr += Temp;

         ERRORCODE   ec;

         ReadOnlyFile	Source(hd.FilePathOrURL);
			StorageFile		Dest(DestStr);
         ec = copy_file(&Source, &Dest);
         
         ST_DEV_POSITION size;
         Dest.length(&size);
         m_dwLinkedFileSize += (DWORD)size;

         if (pFileManager != NULL)
            pFileManager->Add(DestStr);

         if (ec != ERRORCODE_None)
         {
            Temp.LoadString(IDS_PUBLISH_FAIL);
            Temp += "\n";
            Temp += hd.FilePathOrURL;
 
            AfxMessageBox(Temp);   
         }
      }
   }
}

void
CHTMLObject::GetHyperlinkData(HyperlinkData &hd)
{
   hd.Init();
   if (mImageMapElementArray.GetSize() != 1)
      return;

   CImageMapElement* p = mImageMapElementArray.GetAt(0);
   hd = p->GetHyperlinkData();
}

CImageMapElementArray*
CHTMLObject::GetImageMap(void)
{
   return &mImageMapElementArray;
}

void
CHTMLObject::AddImageMapInfo(HyperlinkData &hd, PMObjectShapeData &sd, CImageMapElementArray &imar, LPCSTR url)
{
   CArray<CPoint, CPoint&> Points;
   sd.GetPoints(Points);
   CPoint   point;
   for(int j = 0; j < Points.GetSize(); j ++)
   {
      point = Points.GetAt(j);
      point.x = LogicalToDeviceUnits((PCOORD)point.x);
      point.y = LogicalToDeviceUnits((PCOORD)point.y);
      Points.SetAt(j, point);
   }
   sd.SetPoints(Points);

   imar.Add(hd, sd, url);
}

BOOL
CHTMLObject::GetMapName(CString &s)
{
   s = "";
   
   ASSERT(mImageMapToUse >= 0);  // Name not set

   if (mImageMapToUse >= 0)
   {
		FormImageMapName(s, mImageMapToUse);
      return TRUE;
   }
   
   return FALSE;
}

void
CHTMLObject::FinalizeImageMaps(DocumentRecord *pdr)
{																								
	FinalizeImageMaps(pdr, mImageMapElementArray, m_crectImageArea);
}

void
CHTMLObject::FinalizeImageMaps(DocumentRecord *pdr, CImageMapElementArray &imar, CRect &ImageArea)
{
   for (int i = 0; i < imar.GetSize(); i ++)
   {
      CImageMapElement* p = imar.GetAt(i);
      if (p->IsRect())
      {
         CRect Rect;
         p->GetRectPosition(Rect);
         // Change from page coords to image coords
         Rect.top = Rect.top - ImageArea.top;
         Rect.bottom = Rect.bottom - ImageArea.top;
         Rect.left = Rect.left - ImageArea.left;
         Rect.right = Rect.right - ImageArea.left;
      
         p->SetRectPosition(Rect);
      }
      else
      if (p->IsPoly())
      {
         CArray<CPoint, CPoint&> Points;
         p->GetPoints(Points);
         CPoint   point;
         for(int j = 0; j < Points.GetSize(); j ++)
         {
            point = Points.GetAt(j);
            point.x = point.x - ImageArea.left;
            point.y = point.y - ImageArea.top;
            Points.SetAt(j, point);
         }
         p->SetPoints(Points);
      }
      else
      {
         // Unknown Shape type
         ASSERT(0);
      }
      // Complete the URL
      HyperlinkData hd = p->GetHyperlinkData();
      CString Temp;
      pdr->AppendFullURL(hd, Temp);

      p->SetFullURL(Temp);
   }
}
// Have the CFrameObject* list it's URLs
void
CHTMLObject::TextLinksAsMaps(PMGPageObject *pPMGObject, CImageMapElementArray *pMap)
{
   if (pMap != NULL)
   {
      if ((pPMGObject != NULL) && (pPMGObject->type() == OBJECT_TYPE_Frame))
      {
         CImageMapElementArray   MapTemp;
         // Get image map elements into a temporary array
         ((CFrameObject*)pPMGObject)->GetURLList(&MapTemp);
         // Translate coords and add to this objects's map
         PBOX  Pbox;
         
         for(int i = 0; i < MapTemp.GetSize(); i ++)
         {
            CImageMapElement* p = MapTemp.GetAt(i);

            CRect Rect;
            PMObjectShapeData sd;
            double rotation  = ((CFrameObject*)pPMGObject)->get_rotation();  
            
            if ((p->GetRectPosition(Rect) == TRUE) && (rotation == 0.0))
            {
               Pbox.x0 = Rect.left;
               Pbox.x1 = Rect.right;
               Pbox.y0 = Rect.top;
               Pbox.y1 = Rect.bottom;
               LogicalToDeviceUnits(Pbox, &Rect);
               sd.SetPoints(Rect);
            }
            else
            {                               
               CArray<CPoint, CPoint&> Points;
               p->GetPoints(Points);
               CPoint   point;
               for(int j = 0; j < Points.GetSize(); j ++)
               {
                  point = Points.GetAt(j);
                  point.x = LogicalToDeviceUnits((PCOORD)point.x);
                  point.y = LogicalToDeviceUnits((PCOORD)point.y);
                  Points.SetAt(j, point);
               }
               sd.SetPoints(Points);
            }

            pMap->Add(p->GetHyperlinkData(), sd);                        
         }
      }
   }
}

ERRORCODE CHTMLObject::ConvertToWebGraphic(CString *pcsBaseFileName, CPmwDoc *pDoc)
{
   ERRORCODE         errorcode = ERRORCODE_None;

   ASSERT(pDoc);
   ASSERT(pcsBaseFileName);

   m_csFileName = *pcsBaseFileName;

   errorcode = ConvertToWebGraphic(
      m_CompliantInfo,
      &m_csFileName,
      pDoc,
      PALETTEINDEX(TRANSPARENT_INDEX), // Fill Color
      m_crectImageArea,
      &m_dwGraphicSize,
      m_bObjectClipped,
      GetImageMap());

   return errorcode;
}
// This method converts one or more PrintMaster graphics objects to
// a Web Browser compatable graphic file.
ERRORCODE CHTMLObject::ConvertToWebGraphic(
	CHTMLCompliantInfo &CompliantInfo,
	CString *pcsFileName,
	CPmwDoc *pDoc,
	COLORREF crFillColor, 
	CRect &rImageArea,
	DWORD *pGraphicSize,
	BOOL  bObjectIsClipped/*FALSE*/,
	CImageMapElementArray *pMap/*NULL*/)
{
	PBOX              pboxGraphic;
	CDIBInfo cdibInfo;
	POINT             ptMaxDims;
	ERRORCODE         errorcode = ERRORCODE_None;
	GraphicRecord     *pGraphicRecord;
	GraphicObject     *pGraphicObject;
	PMGPageObject     *pPMGObject;
	CString           csFileExtension = ".gif";  // By default, create a .GIF

	ASSERT(pDoc);
	// If there is only one graphic object and it is of type GIF or JPEG, 
	// copy it directly to a file instead of rendering it as a DIB
	if(CompliantInfo.NumPMObjects() == 1 && !bObjectIsClipped)
	{
		pPMGObject = CompliantInfo.GetFirstObject();
		ASSERT(pPMGObject);
		BOOL           bGraphicHasSpecialAttributes = FALSE;
		CDoubleRect    cdrCrop;
		FLAGS          objectFlags;

		// Get the graphic object if it is one, and if so determine the background color.
		pGraphicObject = dynamic_cast<GraphicObject *>( pPMGObject );
		COLOR bcolor = COLOR_BLACK;
		if (pGraphicObject != NULL)
		{
			bcolor = pGraphicObject->get_bcolor();
		}

		objectFlags = pPMGObject->get_flags();
		// Check if object is rotated, has as special color, 
		// is cropped, or flipped.  
		// If so, use drawing code to create graphic file.
		cdrCrop = pPMGObject->GetVisibleRect();
		if(ISROTATED(pPMGObject) == TRUE ||//pGraphicObject->get_rotation() != 0 ||
			bcolor != COLOR_BLACK ||
			cdrCrop.top != 0 || cdrCrop.left != 0 ||
			cdrCrop.bottom != 1 || cdrCrop.right != 1 ||
			(objectFlags & OBJECT_FLAG_xflipped) ||
			(objectFlags & OBJECT_FLAG_yflipped))
		{
				bGraphicHasSpecialAttributes = TRUE;
		}

		if(!bGraphicHasSpecialAttributes)
		{
			if(pPMGObject->type() == OBJECT_TYPE_Graphic)
			{
				pGraphicRecord = pGraphicObject->LockGraphicRecord();
				ASSERT(pGraphicRecord);
				if(pGraphicRecord)
				{
					if ((pGraphicRecord->record.type == GRAPHIC_TYPE_JPEG) ||
						 (pGraphicRecord->record.type == GRAPHIC_TYPE_GIF))
					{
						if (pGraphicRecord->record.type == GRAPHIC_TYPE_JPEG)
							csFileExtension = ".jpg";
						*pcsFileName += csFileExtension;

						ReadOnlyFile	storageFileSource;
						StorageFile    storageFileDest(*pcsFileName);
						errorcode = pGraphicRecord->prep_storage_file(&storageFileSource);
						if(errorcode == ERRORCODE_None)
							errorcode = copy_file(&storageFileSource, &storageFileDest);
						// Save Files's size if requested
						if (pGraphicSize != NULL)
						{
							ST_DEV_POSITION size;
							storageFileDest.length(&size);
							*pGraphicSize += (DWORD)size;
						}

						pGraphicRecord->release();
						return errorcode;
					}
				}
         
				pGraphicRecord->release();
			}
			else
			{
				// Check to see if the object is a GIF or JPEG image.  If so and
				// we are not sizing down, then simply export the raw data.
				// Animated GIF images export the raw data even if we *are*
				// sizing downward.
				CSize sizeRawImage(0,0);
				pPMGObject->GetRawImageSize(&sizeRawImage);
				PBOX Box = pPMGObject->get_bound();
				PCOORD nWidth = LogicalToDeviceUnits( Box.Width() );
				PCOORD nHeight = LogicalToDeviceUnits( Box.Height() );
				bool bRawImageIsLarger = ((sizeRawImage.cx > nWidth) || (sizeRawImage.cy > nHeight));
				if (pPMGObject->IsAnimatedGIF() ||
					 pPMGObject->IsJPEGImage() ||
					 (!bRawImageIsLarger && pPMGObject->IsWebImageFormat()))
				{
					bool bSuccess = pPMGObject->ExportRawData( pcsFileName );
					if (bSuccess)
					{
						// We exported, so don't bother to perform a flat render below.
						return ERRORCODE_None;
					}
				}
			}
		}
	}
	// If we get here, image will be rendered as a DIB then converted
	// to a GIF
	*pcsFileName += csFileExtension;

	ASSERT(&rImageArea != NULL);
	pboxGraphic.x0 = rImageArea.left;
	pboxGraphic.x1 = rImageArea.right;
	pboxGraphic.y0 = rImageArea.top;
	pboxGraphic.y1 = rImageArea.bottom;
	// Scale from device to logical units
	CHTMLObject::ScalePBOX(&pboxGraphic, 
		PAGE_RESOLUTION,        // Dest Resolution
		sm_nXResolution,         // Source X resolution
		PAGE_RESOLUTION,        // Dest Y resolution
		sm_nYResolution);        // Source Y resolution
	// Set maximum dimensions to Image Area 
	ptMaxDims.x = rImageArea.Width();
	ptMaxDims.y = rImageArea.Height();
	// Any text links need to become image maps, as we are becomming a graphic
	// Also, build a list of all objects 
	CPtrList MyList;
	for (pPMGObject = CompliantInfo.GetFirstObject(); pPMGObject != NULL; pPMGObject = CompliantInfo.GetNextObject()) 
	{
		TextLinksAsMaps(pPMGObject, pMap);   
		MyList.AddTail(pPMGObject);
	}   
	errorcode = pDoc->BuildDIB(cdibInfo, pboxGraphic, ptMaxDims, &MyList, FALSE, crFillColor);

	if(errorcode != ERRORCODE_None)
		return errorcode;
	// Output DIB to file
	errorcode = OutputGIF(pcsFileName, cdibInfo, crFillColor, pGraphicSize);

	return errorcode;
}

ERRORCODE
CHTMLObject::OutputGIF(CString *Filename, CDIBInfo &dibInfo, COLORREF FillColor, DWORD *pGraphicSize/*NULL*/)
{
   ERRORCODE	errorcode;

	StorageFile File(*Filename);
   if ((errorcode = File.initialize()) == ERRORCODE_None)
   {
      // We have a destination file.
      // Setup the data sink.
      CGIFDataSinkDevice Sink(&File);
      CDIBtoGIFWriter Writer;

      TRY
      {
         Writer.CreateGIF(
            dibInfo.BitmapInfo(),
            dibInfo.BitmapData(),
            &Sink,
            FALSE,
            FillColor);
      }
      // Save Files's size if requested
      if (pGraphicSize != NULL)
      {
         ST_DEV_POSITION size;
         File.length(&size);
         *pGraphicSize += (DWORD)size;
      }

      CATCH_ALL(e)
      {
         errorcode = ERRORCODE_Fail;
      }
      END_CATCH_ALL
	}

   return errorcode;
}

ERRORCODE CHTMLObject::GetHTML(CHTMLTag &refHTMLTag)
{
   // Text objects must contain only one PM object
   ASSERT(NumPMObjects() == 1);

   CFrameObject *pFrameObject = (CFrameObject*)GetFirstObject();
   // Text object must contain a PM frame
   ASSERT(pFrameObject->type() == OBJECT_TYPE_Frame);
   if (pFrameObject->type() != OBJECT_TYPE_Frame)
      return ERRORCODE_BadParameter;
   
   pFrameObject->ConvertToHTML(refHTMLTag);
   return ERRORCODE_None;
}

BOOL CHTMLObject::DoesRectIntersect(CRect &crectToCheck)
{
   CRect crIntersect, crectToCheckCopy;
   // Copy callers rect
   crectToCheckCopy = crectToCheck;
   // Grow callers rect since we want to cause adjacent objects to 
   // end up grouped
   crectToCheckCopy.InflateRect(1, 1);

   if(crIntersect.IntersectRect(crectToCheckCopy, m_crectImageArea))
      return TRUE;
   else
      return FALSE;
}

void CHTMLObject::GetRect(CRect *pRect)
{
   ASSERT(pRect);
   *pRect = m_crectImageArea;
}

ERRORCODE CHTMLObject::Add(PMGPageObject *pObject)
{
      CRect    crPageObject, Bound;

      ASSERT(pObject);

      LogicalToDeviceUnits(pObject, crPageObject, &Bound);
      return Add(pObject, crPageObject, Bound);
}

ERRORCODE CHTMLObject::Add(PMGPageObject *pObject, CRect &crectDeviceObjectPosition, CRect &crectDeviceObjectBound)
{
   PBOX              pboxDevicePageDims;
   CRect             crectDevicePageDims;

   ASSERT(pObject);
   if(pObject == NULL || crectDeviceObjectPosition.IsRectEmpty())
      return ERRORCODE_BadParameter;
   // Convert Page Dimensions in logical units to device units
   pboxDevicePageDims.x0 = pboxDevicePageDims.y0 = 0;
   pboxDevicePageDims.x1 = sm_ppntPageDimensions.x;
   pboxDevicePageDims.y1 = sm_ppntPageDimensions.y;
   LogicalToDeviceUnits(pboxDevicePageDims, &crectDevicePageDims);
   // Clip bounding rectangle (int device units) of object if needed
	if(crectDeviceObjectPosition.left < 0)
   {
      crectDeviceObjectPosition.left = 0;
   }
   if(crectDeviceObjectPosition.right < 0)
   {
      crectDeviceObjectPosition.right = 0;
   }
   if(crectDeviceObjectPosition.top < 0)
   {
      crectDeviceObjectPosition.top = 0;
   }
   if(crectDeviceObjectPosition.bottom < 0)
   {
      crectDeviceObjectPosition.bottom = 0;
   }
   if(crectDeviceObjectPosition.right > crectDevicePageDims.right)
   {
      crectDeviceObjectPosition.right = crectDevicePageDims.right;
   }
   if(crectDeviceObjectPosition.bottom > crectDevicePageDims.bottom)
   {
      crectDeviceObjectPosition.bottom = crectDevicePageDims.bottom;
   }
	// Use bound to check for off page clipping
	if(crectDeviceObjectBound.left < 0)
   {
      m_bObjectClipped = TRUE;
   }
   if(crectDeviceObjectBound.right < 0)
   {
      m_bObjectClipped = TRUE;
   }
   if(crectDeviceObjectBound.top < 0)
   {
      m_bObjectClipped = TRUE;
   }
   if(crectDeviceObjectBound.bottom < 0)
   {
      m_bObjectClipped = TRUE;
   }
   if(crectDeviceObjectBound.right > crectDevicePageDims.right)
   {
      m_bObjectClipped = TRUE;
   }
   if(crectDeviceObjectBound.bottom > crectDevicePageDims.bottom)
   {
      m_bObjectClipped = TRUE;
   }
   // Add object to warning list here if object was clipped
   if(m_bObjectClipped == TRUE)
      m_CompliantInfo.SetNonCompliant(CHTMLCompliantInfo::nctObjectClipped);
	// Don't add off-page objects
   if(crectDeviceObjectPosition.IsRectEmpty())
      return ERRORCODE_None;

   if(pObject->IsEmpty())
   {
      // TODO - report error for design checker
      return ERRORCODE_NotInitialized;
   }

   AddObjectData(pObject, crectDeviceObjectPosition);

   return ERRORCODE_None;
}
	  
ERRORCODE CHTMLObject::Add(CHTMLObject *pHTMLObject)
{
   ASSERT(pHTMLObject);
	if (pHTMLObject == NULL)
		return ERRORCODE_BadParameter;
   // Add callers Object(s) to this objects list
   m_CompliantInfo.CombineFrom(pHTMLObject->m_CompliantInfo);

   CImageMapElementArray	*pMap = pHTMLObject->GetImageMap();
	ASSERT(pMap != NULL);
	mImageMapElementArray.Copy(*pMap);

	m_crectImageArea.UnionRect(m_crectImageArea, pHTMLObject->m_crectImageArea);

   SetTypeAndCompliance();
   return ERRORCODE_None;
}

void CHTMLObject::ScalePBOX(PBOX_PTR pbox, SHORT DestXRes, SHORT SourceXRes, SHORT DestYRes, SHORT SourceYRes)
{
   PCOORD x0, x1;
   PCOORD y0, y1;
/* Insanity prevention. */
   if (SourceXRes == 0)
   {
      SourceXRes++;
   }
   if (SourceYRes == 0)
   {
      SourceYRes++;
   }
/* Handle the x coordinates. */
   x1 = pbox->x1 - (x0 = pbox->x0);

   if (DestXRes < SourceXRes)
   {
   /* Compression. */
      if (x0 != 0)
      {
         x0 = (x0*DestXRes)/SourceXRes;
      }
      if (x1 != 0)
      {
         x1 = ((x1-1)*DestXRes)/SourceXRes + 1;
      }
   }
   else
   {
   /* Expansion. */
      if (x0 != 0)
      {
         x0 = ((x0 * DestXRes)-1)/SourceXRes + 1;
      }
      if (x1 != 0)
      {
         x1 = ((x1 * DestXRes)-1)/SourceXRes + 1;
      }
   }

   pbox->x1 = (pbox->x0 = x0) + x1;
/* Handle the y coordinates. */
   y1 = pbox->y1 - (y0 = pbox->y0);

   if (DestYRes < SourceYRes)
   {
   /* Compression. */
      if (y0 != 0)
      {
         y0 = (y0*DestYRes)/SourceYRes;
      }
      if (y1 != 0)
      {
         y1 = ((y1-1)*DestYRes)/SourceYRes + 1;
      }
   }
   else
   {
   /* Expansion. */
      if (y0 != 0)
      {
         y0 = ((y0 * DestYRes)-1)/SourceYRes + 1;
      }
      if (y1 != 0)
      {
         y1 = ((y1 * DestYRes)-1)/SourceYRes + 1;
      }
   }
   pbox->y1 = (pbox->y0 = y0) + y1;
}

BOOL
CHTMLObject::SetTypeAndCompliance(void)
{
   int                  NonHotspotCount, Count;
   PMGPageObject        *pObject;

   for (NonHotspotCount = 0, Count = 0, pObject = GetFirstObject(); pObject != NULL; pObject = GetNextObject(), Count++) 
   {
      ASSERT(pObject);
      if (pObject != NULL) 
      {
         if (pObject->type() != OBJECT_TYPE_Hyperlink)
            NonHotspotCount ++;
               
         if (pObject->type() == OBJECT_TYPE_Frame)
         {
            if (((CFrameObject*)pObject)->IsHTMLTextCompliant(&m_CompliantInfo) == FALSE)
            {
               if (
                     (((CFrameObject*)pObject)->TestExtraFlag(HTMLFrame_ForceText) == TRUE)
                     &&
                     (m_CompliantInfo.CanKeepAsText() == TRUE)
                  )
               {
                  m_enumOutputType = outputtypeText;
               }
               else
               {
                  WasText = TRUE;
                  m_enumOutputType = outputtypeGraphic;
                  if (((CFrameObject*)pObject)->TestExtraFlag(HTMLFrame_ForceGraphic) == TRUE)
                     m_CompliantInfo.ClearKeepAsText();
               }
            }
            else
            if (((CFrameObject*)pObject)->TestExtraFlag(HTMLFrame_ForceGraphic) == TRUE)
            {
               WasText = TRUE;
               m_enumOutputType = outputtypeGraphic;
            }
            else
            {
               m_enumOutputType = outputtypeText;
            }
         }
         else
			{
            m_enumOutputType = outputtypeGraphic;
				if (pObject->type() == OBJECT_TYPE_Table)
					m_CompliantInfo.SetNonCompliant(CHTMLCompliantInfo::nctObjectTable);
			}
      }
   }
   // More than one object in list requires coverting it to a graphic
   if (NonHotspotCount > 1)
   {
      m_CompliantInfo.SetNonCompliant(CHTMLCompliantInfo::nctOverlappingObjects);
      m_CompliantInfo.ClearNonCompliant(CHTMLCompliantInfo::nctObjectIsEmpty);
      m_enumOutputType = outputtypeGraphic;
   }
   else
   if ((Count > 0) && (NonHotspotCount == 0))  // Only  hot spot(s)
   {
      m_CompliantInfo.SetNonCompliant(CHTMLCompliantInfo::nctObjectIsEmpty);
   }

   return !m_CompliantInfo.HasComplianceErrors();
}

CHTMLObject::OutputTypes CHTMLObject::GetOutputType()
{
//   SetTypeAndCompliance();
   ASSERT(m_enumOutputType != outputtypeUndefined);
   return m_enumOutputType;
}

#ifdef _DEBUG
#ifdef DUMP_ON
void CHTMLObject::Dump(CDumpContext& dc) const
{
   GraphicObject *pGraphicObject;

   for(POSITION pos = m_plDisplayObjects.GetHeadPosition();
      pos != NULL;
      m_plDisplayObjects.GetNext(pos))
      pGraphicObject = (GraphicObject *) m_plDisplayObjects.GetAt(pos);
}
#endif
#endif

void CHTMLObject::LogicalToDeviceUnits(PMGPageObject *pObject, CRect &crDevice, CRect *pBound)
{
   CRect    crPageObject;
   PBOX     pboxObject, xBound;

   ASSERT(pObject);

	switch (pObject->type())
   {
		case OBJECT_TYPE_Frame:
      pboxObject = pObject->get_bound();
		xBound = pObject->get_bound();
      break;
      
      default:
      pObject->get_refresh_bound(&pboxObject);
		xBound = pObject->get_bound();
      break;
   }
   LogicalToDeviceUnits(pboxObject, &crDevice);
   if (pBound != NULL)
		LogicalToDeviceUnits(xBound, pBound);
}

void CHTMLObject::LogicalToDeviceUnits(PBOX &refPBOX, CRect *pcrectDevice)
{
   // Convert logical coordinates to device (pixel) coordinates
	pcrectDevice->left   = LogicalToDeviceUnits(refPBOX.x0);
	pcrectDevice->top    = LogicalToDeviceUnits(refPBOX.y0);
	pcrectDevice->right  = pcrectDevice->left + LogicalToDeviceUnits(refPBOX.Width());
	pcrectDevice->bottom = pcrectDevice->top + LogicalToDeviceUnits(refPBOX.Height());
}

PCOORD 
CHTMLObject::LogicalToDeviceUnits(PCOORD p)
{
   if(!sm_bRetrievedResolution)
   {
#ifdef USE_SCREENRES
	   HDC hScreenDC = ::GetDC(NULL);

	   if (hScreenDC == NULL)
		   return;

	   CDC ScreenDC;
	   ScreenDC.Attach(hScreenDC);

	   sm_nXResolution = ScreenDC.GetDeviceCaps(LOGPIXELSX);
	   sm_nYResolution = ScreenDC.GetDeviceCaps(LOGPIXELSY);

	   ::ReleaseDC(NULL, (HDC)ScreenDC.Detach());
#else
      // Always output as 96 DPI for web documents
	   sm_nXResolution = sm_nYResolution = 96;
#endif
      sm_bRetrievedResolution = TRUE;
   }
   
   return scale_pcoord(p, sm_nXResolution, PAGE_RESOLUTION);
}
////////////////////////////////////////////////////////////////////////////////
// CHTMLDocConverter Methods
BOOL     CHTMLDocConverter::sm_bCancelProcess = FALSE;
CString  CHTMLDocConverter::msDir;
CString  CHTMLDocConverter::msHomePage;
CString  CHTMLDocConverter::msExt;

CHTMLDocConverter::CHTMLDocConverter(CPmwDoc *pDoc)//, LPCSTR Dir, LPCSTR HomePage, LPCSTR Ext)
{
   ASSERT(pDoc);
   m_pPmwDoc = pDoc;
   ASSERT(m_pPmwDoc != NULL);
   sm_bCancelProcess = FALSE;
   // Get Document Dimensions and update HTMLObject with page size
   if(m_pPmwDoc)
      CHTMLObject::SetPageDimensions(m_pPmwDoc->get_dimensions());
}

CHTMLDocConverter::~CHTMLDocConverter()
{
   RemoveAll();
   // These are static, and will be read in if needed
   //  so they could hang around, but are not big
   KillHTMLStrings();
}

void
CHTMLDocConverter::GetHTMLFileName(DWORD dwPage, CString &FileName)
{
   CString Ext;
   GetHTMLFileExt(Ext);

   if (dwPage == 0)
   {
      if (msHomePage.IsEmpty())
      {
         CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
         msHomePage = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_WEBSITE_HOMEPAGE_ENTRY, "index");
      }
   
      FileName = msHomePage;   
      FileName += '.';
      FileName += Ext;
   }
   else
      FileName.Format("page%04i.%s", (int)dwPage + 1, Ext);
}

void
CHTMLDocConverter::SetHTMLHomeName(CString &FileName)
{
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_WEBSITE_HOMEPAGE_ENTRY, FileName);
   msHomePage.Empty();
}

void
CHTMLDocConverter::GetHTMLFileExt(CString &Ext)
{
   if (msExt.IsEmpty())
   {
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);

      msExt = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_WEBSITE_FILEEXT_ENTRY, "html");
   }
   
   Ext = msExt;
}

void
CHTMLDocConverter::SetHTMLFileExt(CString &Ext)
{
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_WEBSITE_FILEEXT_ENTRY, Ext);
   msExt.Empty();
}

void
CHTMLDocConverter::GetHTMLPathOnly(CString &Path)
{
   if (msDir.IsEmpty())
   {
      CString csDefaultPath = GetGlobalPathManager()->ExpandPath("[[U]]\\Publish");
      CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
      msDir = IniFile.GetString(SZ_PUBLISH_SECTION, SZ_PUBLISH_PATH_ENTRY, csDefaultPath);
   }
   Path = msDir;
}

void
CHTMLDocConverter::SetHTMLPathOnly(CString &Path)
{
   CIniFile IniFile(GET_PMWAPP()->m_pszProfileName);
   IniFile.WriteString(SZ_PUBLISH_SECTION, SZ_PUBLISH_PATH_ENTRY, Path);
   msDir.Empty();
}

void
CHTMLDocConverter::GetHTMLPathName(DWORD dwPage, CString &FileName)
{
   CString  Temp;
   
   GetHTMLPathOnly(Temp);
   FileName = Temp;
   FileName += '\\';

   GetHTMLFileName(dwPage, Temp);
   FileName += Temp;
}

void
CHTMLDocConverter::SortObjectList(void)
{
   // Sort object list by vertical position, then by horizontal
   CRect       crectGraphic;   
   POSITION    pos;
   CHTMLObject *pHTMLObject, *pOldObject;
   CPoint      cptSmallest;
   POSITION    posSmallest = 0, posUnsortedStart;
   POSITION    posTail = m_plHTMLObjects.GetTailPosition();

   posUnsortedStart = m_plHTMLObjects.GetHeadPosition();
   while(posUnsortedStart > 0)
   {
      posSmallest = 0;
      cptSmallest.x = cptSmallest.y = INT_MAX;
      // Look for next largest item in unsorted portion of list
      for(pos = posUnsortedStart;pos != NULL;m_plHTMLObjects.GetNext(pos))
      {
         pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
         pHTMLObject->GetRect(&crectGraphic);
         if(crectGraphic.top < cptSmallest.y)
         {
            cptSmallest.y = crectGraphic.top;
            cptSmallest.x = crectGraphic.left;
            posSmallest = pos;
         }
         else if(crectGraphic.top == cptSmallest.y)
         {
            if(crectGraphic.left < cptSmallest.x)
            {
               cptSmallest.y = crectGraphic.top;
               cptSmallest.x = crectGraphic.left;
               posSmallest = pos;
            }
         }
      }
      // If we found something smaller, move it to sorted portion
      // of list
      if(posSmallest > 0)
      {
         // If smallest item is already in proper position, move on to next unsorted item
         if(posSmallest != posUnsortedStart)
         {
            pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(posSmallest);
            pOldObject     = (CHTMLObject *) m_plHTMLObjects.GetAt(posUnsortedStart);
            // Swap smallest object position with end of sorted list
            m_plHTMLObjects.SetAt(posUnsortedStart, pHTMLObject);
            m_plHTMLObjects.SetAt(posSmallest, pOldObject);
         }
         // End of sorted section 
         m_plHTMLObjects.GetNext(posUnsortedStart);
      }
   }
}

void
CHTMLDocConverter::SetBackground(CHTMLTagBody &tagBody, const CString &csDirectory, DWORD dwCurPage, CFileManager *pcfmFileList, DWORD *dwBackSize)
{
   ERRORCODE   errorcode = ERRORCODE_None;
   CString     csBaseFileName;

   CPageProperties *pPageProp = m_pPmwDoc->GetPageProperties();
   if(pPageProp)
   {
      COLORREF crFillColor = PALETTEINDEX(TRANSPARENT_INDEX);

      //tagBody.SetTextColor(pPageProp->GetBodyTextColor());
      
      COLOR clr = pPageProp->GetBackgroundColor();
      if((clr != UNDEFINED_COLOR) && (clr != TRANSPARENT_COLOR))
      {
         RGBCOLOR rgbBackground = rgbcolor_from_color(clr);
         tagBody.SetBackgroundColor(COLOR_FROM_RGBCOLOR(rgbBackground));
         
         crFillColor = colorref_from_color(pPageProp->GetBackgroundColor());
      }
      
      tagBody.SetLinkColor(pPageProp->GetNonVisitedLinkColor());
      tagBody.SetVisitedLinkColor(pPageProp->GetVisitedLinkColor());
      tagBody.SetActiveLinkColor(pPageProp->GetActiveLinkColor());
      tagBody.CenterOutput(pPageProp->IsCenteredHTML());
      
      if(pPageProp->HasTexture())
      {
         CBackgroundObject * pBackgroundObject;
         CString csBackgroundHTMLFile, Temp;
         
         Util::SplitPath(csDirectory, &Temp, NULL);
         csBackgroundHTMLFile.Format("%sback%04i",
            Temp,
            (int)dwCurPage);

         pBackgroundObject = m_pPmwDoc->GetBackgroundObject();
         ASSERT(pBackgroundObject);
         ASSERT(pBackgroundObject && pBackgroundObject->HasTexture()
            && pBackgroundObject->GetGraphicObject());
         if(pBackgroundObject && pBackgroundObject->HasTexture()
            && pBackgroundObject->GetGraphicObject())
         {
            CHTMLCompliantInfo   DummyCompliantInfo;

            GraphicObjectPtr  GOP = pBackgroundObject->GetGraphicObject();

            DummyCompliantInfo.AddObjectPointer(GOP);

            CRect crectImageArea;
            CHTMLObject::LogicalToDeviceUnits(GOP, crectImageArea);
            
            errorcode = CHTMLObject::ConvertToWebGraphic(
               DummyCompliantInfo,
               &csBackgroundHTMLFile,
               m_pPmwDoc,
               crFillColor, 
               crectImageArea,
               dwBackSize);
            ASSERT(errorcode == ERRORCODE_None);
            if ((pcfmFileList != NULL) && (errorcode == ERRORCODE_None))
            {
               Util::SplitPath(csBackgroundHTMLFile, NULL, &csBaseFileName);
               tagBody.SetBackgroundImage(csBaseFileName);
               pcfmFileList->Add(csBackgroundHTMLFile);
            }
         }
      }
   }
}

void CHTMLDocConverter::RemoveAll()
{
   CHTMLObject   *pObject;

   for(POSITION pos = m_plHTMLObjects.GetHeadPosition();pos != NULL;m_plHTMLObjects.GetNext(pos))
   {
      pObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
      ASSERT(pObject);
      if(pObject)
         delete pObject;
   }
   m_plHTMLObjects.RemoveAll();
   m_csdwaUniqueX.RemoveAll();
   m_csdwaUniqueY.RemoveAll();
   m_otObjectTracker.RemoveAll();
}

void CHTMLDocConverter::OnCancel()
{
   sm_bCancelProcess = TRUE;
}

ERRORCODE 
CHTMLDocConverter::ConvertToPagedHTML(const CString &Filename,
   CFileManager *pcfmPublishFileList,  // Optional CFileManager list which gets filled in with published filenames
   DWORD          dwCurPage,
   CWebPageStats *pWebStats)
{
   BOOL                 bCreated;
   int                  nRetVal;
   DWORD                dwHTMLSize, dwGraphicsSize = 0, dwLinkedFileSize = 0;
   ERRORCODE            errorcode = ERRORCODE_None;
   LONG                 lBytesFree;
   CString              csGraphicFileName, csResource;
   CString              csMessage, csBaseFileName;
   CRect                crectGraphic;
   POSITION             pos;
   CHTMLObject          *pHTMLObject;
   CHTMLTagTable        tagTable;
   CHTMLTagTableRow     tagTableRow;
   CHTMLTagTableData    tagTableData;
   CPMWProgressBar      progressDialog;
   CFileManager         cfmFileList, *pcfmFileList;

   ASSERT(m_pPmwDoc);
   if(m_pPmwDoc == NULL)
      return ERRORCODE_NotInitialized;

   if(dwCurPage == 1)
      m_usGraphicCount = 0;

   if(pcfmPublishFileList)
      pcfmFileList = pcfmPublishFileList; // Fill in callers file list
   else
      pcfmFileList = &cfmFileList;        // File in local list incase of errors
   // Reset Page Specific members
   m_csdwaUniqueX.RemoveAll();
   m_csdwaUniqueY.RemoveAll();
   m_otObjectTracker.RemoveAll();

   progressDialog.SetCancelMethod(OnCancel);

   sm_bCancelProcess = FALSE;
   // Check for low disk space, abort convertion if low
   lBytesFree = Util::GetAvailableDiskSpace(Filename);
   if(lBytesFree < 1000000)
   {
      CString  csDriveLetter, csLowSpaceMessage;
      Util::DirectoryDrive(Filename, csDriveLetter);
      csResource.LoadString(IDS_PUBLISH_LOW_DISK_SPACE);
      ASSERT(!csResource.IsEmpty());
      GetConfiguration()->ReplaceText(csResource);
      csLowSpaceMessage.Format(csResource, csDriveLetter);
      nRetVal = AfxMessageBox(csLowSpaceMessage, MB_YESNO | MB_DEFBUTTON2);
      if(nRetVal != IDYES)
         return ERRORCODE_Abort;
   }
   // Create Progress Bar control
   bCreated = progressDialog.Create(IDD_PROGRESS_BAR, AfxGetMainWnd());
   if(!bCreated)
   {
      csResource.LoadString(IDS_PROGRESS_CREATE_ERROR);
      ASSERT(!csResource.IsEmpty());
      AfxMessageBox(csResource);
      return ERRORCODE_Fail;
   }
   csResource.LoadString(IDS_PUBLISHING_MESSAGE);
   ASSERT(!csResource.IsEmpty());
   csMessage.Format(csResource, (long)dwCurPage);
   progressDialog.SetWindowText(csMessage);
   CProgressCtrl *pProgressBar = (CProgressCtrl *)progressDialog.GetDlgItem(IDC_PROGRESS_CONTROL);
   pProgressBar->SetRange(0, m_plHTMLObjects.GetCount());
   pProgressBar->SetStep(1);
   // Make sure origin is in coordinate list
	// (skip left origin if output is to be centered)
   CPageProperties *pPageProp;
	if (
			(m_pPmwDoc == NULL) 
			|| 
			((pPageProp = m_pPmwDoc->GetPageProperties()) == NULL)
			||
			(pPageProp->IsCenteredHTML() != TRUE)
		)
	{
		m_csdwaUniqueX.Add(0);
	}

   m_csdwaUniqueY.Add(0);

   // Build list of unique X, Y object positions in preparation for object placement
   for(pos = m_plHTMLObjects.GetHeadPosition();
      pos != NULL && errorcode == ERRORCODE_None;
      m_plHTMLObjects.GetNext(pos))
   {
      pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
      ASSERT(pHTMLObject);
      pHTMLObject->GetRect(&crectGraphic);
      m_csdwaUniqueX.Add(crectGraphic.left);
      m_csdwaUniqueX.Add(crectGraphic.right);
      m_csdwaUniqueY.Add(crectGraphic.top);
      m_csdwaUniqueY.Add(crectGraphic.bottom);
   }
   // Dump arrays of unique horiz and vert positions
#ifdef _DEBUG
#ifdef   DUMP_ON
   int i;
   for(i=0;i < m_csdwaUniqueX.GetSize();i++)
      JTRACE("X Position[%i]: %li\n", i, m_csdwaUniqueX[i]);
   for(i=0;i < m_csdwaUniqueY.GetSize();i++)
      JTRACE("Y Position[%i]: %li\n", i, m_csdwaUniqueY[i]);
#endif
#endif

   SortObjectList();
   // Create table with 1st row containing unique column widths
   ASSERT(m_csdwaUniqueX.GetSize() >= 1);
   // Initialized table properties
   tagTable.SetPadding(0);
   tagTable.SetSpacing(0);
   tagTable.SetBorder(HTML_NOBORDER);
   // Run through object list, create graphics and generate HTML
   int   nXIndex, nYIndex, nCurXPosition=0, nCurYPosition=0;
   int   nColSpan, nRowSpan = 0, nCurObject, nCurRowIndex=0;
   int   nObjectWidth, nRowHeight, nLowestObjectBottom=0, nThisRowHeight=0;
   BOOL  bSetRowHeight = FALSE, bAddedFirstRow = FALSE;
   BOOL  bNeedToAddObject = FALSE;
//////// START REWRITE
   tagTableData.SetHeight(0);
   for(nCurObject=0;(nCurObject+1) < m_csdwaUniqueX.GetSize();nCurObject++)
   {
      // Column width is diff between next position and this one
      nObjectWidth = m_csdwaUniqueX.SizeToNext(nCurObject);
      tagTableData.SetWidth(nObjectWidth);
      tagTableRow.Add(tagTableData);
      tagTableData.Empty();
   }

   tagTable.Add(tagTableRow);

   tagTableRow.Empty();
   // Run through sorted object list while creating HTML and graphics files
   AfxGetMainWnd()->EnableWindow(FALSE);
   progressDialog.EnableWindow(TRUE);     // Re-enable progress dialog

   for(pos = m_plHTMLObjects.GetHeadPosition();
      pos != NULL && errorcode == ERRORCODE_None && !sm_bCancelProcess;
      m_plHTMLObjects.GetNext(pos))
   {
      pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
      ASSERT(pHTMLObject);
      // Get position of Object
      pHTMLObject->GetRect(&crectGraphic);
      // Determine position in table
      nXIndex = m_csdwaUniqueX.Find(crectGraphic.left);
      nYIndex = m_csdwaUniqueY.Find(crectGraphic.top);
      ASSERT(nXIndex >= 0 && nYIndex >= 0);
      if(nXIndex < 0 || nYIndex < 0)      // skip object if error
         continue;
      // If starting new row, add current row to body
      if(nYIndex != nCurYPosition && bNeedToAddObject)
      {
         if ((nRowSpan > 0) && (nCurYPosition < m_csdwaUniqueY.GetSize() - 1) && (nXIndex < m_csdwaUniqueX.GetSize() - 1))
         {
            CHTMLTagTableData Blank;
#ifdef DEBUG_REMARKS
            CString  str;
            str.Format("Rowspan (%d) spacing", nRowSpan);
            Blank.AddRemark(str);
#endif
            SETCOLOR_DB(Blank, COLOR_FROM_RGBCOLOR(RGB(224,224,224)));
            Blank.SetHeight(m_csdwaUniqueY.SizeToNext(nCurYPosition));
            tagTableRow.Add(Blank);
         }
         
         tagTable.Add(tagTableRow);
         tagTableRow.Empty();
         bNeedToAddObject = FALSE;
         bSetRowHeight = FALSE;
         nCurXPosition = 0;
         nRowHeight = 0;
         nCurRowIndex++;
      }
      // If no objects have been added, this is a new row so determine
      // its height by taking difference between top of this object
      // and next vertical position.
      if(!bNeedToAddObject)
      {
         nThisRowHeight = m_csdwaUniqueY.SizeToNext(nYIndex);
      }
      // reset table data object
      tagTableData.Empty();
      // Add empty row if needed
      while(nCurRowIndex < nYIndex)
      {
         SETREMARK_DB(tagTableRow, "Blank row");
         nRowHeight = m_csdwaUniqueY.SizeToNext(nCurRowIndex);
         // If no object has been added yet, height is required for 1st cell
         if(!bNeedToAddObject)
            tagTableData.SetHeight(nRowHeight);
         tagTableRow.Add(tagTableData);
         tagTable.Add(tagTableRow);
         tagTableRow.Empty();
         tagTableData.Empty();
         bSetRowHeight = FALSE;

         nCurRowIndex++;
      }

      m_otObjectTracker.UpdateObjectList(nCurRowIndex);

      nColSpan = m_otObjectTracker.GetFirstColSpan(nCurRowIndex, nXIndex, nCurXPosition);

      while(nColSpan > 0)
      {
         tagTableData.Empty();
         if(!bSetRowHeight)
         {
            tagTableData.SetHeight(nThisRowHeight);
            bSetRowHeight = TRUE;
         }
         if(nColSpan > 1)
            tagTableData.SetColSpan(nColSpan);
         tagTableRow.Add(tagTableData);
         nColSpan = m_otObjectTracker.GetNextColSpan();
      }

      tagTableData.Empty();
      // Set cell height equal to height of this object
      tagTableData.SetHeight(crectGraphic.Height());
      // Set alignment for object
		tagTableData.SetAlignmentV(CHTMLTagTableData::VtypeTop);
		tagTableData.SetAlignmentH(CHTMLTagTableData::HtypeLeft);

      nCurXPosition = m_csdwaUniqueX.Find(crectGraphic.right);
      nColSpan =  nCurXPosition - nXIndex;
      // Set column span of this object
      if(nColSpan > 1)
         tagTableData.SetColSpan(nColSpan);
      // Set row span of this object
      nCurYPosition = m_csdwaUniqueY.Find(crectGraphic.top);
      nRowSpan =  m_csdwaUniqueY.Find(crectGraphic.bottom) - nCurYPosition;
      // Set row span of this object
      if(nRowSpan > 1)
      {
         tagTableData.SetRowSpan(nRowSpan);
         m_otObjectTracker.AddObject(
            nCurRowIndex,
            nRowSpan,
            nXIndex,       // Start column of this object
            nColSpan);
      }

      if (pWebStats != NULL)
      {
         CHTMLCompliantInfo CompliantInfo;
         pHTMLObject->GetCompliantInfo(CompliantInfo);
         if (CompliantInfo.HasComplianceErrors())
            pWebStats->AddCompliantInfoCopy(CompliantInfo);
      }

      if(pHTMLObject->GetOutputType() == CHTMLObject::outputtypeText)
      {
         pHTMLObject->GetHTML(tagTableData);
      }
      else
      {
         // Build filename prefix for CreateGraphic
         CString  csDirectory;
         Util::SplitPath(Filename, &csDirectory, NULL);
         Util::AppendBackslashToPath(csDirectory);
         csGraphicFileName.Format("%simg%05i", csDirectory, (int)m_usGraphicCount + 1);
         errorcode = pHTMLObject->ConvertToWebGraphic(&csGraphicFileName, m_pPmwDoc);
         Util::SplitPath(pHTMLObject->GetFileName(), NULL, &csBaseFileName);
         // Add file to list 
         pcfmFileList->Add(pHTMLObject->GetFileName());

         CHTMLTagImage        tagImage;   // This must be here so it gets destructed
         if(errorcode == ERRORCODE_None)
         {
            tagImage.SetHeight(crectGraphic.Height());
            tagImage.SetWidth(crectGraphic.Width());
            tagImage.SetSource(csBaseFileName);

            if (pHTMLObject->IsLink() == TRUE)
            {
               pHTMLObject->CopyLinkedExternalFiles(csDirectory, pcfmFileList);
               tagImage.NoBorder();
               CHTMLTagLink   Link;

               HyperlinkData  hd;
               pHTMLObject->GetHyperlinkData(hd);
               Link.SetHyperlinkData(hd, m_pPmwDoc->DocumentRecord());

               tagTableData.Merge(Link);
            }
            else
            if (pHTMLObject->NeedsMap() == TRUE)
            {
JTRACE("\nCHTMLDocConverter::ConvertToHTML found image map object\n");
               pHTMLObject->CopyLinkedExternalFiles(csDirectory, pcfmFileList);
               tagImage.NoBorder();
               CString  mapname;
               pHTMLObject->GetMapName(mapname);
               tagImage.SetUseMap(mapname);
            }

            tagTableData.Add(tagImage);
            m_usGraphicCount++;
         }
         else
            continue;   // Bail!
      }
      // Set dimensions of table data object
      tagTableData.SetWidth(crectGraphic.Width());
      bNeedToAddObject = TRUE;

      if(nRowSpan == 1)
         bSetRowHeight = TRUE;

      if(crectGraphic.bottom > nLowestObjectBottom)
         nLowestObjectBottom = crectGraphic.bottom;
      // Add object to row
      tagTableRow.Add(tagTableData);

      tagTableData.Empty();

      pProgressBar->StepIt();

      Util::YieldToWindows();
   }
   pProgressBar->UpdateWindow();
   AfxGetMainWnd()->EnableWindow(TRUE);

   if(sm_bCancelProcess)
   {
      errorcode = ERRORCODE_Abort;
   }

   if(errorcode == ERRORCODE_None)
   {
      if(bNeedToAddObject)
      {
         SETREMARK_DB(tagTableRow, "leftover row");
         
         tagTable.Add(tagTableRow);
         nCurRowIndex++;
      }
      // Add Remaining Rows
      nYIndex = m_csdwaUniqueY.GetSize() - 1;
      while(nCurRowIndex < nYIndex)
      {
         tagTableRow.Empty();
         tagTableData.Empty();
         nRowHeight = m_csdwaUniqueY[nCurRowIndex+1] - m_csdwaUniqueY[nCurRowIndex];
         tagTableData.SetHeight(nRowHeight);
         SETCOLOR_DB(tagTableData, COLOR_FROM_RGBCOLOR(RGB(255,224,224)));
         SETREMARK_DB(tagTableRow, "Bottom filler rows");
         tagTableRow.Add(tagTableData);
         tagTable.Add(tagTableRow);

         nCurRowIndex++;
      }
//////// END REWRITE
      // Output HTML Document
      CHTMLTagBody   tagBody;
      SetBackground(tagBody, Filename, dwCurPage, pcfmFileList, &dwGraphicsSize);
      tagBody.Add(tagTable);
      // Dump all image maps
      // And add up graphics sizes
      CHTMLObject *pHTMLObject;
      for (POSITION pos = m_plHTMLObjects.GetHeadPosition();pos != NULL; m_plHTMLObjects.GetNext(pos))
      {
         pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
         ASSERT(pHTMLObject);
         if(pHTMLObject)
         {
            dwGraphicsSize += pHTMLObject->GetGraphicSize();
            dwLinkedFileSize += pHTMLObject->GetLinkedFileSize();
            if (pHTMLObject->NeedsMap() == TRUE)
            {
               pHTMLObject->FinalizeImageMaps(m_pPmwDoc->DocumentRecord());
               CImageMapElementArray *pMap;
               if ((pMap = pHTMLObject->GetImageMap()) != NULL)
               {
                  CHTMLTagImageMap  TagImageMap;
                  CString  temp;
                  pHTMLObject->GetMapName(temp);
                  TagImageMap.SetName(temp);
                  TagImageMap.CopyImageMapElementArray(*pMap);
                  tagBody.Add(TagImageMap);
               }
            }
         }
      }
      
      CHTMLTagDocument     tagDocument;
		CreateHTMLDocument(tagDocument);

      tagDocument.Add(tagBody);

      TRY
      {
         CFile cfileHTMLDoc(Filename, CFile::modeReadWrite | CFile::modeCreate);
         pcfmFileList->Add(Filename);

         tagDocument.Write(cfileHTMLDoc);
         
         dwHTMLSize = cfileHTMLDoc.GetLength();

         cfileHTMLDoc.Close();
      }
      END_TRY

      if (pWebStats != NULL)
         pWebStats->SetSizes (dwGraphicsSize, dwLinkedFileSize, dwHTMLSize);
   }
   else
      pcfmFileList->RemoveAll();

   progressDialog.DestroyWindow();

   return errorcode;
}

#define	PANELPADDING	16

void
CHTMLDocConverter::CreateHTMLPanel(CHTMLTagDiv &tagDiv, int idx, LPCSTR Title, BOOL Prev, BOOL Next, int Width, CHTMLPanelData *pPanelData)
{
   CHTMLTagTable        tagTable;
   CHTMLTagTableRow     tagTableRow;
   CHTMLTagTableData    tagTableData;
	CHTMLFont				tagFont;
   CHTMLTagImage        tagImage;

	tagDiv.Empty();
	tagDiv.SetABSPosition(0, 0);
	
	CString	Temp;
	Temp.Format("div%d", idx);
	tagDiv.SetID(Temp);
#ifdef _DEBUG	
//	tagTable.SetBorder(1);
#endif	
	tagFont.SetSize(4);
////
	if (Prev)
   {
		CHTMLFont	LinkFont;
		LinkFont.SetStyle(CHTMLFont::styleItalic);
		LinkFont.AddText("Previous");
		LinkFont.SetSize(12);
		
		CHTMLTagLink	tagLink;
		tagLink.Add(LinkFont);
		tagLink.SetOnClick("PrevPage()");
		tagLink.SetLocalHyperlinkData("#", m_pPmwDoc->DocumentRecord()); 
		tagTableData.SetAlignmentH(CHTMLTagTableData::HtypeLeft);   
      tagTableData.Merge(tagLink);
	}
	else
	{
		tagFont.AddText(" ");
		tagTableData.Add(tagFont);
	}

	int	Width3 = (Width / 3) + PANELPADDING;

	tagTableData.SetWidth(Width3);
	tagTableRow.Add(tagTableData);
	tagFont.EmptyBody();
	tagTableData.Empty();
////
	tagFont.AddText(Title);
	tagFont.SetSize(14);
	tagTableData.Add(tagFont);
	tagTableData.SetWidth(Width3);
	tagTableData.SetAlignmentH(CHTMLTagTableData::HtypeCenter);   
	tagTableRow.Add(tagTableData);
	tagFont.EmptyBody();
	tagTableData.Empty();
////
	if (Next)
   {
		CHTMLFont	LinkFont;
		LinkFont.SetStyle(CHTMLFont::styleItalic);
		LinkFont.AddText("Next");
		LinkFont.SetSize(12);

		CHTMLTagLink	tagLink;
		tagLink.Add(LinkFont);
      tagLink.SetOnClick("NextPage()");
		tagLink.SetLocalHyperlinkData("#", m_pPmwDoc->DocumentRecord()); 
		tagTableData.SetAlignmentH(CHTMLTagTableData::HtypeRight);   
      tagTableData.Merge(tagLink);
   }
	else
	{
		tagFont.AddText(" ");
		tagTableData.Add(tagFont);
	}

	tagTableData.SetWidth(Width3);
	tagTableRow.Add(tagTableData);
	tagTableData.Empty();
////
	tagTable.Add(tagTableRow);

	tagTableRow.Empty();

	tagTableData.SetColSpan(3);
	tagTableData.SetAlignmentH(CHTMLTagTableData::HtypeCenter);

	tagImage.SetSource(pPanelData->GetName());
	if (pPanelData->GetSize() > 0)
	{
		CString s;
		CHTMLObject::FormImageMapName(s, idx);
		tagImage.SetUseMap(s);
	}

	tagImage.NoBorder();
   tagTableData.Add(tagImage);
	tagTableData.SetWidth(Width + (PANELPADDING * 3));

	tagTableRow.Add(tagTableData);
	tagTable.Add(tagTableRow);

	tagDiv.Add(tagTable);
}

void	
CHTMLDocConverter::CreateHTMLDocument(CHTMLTagDocument &d, CJavaScript *js/*=NULL*/)
{
	CString	WebTitle = "@@N Web Page";
	GetConfiguration()->ReplaceText(WebTitle);
	CHTMLTagHeader tagHeader(WebTitle);

	WebTitle = "@@T user";
	GetConfiguration()->ReplaceText(WebTitle);
	tagHeader.SetAuthor(WebTitle);

	if (js != NULL)
		tagHeader.Add(*js);

	d.Init(tagHeader);
}

ERRORCODE
CHTMLDocConverter::BuildHTMLForGifs(LPCSTR FileName, CHTMLPanelArrayArray &GifNames, int width, CFileManager *pcfmFileList, DWORD *pd)
{
   int i;

   ASSERT(m_pPmwDoc);
   if(m_pPmwDoc == NULL)
      return ERRORCODE_NotInitialized;
	
	ERRORCODE	error = ERRORCODE_Fail;
	// Build the HTML file
   CHTMLTagBody   tagBody;
	CHTMLTagJSCall	tagJSCall;
   CHTMLTagDiv		tagDiv;

	tagBody.SetOnLoad("Init()");
   //SetBackground(tagBody, FileName, 0, pcfmFileList, pd);
	tagDiv.SetABSPosition(0, 0);
	// Create a DIV for each GIF file created
	for (i = 0; i < GifNames.GetSize(); i ++)
	{
		BOOL	LastPict = i < GifNames.GetSize() - 1 ? FALSE : TRUE;
		CString cp;
		if (i == 0)
			cp.LoadString(IDS_FRONT);
		else
		if (LastPict)
			cp.LoadString(IDS_BACK);
		else
			cp.LoadString(IDS_INSIDE);
		
		CreateHTMLPanel(tagDiv, 
								i, 
								cp, 
								i == 0 ? FALSE : TRUE, 
								!LastPict, 
								width, 
								GifNames.GetAt(i)); 

		tagBody.Add(tagDiv);
	}

	for (i = 0; i < GifNames.GetSize(); i ++)
	{
		CHTMLPanelData *pPanelData = GifNames.GetAt(i);
		if (pPanelData->GetSize() > 0)
		{
//			CRect	Rect;
//			pPanelData->GetRect(Rect);
//			CHTMLObject::FinalizeImageMaps(m_pPmwDoc->DocumentRecord(), *pPanelData, Rect);
			
			CHTMLTagImageMap  TagImageMap;
			CString  temp;
			CHTMLObject::FormImageMapName(temp, i);
			TagImageMap.SetName(temp);
			TagImageMap.CopyImageMapElementArray(*pPanelData);
			tagBody.Add(TagImageMap);
		}
	}
	
	CHTMLTagDocument     tagDocument;
	CJavaScriptPager		jp(width + (PANELPADDING * 3), GifNames.GetSize());
	
	CreateHTMLDocument(tagDocument, &jp);

   tagDocument.Add(tagBody);
	// Save the HTML document
   TRY
   {
      CFile cfileHTMLDoc(FileName, CFile::modeReadWrite | CFile::modeCreate);

      tagDocument.Write(cfileHTMLDoc);
      
      if (pd != NULL)
			*pd = cfileHTMLDoc.GetLength();

      cfileHTMLDoc.Close();
		
		error = ERRORCODE_None;
   }
   END_TRY

	return error;
}

#ifdef _DEBUG
#ifdef   DUMP_ON
void CHTMLDocConverter::Dump(CDumpContext& dc) const
{
   CRect                crectGraphic;
   CString              csRectPos;
   CHTMLObject * pHTMLObject;
   CHTMLObject::OutputTypes  outputType;

   for(POSITION pos = m_plHTMLObjects.GetHeadPosition();pos != NULL;)
   {
      pHTMLObject = (CHTMLObject *) m_plHTMLObjects.GetAt(pos);
      ASSERT(pHTMLObject);
      pHTMLObject->GetRect(&crectGraphic);
      csRectPos.Format("(%i,%i,%i,%i)", crectGraphic.left, crectGraphic.top, crectGraphic.right, crectGraphic.bottom);
      dc << "Graphic Rect Position: " << csRectPos << "\r\n";
      outputType = pHTMLObject->GetOutputType();
      dc << "Graphic Type: ";
      switch((int)outputType)
      {
         case CHTMLObject::outputtypeGraphic:
            dc << "GIF";
            break;
         case CHTMLObject::outputtypeText:
            dc << "Text";
            break;
         default:
            dc << "Undefined";
            break;
      }
      dc << "\r\n";
      m_plHTMLObjects.GetNext(pos);
   }
}
#endif
#endif
////////////////////////////////////////////////////////////////////////////////
// CHTMLObjectTracker methods
CHTMLObjectTracker::CHTMLObjectTracker()
{
   RemoveAll();
}

CHTMLObjectTracker::~CHTMLObjectTracker()
{
   RemoveAll();
}

void CHTMLObjectTracker::AddObject(int nStartRow, int nRowSpan, int nStartCol, int nColSpan)
{
   CHTMLObjectSpanInfo  *pSpanInfo, *pCurSpanInfo;
   BOOL                 bAddedItem = FALSE;
   POSITION             pos;

   ASSERT(nRowSpan > 1);
   // Only add objects that span rows
   if(nRowSpan <= 1)
      return;
   pSpanInfo = new CHTMLObjectSpanInfo;
   ASSERT(pSpanInfo);
   if(pSpanInfo == NULL)
      AfxThrowMemoryException();

   pSpanInfo->Set(nStartRow, nStartRow + nRowSpan - 1, nStartCol, nStartCol + nColSpan - 1);
   // Add items in sorted order 
   // Sort order should be by leftmost column, then row within same column
   for(pos = m_plObjectSpanInfo.GetHeadPosition();
      pos != NULL && !bAddedItem;
      m_plObjectSpanInfo.GetNext(pos))
   {
      pCurSpanInfo = (CHTMLObjectSpanInfo *) m_plObjectSpanInfo.GetAt(pos);
      ASSERT(pCurSpanInfo);
      // Insert new item before this one if column position is at or before
      if(pSpanInfo->GetStartCol() == pCurSpanInfo->GetStartCol())
      {
         // if new item is on same column and positioned before this one, 
         // then Add it
         if(pSpanInfo->GetStartRow() < pCurSpanInfo->GetStartRow())
         {
            m_plObjectSpanInfo.InsertBefore(pos, pSpanInfo);
            bAddedItem = TRUE;
         }
      }
      else if(pSpanInfo->GetStartCol() < pCurSpanInfo->GetStartCol())
      {
         m_plObjectSpanInfo.InsertBefore(pos, pSpanInfo);
         bAddedItem = TRUE;
      }
   }
   // If it didn't get added, make it last item in list
   if(!bAddedItem)
      m_plObjectSpanInfo.AddTail(pSpanInfo);
}
// This method runs through object list to determine if any objects can be
// by checking if current row if greater than end row for an object
// and removes it from list
void CHTMLObjectTracker::UpdateObjectList(int nCurRow)
{
   CHTMLObjectSpanInfo  *pSpanInfo;
   POSITION             pos, posNext;

   for(pos = m_plObjectSpanInfo.GetHeadPosition();pos != NULL;)
   {
      pSpanInfo = (CHTMLObjectSpanInfo *) m_plObjectSpanInfo.GetAt(pos);
      ASSERT(pSpanInfo);
      // Use current position to get next position
      posNext = pos;
      m_plObjectSpanInfo.GetNext(posNext);
      if(nCurRow > pSpanInfo->GetEndRow())
      {
         delete pSpanInfo;
         pSpanInfo = NULL;
         m_plObjectSpanInfo.RemoveAt(pos);
      }
      pos = posNext;
   }
}

int CHTMLObjectTracker::GetFirstColSpan(int nThisObjectStartRow, int nThisObjectStartCol, int nPrevObjectEndCol)
{
   m_nThisObjectStartCol = nThisObjectStartCol;
   m_nThisObjectStartRow = nThisObjectStartRow;
   m_nPrevObjectEndCol = nPrevObjectEndCol;

   return GetColSpan(m_plObjectSpanInfo.GetHeadPosition(), m_nThisObjectStartCol - m_nPrevObjectEndCol);
}

int CHTMLObjectTracker::GetNextColSpan()
{
   if(m_bNextObjectSpansRow == FALSE)
      return 0;

   return GetColSpan(m_posCurrent);
}

int CHTMLObjectTracker::GetColSpan(POSITION posStart, int nDefaultColSpan /* =0 */)
{
   CHTMLObjectSpanInfo  *pSpanInfo;
   POSITION             pos;
   int                  nColSpan = nDefaultColSpan;

   m_bNextObjectSpansRow = FALSE;
   // Run though object list to determine which ones are currently
   // spanning rows the preceed this object horizontally
   for(pos = posStart, m_posCurrent=NULL;pos != NULL && m_posCurrent == NULL;)
   {
      POSITION posThis;

      pSpanInfo = (CHTMLObjectSpanInfo *) m_plObjectSpanInfo.GetAt(pos);
      ASSERT(pSpanInfo);
      if(pSpanInfo == NULL)
         continue;

      posThis = pos;
      m_plObjectSpanInfo.GetNext(pos);
      if(m_nThisObjectStartRow > pSpanInfo->GetStartRow() && m_nThisObjectStartRow <= pSpanInfo->GetEndRow() &&
         m_nThisObjectStartCol > pSpanInfo->GetEndCol() && m_nPrevObjectEndCol <= pSpanInfo->GetEndCol())
      {
         CHTMLObjectSpanInfo  *pNextSpanInfo = NULL;
         int nNextObjectStartCol = m_nThisObjectStartCol;
         // If there are spanning columns preceeding this object, add those first
         // then add column spans that follow this object in get next logic
         if(m_nPrevObjectEndCol < pSpanInfo->GetStartCol())
         {
            nColSpan = pSpanInfo->GetStartCol() - m_nPrevObjectEndCol;
            // We leave m_posCurrent to point to this object so GetNext logic
            // starts with this object again
            m_posCurrent = posThis;
            m_nPrevObjectEndCol = pSpanInfo->GetEndCol();
            m_bNextObjectSpansRow = TRUE;
            continue;
         }
         m_nPrevObjectEndCol = pSpanInfo->GetEndCol();

         if(pos != NULL)
            pNextSpanInfo = (CHTMLObjectSpanInfo *) m_plObjectSpanInfo.GetAt(pos);
         // If next object in list spans row of the object we are 
         // getting span for, compute column span based upon 
         // difference between the end column of the object we first 
         // found in list and the start column of next object in list
         if(pNextSpanInfo &&
            m_nThisObjectStartRow > pNextSpanInfo->GetStartRow() && m_nThisObjectStartRow <= pNextSpanInfo->GetEndRow() &&
            m_nThisObjectStartCol > pNextSpanInfo->GetEndCol())
         {
            nNextObjectStartCol = pNextSpanInfo->GetStartCol();
            m_bNextObjectSpansRow = TRUE;
            m_nPrevObjectEndCol = pNextSpanInfo->GetEndCol();
         }

         nColSpan = nNextObjectStartCol - (pSpanInfo->GetEndCol() + 1);
         m_posCurrent = pos;
      }
   }
   return nColSpan;
}

void CHTMLObjectTracker::RemoveAll()
{
   // Cleanup object list
   CHTMLObjectSpanInfo  *pSpanInfo;
   POSITION             pos;

   for(pos = m_plObjectSpanInfo.GetHeadPosition();
      pos != NULL;
      m_plObjectSpanInfo.GetNext(pos))
   {
      pSpanInfo = (CHTMLObjectSpanInfo *) m_plObjectSpanInfo.GetAt(pos);
      ASSERT(pSpanInfo);
      if(pSpanInfo)
         delete pSpanInfo;
   }
   m_plObjectSpanInfo.RemoveAll();
   m_posCurrent = NULL;
   m_nThisObjectStartCol = m_nThisObjectStartRow = 0;
   m_bNextObjectSpansRow = FALSE;
}
