//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "ppafx.h"
#include "ppimport.h"
#include "pp5io.h"

extern FNAME szExportProfile;

LOCAL void write_thumb(LPTSTR lpName, LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType, BITMAP_FILE_TYPE FileType);

LOCAL BOOL CanOpenFile(LPTSTR lpFileName, LPTSTR lpVolumeID = NULL);

LOCAL BOOL AddConvertCommand(LPLIST lpPacketList, FRMTYPEINFO OutType);

LOCAL BOOL SwapStorage(LPIMAGE lpImage, LPTSTR lpFileName);

LOCAL BOOL IsImportFile(LPTSTR lpFileName)
{
	if (lpFileName)
	{
		ITEMID idFileType = GetPPFileType(lpFileName);
    	return( idFileType >= IDN_FIRSTIMPORTFILETYPE && 
                idFileType <= IDN_LASTIMPORTFILETYPE );
	}
	else
		return(FALSE);
}
typedef struct _errmsg 
{
	int idMsg;
	BOOL fUseFileName;
} ERRMSG;

LOCAL void DisplayError(BITMAP_ERROR_CODE errCode, LPSTR lpFileName)
{
	int idMsg;
	BOOL fUseFileName;
	static ERRMSG errTable[] = {
		0, FALSE, /*BEC_errNone,*/
		IDS_EOPEN, TRUE, /*BEC_errOpen*/
		IDS_EMEMALLOC,FALSE, /*BEC_errMemory*/
		IDS_CMS_EFILERGBPTCMYK,FALSE, /*BEC_errFileRGBPTCMYK*/
		IDS_CMS_EFILECMYKPTRGB,FALSE, /*BEC_errFileCMYKPTRGB*/
		IDS_CMS_NODESTNOMON,FALSE, /*BEC_errPTNoMon*/
		IDS_ECOMPRESS,FALSE, /*BEC_errCompress*/
		IDS_EREAD,TRUE, /*BEC_errFileRead*/
		IDS_EFRAMEREAD,FALSE, /*BEC_errFrameRead*/
		IDS_COMPRESSFASTBITS,FALSE, /*BEC_errCompressFastBits*/
		IDS_EIMGBAD,TRUE, /*BEC_errBadTIFF*/
		IDS_EIMGVERSION,TRUE, /*BEC_errTIFFVersion*/
		IDS_EIMGCOMP,TRUE, /*BEC_errUnsupportedTIFFCompress*/
		IDS_EIMGPHOTO,TRUE, /*BEC_errUnsupportedTIFFPhoto*/
		IDS_EIMGFILL,TRUE, /*BEC_errUnsupportedTIFFFill*/
		IDS_EIMGORIENT,TRUE, /*BEC_errUnsupportedTIFFOrient*/
		IDS_ESAMPLES,TRUE, /*BEC_errTIFFSamples*/
		IDS_EIMAGEOPEN,FALSE, /*BEC_errFrameOpen*/
		IDS_EWRONGDATAOPEN,FALSE, /*BEC_errBadFormat*/
		IDS_EDISKSPACE,TRUE, /*BEC_errDiskSpace*/
		IDS_EWRITEPROTECT,TRUE, /*BEC_errWriteProtect*/
		IDS_EWRITE,TRUE, /*BEC_errWrite*/
		IDS_EWRITEFILE,FALSE, /*BEC_errWriteFile*/
		IDS_FILEPTMISMATCH,FALSE, /*BEC_errPTMismatch*/
		IDS_EGETTINGPT,FALSE, /*BEC_errGettingPT*/
		IDS_BADVERSION,FALSE, /*BEC_errBadVersion*/
		IDS_BADENCODING,FALSE, /*BEC_errBadEncoding*/
		IDS_BADBITSPERPIXEL,FALSE, /*BEC_errBadBitsPerPixel*/
		IDS_BADNUMCOLORIMAGES,FALSE, /*BEC_errBadNumColorImages*/
		IDS_INVALIDOPCODE,FALSE, /*BEC_errInvalidOpCode*/
		IDS_BADSIGNATURE,FALSE, /*BEC_errBadSignature*/
		IDS_NODATA,FALSE, /*BEC_errNoData*/
		IDS_BADCOLORMAPSIZE,FALSE, /*BEC_errBadColorMapSize*/
		IDS_BADINTERLEAVE,FALSE, /*BEC_errBadInterleave*/
		IDS_PCDERROR, FALSE /*BEC_errPCDToolkit*/};

	if (errCode >= 0 && errCode <= IDS_PCDERROR)
	{
		idMsg = errTable[errCode].idMsg;
		fUseFileName = errTable[errCode].fUseFileName;
		if (idMsg == IDS_EIMAGEOPEN)
			FrameError(idMsg);
		else
		if (fUseFileName)
		{
			if (lpFileName)
				Message(idMsg, lpFileName);
			else
				Message(idMsg, "");
		}
		else
			Message(idMsg);
	}
}
	
LOCAL BOOL CanOpenFile(LPTSTR lpFileName, LPTSTR lpVolumeID)
{
	// do a volume check for PhotoCD images cause there
	// names are not unique
	if (lpVolumeID && GetPPFileType(lpFileName) == IDN_PHOTOCD)
	{
		VOLID szVolumeID;

		GetVolumnID(lpFileName, szVolumeID);
		if (!StringsEqual(szVolumeID, lpVolumeID))
			return(FALSE);
	}
	return(FileExists(lpFileName));
//	FILE_HANDLE fh = FileOpen(lpFileName, FO_READ);
//	if (fh != FILE_HANDLE_INVALID)
//	{
//		FileClose(fh);
//		return(TRUE);
//	}
//	else
//	{
//		//DWORD dwError = GetLastError();
//		//Print("dwError = %ld", dwError);
//		return(FALSE);
//	}
}

