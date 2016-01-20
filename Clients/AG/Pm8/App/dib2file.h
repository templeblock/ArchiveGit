// $Workfile: dib2file.h $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/dib2file.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 15    1/28/99 6:19p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 14    9/23/98 2:40p Johno
// Stripped out cache to tinycash.*
// 
// 13    2/11/98 5:10p Hforman
// moved SavePMObjectsToFile() here
// 
// 12    1/27/98 6:47p Hforman
// add "pmo" in SaveGraphicType

#ifndef __DIB2FILE_H__
#define __DIB2FILE_H__

#include "stdafx.h"
#include "tinycash.h"
#include "pmwdoc.h"
// CStorageFileTinyCache
/////////////////////////////////////////////////////////////////////////////
//class CDibToGraphicFile : public StorageFileOnCFile

// global function for saving PM objects to a file (.pmo)
extern ERRORCODE SavePMObjectsToFile(const CString& filename, CPmwDoc* pDoc, BOOL fSelOnly);

enum SaveGraphicType
{
   None,
   Pmo,			// PrintMaster Objects
   Gif,
   Bmp8,
   Bmp24,
   Jpeg,
   WMeta,
   Tiff,
   Pcx,
   Png,
   Invalid
};

class CDibToGraphicFile : public CStorageFileTinyCache
{
public:
   CDibToGraphicFile (LPCSTR  FileName);

   BOOL
   CreateGraphicFile (CPmwDoc *pDoc, 
                        enum SaveGraphicType Type, 
                        CPoint Dims, 
                        BOOL Selected = FALSE,
                        HWND BreatheDlg = NULL,
                        long dpi = 0);
   
protected:

   void 
   Breathe (HWND BreatheDlg = NULL);

   void 
   DoBmp (CDIBInfo *pInfo);
   
   void 
   DoGif (CDIBInfo *pInfo);

   void 
   DoJpeg (CDIBInfo *pInfo);

   void 
   DoTiff (CDIBInfo *pInfo, long dpi);
   
   void
   DoPng (CDIBInfo *pInfo);
   
   void
   DoPcx (CDIBInfo *pInfo, long dpi);
   
   void
   DoWmf (PBOX World, CPoint Doms, CPtrList *pList, long dpi);

   void
   DrawIntoMetafile (CDC &dc, PBOX Bounds, CPtrList *pList);
};

#endif // __DIB2FILE_H__
