// maskedbm.cpp : implementation file
//

#include "stdafx.h"
#include "maskedbm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaskedBitmap

CMaskedBitmap::CMaskedBitmap()
{
	m_nBitmapWidth = 0;
	m_nBitmapHeight = 0;
	
	m_fBitmapsLoaded = FALSE;
}

CMaskedBitmap::~CMaskedBitmap()
{
	DeleteObject();
}

BOOL CMaskedBitmap::LoadBitmap(LPCSTR lpszResource, COLORREF crBackgroundColor, CDC *pDC)
{
	/*
	// Delete any current bitmaps.
	*/
	
	DeleteObject();
	
	/*
	// Attempt to load the resource.
	*/
	
	if (m_bmImage.LoadBitmap(lpszResource))
	{
		/*
		// Get the size of the image.
		*/
		
		BITMAP Bitmap;
		
		if (m_bmImage.GetObject(sizeof(Bitmap), &Bitmap) == sizeof (Bitmap))
		{
			m_nBitmapWidth = Bitmap.bmWidth;
			m_nBitmapHeight = Bitmap.bmHeight;
			
		   /*
   		// Create the mask bitmap.
		   */
		   
			if (m_bmMask.CreateBitmap(GetWidth(), GetHeight(), 1, 1, NULL))
		   {
		   	/*
		   	// Create a compatible memory DC and select the image bitmap into it.
		   	*/
		   	
		   	CDC dcImage;
		   	
		   	if (dcImage.CreateCompatibleDC(pDC))
		   	{
		   		CBitmap *pbmOldImage;
		   		
		   		if ((pbmOldImage = dcImage.SelectObject(GetImage())) != NULL)
		   		{
   					/*
   					// Create another compatible memory DC and select the
   					// monochrome mask bitmap into it.
   					*/
   					
   					CDC dcMask;
   					
   					if (dcMask.CreateCompatibleDC(pDC))
   					{
   						CBitmap *pbmOldMask;
   						
   						if ((pbmOldMask = dcMask.SelectObject(GetMask())) != NULL)
   						{
   							/*
   							// Create the mask by blitting the image onto the monochrome mask
   							// bitmap with the background set to the background color.
   							*/
		   							
	   						dcImage.SetBkColor(crBackgroundColor);
		   							
	   						if (dcMask.BitBlt(0, 0, GetWidth(), GetHeight(), &dcImage, 0, 0, SRCCOPY))
   							{
									/*
									// Now, change all pixels in the background color in the image bitmap
									// to black so that we can OR it into the destination bitmap.
									*/
											
									dcImage.SetTextColor(RGB(255,255,255));
									dcImage.SetBkColor(RGB(0,0,0));
											
									if (dcImage.BitBlt(0, 0, GetWidth(), GetHeight(), &dcMask, 0, 0, SRCAND))
									{
										/*
										// We have the final bitmaps.
										*/
												
										m_fBitmapsLoaded = TRUE;
									}
								}
							}
							
							dcMask.SelectObject(pbmOldMask);
						}
						
						dcImage.SelectObject(pbmOldImage);
					}
				}
			}
		}
	}
	
	return m_fBitmapsLoaded;
}

BOOL CMaskedBitmap::DeleteObject()
{
	m_bmImage.DeleteObject();
	m_bmMask.DeleteObject();
	m_nBitmapWidth = 0;
	m_nBitmapHeight = 0;
	m_fBitmapsLoaded = FALSE;
	
	return TRUE;
}