LOCAL BITMAP_FILE_TYPE ConvertToBitmapFileType(ITEMID idFileType)
{
	
	if ( idFileType >= IDN_FIRSTIMPORTFILETYPE && 
         idFileType <= IDN_LASTIMPORTFILETYPE )
		 return(BFT_TIFF);

    // Force the extension based on the file type
    if ( idFileType == IDN_SCITEX )   return(BFT_SCITEX); else
    if ( idFileType == IDN_EPS )      return(BFT_EPS); else   
    if ( idFileType == IDN_TIFF )     return(BFT_TIFF); else
    if ( idFileType == IDN_BMP )      return(BFT_BMP); else
    if ( idFileType == IDN_DIB )      return(BFT_DIB); else
    if ( idFileType == IDN_GIF )      return(BFT_GIF); else
    if ( idFileType == IDN_PCX )      return(BFT_PCX); else
    if ( idFileType == IDN_TARGA )    return(BFT_TARGA); else
    if ( idFileType == IDN_JPEG )     return(BFT_JPEG); else
    if ( idFileType == IDN_AVI )      return(BFT_AVI); else
    if ( idFileType == IDN_PHOTOCD )  return(BFT_PHOTOCD); else
    if ( idFileType == IDN_DCS ) 	  return(BFT_DCS); else
    if ( idFileType == IDN_PP ) 	  return(BFT_PP); else
    if ( idFileType == IDN_PPCOMPOUND )return(BFT_PPCOMPOUND); else
    if ( idFileType == IDN_PP4 )      return(BFT_PP4); else
 		return(BFT_UNKNOWN);
}

LOCAL ITEMID ConvertToPPFileType(BITMAP_FILE_TYPE FileType)
{
    // Force the extension based on the file type
    if ( FileType == BFT_SCITEX )   return(IDN_SCITEX); else
    if ( FileType == BFT_EPS )      return(IDN_EPS); else   
    if ( FileType == BFT_TIFF )     return(IDN_TIFF); else
    if ( FileType == BFT_BMP )      return(IDN_BMP); else
    if ( FileType == BFT_DIB )      return(IDN_DIB); else
    if ( FileType == BFT_GIF )      return(IDN_GIF); else
    if ( FileType == BFT_PCX )      return(IDN_PCX); else
    if ( FileType == BFT_TARGA )    return(IDN_TARGA); else
    if ( FileType == BFT_JPEG )     return(IDN_JPEG); else
    if ( FileType == BFT_AVI )      return(IDN_AVI); else
    if ( FileType == BFT_PHOTOCD )  return(IDN_PHOTOCD); else
    if ( FileType == BFT_DCS ) 	  return(IDN_DCS); else
    if ( FileType == BFT_PP ) 	  return(IDN_PP); else
    if ( FileType == BFT_PPCOMPOUND )return(IDN_PPCOMPOUND); else
    if ( FileType == BFT_PP4 )      return(IDN_PP4); else
 		return(0);
}

LOCAL FRMDATATYPE ConvertToFrmDataType(ITEMID idDataType)
{
    if ( idDataType == IDC_SAVELA )   return(FDT_LINEART); else
    if ( idDataType == IDC_SAVESP )   return(FDT_LINEART); else
    if ( idDataType == IDC_SAVECT )   return(FDT_GRAYSCALE); else
    if ( idDataType == IDC_SAVE4BITCOLOR )   return(FDT_PALETTECOLOR); else
    if ( idDataType == IDC_SAVE8BITCOLOR )   return(FDT_PALETTECOLOR); else
    if ( idDataType == IDC_SAVE24BITCOLOR )   return(FDT_RGBCOLOR); else
    if ( idDataType == IDC_SAVE32BITCOLOR )   return(FDT_CMYKCOLOR); else
		return(FDT_NONE);
}

LOCAL ITEMID ConvertToPPDataType(FRMDATATYPE DataType)
{
    if ( DataType == FDT_LINEART )   return(IDC_SAVELA); else
    if ( DataType == FDT_GRAYSCALE )   return(IDC_SAVECT); else
    if ( DataType == FDT_PALETTECOLOR )   return(IDC_SAVE8BITCOLOR); else
    if ( DataType == FDT_RGBCOLOR )   return(IDC_SAVE24BITCOLOR); else
    if ( DataType == FDT_CMYKCOLOR )   return(IDC_SAVE32BITCOLOR); else
		return(0);
}

ITEMID GetSaveDataType(FRMTYPEINFO TypeInfo)
   {
   switch (TypeInfo.DataType)
      {
      case FDT_LINEART:
         return(IDC_SAVELA);
      case FDT_GRAYSCALE:
         return(IDC_SAVECT);
      case FDT_PALETTECOLOR:
         if (TypeInfo.ColorMap->NumEntries > 16)
            return(IDC_SAVE8BITCOLOR);
         else
            return(IDC_SAVE4BITCOLOR);
      case FDT_RGBCOLOR:
         return(IDC_SAVE24BITCOLOR);
      case FDT_CMYKCOLOR:
         return(IDC_SAVE32BITCOLOR);
      default:
         return(IDC_SAVECT);
      }
   }

LOCAL BOOL SwapStorage(LPIMAGE lpImage, LPTSTR lpFileName)
{
	if (lpImage->pServerDoc)
		return(lpImage->pServerDoc->SwapStorage(lpFileName));
	else
		return(FALSE);
}

// Constructor
CReadImage::CReadImage(LPTSTR lpFileName, ITEMID idFileType, LPTSTR lpVolumeID) :
	CReadBitmap(lpFileName, ConvertToBitmapFileType(idFileType))
{
	CommonConstruct(lpVolumeID);
}

// Constructor
CReadImage::CReadImage(LPTSTR lpFileName, ITEMID idFileType, LPCMSINFO pcmsInfo,
						LPTSTR lpVolumeID) :
	CReadBitmap(lpFileName, ConvertToBitmapFileType(idFileType), pcmsInfo)
{
	CommonConstruct(lpVolumeID);
}

// Constructor
CReadImage::CReadImage(CFile *pFile, ITEMID idFileType, BOOL bEscapable, 
									LPCMSINFO pCmsInfo) :
	CReadBitmap(pFile, ConvertToBitmapFileType(idFileType), bEscapable, pCmsInfo)
{
	CommonConstruct(NULL);
}

void CReadImage::CommonConstruct(LPTSTR lpVolumeID)
{
	m_lpVolumeID = lpVolumeID;

	m_fNoUndo = Control.NoUndo;
	m_CMSEnabled = Control.CMSEnabled;
	m_fOneBitMask = Mask.OneBitMask;
	m_bNoHiResRebuild = Control.bNoHiResRebuild;
	m_fLineArtAsGray = Control.LineArtAsGray;
	m_fTGACMYK = Control.TGACMYK;
	m_DitherType = Control.DefaultDitherType;
}

// Destructor
CReadImage::~CReadImage()
{
}

