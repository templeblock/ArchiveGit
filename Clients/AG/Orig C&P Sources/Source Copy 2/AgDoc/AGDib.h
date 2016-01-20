#ifndef __AGDIB_H_
#define __AGDIB_H_

inline RGBQUAD *DibColors(const BITMAPINFOHEADER *pbih)
{
	return ((RGBQUAD *)(((BYTE *) pbih) + pbih->biSize +
	 (pbih->biCompression == BI_BITFIELDS ? sizeof(DWORD) * 3 : 0)));
}

inline DWORD DibNumColors(const BITMAPINFOHEADER *pbih)
{
	return ((pbih->biClrUsed == 0 && pbih->biBitCount <= 8)
	  ?(1 << pbih->biBitCount) : pbih->biClrUsed);
}

inline DWORD DibPaletteSize(const BITMAPINFOHEADER *pbih)
{
	return (DibNumColors(pbih) * sizeof(RGBQUAD));
}

inline BYTE *DibPtr(const BITMAPINFOHEADER *pbih)
{
	return ((BYTE *)(DibColors(pbih) + DibNumColors(pbih)));
}

inline DWORD DibWidthBytes(const BITMAPINFOHEADER *pbih)
{
	return (((pbih->biWidth * pbih->biBitCount + 31L) / 32) * 4);
}

inline DWORD DibSizeImage(const BITMAPINFOHEADER *pbih)
{
	return (pbih->biSizeImage != 0 ? pbih->biSizeImage
	  : DibWidthBytes(pbih) * pbih->biHeight);
}

inline DWORD DibSize(const BITMAPINFOHEADER *pbih)
{
	return (pbih->biSize + DibSizeImage(pbih) + DibPaletteSize(pbih) +
	 (pbih->biCompression == BI_BITFIELDS ? sizeof(DWORD) * 3 : 0));
}

#endif //__AGDIB_H_
