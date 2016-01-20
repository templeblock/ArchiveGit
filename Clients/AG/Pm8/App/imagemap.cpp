/*
// $Workfile: imagemap.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/imagemap.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 8     1/07/99 6:09p Johno
// Changes to eliminate some h file dependecies
// 
// 7     4/23/98 3:25p Johno
// Removed some rectangle oriented code
// 
// 6     4/23/98 10:52a Johno
// Uses ShapeData class for polygon support
// 
// 5     4/03/98 4:36p Johno
// Added FullURL functions, changes to copy function(s)
// 
// 4     3/31/98 10:14a Johno
// Added Empty()
// 
// 3     3/25/98 6:35p Johno
// HyperLink and Hyperlink now all Hyperlink for sanity
// 
// 2     3/20/98 2:04p Johno
// New methods, moved inlines to CPP
// 
// 1     3/19/98 3:14p Johno
// There is something wrong with this SourceSafe thing...
// 
// 1     3/19/98 3:07p Johno
// 
*/
#include "stdafx.h"
#include "pmgobj.h"
#include "hyperrec.h"
#include "imagemap.h"
////////////////////////////////////////////////////////////////////////////////
CImageMapElement::CImageMapElement(HyperlinkData &hd, PMObjectShapeData &sd)
{
   mHyperlinkData.Copy(hd);
   sd.CopyTo(mShapeData);
}
////////////////////////////////////////////////////////////////////////////////
CImageMapElementArray::~CImageMapElementArray() 
{
   Empty();
}

CImageMapElement *
CImageMapElementArray::GetAt(int idx)
{
   if ((idx < 0) || (idx >= mMaps.GetSize()))
      return NULL;
   return (CImageMapElement*)mMaps.GetAt(idx);
}

void
CImageMapElementArray::Empty(void)
{
   for(int i = 0; i < mMaps.GetSize(); i ++)
   {
      CImageMapElement* p = GetAt(i);
      delete p;
   }
   mMaps.RemoveAll();
}

int
CImageMapElementArray::Add(CImageMapElement *pe)
{
   TRY
   {
      int idx = mMaps.Add(pe);
      return idx;
   }
   END_TRY
   
   return -1;
}
// Creates new CImageMapElements
int
CImageMapElementArray::Add(HyperlinkData &hd, PMObjectShapeData &sd, LPCSTR url /* NULL*/)
{
   TRY
   {
      CImageMapElement *ime = new CImageMapElement(hd, sd);
      if (url != NULL)
         ime->SetFullURL(url);
      return Add(ime);
   }
   END_TRY

   return -1;
}

void
CImageMapElementArray::Copy(CImageMapElementArray &ar)
{
   for(int i = 0; i < ar.GetSize(); i ++)
   {
      CImageMapElement* p = ar.GetAt(i);
      // This call will cause a new CImageMapElement to be created
      CString Temp;
      p->AppendFullURL(Temp);
      
      HyperlinkData hd = p->GetHyperlinkData();
      PMObjectShapeData ShapeData;
      p->GetShapeData(ShapeData);
      
      Add(hd, ShapeData, Temp);      
   }
}