// function to read in a image
LPIMAGE CReadImage::ReadImage(LPCMDLIST lpCmdList, int DocumentType)
{
    LPIMAGE lpImage;
	OBJECTLIST ObjList;
	LPOBJECT lpBase = NULL;
	BOOL fUntitled = FALSE, fImport;
	int nObjects;
	LPCMDPKT lpCmdPkt;
	LIST PacketList;
	FRMTYPEINFO TypeInfo;

	// check to see if the file exists
	if (m_lpFileName)
	{
		// bring up browser to prompt user for location
		if (!PromptForFileName(m_lpFileName, m_lpVolumeID))
			return(NULL);
	}

	// do some special stuff if we have a command list
	if (lpCmdList)
	{
		// if in command list edit mode, we want the original
		// and we don't want a command list
		if (lpCmdList->fCmdListEdit)
		{
			SetReadCmdList(FALSE);
			m_fWantOriginal = TRUE;
			// see if we are trying to read from the file associated
			// with the document associated with the command list
			if (lpCmdList->pDocument && m_lpFileName)
			{
				FNAME szPathName;
				lstrcpy(szPathName, lpCmdList->pDocument->GetPathName());
				if (StringsEqual(szPathName, m_lpFileName))
				{
					BOOL bTempStg;
					LPSTORAGE lpStorage = lpCmdList->pDocument->GetRootCompoundStorage(&bTempStg);
					if (lpStorage && !bTempStg)
						SetStorage(lpStorage);
				}
			}
		}
		// set the photocd resolution override for this image
		SetPhotoCDResOverride(lpCmdList->PhotoCDResOverride);
	}

	// read int the object list
   	if (!ReadObjectList(&ObjList))
       	return(NULL);

	SetStorage(NULL);

	// find out how many objects
	nObjects = ObjCountObjects(&ObjList);

	// if we have more than one object and we are not
	// in object mode, combine the objects to the base
  	// mark as untitled if we combine
	if (!Control.MultipleObjects && (nObjects > 1))
	{
		lpBase = CombineToBase(&ObjList);
		fUntitled = TRUE;
	}
	else
		lpBase = (LPOBJECT)ObjList.lpHead;

	// error, get out
	if (!lpBase)
       	return(NULL);

	if (m_lpFileName)
	{
		// Open untitled if already open
    	if (AlreadyOpen(m_lpFileName) || fUntitled)
        	m_lpFileName = NULL;
	}

	// if we read in a proxy throw us into low-res mode
	lpCmdPkt = (LPCMDPKT)ListGetHead(&m_PacketList);
	if (lpCmdPkt && lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD)
		DocumentType = IMG_LOWRES;

	FrameGetTypeInfo(ObjGetEditFrame(lpBase), &TypeInfo);
	ITEMID idDataType = ConvertToPPDataType(TypeInfo.DataType);

	fImport = IsImportFile(m_lpFileName);
    // Setup the new image
    lpImage = CreateImage(
		fImport ? NULL : m_lpFileName, // Filename
        NULL,                   // lpNewFrame
        lpBase, 			    // new base object
        lpCmdList,              // command list
        fImport ? Control.DefaultFileType : GetFileType(), // lpImage->FileType
        idDataType,             // lpImage->DataType
        DocumentType,           // lpImage->DocumentType
        NULL);                  // lpImage->ImageName

	if (lpCmdList != NULL)
	{
		GetPacketList(&PacketList);
		while (lpCmdPkt = (LPCMDPKT)ListGetTail(&PacketList))
		{
			ListUnlink(&PacketList, lpCmdPkt);
			ListAddHead(&lpCmdList->LoadPacketList, lpCmdPkt);
		}
	}

    return(lpImage);
}

// function to read in a frame
BOOL CReadImage::ReadObjectList(LPOBJECTLIST lpObjList)
{
	if (IsImportFile(m_lpFileName))
		return(ReadObjectList(lpObjList, m_cmsInfo.dst));
	else
	{
		if (!CReadBitmap::ReadObjectList(lpObjList))
			return(FALSE);

    	if (( m_FileType == BFT_PP ) || ( m_FileType == BFT_PPCOMPOUND))
			HandleImageBuild(lpObjList);
		return(lpObjList->lpHead != NULL);
	}
}

// function to read in an object list - all calls boil down to this one
BOOL CReadImage::ReadObjectList(LPOBJECTLIST lpObjList, FRMTYPEINFO TypeInfo)
{
	if (IsImportFile(m_lpFileName))
	{
		lpObjList->lpHead = lpObjList->lpTail = NULL;
		LPFRAME lpFrame = ReadFrame(TypeInfo);
		if (lpFrame)
		{
			LPOBJECT lpObject = CreateObjectFromFrame(lpFrame);
        	if (!lpObject)
			{
				SetError(BEC_errMemory);
            	FrameClose(lpFrame);
			}
			else
			{
				ObjAddTail(lpObjList, lpObject);
			}
		}
	}
	else
	{
		if (!CReadBitmap::ReadObjectList(lpObjList, TypeInfo))
			return(FALSE);

    	if (( m_FileType == BFT_PP ) || ( m_FileType == BFT_PPCOMPOUND))
			HandleImageBuild(lpObjList);
	}
	return(lpObjList->lpHead != NULL);
}

// function to read in a frame
LPOBJECT CReadImage::ReadObject()
{
	if (IsImportFile(m_lpFileName))
		return(ReadObject(m_cmsInfo.dst));
	else
	{
		OBJECTLIST ObjList;
		LPOBJECT lpBase;

		if (!ReadObjectList(&ObjList))
			return(NULL);
 		lpBase = CombineToBase(&ObjList);
		return(lpBase);
	}
}

// function to read in a frame
LPOBJECT CReadImage::ReadObject(FRMTYPEINFO TypeInfo)
{
	if (IsImportFile(m_lpFileName))
	{
		LPOBJECT lpObject = NULL;
		LPFRAME lpFrame = ReadFrame(TypeInfo);
		if (lpFrame)
		{
			LPOBJECT lpObject = CreateObjectFromFrame(lpFrame);
        	if (!lpObject)
			{
				SetError(BEC_errMemory);
            	FrameClose(lpFrame);
			}
		}
		return(lpObject);
	}
	else
	{
		OBJECTLIST ObjList;
		LPOBJECT lpBase;
	 
		if (!ReadObjectList(&ObjList, TypeInfo))
			return(NULL);
		lpBase = CombineToBase(&ObjList);
		return(lpBase);
	}
}

// function to read in a frame
LPFRAME CReadImage::ReadFrame()
{
	if (IsImportFile(m_lpFileName))
		return(ReadFrame(m_cmsInfo.dst));
	else
	{
		LPFRAME lpFrame;

		// file types that support command list must always funnel
 		// through ReadObject() and ReadObjectList() so that 
		// hi-res regeneration can occur
		if (SupportsCommandList())
		{
			lpFrame = NULL;
			LPOBJECT lpObject = ReadObject();
			if (lpObject)
			{
				lpFrame = ObjGetEditFrame(lpObject);
				lpObject->Pixmap.EditFrame = NULL;
				ObjFreeUp(lpObject);
			}
		}
		else
		{
			lpFrame = CReadBitmap::ReadFrame();
		}
    	return(lpFrame);
	} 
}

