/***************************************************************************
*  FILE:        HTMLCONV.H                                                 *
*  SUMMARY:     Class for converting a PrintMaster document to HTML        *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  1/20/98  DGP      Created                                               *

   See htmlconv.cpp for revision history

***************************************************************************/
#ifndef _HTMLCONV_INC
#define _HTMLCONV_INC

#include "pmgobj.h"
#include "hyperrec.h"
#include "imagemap.h"
#include "htmldoc.h"
#include "sortarry.h"   // For CSortedDWordArray
#include "filemgr.h"    // For CFileManager
#include "pmwdoc.h"

//class CPmwDoc;
class CHTMLObjectSpanInfo;
class CHTMLObjectTracker;
class CImageMapElementArray;
class CHTMLCompliantInfo;

//#define  DUMP_ON

class CWebPageStats
{
public:   
   CWebPageStats()
   {
      Init();
   }

   ~CWebPageStats()
   {
      EmptyGroups();
   }

   void
   EmptyGroups(void);

   void
   Init(void)
   {
      SetSizes(0, 0, 0);
      EmptyGroups();
   }

   void
   SetSizes(DWORD GraphicSize, DWORD LinkedFileSize, DWORD HTMLSize)
   {
      SetSizeGraphic(GraphicSize);
      SetSizeLinked(LinkedFileSize); 
      SetSizeHTML(HTMLSize);
   }

   void
   SetSizeGraphic(DWORD gs)
   {
      m_dwGraphicSize = gs;
   }

   void
   SetSizeLinked(DWORD ls)
   {
      m_dwLinkedFileSize = ls;
   }
   
   void
   SetSizeHTML(DWORD hs)
   {
      m_dwHTMLSize = hs;
   }
////////
   void
   AddCompliantInfoCopy(CHTMLCompliantInfo &ci);
////////
   int
   GetDownloadSecondsTotalInt(DWORD bps)
   {
      double d = GetDownloadSecondsTotal(bps);
      return (int)ceil(d); 
   }
   
   double
   GetDownloadSecondsTotal(DWORD bps)
   {
      return 
      GetDownloadSecondsGraphics(bps) + 
      GetDownloadSecondsLinked(bps) +
      GetDownloadSecondsHTML(bps);
   }
   
   double
   GetDownloadSecondsGraphics(DWORD bps)
   {
      return((double) m_dwGraphicSize * 10.0) / (double)bps;
   }
   
   double
   GetDownloadSecondsLinked(DWORD bps)
   {
      return((double) m_dwLinkedFileSize * 10.0) / (double)bps;
   }
   
   double
   GetDownloadSecondsHTML(DWORD bps)
   {
      return((double) m_dwHTMLSize * 10.0) / (double)bps;
   }

   void
   CopyOf(CWebPageStats &ps);
 
   int
   NumComplianceGroups(void)
   {
      return m_ComplianceGroups.GetSize();
   }

   CHTMLCompliantInfo*
   GetComplianceGroup (int idx)
   {
      if (idx < NumComplianceGroups())
         return m_ComplianceGroups[idx];
      return NULL;
   }

protected:
   DWORD                m_dwGraphicSize, m_dwLinkedFileSize, m_dwHTMLSize;
   CArray<CHTMLCompliantInfo*, CHTMLCompliantInfo*> m_ComplianceGroups;
};

class CHTMLObject
{
public:
// Construction
   CHTMLObject();
   virtual ~CHTMLObject();
// Operations
   enum OutputTypes
   {
      outputtypeUndefined,
      outputtypeGraphic,
      outputtypeText,
   };
   // Checks if passed rect intersects with this object
   BOOL  DoesRectIntersect(CRect &crectToCheck);
   ERRORCODE Add(PMGPageObject *pPMGPageObject, CRect &crectDeviceObjectPosition, CRect &crectDeviceObjectBound);
   ERRORCODE Add(PMGPageObject *pObject);
   ERRORCODE Add(CHTMLObject *pGraphicObject);
   OutputTypes GetOutputType(void);
   ERRORCODE GetHTML(CHTMLTag &refHTMLTag);
   // Creates a graphic from current list of PrintMaster objects
   // The supplied filename should be prefix only (no extension)
   // ConvertToWebGraphic will automatically determine file type and
   // append the appropriate filename when creating the file
   // Use GetFileName() to get the name of the file created.
   // 
   // Note: There is a static version of this method which doesn't
   // use information available from this object.
   ERRORCODE   ConvertToWebGraphic(CString *pcsFileName, CPmwDoc *pPmwDoc);
   // Returns name of file created by CreateGraphic
   CString &   GetFileName()
   {  return m_csFileName; }

