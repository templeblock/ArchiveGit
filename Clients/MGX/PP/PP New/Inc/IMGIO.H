//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _IMGIO_H_
#define _IMGIO_H_

#include "mgxbm.h"
#include "id.h"

ITEMID GetSaveDataType(FRMTYPEINFO TypeInfo);
BOOL DoPCDResOverride(int OldPhotoCDRes, int PhotoCDResOverride);

// read image class
class CReadImage : public CReadBitmap
{
public:
    // Constructors
    CReadImage(LPTSTR lpFileName, ITEMID idFileType = 0, LPTSTR lpVolumeID = NULL);
    CReadImage(LPTSTR lpFileName, ITEMID idFileType, LPCMSINFO pcmsInfo, LPTSTR lpVolumeID = NULL);
    CReadImage(CFile *pFile, ITEMID idFileType, BOOL bEscapable = FALSE, LPCMSINFO pCmsInfo = NULL);

    // Destructor
    ~CReadImage();                       

	virtual void ProgressBegin(int nPasses) { ::ProgressBegin(nPasses, PROGRESS_ID(IDS_UNDOLOADIMAGE));} 
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return ::AstralClockCursor(nPart, nWhole, fEscapable); }
	virtual void ProgressEnd() { ::ProgressEnd(); }
	// internal set error code
	virtual void SetError(BITMAP_ERROR_CODE errCode);

	// PP specific overridables
	virtual BOOL ReadCmdList(	CFile *pFile,
								LPLIST lpPacketList,
								DWORD dwSize,
								BOOL fReadLoadPacketOnly);
	virtual void DestroyPacketList(LPLIST lpPacketList) { ::DestroyPacketList(lpPacketList); }
	virtual void PP5CheckCmsInfo(	LPCMSINFO pCmsInfo,
									LPLIST lpPacketList,
									FRMTYPEINFO SrcTypeInfo,
									BOOL fWantOriginal,
									BOOL fReadNativeType);
 	virtual BOOL DoPCDResOverride(LPLIST lpPacketList, int PhotoCDResOverride);

    // read in an image
    LPIMAGE     ReadImage(LPCMDLIST lpCmdList = NULL, int DocumentType = IMG_DOCUMENT);

    // Read in an object list
    // lpFileType and lpDataType are for return values
    BOOL    ReadObjectList(LPOBJECTLIST lpObjList);
    // TypeInfo specifies the data type to create when reading
    BOOL    ReadObjectList(LPOBJECTLIST lpObjList, FRMTYPEINFO TypeInfo);

    // Read in an object
    // lpFileType and lpDataType are for return values
    LPOBJECT    ReadObject();
    // TypeInfo specifies the data type to create when reading
    LPOBJECT    ReadObject(FRMTYPEINFO TypeInfo);

    // Read in a frame
    // lpFileType and lpDataType are for return values
    LPFRAME     ReadFrame();
    // TypeInfo specifies the data type to create when reading
    LPFRAME     ReadFrame(FRMTYPEINFO TypeInfo);
    // Read a CMS frame
    //LPFRAME     ReadFrame(LPCMSINFO pCmsInfo);

    // capability functions
	BOOL SupportsCommandList() {return (m_FileType == BFT_PP || m_FileType == BFT_PPCOMPOUND);}

	// information functions for after a read
	ITEMID GetFileType();
	BITMAP_FILE_TYPE GetBitmapFileType();
	ITEMID GetDataType();
	FRMDATATYPE GetFrmDataType();

	// get packet list read in
	void GetPacketList(LPLIST PacketList);

    // prompt user for read options
    BOOL PromptForOptions(LPPVOID lppOptions);

private:
	void CommonConstruct(LPTSTR lpVolumeID);
	LPPCDOPTIONS GetPCDOptions();
	BOOL PromptForResolution(LPTSTR lpFileName, LPPCDOPTIONS lpOptions);
	LPOBJECT ReadObjectBlock(CFile *pFile, LPTR lpData, DWORD dwBytes,
								BOOL fBase, FRMTYPEINFO SrcTypeInfo);
	BOOL HandleImageBuild(LPOBJECTLIST lpObjList);

	BOOL ImportRead();

private:
	LPTSTR			m_lpVolumeID;			// in - volume id of file to read
};

// write image class
class CWriteImage : public CWriteBitmap
{
public:
    // Constructors
    CWriteImage(LPTSTR lpFileName, ITEMID idFileType = 0);
    CWriteImage(CFile *pFile, ITEMID idFileType, BOOL bEscapable = FALSE);

    // Destructor
    ~CWriteImage() {};                      

	// overridables
	virtual void ProgressBegin(int nPasses) { ::ProgressBegin(nPasses, PROGRESS_ID(IDS_UNDOSAVEIMAGE));} 
	virtual BOOL Progress(int nPart, int nWhole, BOOL fEscapable) { return ::AstralClockCursor(nPart, nWhole, fEscapable); }
	virtual void ProgressEnd() { ::ProgressEnd(); }
	virtual void SetError(BITMAP_ERROR_CODE errCode);

	// PP specific overridables
	virtual BOOL EPSWrite(BOOL fDoDCS);
	virtual BOOL WriteCmdList(CFile *pFile, LPLIST lpPacketList);
	virtual LONG GetXformDataSize( LONG lCrc, LPTSTR pMandM ) { return ::GetXformDataSize(lCrc, pMandM); }
	virtual BOOL GetXformData( LONG lCrc, LPTR pData ) { return ::GetXformData(lCrc, pData); }
	virtual void GetCommandInfo(LPLIST lpPacketList, LPVOID lpOptions, LPBOOL fLowRes, LPBOOL fFastBits, LPBOOL fPhotoCD, LPBOOL bSaveCmdList);

    // Write out an image
    BOOL    WriteImage(LPIMAGE lpImage, ITEMID idDataType = 0);
    // Write out an object list
    BOOL    WriteObjectList(LPOBJECTLIST lpObjList, ITEMID idDataType = 0);
    // Write out an object
    BOOL    WriteObject(LPOBJECT lpObject, ITEMID idDataType = 0);
    // Write out a frame
    BOOL    WriteFrame(LPFRAME lpFrame, ITEMID idDataType = 0);

	// Should we update/create a thumbnail... default false
	void SetUpdateThumb(BOOL bUpdateThumb){m_bUpdateThumb = bUpdateThumb;};

    // prompt user for save options
    BOOL PromptForOptions();

private:
	BOOL 			m_bUpdateThumb;	  // in - save a thumbnail also??
};

// prompt user to find file
BOOL PromptForFileName(LPTSTR lpFileName, LPTSTR lpVolumeID = NULL);

#endif // _IMGIO_H_