// function to read in a frame
LPFRAME CReadImage::ReadFrame(FRMTYPEINFO TypeInfo)
{
	if (IsImportFile(m_lpFileName))
	{
		if (!ImportRead())
			return(NULL);
		else
			return(m_lpFrame);
	}
	else
	{
		LPFRAME lpFrame;

		// file types that support command list must always funnel
 		// through ReadObject() and ReadObjectList() so that 
		// hi-res regeneration can occur
		if (SupportsCommandList())
		{
			lpFrame = NULL;
			LPOBJECT lpObject = ReadObject(TypeInfo);
			if (lpObject)
			{
				lpFrame = ObjGetEditFrame(lpObject);
				lpObject->Pixmap.EditFrame = NULL;
				ObjFreeUp(lpObject);
			}
		}
		else
		{
			lpFrame = CReadBitmap::ReadFrame(TypeInfo);
		}
    	return(lpFrame);
	}
}

FRMDATATYPE CReadImage::GetFrmDataType()
{
	return CReadBitmap::GetDataType();
}

ITEMID CReadImage::GetDataType()
{
	return ConvertToPPDataType(CReadBitmap::GetDataType());
}

BITMAP_FILE_TYPE CReadImage::GetBitmapFileType()
{
	return CReadBitmap::GetFileType();
}

ITEMID CReadImage::GetFileType()
{
	return ConvertToPPFileType(CReadBitmap::GetFileType());
}

BOOL CReadImage::PromptForOptions(LPPVOID lppOptions)
{
	BOOL bRet = TRUE;
	LIST PacketList;
	LPCMDPKT lpCmdPkt;

	*lppOptions = NULL;
	switch (m_FileType)
	{
		case BFT_PHOTOCD:
			*lppOptions = GetPCDOptions();
			bRet = *lppOptions != NULL;
		break;

		case BFT_PP:
		case BFT_PPCOMPOUND:
			PPFFOpenOptions.PhotoCDResOverride = 0;
			if (!ReadPacketList(TRUE))
			{
				bRet = FALSE;
				break;
			}
			GetPacketList(&PacketList);
			lpCmdPkt = (LPCMDPKT)ListGetHead(&PacketList);
			if (lpCmdPkt)
			{
				int PhotoCDRes;
				if (lpCmdPkt->idCommand == IDS_CMD_LOADFILE && 
					GetLoadFileType(lpCmdPkt) == IDN_PHOTOCD)
				{
					LPLOADFILE_PARMS lpParms = (LPLOADFILE_PARMS)lpCmdPkt->lpParms;
					LPPCDOPTIONS lpOptions = (LPPCDOPTIONS)lpParms->lpOptions;
					LPTSTR lpFileName = GetLoadFileName(lpCmdPkt);
					LPTSTR lpVolumeID = GetLoadVolumeID(lpCmdPkt);
					if (!PromptForFileName(lpFileName, lpVolumeID))
						return(FALSE);
					PhotoCDRes = lpOptions->PhotoCDRes;
					if (bRet = PromptForResolution(lpFileName, lpOptions))
					{
						// setup resolution override
						PPFFOpenOptions.PhotoCDResOverride = lpOptions->PhotoCDRes-PhotoCDRes;
					}
				}
			}
			DestroyPacketList(&PacketList);
		default:
		break;
	}
    return(bRet);
}

// function to get the packet list read in
void CReadImage::GetPacketList(LPLIST lpPacketList)
{
	CReadBitmap::GetPacketList(lpPacketList);
  	FixExportFileNames(m_lpFileName, lpPacketList);
}

