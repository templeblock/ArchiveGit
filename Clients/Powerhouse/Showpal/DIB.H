// dib.h : header file
//
// CDIB class
//

#ifndef __DIB__
#define __DIB__

class CDIB : public CObject
{
    DECLARE_SERIAL(CDIB)
public:
    CDIB();
    ~CDIB();

    BITMAPINFO* GetBitmapInfoAddress()
        {return m_pBMI;}                        // Pointer to bitmap info
    BYTE* GetBitsAddress()
        {return m_pBits;}                       // Pointer to the bits
    RGBQUAD* GetClrTabAddress()
        {return (LPRGBQUAD)(((BYTE*)(m_pBMI)) 
            + sizeof(BITMAPINFOHEADER));}       // Pointer to color table
    int GetNumClrEntries();                     // Number of color table entries

    virtual BOOL Create(int width, int height); // Create a new DIB
    virtual BOOL Load(CFile* fp);               // Load from file
    virtual void Serialize(CArchive& ar);
    virtual void Draw(CDC* pDC, int x, int y);

protected:
    BITMAPINFO* m_pBMI;         // Pointer to BITMAPINFO struct
    BYTE* m_pBits;              // Pointer to the bits

private:
    int DibWidth()
        {return m_pBMI->bmiHeader.biWidth;}
    int DibHeight() 
        {return m_pBMI->bmiHeader.biHeight;}
};

#endif // __DIB__
