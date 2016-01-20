/*
// $Header: /PM8/App/CDIBDC.H 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CDIBDC.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:50   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:44:04   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:07:04   FRED
// Initial revision.
// 
//    Rev 1.1   21 Sep 1995 13:45:44   JAY
// Now uses CreateDIBSection in Win32
// 
//    Rev 1.0   06 Jun 1995 10:08:44   JAY
// Initial revision.
*/

/*
// A DC which is based on a DIB.
// A packed DIB is required.
*/

#ifndef __CDIBDC_H__
#define __CDIBDC_H__

class CDibDC : public CDC
{
public:
	CDibDC();
	virtual ~CDibDC();
	BOOL Create(LPBITMAPINFO pbi);
#if defined(WIN32) && !defined(_MAC)
   // CreateDIBSection not defined on MAC
	LPBYTE GetData(void);			// Call this after drawing!
	virtual BOOL DeleteDC(void);
protected:
	HBITMAP m_hDibBitmap;
	HBITMAP m_hOldBitmap;
	LPVOID m_pData;
#endif
};

#endif					// #ifndef __CDIBDC_H__