   void  GetRect(CRect *pRect);
// Hyperlink stuff
   void
   CopyLinkedExternalFiles(LPCSTR Dir, CFileManager *FileManager);

   void
   GetHyperlinkData(HyperlinkData &hd);

   BOOL
   IsLink(void);

   DWORD
   GetLinkedFileSize(void)
   {
      return m_dwLinkedFileSize;
   }
//////// Image map stuff
   BOOL
   NeedsMap(void);

   BOOL
   GetMapName(CString &s);

	void
   FinalizeImageMaps(DocumentRecord *pdr);

   void
   AddImageMapInfo(HyperlinkData &hd, PMObjectShapeData &sd)
	{
		AddImageMapInfo(hd, sd, mImageMapElementArray);
	}

   CImageMapElementArray*
   GetImageMap(void);
//////// HTML Compliance
   void
   GetCompliantInfo(CHTMLCompliantInfo &ci)
   {
      ASSERT(&ci != NULL);
		ci.CopyOf(m_CompliantInfo);
   }
//////// PM Objects
   void
   AddObjectData(PMGPageObject *pObject, CRect &crectDeviceObjectPosition);

   PMGPageObject*
   GetFirstObject(void)
   {
      return m_CompliantInfo.GetFirstObject();
   }
   
   PMGPageObject*
   GetNextObject(void)
   {
      return m_CompliantInfo.GetNextObject();
   }
   
   BOOL
   AddObjectPointer(PMGPageObject* po)
   {
      return m_CompliantInfo.AddObjectPointer(po);
   }

   int
   NumPMObjects(void) const
   {
      return m_CompliantInfo.NumPMObjects();
   }
////////
   BOOL
   SetTypeAndCompliance(void);

   DWORD
   GetGraphicSize(void)
   {
      return m_dwGraphicSize;
   }

protected:
   BOOL                    WasText;
   int                     mImageMapToUse;

   static int              sInstanceCount, sMapName;  
   CImageMapElementArray   mImageMapElementArray;
   
public:
// PUBLIC STATICS
   static void
   AddImageMapInfo(HyperlinkData &hd, PMObjectShapeData &sd, CImageMapElementArray &imar, LPCSTR url = NULL);

	static void
   FinalizeImageMaps(DocumentRecord *pdr, CImageMapElementArray &imar, CRect &ImageArea);

   static void
	FormImageMapName(CString &s, int i)
	{
		s.Format("map%d", i);
	}

   static void
   TextLinksAsMaps(PMGPageObject *pPMGObject, CImageMapElementArray *pMap);

   static void LogicalToDeviceUnits(PMGPageObject *pObject, CRect &crDevice, CRect *pBound = NULL);
   static void LogicalToDeviceUnits(PBOX &refPBOX, CRect *pcrectDevice);
   static void LogicalToDeviceUnits(PBOX &pb, CPoint &p)
	{
		CRect	Rect;
		LogicalToDeviceUnits(pb, &Rect);
		p.x = Rect.Width();
		p.y = Rect.Height();
	}
	static PCOORD LogicalToDeviceUnits(PCOORD p);
   static void SetPageDimensions(PPNT ppntPageDimensions)
   {
      sm_ppntPageDimensions = ppntPageDimensions;
   }
   static ERRORCODE   ConvertToWebGraphic (CHTMLCompliantInfo &CompliantInfo,
                  CString *pcsFileName, 
                  CPmwDoc *pDoc,
                  COLORREF crFillColor, 
                  CRect &rImageArea,
                  DWORD *pGraphicSize = NULL,
                  BOOL  bObjectIsClipped = FALSE,
                  CImageMapElementArray *pMap = NULL);