//***************************************************************************
BOOL CReadImage::HandleImageBuild(LPOBJECTLIST lpObjList)
 //***************************************************************************
{
	LPCMDPKT lpLoadPkt;
	FNAME szFileName;
	CMSINFO	CmsInfo, CmsInfo2;
	LPCMSINFO	lpCmsInfo;
	BOOL	fAddedCommand = FALSE;
	FRMTYPEINFO TypeInfo;

	// try to get the load packet from the command list
	// if we don't have a load command we cannot do 
	// any building
 	FixExportFileNames(m_lpFileName, &m_PacketList);

	lpLoadPkt = (LPCMDPKT)ListGetHead(&m_PacketList);
	if (!lpLoadPkt ||
		(GetCommandType(lpLoadPkt->idCommand) != CT_LOAD) )
		return(TRUE);

	lpCmsInfo = GetLoadCMSInfo(lpLoadPkt);

	// see if we read in any image data
	if (lpObjList->lpHead)
	{
		// we read data so, set the load packet to have the correct
		// cmsInfo
		// the pp5 reader always takes care of setting the 
		// load packet correctly so don't do it here
		//FrameGetTypeInfo(ObjGetEditFrame(lpObjList->lpHead), &TypeInfo);
		//if (IsDstPTSelected(&TypeInfo.ptInfo))
		//	lpCmsInfo->dst = TypeInfo;

		// if we read data that's not the original data
		// we can't do any building
		if (!m_fReadOriginal)
			return(TRUE);
	}
	else
	{
		LPTSTR lpFileName = GetLoadFileName(lpLoadPkt);
		LPTSTR lpVolumeID = GetLoadVolumeID(lpLoadPkt);
		if (!lpFileName)
			return(FALSE);

		lstrcpy(szFileName, lpFileName);
		//FixExportFileName(m_lpFileName, szFileName);
		// setup to read with the same options we
		// were invoked with
		if (!PromptForFileName(szFileName, lpVolumeID))
			return(FALSE);

		if( lpCmsInfo )
			{
			#ifdef UNUSED
			CmsInfo     = *lpCmsInfo;

			// Ted 8/12/94 - we can only use the passed in destination
			// if we don't have a command list
			if (ListGetCount(&m_PacketList) <= 1 &&
				IsDstPTSelected(&m_cmsInfo.dst.ptInfo))
				CmsInfo.dst = m_cmsInfo.dst;

			// if we don't have a dest, then wack the source
			// because it would not be right
			if (!IsDstPTSelected( &CmsInfo.dst.ptInfo ))
				FrameSetTypeInfo(&CmsInfo.src, FDT_NONE);
			else
			// if CMS is on, and we have a dest but we don't have
			// a source, then ask the user for a source
			if (Control.CMSEnabled &&
				IsDstPTSelected( &CmsInfo.dst.ptInfo) &&
				!IsSrcPTSelected( &CmsInfo.src.ptInfo ))
				{
				// FIX - re-entrancy problem - this call brings up
				// UI (which is badd!!!)
				CmsGetSrcPT(szFileName, &CmsInfo2);
				CmsInfo.src = CmsInfo2.src;
				SetLoadCMSInfo( lpLoadPkt, &CmsInfo );
				}
			#endif

			CmsInfo     = *lpCmsInfo;

			// Ted 8/12/94 - we can only use the passed in destination
			// if we don't have a command list
			if (ListGetCount(&m_PacketList) <= 1 &&
				IsDstPTSelected(&m_cmsInfo.dst.ptInfo))
				{
				CmsInfo.dst = m_cmsInfo.dst;
				// tedr 3/29/95 - the following 2 lines are new
				if (!IsSrcPTSelected(&CmsInfo.src.ptInfo))
					{
					CmsInfo.src = m_cmsInfo.src;
					SetLoadCMSInfo( lpLoadPkt, &CmsInfo );
					}
				}

			// if we don't have a dest, then wack the source
			// because it would not be right
			if (!IsDstPTSelected( &CmsInfo.dst.ptInfo ))
				FrameSetTypeInfo(&CmsInfo.src, FDT_NONE);
			}
		else
			cmsInfoInit( &CmsInfo );

		CReadImage read(szFileName, GetPPFileType(szFileName), &CmsInfo);
		if (m_SampleMode == modeSampleRect)
			read.SetSampling(&m_rArea);
		else
		if (m_SampleMode == modeSampleRes)
			read.SetSampling(m_SampleRes);
		read.SetOptions(GetLoadOptions(lpLoadPkt));
		read.SetPhotoCDResOverride(m_PhotoCDResOverride);

		if (!read.ReadObjectList(lpObjList))
			return(FALSE);
		m_iWidth = read.GetFileWidth();
		m_iHeight = read.GetFileHeight();
		m_iRes = read.GetFileResolution();
		read.GetCMSSrcTypeInfo(&m_cmsInfo.src);
	}

	// we should have a packet list, so do it to it
	if (ListGetCount(&m_PacketList) > 1)
	{
    	// Create an image so that we can do the building
    	LPIMAGE lpImage = CreateImage(
        	m_lpFileName,    // Name
        	NULL,                   // lpNewFrame
        	lpObjList->lpHead,	    // new base object
        	NULL,              // command list
        	GetFileType(),             // lpImage->FileType
        	GetDataType(),             // lpImage->DataType
        	IMG_DOCUMENT,           // lpImage->DocumentType
        	NULL);                  // lpImage->ImageName

		if (!lpImage)
		{
			LPOBJECT lpObject;
			while (lpObject = (LPOBJECT)ObjGetNextObject(lpObjList, NULL, YES))
				{
				ObjUnlinkObject(lpObjList, (LPPRIMOBJECT)lpObject);
				ObjFreeUp(lpObject);
				}
			lpObjList->lpHead = lpObjList->lpTail = NULL;
			return(FALSE);
		}

		lpImage->GetTypeInfo(&TypeInfo);
		lpObjList->lpHead = lpObjList->lpTail = NULL;

		// if we're not reading the native type, then add a convert
		// command to the packet list, strip it off later
		if (!m_fReadNativeType && m_cmsInfo.dst.DataType != FDT_NONE &&
			!FrameTypeInfoEqual(TypeInfo, m_cmsInfo.dst))
		{
			if (!AddConvertCommand(&m_PacketList, m_cmsInfo.dst))
			{
				DestroyImage(lpImage);
				lpObjList->lpHead = lpObjList->lpTail = NULL;
				return(FALSE);
			}
			fAddedCommand = TRUE;
		}
		// unlink the load packet, cause we just did the load
		ListUnlink(&m_PacketList, lpLoadPkt);

	 	if (BuildImage(lpImage, &m_PacketList, m_PhotoCDResOverride))
		{
			*lpObjList = lpImage->ObjList;
			lpImage->ObjList.lpHead = lpImage->ObjList.lpTail = NULL;
		}
		if (fAddedCommand)
		{
			LPCMDPKT lpConvertPkt = (LPCMDPKT)ListGetTail(&m_PacketList);
			if (lpConvertPkt && (lpConvertPkt->idCommand == IDS_CMD_CONVERT))
			{
				ListUnlink(&m_PacketList, lpConvertPkt);
				FreeUpPacket(lpConvertPkt);
			}
		}
		// add the load command back in cause we'll need it
		// for rebuilds
		ListAddHead(&m_PacketList, lpLoadPkt);
		// nuke the image
		DestroyImage(lpImage);
	}
	return(lpObjList->lpHead != NULL);
}

void CReadImage::SetError(BITMAP_ERROR_CODE errCode)
{
	::DisplayError(errCode, m_lpFileName);
}

/************************************************************************/
BOOL CReadImage::ReadCmdList(CFile *pFile, LPLIST lpPacketList, DWORD dwSize, BOOL fReadLoadPacketOnly)
/************************************************************************/
{
	MACRO_FILE_HANDLE fh;
	BOOL fRet = FALSE;
	LPTR lpBuf = NULL;
	FNAME szTempFile;
	FNAME    OEMName;
	size_t size;

	ListInit(lpPacketList);
	lstrcpy(szTempFile, Control.PouchPath);
	lstrcat(szTempFile, "PP");
	if (!GetTempFilename(szTempFile, ".TMP"))
		return(FALSE);

	AnsiToOem(szTempFile, OEMName);
	fh = fopen(OEMName, "w+b");
	if (fh == MACRO_FILE_HANDLE_INVALID)
		return(FALSE);

	size = 16384;
	if (size > dwSize)
		size = dwSize;
	lpBuf = Alloc(size);
	if (!lpBuf)
		goto Exit;

	TRY
	{
		while (dwSize)
		{
			if (size > dwSize)
				size = dwSize;
			if (pFile->Read(lpBuf, size) != size)
				goto Exit;
			if (fwrite(lpBuf, 1, size, fh) != size)
				goto Exit;
			dwSize -= size;
 		}
		fclose(fh);
		fh = MACRO_FILE_HANDLE_INVALID;
		fRet = ReadMacro(szTempFile, lpPacketList, fReadLoadPacketOnly);
	}
	END_TRY

	Exit:
	if (lpBuf)
		FreeUp(lpBuf);
	if (fh != MACRO_FILE_HANDLE_INVALID)
		fclose(fh);
	FileDelete(szTempFile);
	return(fRet);
}

