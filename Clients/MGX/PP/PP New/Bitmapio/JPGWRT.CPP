#include "stdafx.h"
#include "mgxbm.h"
#include "dllmain.h"
#include "getline.h"
#include "l_bitmap.h"
#include "l_error.h"
#include "math.h"

extern HANDLE hLeadMutex;

L_INT WriteCB(BITMAPHANDLE* pBitmapHandle,                                       //will be called by L_CompressData to process
			 unsigned char L_FAR* pDestinationBuffer,                                //the contents of DestinationBuffer
			 int nCompressedBytes,
			 void L_FAR* pAdditionalData); 

#define JPEG_BUF_SIZE   65530                                                            //size of the DestinationBuffer
#define NUM_DATA_LINES  16                                                                       //number of lines that will be processed during each

BOOL CWriteBitmap::JPGWrite()
{                                                                                                                        
	BOOL bEscapable;
	RECT rSave;                                                                                                      //dimensions of image
	LPTR pSourceBuffer = NULL;                                                                       //buffer for raw data
	LPTR pSavedPointer = NULL;
	LPTR pDestinationBuffer = NULL;                                                          //buffer for compressed data
	UINT nBytesPerLine;                                                                                      //number of bytes required for one line of the image
	unsigned long nBytesInSourceBuffer;                                                      //the number of bytes in source buffer
	unsigned int nBytesInDestinationBuffer;                                          //the number of bytes allocated for the destination buffer
	BITMAPHANDLE BitmapHandle;                                                                       //lead bitmap handle
	int nOutputType;                                                                                         //the type of compression to use
	int nQFactor;                                                                                            //ammount of compression to use
	int npix;                                                                                                        //number of (horizontal) pixels
	int nlin;                                                                                                        //number of (vertical) pixels
	int in_depth;                                                                                            //color depth of input
	int out_depth;                                                                                           //color depth of output
	LPFRAME lpFrame = NULL;                                                                          //pointer to the frame
	LPOBJECT lpObject = NULL;                                                                        //pGointer to objects
	CFile *pTheFile;                                                                                                 //create a CFile object for output
	int nChunksToRead;                                                                                       //number of "chunks" to read per pass
	int i,j,k;                                                                                                       //for counting
	CFrameTypeConvert TypeConvert;                                                           //for converting input to appropriate output (RBG or grayscale)
	LPTR pLineInputBuffer = NULL;                                                            //buffer for line of input data
	LPTR pLineOutputBuffer = NULL;                                                           //buffer for line of output data
	int nLineNo;                                                                                             //keeps track of the line number
	HANDLE ofp;
	FRMTYPEINFO inType, outType;
	int nLinesLeft;
	int Lines = NUM_DATA_LINES;
	BOOL fRet = FALSE;
	LPJPEGOPTIONS lpOptions = (LPJPEGOPTIONS)m_lpOptions;

	ASSERT(lpOptions);

	ASSERT(m_pFile == NULL);
	if (m_pFile != NULL)
		return(FALSE);

	if (m_lpObjList)                                                                                         //if image contains objects, get the object edit frame
	{
		lpObject = m_lpObjList->lpHead;
		if (lpObject)
			lpFrame = ObjGetEditFrame(lpObject);
	}
	else                                                                                                             //if image has no objects, get the (standard) frame
	{
		lpFrame = m_lpFrame;
		lpObject = NULL;
	}

	ASSERT(lpFrame);                                                                                         //throw an assert of lpFrame is no good
	if (!lpFrame)
		return(FALSE);

	ProgressBegin(1);
	BitmapDllApp.BlockJPGWrite();

	bEscapable = !FileExists(m_lpFileName);

	if ((pTheFile = OpenFile()) == NULL)
	{
		ProgressEnd();
		BitmapDllApp.BlockJPGWrite();
		return(FALSE);
	}

	if(m_fSrcArea)                                                                                   //obtain dimensions of image
		rSave = m_rSrcArea;
	else
	{
		rSave.top = rSave.left = 0;
		rSave.bottom = FrameYSize(lpFrame) - 1;
		rSave.right = FrameXSize(lpFrame) - 1;
	}

	in_depth = FrameDepth(lpFrame);                                                          //obtain the depth of the image
	if (!in_depth)                                                                                           //if no depth is returned, assign the image a depth of 1
		in_depth = 1;
	inType = FrameTypeInfo(lpFrame);                                                         //obtain the frame data type

	if (inType.DataType == FDT_LINEART)                                                      //cause FramePointer never returns line art
		inType.DataType = FDT_GRAYSCALE;

	FrameSetTypeInfo(&outType, FDT_NONE);                                            //make sure colormap is initialized for outType so we don't
																//free a bogus ptr
	if (m_DataType == FDT_PALETTECOLOR  ||                                   //Set the output data type; JPEG supports only RGB and grayscale
		m_DataType == FDT_RGBCOLOR  ||
		m_DataType == FDT_CMYKCOLOR)
	{
		FrameSetTypeInfo(&outType, FDT_RGBCOLOR);
		out_depth = 3;
	}
	else
	{
		FrameSetTypeInfo(&outType, FDT_GRAYSCALE);
		out_depth = 1;
	}

	npix    = RectWidth(&rSave);                                                             //initialize npix
	nlin    = RectHeight(&rSave);                                                            //initialize nlin

	if(nlin%NUM_DATA_LINES==0)
		nChunksToRead = (int)(nlin/NUM_DATA_LINES);
	else
		nChunksToRead = (int)(nlin/NUM_DATA_LINES) + 1;

	nBytesPerLine = npix*out_depth;                                                          //calculate number of bytes each line of image will require 

	AllocLines(&pLineInputBuffer, 1, npix, in_depth);                        //allocate memory for line input buffer
	AllocLines(&pLineOutputBuffer, 1, npix, out_depth);                      //allocate memory for line output buffer

	if(!pLineInputBuffer || !pLineOutputBuffer)                                      //check for errors
	{
		SetError(BEC_errMemory);
		goto BadJPG;
	}

	nBytesInSourceBuffer = nBytesPerLine*NUM_DATA_LINES;
																//allocate memory for source buffer and check for errors
	if(!(pSavedPointer = pSourceBuffer = Alloc(nBytesInSourceBuffer)))       
	{
		SetError(BEC_errMemory);
		goto BadJPG;
	}

	nBytesInDestinationBuffer = JPEG_BUF_SIZE;                                      //allocate memory for destination buffer and check for errors
	if(!(pDestinationBuffer = Alloc(nBytesInDestinationBuffer)))
	{
		SetError(BEC_errMemory);
		goto BadJPG;
	}


	L_InitBitmap(&BitmapHandle, npix, nlin, out_depth*8);           //initialize the BitmapHandle structure
	BitmapHandle.Order = ORDER_RGB;

	if(out_depth == 3)                                                                                       //if image is RGB ....
	{
		BitmapHandle.Flags.GrayScale = 0;
	}
	else                                                                                                             //if image is grayscale...
	{
		BitmapHandle.Flags.GrayScale = 1;
	}

	if(!TypeConvert.Init(inType, outType, npix))                             //prepare for converting input type to output type
	{
		SetError(BEC_errMemory);
		goto BadJPG;
	}

	nOutputType = lpOptions->nSubsampling;
	nQFactor = lpOptions->nCompressionFactor;

	//convert nOutputType and nQFactor to values that compression engine will understand
	if(nOutputType==0)
		nOutputType = LEAD1JFIF;
	else if(nOutputType==1)
		nOutputType = LEAD2JFIF;
	else 
		nOutputType = JFIF;

	nQFactor = (int)(2.55*nQFactor -.55);

																//call L_StartCompressBuffer to initialize compression engine
	if (L_StartCompressBuffer(&BitmapHandle, WriteCB, nBytesInSourceBuffer,
						nBytesInDestinationBuffer, (char*)pDestinationBuffer,
						nOutputType, nQFactor, (L_VOID L_FAR *)pTheFile) != SUCCESS)
		goto BadWrite;

	nLineNo = 0;
	nLinesLeft = nlin;
	for(j=0; j< nChunksToRead; j++)
	{

		if(nLinesLeft < Lines)
			Lines = nLinesLeft;

		for(k = 0; k< Lines; k++)
		{               
			if(Progress(nLineNo - rSave.top, nlin, bEscapable))
			{
				L_EndCompressBuffer();                                                                           //free up lead buffers
				goto BadJPG;
			}

			if(lpObject)
			{
				if(!ObjGetLine(lpObject, rSave.left, nLineNo, (rSave.right - rSave.left) + 1, pLineInputBuffer)) 
				{
					L_EndCompressBuffer();                                                                           //free up lead buffers
					goto BadRead;
				}
			}
			else
			{
				if(!(pLineInputBuffer = FramePointer(lpFrame, rSave.left, nLineNo, NO)))
				{
					L_EndCompressBuffer();                                                                           //free up lead buffers
					goto BadRead;
				}
			}        
	 

			TypeConvert.ConvertData(pLineInputBuffer, pLineOutputBuffer, nLineNo, npix); 
			memcpy(pSourceBuffer, pLineOutputBuffer, nBytesPerLine);
			pSourceBuffer += nBytesPerLine;
			nLineNo++;
			nLinesLeft -= 1;

		}

		pSourceBuffer = pSavedPointer;  
		if (L_CompressBuffer(pSourceBuffer) != SUCCESS)
		{
			L_EndCompressBuffer();                                                                           //free up lead buffers
			goto BadWrite;
		}
	}

	L_EndCompressBuffer();                                                                           //free up lead buffers
	fRet = TRUE;
	goto Exit;

	BadRead:
		SetError(BEC_errFrameRead);
		goto Exit;

	BadWrite:
		SetError(BEC_errWriteFile);
			  
	Exit:                                                                                    
	BadJPG:                                                                                                          //Clean up and exit
	if(pSourceBuffer)
		FreeUp(pSourceBuffer);
	pSourceBuffer = NULL;

	if(pDestinationBuffer)
		FreeUp(pDestinationBuffer);
	pDestinationBuffer = NULL;

	if(pLineInputBuffer)
		FreeUp(pLineInputBuffer);
	pLineInputBuffer = NULL;

	if(pLineOutputBuffer)
		FreeUp(pLineOutputBuffer);
	pLineOutputBuffer = NULL;

	CloseFile(pTheFile, fRet);

	ProgressEnd();
	BitmapDllApp.BlockJPGWrite();
	return(fRet);
}

L_INT WriteCB(BITMAPHANDLE* pBitmapHandle, unsigned char L_FAR* pDestinationBuffer,                             
			 int nCompressedBytes, void L_FAR* pOutfile)
{
	L_INT iRet = SUCCESS;
	CFile* pFile = (CFile*) pOutfile;
	TRY
	{
		pFile->Write(pDestinationBuffer, nCompressedBytes);
	}
	CATCH_ALL(e)
	{
		iRet = FAILURE;
	}
	END_CATCH_ALL
	return(iRet);

}
	 
