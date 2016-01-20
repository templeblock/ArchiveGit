
#ifndef __CDIB_H
#define __CDIB_H

class CPDib : public CObject {
protected:
	DECLARE_DYNAMIC(CPDib)
public:
	CPDib();
	~CPDib();
	CPDib &operator=(const CPDib &srcDib);

	int GetWidth() const;
	int GetHeight() const;
	int GetBitCount() const;
	BYTE *GetDIBData() const;
	BYTE *GetColorMap() const;
	void GetBitMapInfoHeader(BITMAPINFOHEADER &ptrBitMapHeader) const;
	void GetBitMapInfo(BITMAPINFO &ptrBitMapInfo) const;
	BOOL AllocateDIBandHeaderData(int header_size, int data_size);
	BYTE *ReleaseDIBData() const;
	BYTE *ReleaseHeaderData() const;
	BYTE *ReleaseDIBandHeaderData() const;
	void SetDIBHeader(BITMAPINFOHEADER *data_ptr, int data_len);
	void SetDIBData(BYTE *data_ptr, int data_len);
	int GetDIBHeaderSize() const;
	int GetDIBDataSize() const;
	WORD GetPaletteSize() const;
	WORD GetNumColors() const ;
	DWORD GetImageSize() const;

private:
	DeleteData();

//Data

private:
	BITMAPINFO			*m_ptrToDIBInfo;
	BITMAPINFOHEADER	*m_ptrToDIBHeader;
	BYTE					*m_ptrToData;

	int					m_nHeaderSize;
	int					m_nDataSize;

};

#endif // __CDIB_H