void CReadImage::PP5CheckCmsInfo(	LPCMSINFO pCmsInfo,
									LPLIST lpPacketList,
									FRMTYPEINFO SrcTypeInfo,
									BOOL fWantOriginal,
									BOOL fReadNativeType)
{
	if (fWantOriginal) // command list editing?
	{
		LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(lpPacketList);
		if (lpLoadPkt)
		{
			LPCMSINFO lpLoadCmsInfo = GetLoadCMSInfo(lpLoadPkt);
			if (lpLoadCmsInfo)
			{
				if (ListGetCount(lpPacketList) > 1)
				{
					*pCmsInfo = *lpLoadCmsInfo;
				}
			}
		}
	}
	else
	// see if this PP5 has been opened with color management
 	if (IsDstPTSelected( &SrcTypeInfo.ptInfo ))
	{				   
		// if so, we always have the source
 		pCmsInfo->src = SrcTypeInfo;

		// if caller wants native data type then we want to so
		// whether we can convert to a new dest or not
		if (fReadNativeType)
		{
			// if we have a command list or
			// there is no destination selection, we must
			// use our source as the destination
			if ((ListGetCount(lpPacketList) > 1) || !IsDstPTSelected(&pCmsInfo->dst.ptInfo))
			{
				pCmsInfo->dst = SrcTypeInfo;
			}
			// if the command list for this file has only a load
			// then we should replace its CmsInfo with what we
			// are really going to use
			if (ListGetCount(lpPacketList) == 1)
			{
				LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(lpPacketList);
				if (lpLoadPkt)
					SetLoadCMSInfo(lpLoadPkt, pCmsInfo);
			}
		}
	}
	else
	// is someone try to color manage us
	if (IsDstPTSelected(&pCmsInfo->dst.ptInfo) && fReadNativeType)
	{
		// do we have a command list, cause if we do
		// we can never color manage it
		if (ListGetCount(lpPacketList) > 1)
		{
			cmsInfoInit(pCmsInfo);
		}

		// if the command list for this file has only a load
		// then we should replace its CmsInfo with what we
		// are really going to use
		if (ListGetCount(lpPacketList) == 1)
		{
			LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(lpPacketList);
			if (lpLoadPkt)
				SetLoadCMSInfo(lpLoadPkt, pCmsInfo);
		}
	}
}

BOOL CReadImage::DoPCDResOverride(LPLIST lpPacketList, int PhotoCDResOverride)
{
	LPPCDOPTIONS lpOptions = NULL;
	LPCMDPKT lpLoadPkt = (LPCMDPKT)ListGetHead(lpPacketList);
	if (lpLoadPkt && (GetLoadFileType(lpLoadPkt) == IDN_PHOTOCD))
	{
		lpOptions = (LPPCDOPTIONS)GetLoadOptions(lpLoadPkt);
		if (lpOptions)
			return(::DoPCDResOverride(lpOptions->PhotoCDRes, m_PhotoCDResOverride));
	}	
	return(FALSE);
}

LOCAL BOOL AddConvertCommand(LPLIST lpPacketList, FRMTYPEINFO OutType)
{
	CONVERT_PARMS   parms;
	LPCMDPKT lpCmdPkt;

	FrameSetTypeInfo(&parms.cmsInfo.src, FDT_NONE);
	parms.cmsInfo.dst = OutType;
	parms.DitherType = DT_NONE;
	parms.ConvertType = CT_STANDARD;
	parms.iLevels = 0;
	parms.szPalette[0] = '\0';

	lpCmdPkt = CreatePacket(IDS_CMD_CONVERT, &parms, TRUE);
	if (!lpCmdPkt)
		return(FALSE);
	ListAddTail(lpPacketList, lpCmdPkt);
	return(TRUE);
}

// Constructor
CWriteImage::CWriteImage( LPTSTR lpFileName, ITEMID idFileType ) :
	CWriteBitmap(lpFileName, ConvertToBitmapFileType(idFileType))
{
	m_bUpdateThumb = FALSE;
	m_DitherType = Control.DefaultDitherType;
}

// Constructor
CWriteImage::CWriteImage(CFile *pFile, ITEMID idFileType, BOOL bEscapable) :
	CWriteBitmap(pFile, ConvertToBitmapFileType(idFileType))
{
	m_bUpdateThumb = FALSE;
	m_DitherType = Control.DefaultDitherType;
}

