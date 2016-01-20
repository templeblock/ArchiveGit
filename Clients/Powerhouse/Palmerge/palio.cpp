#include "stdafx.h"
#include "mmsystem.h"
#include "palio.h"

// Load a palette from an open MMIO handle.
static int Load(HMMIO hmmio, LPRGBQUAD lpMap)
{
    // Check it's a RIFF PAL file.
    MMCKINFO ckFile;
    ckFile.fccType = mmioFOURCC('P','A','L',' ');
    if (mmioDescend(hmmio,
                    &ckFile,
                    NULL,
                    MMIO_FINDRIFF) != 0) {
        TRACE("Not a RIFF or PAL file");
        return 0;
    }
    // Find the 'data' chunk.
    MMCKINFO ckChunk;
    ckChunk.ckid = mmioFOURCC('d','a','t','a');
    if (mmioDescend(hmmio,
                    &ckChunk,
                    &ckFile,
                    MMIO_FINDCHUNK) != 0) {
        TRACE("No data chunk in file");
        return 0;
    }
    // Allocate some memory for the data chunk.
    int iSize = ckChunk.cksize;
    void* pdata = malloc(iSize);
    if (!pdata) {
        TRACE("No mem for data");
        return 0;
    }
    // Read the data chunk.
    if (mmioRead(hmmio,
                 (char*)pdata,
                 iSize) != iSize) {
        TRACE("Failed to read data chunk");
        free(pdata);
        return 0;
    }
    // The data chunk should be a LOGPALETTE structure
    // from which we can create a palette.
    LOGPALETTE* pLogPal = (LOGPALETTE*)pdata;
    if (pLogPal->palVersion != 0x300) {
        TRACE("Invalid version number");
        free(pdata);
        return 0;
    }
    // Get the number of entries.
    int nEntries = pLogPal->palNumEntries;
    if (nEntries <= 0) {
        TRACE("No colors in palette");
        free(pdata);
        return 0;
    }
	LPPALETTEENTRY lpEntry = pLogPal->palPalEntry;
	for (int i = 0; i < nEntries; ++i)
	{
		lpMap[i].rgbRed = lpEntry[i].peRed;
		lpMap[i].rgbGreen = lpEntry[i].peGreen;
		lpMap[i].rgbBlue = lpEntry[i].peBlue;
		lpMap[i].rgbReserved = 0;
	}
	return(nEntries);
}

// Load a palette from an open file handle.
static int Load(UINT hFile, LPRGBQUAD lpMap)
{
    HMMIO hmmio;
    MMIOINFO info;
    memset(&info, 0, sizeof(info));
    info.adwInfo[0] = hFile;
    hmmio = mmioOpen(NULL,
                     &info,
                     MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio) {
        TRACE("mmioOpen failed");
        return 0;
    }
    int nEntries = Load(hmmio, lpMap);
    mmioClose(hmmio, MMIO_FHOPEN);
    return nEntries;
}

// Load a palette from an open CFile object.
static int Load(CFile* fp, LPRGBQUAD lpMap)
{
    return Load(fp->m_hFile, lpMap);
}

// Load a palette from a named file.
int LoadPalette(LPSTR lpFileName, LPRGBQUAD lpMap)
{
    // Try to open the file for read access.
    CFile file;
    if (!file.Open(lpFileName,
                    CFile::modeRead | CFile::shareDenyWrite)) {
        AfxMessageBox("Failed to open file");
        return 0;
    }

    int nEntries = Load(&file, lpMap);
    file.Close();
    if (!nEntries) AfxMessageBox("Failed to load file");
    return nEntries;
}

// Save a palette to an open MMIO handle.
static BOOL Save(HMMIO hmmio, LPRGBQUAD lpRGBMap, int nEntries)
{
    // Create a RIFF chunk for a PAL file.
    MMCKINFO ckFile;
    ckFile.cksize = 0; // Corrected later.
    ckFile.fccType = mmioFOURCC('P','A','L',' ');
    if (mmioCreateChunk(hmmio,
                        &ckFile,
                        MMIO_CREATERIFF) != 0) {
        TRACE("Failed to create RIFF-PAL chunk");
        return FALSE;
    }
    // Create the LOGPALETTE data which will become
    // the data chunk.
    int iColors = nEntries;
    ASSERT(iColors > 0);
    int iSize = sizeof(LOGPALETTE)
                + (iColors-1) * sizeof(PALETTEENTRY);
    LOGPALETTE* plp = (LOGPALETTE*) malloc(iSize);
    ASSERT(plp);
	ZeroMemory(plp, iSize);
    plp->palVersion = 0x300;
    plp->palNumEntries = nEntries;
	LPPALETTEENTRY lpEntry = plp->palPalEntry;
	for (int i = 0; i < nEntries; ++i)
	{
		lpEntry[i].peRed = lpRGBMap[i].rgbRed;
		lpEntry[i].peGreen = lpRGBMap[i].rgbGreen;
		lpEntry[i].peBlue = lpRGBMap[i].rgbBlue;
	}
    // Create the data chunk.
    MMCKINFO ckData;
    ckData.cksize = iSize; 
    ckData.ckid = mmioFOURCC('d','a','t','a');
    if (mmioCreateChunk(hmmio,
                        &ckData,
                        0) != 0) {
        TRACE("Failed to create data chunk");
        return FALSE;
    }
    // Write the data chunk.
    if (mmioWrite(hmmio,
                 (char*)plp,
                 iSize) != iSize) {
        TRACE("Failed to write data chunk");
        free(plp);
        return FALSE;
    }
    free(plp);
    // Ascend from the data chunk which will correct the length.
    mmioAscend(hmmio, &ckData, 0);
    // Ascend from the RIFF/PAL chunk.
    mmioAscend(hmmio, &ckFile, 0);

    return TRUE;
}

// Save a palette to an open file handle.
BOOL SavePalette(LPSTR lpFileName, LPRGBQUAD lpRGBMap, int nEntries)
{
	CFile file;

	if (!file.Open(lpFileName, CFile::modeCreate|CFile::modeReadWrite))
		return(FALSE);

    HMMIO hmmio;
    MMIOINFO info;
    memset(&info, 0, sizeof(info));
    info.adwInfo[0] = file.m_hFile;
    hmmio = mmioOpen(NULL,
                     &info,
                     MMIO_WRITE | MMIO_CREATE | MMIO_ALLOCBUF);
    if (!hmmio) {
        TRACE("mmioOpen failed");
		file.Close();
		file.Remove(lpFileName);
        return FALSE;
    }
    BOOL bResult = Save(hmmio, lpRGBMap, nEntries);
    mmioClose(hmmio, MMIO_FHOPEN);
	file.Close();
	if (!bResult)
		file.Remove(lpFileName);
    return bResult;
}