   static void ScalePBOX(PBOX_PTR pbox, SHORT DestXRes, SHORT SourceXRes, SHORT DestYRes, SHORT SourceYRes);

	static ERRORCODE
	OutputGIF(CString *Filename, CDIBInfo &Info, COLORREF FillColor, DWORD *pGraphicSize = NULL);
// Diagnostics
#ifdef _DEBUG
#ifdef DUMP_ON
   void CHTMLObject::Dump(CDumpContext& dc) const;
#endif
#endif

protected:
// Data Members
   CRect                m_crectImageArea; // Area occupied by this object/group
                                          //   in Device (pixels) units
   CString              m_csFileName;     // Output Filename
   OutputTypes          m_enumOutputType;
   BOOL                 m_bObjectClipped;
   CHTMLCompliantInfo   m_CompliantInfo;         
   DWORD                m_dwGraphicSize, m_dwLinkedFileSize;
// Static
   static int           sm_nXResolution;      // Device X resolution (pixels)
   static int           sm_nYResolution;      // Device Y resolution (pixels)
   static PPNT          sm_ppntPageDimensions;
   static BOOL          sm_bRetrievedResolution;
};

// This class contains information used for tracking objects that span rows
class CHTMLObjectSpanInfo
{
public:
   void  Set(int nStartRow, int nEndRow, int nStartCol, int nEndCol)
   {
      m_nStartRow = nStartRow;
      m_nEndRow = nEndRow;
      m_nStartCol = nStartCol;
      m_nEndCol = nEndCol;
   }
   int GetStartRow()
   {  return m_nStartRow; }
   int GetEndRow()
   {  return m_nEndRow; }
   int GetStartCol()
   {  return m_nStartCol; }
   int GetEndCol()
   {  return m_nEndCol; }
   int GetColSpan()
   {  return m_nEndCol - m_nStartCol + 1; }
   int   m_nStartRow;
   int   m_nEndRow;
   int   m_nStartCol;
   int   m_nEndCol;
};
// This class tracks objects that span rows
class CHTMLObjectTracker
{
   public:
   CHTMLObjectTracker();
   ~CHTMLObjectTracker();
   void  AddObject(int nStartRow, int nRowSpan, int nStartCol, int nColSpan);
   void  UpdateObjectList(int nCurRow);
   int   GetFirstColSpan(int nCurRow, int nCurCol, int nPrevCol);
   int   GetNextColSpan();
   void  RemoveAll();
   
   protected:
   // Helpers
   int   GetColSpan(POSITION posStart, int nDefaultColSpan=0);
   
   protected:
   BOOL     m_bNextObjectSpansRow;
   int      m_nThisObjectStartCol, m_nThisObjectStartRow, m_nPrevObjectEndCol;
   POSITION m_posCurrent;
   CPtrList m_plObjectSpanInfo;
};

class CUniqueArray : public CSortedDWordArray
{
public:
   DWORD
   SizeToNext(int idx)
   {
      ASSERT(idx + 1 < GetSize());
      if (idx + 1 < GetSize())
         return GetAt(idx + 1) - GetAt(idx);
      return 0;
   }   
};
// An array of image map elements, with a file name and rectangle
class	CHTMLPanelData : public CImageMapElementArray
{
	INHERIT(CHTMLPanelData, CImageMapElementArray)
public:
	CHTMLPanelData(void)
	{
//		Rect.SetRectEmpty();
	}
	CHTMLPanelData(CHTMLPanelData &pd)
	{
		CardGifName = pd.CardGifName;
//		pd.GetRect(Rect);
		INHERITED::Copy(pd);
	}
	LPCSTR
	GetName(void)
	{
		return CardGifName;
	}
	void
	SetName(LPCSTR	s)
	{
		CardGifName = s;
	}
/*
	void
	SetRect(int w, int h)
	{
		Rect.left = 0;
		Rect.right = w;
		Rect.top = 0;
		Rect.bottom = h;	
	}
	void
	SetRect(int l, int t, int r, int b)
	{
		Rect.left = l;
		Rect.right = r;
		Rect.top = t;
		Rect.bottom = b;	
	}
	void
	GetRect(CRect &r)
	{
		r = Rect;
	}
*/
protected:
	CString	CardGifName;
//	CRect		Rect;
};
// An array of CHTMLPanelDatas (arrays)
class CHTMLPanelArrayArray
{
public:
   ~CHTMLPanelArrayArray(void)
	{
		Empty();
	}
	int
   GetSize(void)
   {
      return mArray.GetSize(); 
   }
	int
	Add(CHTMLPanelData &pd)
	{
		TRY
		{
			CHTMLPanelData *p = new CHTMLPanelData(pd);
			int idx = mArray.Add(p);
			return idx;
		}
		END_TRY
   
		return -1;
	}
	CHTMLPanelData *
	GetAt(int idx)
	{
		if ((idx < 0) || (idx >= mArray.GetSize()))
			return NULL;
		return (CHTMLPanelData*)mArray.GetAt(idx);
	}
	void
	Empty(void)
	{
		for(int i = 0; i < mArray.GetSize(); i ++)
		{
			CHTMLPanelData* p = GetAt(i);
			delete p;
		}
		mArray.RemoveAll();
	}
protected:
	CPtrArray	mArray;
};