// function to write out a frame
BOOL CWriteImage::WriteImage(LPIMAGE lpImage, ITEMID idDataType)
{
	FRMTYPEINFO TypeInfo;
	BOOL fFreeOptions = FALSE;
	BOOL fRet = FALSE;

	// tedr - why do we need to do this here?
	if (!m_lpOptions)
   	{
       	if (!GetSaveOptions(ConvertToPPFileType(m_FileType), &m_lpOptions))
       		return(FALSE);
		fFreeOptions = TRUE;
   	}

	// setup storage if we are writing from or to a compound file
    if (m_lpFileName &&
        (m_FileType == BFT_PPCOMPOUND || lpImage->FileType == IDN_PPCOMPOUND))
    {
        //Destination is a Compound File
        if (m_FileType == BFT_PPCOMPOUND)
        {
            // Source is NOT a Compound File
            SwapStorage(lpImage, m_lpFileName);
			SetStorage(lpImage->GetRootCompoundStorage());
        }
        else // Destination NOT PPF
        {
            // Source is a Compound
            if (lpImage->FileType == IDN_PPCOMPOUND)
                SwapStorage(lpImage,NULL);
        }
    }

	// set packet list to write
	SetPacketList(&lpImage->lpCmdList->UndoPacketList);

	// if we are writing to a storage write out the clipbits
	if ((GetStorage() != NULL) && (m_FileType == BFT_PPCOMPOUND))
	{
		UpdateClipBits(GetStorage(), &m_PacketList, SavingPP5CmdList(&m_PacketList, m_lpOptions), FALSE);
		UpdateMaskBits(GetStorage(), &m_PacketList, SavingPP5CmdList(&m_PacketList, m_lpOptions), FALSE);
	}

	// see if we need to write out an object list
	// 1. if we are writing a PP file or
	// 2. we have more than one object so the write must do a combine
    if (m_FileType == BFT_PP4 || m_FileType == BFT_PP  ||
        m_FileType == BFT_PPCOMPOUND || m_FileType == BFT_TIFF ||
		m_FileType == BFT_TARGA ||
		ImgCountObjects( lpImage ) > 1)
    {
		// see if we can just update the file
		// 1. We must be writing to a file
		// 2. It must be a PP or PP compound
		// 3. We must have be read from a PP or PP compound
		// 4. The file must exist
		// 5. It must be the same file we read from
		// 6. We must be saving the command list
		if (m_lpFileName &&  			// we are writing to a file
			((m_FileType == BFT_PP) || (m_FileType == BFT_PPCOMPOUND)) &&		// it is pp5 or >
			((lpImage->FileType == IDN_PP) || (lpImage->FileType == IDN_PPCOMPOUND)) &&		// it is pp5 or >
			FileExists(m_lpFileName) &&	// the file we are writing to exists
			StringsEqual(lpImage->CurFile, m_lpFileName) &&  // we loaded from the file we are writing to
			SavingPP5CmdList(&m_PacketList, m_lpOptions) ) // only do an update when we are writing out the command list
		{

			ImgGetTypeInfo(lpImage, &TypeInfo);
	        m_DataType = TypeInfo.DataType;
			m_lpObjList = &lpImage->ObjList;
			fRet = PP5Update();
		}
		else
		{
        	fRet = WriteObjectList(&lpImage->ObjList, idDataType);
		}
    }
    else
    {
        fRet = WriteFrame(ImgGetBaseEditFrame(lpImage), idDataType);
    }

 	if (fFreeOptions && m_lpOptions)
 	{
    	FreeUp(m_lpOptions);
		m_lpOptions = NULL;
 	}

	if (fRet)
	{
    	lpImage->WriteSummaryInfo();
		LPSTORAGE lpStorage = GetStorage();
    	if (lpStorage)
        	lpStorage->Commit(STGC_ONLYIFCURRENT);
		SetStorage(NULL);

		if (m_FileType != BFT_EPS && m_FileType != BFT_DCS && m_lpFileName &&
			lpImage->DocumentType != IMG_DOCPIECE)
		{
			lstrcpy(lpImage->CurFile, m_lpFileName);
			lpImage->FileType = ConvertToPPFileType(m_FileType);
		}

		if (m_bUpdateThumb)
			write_thumb(m_lpFileName, m_lpFrame, m_lpObjList, m_DataType, m_FileType);
	}
	else
	{
        if (!lpImage->fUntitled && !FileExists(lpImage->CurFile))
            lpImage->fUntitled = YES;
	}
    return( fRet );
}

// function to write out an object list
BOOL CWriteImage::WriteObjectList(LPOBJECTLIST lpObjList, ITEMID idDataType)
{
	BOOL fRet, fFreeOptions = FALSE;

   	if (!m_lpOptions)
   	{
       	if (!GetSaveOptions(ConvertToPPFileType(m_FileType), &m_lpOptions))
           	return(FALSE);
		fFreeOptions = TRUE;
   	}
	if (idDataType == IDC_SAVESP)
		SetDitherType(DT_SCATTER);
	else
	if (idDataType == IDC_SAVELA)
		SetDitherType(DT_NONE);
	fRet = CWriteBitmap::WriteObjectList(lpObjList, ConvertToFrmDataType(idDataType));
    if (fFreeOptions && m_lpOptions)
 	{
    	FreeUp(m_lpOptions);
		m_lpOptions = NULL;
 	}
	return(fRet);
}

// function to write out an object
BOOL CWriteImage::WriteObject(LPOBJECT lpObject, ITEMID idDataType)
{
	BOOL fRet, fFreeOptions = FALSE;

   	if (!m_lpOptions)
   	{
       	if (!GetSaveOptions(ConvertToPPFileType(m_FileType), &m_lpOptions))
           	return(FALSE);
		fFreeOptions = TRUE;
   	}
	if (idDataType == IDC_SAVESP)
		SetDitherType(DT_SCATTER);
	else
	if (idDataType == IDC_SAVELA)
		SetDitherType(DT_NONE);
	fRet = CWriteBitmap::WriteObject(lpObject, ConvertToFrmDataType(idDataType));
    if (fFreeOptions && m_lpOptions)
 	{
    	FreeUp(m_lpOptions);
		m_lpOptions = NULL;
 	}
	return(fRet);
}

// function to write out a frame or an object
BOOL CWriteImage::WriteFrame(LPFRAME lpFrame, ITEMID idDataType)
{
	BOOL fRet, fFreeOptions = FALSE;

   	if (!m_lpOptions)
   	{
       	if (!GetSaveOptions(ConvertToPPFileType(m_FileType), &m_lpOptions))
           	return(FALSE);
		fFreeOptions = TRUE;
   	}
	if (idDataType == IDC_SAVESP)
		SetDitherType(DT_SCATTER);
	else
	if (idDataType == IDC_SAVELA)
		SetDitherType(DT_NONE);
	fRet = CWriteBitmap::WriteFrame(lpFrame, ConvertToFrmDataType(idDataType));
    if (fFreeOptions && m_lpOptions)
 	{
    	FreeUp(m_lpOptions);
		m_lpOptions = NULL;
 	}
	return(fRet);
}

// function to set write specific options
BOOL CWriteImage::PromptForOptions()
{
    return(SetupSaveOptions(ConvertToPPFileType(m_FileType), NO));
}

// function to convert data type from FRMTYPEINFO to ITEMID
LOCAL void write_thumb(LPTSTR lpName, LPFRAME lpFrame, LPOBJECTLIST lpObjList, FRMDATATYPE DataType, BITMAP_FILE_TYPE FileType)
{
	RECT rArea;
	LPFRAME lpCombinedFrame;
	
	if (!lpFrame && !lpObjList)
		return;
	if (!lpFrame)
		 lpFrame = ObjGetEditFrame((LPOBJECT)lpObjList->lpHead);
	if (!lpFrame)
		return;
	
	lpCombinedFrame = lpFrame;
	
	if (lpObjList)
	{
		
		rArea.left = rArea.top = 0;
		rArea.right = 63;
		rArea.bottom = 63;
		lpCombinedFrame = ImgCopyFrame(NULL,lpObjList->lpHead,&rArea,NO,NO);
	}
	if (lpCombinedFrame)
		UpdateThumbnail(lpName,
			lpCombinedFrame,
			FrameXSize(lpFrame), 
			FrameYSize(lpFrame),
			ConvertToPPDataType(DataType),
			ConvertToPPFileType(FileType));
	if (lpCombinedFrame && lpCombinedFrame != lpFrame)
		FrameClose(lpCombinedFrame);
}

