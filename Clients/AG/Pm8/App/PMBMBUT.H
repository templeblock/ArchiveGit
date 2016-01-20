/////////////////////////////////////////////////////////////////////////////
// CPmwBitmapButton

#define  USE_GLOBAL_PALETTE   // This also controls wndhub.cpp

class CPmwBitmapButton : public CBitmapButton
{
   DECLARE_DYNAMIC(CPmwBitmapButton)
public:   
   CPmwBitmapButton();

   virtual ~CPmwBitmapButton();
   
   virtual void 
   DrawItem (LPDRAWITEMSTRUCT lpDIS);

   BOOL 
   LoadBitmaps (LPCTSTR lpszBitmapResource, LPCTSTR lpszBitmapResourceSel = NULL, LPCTSTR lpszBitmapResourceFocus = NULL, LPCTSTR lpszBitmapResourceDisabled = NULL);
  
   BOOL
   ConvertToTransparent (CBitmap *pBitmapBack, CRect &Rect);

protected:
   //{{AFX_MSG(CPmwBitmapButton)
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   BOOL
   ConvertBitmap (CBitmap *pBitmapBack, CRect &Rect, CBitmap *pBitmap);

#ifndef  USE_GLOBAL_PALETTE
   CPalette m_Palette;

   void
   KillPalette (void)
   {
      HPALETTE h = (HPALETTE)m_Palette.Detach ();
      if (h != NULL)
         DeleteObject (h);
   }
#endif
};
