#ifndef  IMAGEMAP_H
#define  IMAGEMAP_H
////////////////////////////////////////////////////////////////////////////////   
struct   HyperlinkData;
////////////////////////////////////////////////////////////////////////////////   
class CImageMapElement
{
public:
   // Construction
   CImageMapElement(HyperlinkData &hd, PMObjectShapeData &sd); 
   // Hyperlink stuff
   BOOL
   IsURL(void)
   {
      if (mHyperlinkData.StringType == TYPE_Invalid)
         return FALSE;
   
      if((mHyperlinkData.StringType != TYPE_PageURL) && (mHyperlinkData.FilePathOrURL.IsEmpty() == TRUE))
         return FALSE;
   
      return TRUE;      
   }

   HyperlinkData
   GetHyperlinkData(void)
   {
      return mHyperlinkData;
   }

   void
   AppendFullURL(CString &str)
   {
      str += mFullURL;
   }

   void 
   SetFullURL(LPCSTR str)
   {
      mFullURL = str;
   }
   // Position stuff
   void
   GetShapeData(PMObjectShapeData &od)
   {
      if (&od != NULL)
         mShapeData.CopyTo(od);   
   }

   void
   GetPoints(CArray<CPoint, CPoint&> &Shape)
   {
      mShapeData.GetPoints(Shape);
   }

   BOOL
   GetRectPosition(CRect &r) const
   {
      BOOL  ret = FALSE;
      if (&r != NULL)
         ret = mShapeData.GetRect(r);

      return ret;
   }

   void
   SetRectPosition(CRect r)
   {
      mShapeData.SetPoints(r);
   }

   void
   SetPoints(CArray<CPoint, CPoint&> &Shape)
   {
      mShapeData.SetPoints(Shape);
   }

   BOOL
   IsRect(void)
   {
      return mShapeData.IsRect();
   }

   BOOL
   IsPoly(void)
   {
      if (mShapeData.IsRect() == TRUE)
         return FALSE;
      
      if (mShapeData.NumPoints() < 3) // < 2 to support lines
         return FALSE;

      return TRUE;
   }
   // Misc. stuff
   void
   Copy(CImageMapElement ime)
   {
      mHyperlinkData = ime.GetHyperlinkData();
      GetShapeData (mShapeData);
      mFullURL = ime.mFullURL;
   }
protected:
   HyperlinkData              mHyperlinkData;
   PMObjectShapeData          mShapeData;
   CString                    mFullURL;
};
////////////////////////////////////////////////////////////////////////////////
class CImageMapElementArray
{
   public:
  
   CImageMapElementArray(){};
   virtual
   ~CImageMapElementArray();
   
   void
   Empty(void);

   int
   Add(CImageMapElement *pe);

   int
   Add(HyperlinkData &hd, PMObjectShapeData &sd, LPCSTR url = NULL);
   
   int
   GetSize(void)
   {
      return mMaps.GetSize(); 
   }

   CImageMapElement*
   GetAt(int idx);

   void
   Copy(CImageMapElementArray &ar);
   
   protected:
   CPtrArray mMaps;
};
////////////////////////////////////////////////////////////////////////////////   
#endif