BOOL CWriteImage::WriteCmdList(CFile *pFile, LPLIST lpPacketList)
{
	MACRO_FILE_HANDLE fh;
	BOOL fRet = FALSE;
	LPTR lpBuf = NULL;
	DWORD dwSize;
	FNAME szTempFile;
	FNAME OEMName;
	LPCMDPKT lpCmdPkt;
	size_t size, read;

	ASSERT(ListGetHead(lpPacketList));

	lstrcpy(szTempFile, Control.PouchPath);
	lstrcat(szTempFile, "PP");
	if (!GetTempFilename(szTempFile, ".TMP"))
		return(FALSE);

	AnsiToOem(szTempFile, OEMName);
	fh = fopen(OEMName, "w+b");
	if (fh == MACRO_FILE_HANDLE_INVALID)
		return(FALSE);
	lpCmdPkt = (LPCMDPKT)ListGetHead(lpPacketList);
	if (!WritePacketList(lpCmdPkt, fh))
		goto Exit;
	if (fseek(fh, 0, SEEK_END))
		goto Exit;
	dwSize = ftell(fh);
	if (dwSize & 1)
		++dwSize;

	if (!WriteBlockInfo(pFile, dwSize, PPFF_CMDLIST_OPCODE))
		return(FALSE);

	if (fseek(fh, 0, SEEK_SET))
		goto Exit;
	size = 1024;
	lpBuf = Alloc(size);
	if (!lpBuf)
		goto Exit;

	TRY
	{
		while ((read = fread(lpBuf, 1, size, fh)) > 0)
		{
			pFile->Write(lpBuf, read);
			dwSize -= read;
		}
		if (dwSize > 0)
		{
			// should never need to write more than 1 byte
			if (dwSize > 1)
				goto Exit;
			*lpBuf = '\n';
			pFile->Write(lpBuf, 1);
		}
		fRet = TRUE;
	}
	END_TRY

	Exit:
	if (lpBuf)
		FreeUp(lpBuf);
	fclose(fh);
	FileDelete(szTempFile);
	return(fRet);
}

void CWriteImage::GetCommandInfo(LPLIST lpPacketList, LPVOID lpOptions, LPBOOL fLowRes, LPBOOL fFastBits, LPBOOL fPhotoCD, LPBOOL bSaveCmdList)
{
	LPCMDPKT lpCmdPkt = (LPCMDPKT)ListGetHead(&m_PacketList);
	if (lpCmdPkt)
	{
		*fLowRes = lpCmdPkt->idCommand == IDS_CMD_LOWRESLOAD;
		*fFastBits = lpCmdPkt->idCommand == IDS_CMD_FASTBITS;
		*fPhotoCD = GetLoadFileType(lpCmdPkt) == IDN_PHOTOCD;
	}
	else
	{
		*fLowRes = FALSE;
		*fFastBits = FALSE;
		*fPhotoCD = FALSE;
	}
	*bSaveCmdList = SavingPP5CmdList(lpPacketList, lpOptions);
}
 
void CWriteImage::SetError(BITMAP_ERROR_CODE errCode)
{
	::DisplayError(errCode, m_lpFileName);
}

//***************************************************************************
BOOL PromptForFileName(LPTSTR lpFileName, LPTSTR lpVolumeID)
//***************************************************************************
{
	if (CanOpenFile(lpFileName, lpVolumeID))
		return(TRUE);

	if (GetCurrentThreadId() != PictPubApp.GetMainThreadId())
	{
		return((BOOL)PictPubApp.m_pMainWnd->SendMessage(WM_PROMPTFORFILENAME, (WPARAM)lpFileName, (LPARAM)lpVolumeID));
	}
	else
	{
		OPENSTATE info;
		STRING szString;
		static FNAME szLastLocation;
		FNAME szFileName;
		int ret;
		BOOL fFirst = TRUE;

		if (szLastLocation[0])
		{
			lstrcpy(szFileName, szLastLocation);
			lstrcat(szFileName, filename(lpFileName));
			if (CanOpenFile(szFileName, lpVolumeID))
			{
				lstrcpy(lpFileName, szFileName);
				return(TRUE);
			}
		}

		while (TRUE)
		{
			if (fFirst)
			{
				lstrcpy(szFileName, lpFileName);
			}
			else
			{
				// Bring up the modal 'File Open' box
				clr( (LPTR)&info, sizeof(info) );
				if ( AstralStrEx(IDS_FINDFILE_TITLE, szString, sizeof(szString) ) )
					lstrcpy(info.szTitle, szString);
				if ( AstralStrEx(IDS_FINDFILE_BUTTON, szString, sizeof(szString) ) )
					lstrcpy(info.szOK, szString);

				info.bImported = FALSE;
				info.bSaving = FALSE;
				info.idFileType = GetPPFileType(lpFileName);
				info.szFileSpec[0] = info.szAlbum[0] = info.szPath[0] = '\0';
				info.wFileAttr = (WORD)-1;
				info.idDataType = IDC_SAVELA;
				lstrcpy(info.szFileSpec, filename(lpFileName));

				if ( !DoOpenDlg( MessageParent(), &info, TRUE ) )
					return(FALSE);

				lstrcpy(szFileName, info.lpFileList[0]);
				stripfile(szFileName);
				lstrcat(szFileName, filename(lpFileName));
				FreeUp(info.lpFileList);
			}

			ret = IDRETRY;
			while (ret == IDRETRY)
			{
				if (!fFirst && CanOpenFile(szFileName, lpVolumeID))
				{
					break;
				}
				else
				{
					fFirst = FALSE;
					if (GetPPFileType(lpFileName) == IDN_PHOTOCD)
					{
						// see if the user wants to look for the file
						ret = AstralYesNoRetry(MessageParent(),
											IDD_YESNORETRY, IDS_LOOKFORPCDFILE,
											(LPTSTR)lpVolumeID,
											(LPTSTR)filename(lpFileName));
					}
					else
					{
						// see if the user wants to look for the file
						ret = AstralYesNoRetry(MessageParent(),
											IDD_YESNORETRY, IDS_LOOKFORFILE,
											(LPTSTR)szFileName);
					}
					if (ret == IDNO)
						return(FALSE);
				}
			}

			if (ret == IDYES)
				continue;

			lstrcpy( lpFileName, szFileName );

			// save location of this file so we can try here next time
			lstrcpy(szLastLocation, lpFileName);
			stripfile(szLastLocation);
			break;
		}
		return(TRUE);
	}
}