class CHTMLDocConverter
{
public:
   // Construction
   CHTMLDocConverter(CPmwDoc *pDoc);
   virtual ~CHTMLDocConverter();

   // Operations
	CPtrList*
	GetHTMLObjectList(void)
	{
		return &m_plHTMLObjects;
	}

   void      RemoveAll();     // Removes all objects
   // ConvertToHTML() converts the objects(s) which have been added
   // from calls to Add() to an HTML document and .GIF/JPEG files
   // in the specified directory.
   ERRORCODE 
	ConvertToPagedHTML(
      const CString &Filename,
      CFileManager *pcfmFileList,
      DWORD dwCurPage,
      CWebPageStats *pWebStats = NULL);
   // Helpers
   void  DetermineTextObjectConversionType();
#ifdef _DEBUG
#ifdef DUMP_ON
   void CHTMLDocConverter::Dump(CDumpContext& dc) const;
#endif
#endif
public:

   static void
   GetHTMLFileName(DWORD dwPage, CString &FileName);

   static void
   GetHTMLHomeName(CString &FileName)
   {
      GetHTMLFileName(0, FileName);
   }

   static void
   SetHTMLHomeName(CString &FileName);

   static void
   GetHTMLFileExt(CString &Ext);

   static void
   SetHTMLFileExt(CString &Ext);

   static void
   GetHTMLPathName(DWORD dwPage, CString &FileName);

   static void
   GetHTMLPathOnly(CString &Path);

   static void
   SetHTMLPathOnly(CString &Path);

   void
   KillHTMLStrings(void)
   {
      msDir.Empty();
      msHomePage.Empty();
      msExt.Empty();   
   }

	ERRORCODE
	BuildHTMLForGifs(LPCSTR FileName, CHTMLPanelArrayArray &GifNames, int width, CFileManager *pcfmFileList, DWORD *pd = NULL);	

protected:

   static CString msDir, msHomePage, msExt;
   // Helpers
   void YieldToWindows();
   static void OnCancel(void);
   // Data Members
   CPtrList             m_plHTMLObjects;     // List of HTML Objects represent
                                             // graphic that ends up in HTML
   CUniqueArray         m_csdwaUniqueX;      // Array of unique X positions
   CUniqueArray         m_csdwaUniqueY;      // Array of unique Y positions
   CPmwDoc              *m_pPmwDoc;
   CHTMLObjectTracker   m_otObjectTracker;
   static BOOL          sm_bCancelProcess;
   unsigned short       m_usGraphicCount;
   
   void
   SortObjectList(void);

   void
   SetBackground(CHTMLTagBody &tagBody, const CString &csDirectory, DWORD dwCurPage, CFileManager *pcfmFileList, DWORD *dwBackSize = NULL);
	
	void	
	CreateHTMLDocument(CHTMLTagDocument &d, CJavaScript *js = NULL);

	void
	CreateHTMLPanel(CHTMLTagDiv &tagDiv, int idx, LPCSTR Title, BOOL Prev, BOOL Next, int Width, CHTMLPanelData *pPanelData);
};

#endif

