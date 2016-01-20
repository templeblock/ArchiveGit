/*
 @	PICZOOM:  Test program for Zoomer (PIC_PARM interface)
 @
 @	Revision History:
 @	Version 1.00 created 31 August 1996 Els Withers
 @	Version 1.10 created 15 September 1996 Els Withers
 @		Added support for halftoning
 @		Modified interface to ZOOM opcode
 @
 */

#pragma pack(1)

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <ctype.h>
#include    <string.h>
#include    <fcntl.h>
#include    <dos.h>
#include    <assert.h>
#include    <conio.h>
#include    <io.h>
#include    <conio.h>
#include    <sys\types.h>
#include    <sys\stat.h>
#include	<errno.h>
#include	<stddef.h>

#define YIELD_TEST 1    // 1/0  => Yield/no yield to other window tasks
#ifdef  WINDOWS         // Needed for Windows
		int printf(const char *f, ...) { return 0; }
		int vprintf(const char *f, va_list v) { return 0; }
		int getch(void) { return 'C'; }
		int fflush(FILE *f) { return 0; }
		#define EXPORT  __export

#else
		#define EXPORT
#endif

#define	PUT_BUFF_SIZE	(32768+1)
#define	GET_BUFF_SIZE	8192

#include    <stdtypes.h>
#include    <targa.h>
#include    <bmp.h>
#include    <errors.h>
#include	<pic.h>


/*  Globals for main */

PRIVATE PIC_PARM  Pp;
PRIVATE INT     SrcFile = -1, DesFile = -1;
PRIVATE char    SrcName[100];
PRIVATE char    DesName[100];
PRIVATE char	Comment[257];
PRIVATE BOOL    SkipCopyright;
PRIVATE INT		OptWidth,OptHeight,OptBitCount;
PRIVATE INT		OptMode;
PRIVATE BOOL	OptConvertGray;
PRIVATE DWORD	InFormat,OutFormat;	/* Formats used for input/output files */
PRIVATE INT		InBytesPerRow,InPad,InX;
PRIVATE INT		OutBytesPerRow,OutPad,OutX;

//  Local procedures
PRIVATE long    Zoom (char *SrcName, char *DesName, int Mode,
 INT NewWidth, INT NewHeight, INT NewBitCount, BOOL ConvertGray);
PRIVATE DWORD   DetermineFileType (INT SrcFile);
PRIVATE void    CopyFromFileToQueue (QUEUE *Q);
PRIVATE void    CopyToFileFromQueue (QUEUE *Q);
PRIVATE WORD    CRC (BYTE  *P);
PRIVATE WORD    ShowCopyright (BOOL Visible);
PRIVATE void    PrintHelp (void);
PRIVATE void    ParseParms (INT ArgC, char *ArgV[]);
PRIVATE void    Check (BOOL Exp, char *FMsg, ... );
PRIVATE void    BgrToRgbQuad (RGBTRIPLE *b,  RGBQUAD *q, DWORD n);
PRIVATE void    WriteOutputHeader (void);
PRIVATE void    ReadInputHeader (void);


/*
 @	Test the Pegasus routine interface.  (OP_ZOOM)
 */
#ifdef  WINDOWS

#define ArgC _argc
#define ArgV _argv
extern  INT  ArgC;
extern  char **ArgV;

INT     PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
						char *szCmdLine, INT nCmdShow)
#else

INT     main (INT ArgC, char *ArgV[])

#endif
	{
	if (ArgC < 3)
		{
		PrintHelp();   exit(1);
		}

	(void) printf
		(
		"\nPICZOOM - version 1.00, Test the Zoomer.\n"
		"(c) 1996, Pegasus Imaging Corporation, Tampa, Florida\n"
		);

	/*
	 @	Sets some entries in options structure, and command line arguments.
	 */
	ParseParms(ArgC, ArgV);

	if (!SkipCopyright)
		ShowCopyright(TRUE);

	Zoom(SrcName,DesName,OptMode,OptWidth,OptHeight,OptBitCount,
	 OptConvertGray);

	return 0;
	}


/*
 @	Zoom an image from one file to another.
 */
PRIVATE long    Zoom (char *SrcName, char *DesName, int Mode,
 INT NewWidth, INT NewHeight, INT NewBitCount, BOOL ConvertGray)
	{
	RESPONSE	Res;

	memset(&Pp, 0, sizeof(PIC_PARM));	/* important: 0 out default values */

	/*
	 @	Set up the uncompressed image characteristics.
	 */

	Pp.ParmSize = sizeof(PIC_PARM);
	Pp.ParmVer = CURRENT_PARMVER;   Pp.ParmVerMinor = 1;
	Pp.Op = OP_ZOOM;
	Pp.u.ZOOM.PicFlags = 0;
	Pp.u.ZOOM.PicFlags |= PF_YieldPut;
	if (ConvertGray)
		Pp.u.ZOOM.PicFlags |= PF_ConvertGray;
	Pp.u.ZOOM.NewWidth = NewWidth;
	Pp.u.ZOOM.NewHeight = NewHeight;
	Pp.u.ZOOM.NewBitCount = NewBitCount;
	Pp.u.ZOOM.Mode = Mode;

	/* Open the source file */
	SrcFile = open(SrcName, O_BINARY | O_RDONLY);
	Check(SrcFile != -1, "Could not open %s", SrcName);

	/*	See what kind of file it is */
	InFormat = DetermineFileType(SrcFile);
	Check(InFormat != -2, "Error reading %s", SrcName);
	Check(InFormat != -1, "Unrecognized file type");

	ReadInputHeader();

	/*
	 @	Given Pp information, initialize zoomer
	 */
	Res = Pegasus(&Pp, REQ_INIT);	/* Does not get or put data */
	while (Res != RES_DONE)
		{
		if (Res == RES_ERR)
			Check(FALSE, "Error number %d ",Pp.Status);
		else
			Check(FALSE, "Unexpected or unknown response %d ", Res);
		Res = Pegasus(&Pp, REQ_CONT);
		}
	Check(Pp.Status >= 0, "Error %d in Pegasus", Pp.Status);
	if (Pp.Status > 0)  printf("Status %d in Pegasus.\n", Pp.Status);

	/*
	 @	Setup input buff
	 */
	Check((BOOL)(Pp.Get.Start = (BYTE  *) malloc(GET_BUFF_SIZE)),
	 "Out of memory for Get queue");
	Pp.Get.End = Pp.Get.Start + GET_BUFF_SIZE;
	Pp.Get.Front = Pp.Get.Rear = Pp.Get.Start;

	/*
	 @	Create des file
	 */
	DesFile = open(DesName, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IWRITE);
	Check(DesFile != -1, "Can't create file %s", DesName);

	/*
	 @  Determine output format.
	 */
	if (InFormat==BI_RGB)
		OutFormat = InFormat;
	else
		{
		switch(Pp.u.ZOOM.BiOut.biBitCount)
			{
		case 8:
			OutFormat = BI_TGA1;
			break;
		case 24:
			OutFormat = BI_TGA2;
			break;
		default:
			fprintf(stderr,"Invalid output bitcount for TARGA format.\n");
			exit(1);
			}
		}

	/*
	 @	Make BMP header and Color table
	 */
	WriteOutputHeader();

	/*
	 @	As an example, we will allocate the output buffer dynamically during
	 @	EXEC. Seting Start to NULL will cause RES_PUT_NEED_SPACE response
	 @	Pp.Put.Start = NULL;
	 */

	/*
	 @	Call Pegasus to allocate space and output strips to the file
	 */
	Res = Pegasus(&Pp, REQ_EXEC);
	while (Res != RES_DONE)
		{
		if (Res == RES_GET_NEED_DATA  ||  Res == RES_GET_DATA_YIELD)
			CopyFromFileToQueue(&Pp.Get);
		else if (Res == RES_PUT_NEED_SPACE  &&  Pp.Put.Start == NULL)
			{
			/*
			 @ Allocate Put Buff.
			 */
			Pp.Put.Start = malloc(PUT_BUFF_SIZE);
			Check(Pp.Put.Start != NULL, "Out of memory for Put queue");
			Pp.Put.End = Pp.Put.Start + PUT_BUFF_SIZE;
			Pp.Put.Front = Pp.Put.Rear = Pp.Put.Start;
			}
		else if (Res == RES_PUT_NEED_SPACE || Res == RES_PUT_DATA_YIELD)
			CopyToFileFromQueue(&Pp.Put);
		else
			{
			Check(Res != RES_ERR, "Error number %d", Pp.Status);
			Check(FALSE,"Unknown Response  %d",Res);
			}
		Check(Pp.Status >= 0, "Error %d in Pegasus", Pp.Status);
		if (Pp.Status > 0)
			(void) printf("Status %d in Pegasus.\n", Pp.Status);
		Res = Pegasus(&Pp, REQ_CONT);
		}
	Check(Pp.Status >= 0, "Error %d in Pegasus", Pp.Status);
	if (Pp.Status > 0)  printf("Status %d in Pegasus.\n", Pp.Status);

	/*
	 @	Close the file - since Res == RES_DONE, we know the queue has
	 @	been flushed
	 */
	close(DesFile);


	/*
	 @	Display statistics
	 */
	Check(Pp.Status >= 0, "Error %d in Pegasus", Pp.Status);
	close(SrcFile);
	(void) printf
		(
		"\nZoomed %s (%ux%u) to %s (%ux%u)\n",
		SrcName, Pp.Head.biWidth, labs(Pp.Head.biHeight),
		DesName, Pp.u.ZOOM.BiOut.biWidth, labs(Pp.u.ZOOM.BiOut.biHeight)
		);
	/*
	 @	Terminate ZOOM
	 */
	Pegasus(&Pp, REQ_TERM);

	/*
	 @	Free memory
	 */
	if (Pp.Put.Start)
		{
		free(Pp.Put.Start);
		Pp.Put.Start = NULL;
		}
	if (Pp.Get.Start)
		{
		free(Pp.Get.Start);
		Pp.Get.Start = NULL;
		}
	return (Pp.Status);
	}
/*
 @	Determine the file type - only uncompressed bit-mapped BMP or
 */
PRIVATE DWORD   DetermineFileType (INT SrcFile)
	{
	union
		{
		BITMAPFILEHEADER    bhead;
		TARGA_HEAD          thead;
		}
	BmpTga;
	LONG   Type;

	if (read(SrcFile, &BmpTga.thead, sizeof(TARGA_HEAD)) == -1)
		return -2;
	if (BmpTga.bhead.bfType == 0x4D42)
		Type = BI_RGB;
	else if (BmpTga.thead.image_type == 1)
		Type = BI_TGA1;
	else if (BmpTga.thead.image_type == 2)
		Type = BI_TGA2;
	else if (BmpTga.thead.image_type == 3)
		Type = BI_TGA3;
	else
		Type = -1;
	lseek(SrcFile, 0, SEEK_SET);
	return (Type);
}


/*
 @	Convert RGBTRIPLE into RGBQUADS, possibly within the same buffer.
 */
PRIVATE void    BgrToRgbQuad (RGBTRIPLE *T,  RGBQUAD *Q, DWORD N)
	{
	T += N;
	Q += N;
	while (N--)
		{
		--T;
		--Q;
		/*
		 @	The order of assignment is crucial!
		 */
		Q->rgbReserved = 0;
		Q->rgbRed = T->rgbtRed;
		Q->rgbGreen = T->rgbtGreen;
		Q->rgbBlue = T->rgbtBlue;
		}
	}

PRIVATE void    WriteOutputHeader ()
	{
	BITMAPFILEHEADER BF;
	BITMAPINFOHEADER BI;
	TARGA_HEAD TH;
	RGBTRIPLE	RGB[256];
	int I;

	switch(OutFormat)
		{
	case BI_RGB:
		BI = Pp.u.ZOOM.BiOut;
		BI.biSize = sizeof(BITMAPINFOHEADER);
		BI.biCompression = BI_RGB;
		BI.biSizeImage = (((BI.biWidth*BI.biBitCount + 31) >> 3) & -4) *
		 BI.biHeight;
		BF.bfType = 0x4D42;       /* BMP file signature */
		BF.bfReserved1 = BF.bfReserved2 = 0;
		BF.bfOffBits = sizeof(BITMAPFILEHEADER) + BI.biSize + 4*BI.biClrUsed;
		BF.bfSize = BF.bfOffBits + BI.biSizeImage;
		Check(write(DesFile, &BF, sizeof(BITMAPFILEHEADER)) ==
		 sizeof(BITMAPFILEHEADER),"Error writing header for %s", DesName);
		Check(write(DesFile, &BI, sizeof(BITMAPINFOHEADER)) ==
		 sizeof(BITMAPINFOHEADER),"Error writing header for %s", DesName);
		Check(write(DesFile, Pp.ColorTable, BI.biClrUsed*4) == BI.biClrUsed*4,
		 "Error writing header for %s", DesName);
		OutBytesPerRow = ((BI.biWidth * BI.biBitCount>>3)+3)&0xFFFFFFFC;
		OutPad = 0;
		break;
	case BI_TGA1:/* Targa color-mapped */
	case BI_TGA2:/* Targa RGB */
		if (Pp.CommentLen)
			TH.id_size = Pp.CommentLen-1;
		else
			TH.id_size = 0;
		TH.cm_type = (OutFormat==BI_TGA1);
		switch(OutFormat)
			{
		case BI_TGA1:
			TH.image_type = 1;
			TH.cm_length = min(256,Pp.u.ZOOM.BiOut.biClrUsed);
			TH.cm_elt_size = 24;
			TH.ips = 8;
			break;
		case BI_TGA2:
			TH.image_type = 2;
			TH.cm_length = 0;
			TH.cm_elt_size = 0;
			TH.ips = 24;
			break;
			}
		TH.cm_org = 0;
		TH.x_org = 0;
		TH.y_org = 0;
		TH.width = Pp.u.ZOOM.BiOut.biWidth;
		TH.height = abs(Pp.u.ZOOM.BiOut.biHeight);
		if (Pp.u.ZOOM.BiOut.biHeight < 0)
			TH.idb = 32;
		else
			TH.idb = 0;
		Check(write(DesFile, &TH, sizeof(TARGA_HEAD)) ==
		 sizeof(TARGA_HEAD),"Error writing header for %s", DesName);
		Check(write(DesFile, Pp.Comment, TH.id_size) == TH.id_size,
		 "Error writing header for %s", DesName);
		if (TH.cm_type)
			{
			for (I=0; I<TH.cm_length; I++)
				{
				RGB[I].rgbtBlue = Pp.ColorTable[I].rgbBlue;
				RGB[I].rgbtGreen = Pp.ColorTable[I].rgbGreen;
				RGB[I].rgbtRed = Pp.ColorTable[I].rgbRed;
				}
			Check(write(DesFile, RGB, TH.cm_length*sizeof(RGBTRIPLE)) ==
			 TH.cm_length*sizeof(RGBTRIPLE),
			 "Error writing header for %s", DesName);
			}
		OutBytesPerRow = TH.width * TH.ips>>3;
		OutPad = (((TH.width * TH.ips>>3)+3)&0xFFFFFFFC) - OutBytesPerRow;
		}
	OutX = 0; /* Signals start-of-row position */
	}

PRIVATE void    ReadInputHeader ()
	{
	BITMAPFILEHEADER  BF;
	TARGA_HEAD THead;
	int I;
	/*
	 @	Read in file header
	 */
	switch (InFormat)
		{
	case BI_RGB:/* BMP RGB */
		Check(read(SrcFile, &BF, sizeof(BITMAPFILEHEADER)) ==
		 sizeof(BITMAPFILEHEADER)!= -1,"Error reading %s", SrcName);
		Check(read(SrcFile, &Pp.Head, sizeof(BITMAPINFOHEADER)) ==
		 sizeof(BITMAPINFOHEADER),"Error reading %s", SrcName);
		Check(Pp.Head.biCompression == BI_RGB,"Bad file type %s", SrcName);
		if (Pp.Head.biBitCount <= 8)
			{
			Check(lseek(SrcFile, sizeof(BITMAPFILEHEADER)+Pp.Head.biSize,
			 SEEK_SET) != -1,"Bad seek");
			if (Pp.Head.biClrUsed == 0)
				Pp.Head.biClrUsed = 1 << Pp.Head.biBitCount;
			Check(read(SrcFile, Pp.ColorTable, Pp.Head.biClrUsed << 2) != -1,
			 "Error reading color table from %s", SrcName);
			}
		Check(lseek(SrcFile, BF.bfOffBits, SEEK_SET) != -1, "Bad seek");
		InBytesPerRow = ((Pp.Head.biWidth * Pp.Head.biBitCount>>3)+3)
		 & 0xFFFFFFFC;
		InPad = 0;
		InX = 0; /* Signals start-of-row position */
		break;

	case BI_TGA1:/* Targa color-mapped */
	case BI_TGA2:/* Targa RGB */
	case BI_TGA3:/* Targa grayscale */
		Check(read(SrcFile, &THead, sizeof(TARGA_HEAD)) ==sizeof(TARGA_HEAD),
		 "Error reading %s", SrcName);
		if (InFormat == BI_TGA1)
			Check(THead.cm_type > 0  &&  THead.cm_org == 0  &&
			 THead.cm_elt_size == 24, "Bad color map for %s", SrcName);
		if (THead.id_size)
			{
			Pp.Comment = Comment;
			Check(read(SrcFile, Pp.Comment, THead.id_size) == THead.id_size,
			 "Error reading %s", SrcName);
			Pp.Comment[THead.id_size] = '\0';
			Pp.CommentLen = THead.id_size + 1;
			}
		if (THead.cm_type > 0)
			{
			Check(read(SrcFile, Pp.ColorTable, THead.cm_length*3) ==
			 THead.cm_length*3,"Error reading %s", SrcName);
			BgrToRgbQuad((RGBTRIPLE *) Pp.ColorTable, Pp.ColorTable,
			 THead.cm_length);
			Pp.Head.biClrUsed = Pp.Head.biClrImportant = THead.cm_length;
			}
		else if (InFormat == BI_TGA3)
			{  /* add a palette to make this a color-mapped image */
			for (I = 0;  I < 256;  I++)
				{
				Pp.ColorTable[I].rgbBlue  =
				Pp.ColorTable[I].rgbGreen =
				Pp.ColorTable[I].rgbRed   = I;
				}
			Pp.Head.biClrUsed = Pp.Head.biClrImportant = 256;
			}
		else
			Pp.Head.biClrUsed = Pp.Head.biClrImportant = 0;

		Pp.Head.biSize = sizeof(BITMAPINFOHEADER);
		Pp.Head.biWidth = THead.width;
		Pp.Head.biHeight = THead.height;
		if (THead.idb & 32)
			Pp.Head.biHeight = -THead.height;
		else
			Pp.Head.biHeight = THead.height;
		Pp.Head.biPlanes = 1;
		Pp.Head.biBitCount = 8;
		if (InFormat==BI_TGA2)
			Pp.Head.biBitCount = 24;
		Pp.Head.biCompression = InFormat;
		Pp.Head.biSizeImage = THead.width * THead.height;
		Pp.Head.biXPelsPerMeter = Pp.Head.biYPelsPerMeter = 0;

		InBytesPerRow = Pp.Head.biWidth * Pp.Head.biBitCount>>3;
		InPad = ((InBytesPerRow+3)&0xFFFFFFFC)-InBytesPerRow;
		InX = 0; /* Signals start-of-row position */
		break;

	default:
		Check(FALSE, "Bad image type");
		}
	/*
	 @	Now positioned so the next read gives pixel information
	 */
	}


/*
 @	Routine to move data from input file to Get queue; may need to
 @	pad the end of each image row.
 */
PRIVATE void   CopyFromFileToQueue (QUEUE *Q)
	{
	LONG    Bytes,Len,Space;

	/*
	 @	Rear will not equal End because we never leave it so.
	 */
	if (Q->QFlags & Q_EOF)
		return;
	while (Space = Q->Front>Q->Rear ? Q->Front-Q->Rear-1 :
	 (Q->Front>Q->Start ? Q->End-Q->Rear : Q->End-Q->Rear-1))
		{
		if (InX<0) /* Need to write padding */
			{
			Bytes = min(-InX,Space);
			memset(Q->Rear,0,Bytes);
			InX += Bytes;
			Q->Rear += Bytes;
			if (Q->Rear >= Q->End)
				Q->Rear = Q->Start;
			Space -= Bytes;
			}
		if (InX>=0) /* Get some real picture data */
			{
			Bytes = min(InBytesPerRow-InX,Space);
			Len = (unsigned)read(SrcFile,Q->Rear,Bytes);
			if (Len == (unsigned)-1)
				Q->QFlags |= Q_IO_ERR;
			InX += Bytes;
			if (InX >= InBytesPerRow)
				InX = -InPad;
			Q->Rear += Bytes;
			if (Q->Rear >= Q->End)
				Q->Rear = Q->Start;
			if (Len<Bytes)
				{
				Q->QFlags |= Q_EOF;
				return;
				}
			Space -= Bytes;
			}
		}
		printf("\r%3d%% done", Pp.PercentDone);
		fflush(stdout);

#if defined(WINDOWS) & YIELD_TEST
		{
		/*
		 @	Yield test
		 */
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE))
			{
			TranslateMessage((LPMSG) &msg);
			DispatchMessage((LPMSG) &msg);
			}
		}
#endif
	}


/*
 @	Routine to move data from Put queue to output file; may need to
 @	remove padding from the end of each image row.
 */

PRIVATE void   CopyToFileFromQueue (QUEUE *Q)
	{
	LONG    Bytes,Len,Data;

	/*
	 @	Q->Front will not equal Q->End because we never leave it so.
	 */
	while (Data = Q->Front<=Q->Rear ? Q->Rear-Q->Front : Q->End-Q->Front)
		{
		if (OutX<0) /* Need to skip padding */
			{
			Bytes = min(-OutX,Data);
			OutX += Bytes;
			Q->Front += Bytes;
			if (Q->Front >= Q->End)
				Q->Front = Q->Start;
			Data -= Bytes;
			}
		if (OutX>=0) /* Put some real picture data */
			{
			Bytes = min(OutBytesPerRow-OutX,Data);
			Len = (unsigned)write(DesFile,Q->Front,Bytes);
			if (Len!=Bytes)
				{
				Q->QFlags |= Q_IO_ERR;
				if (errno == EACCES)
					fprintf(stderr,"Write permission denied.\n");
				else if (errno == EBADF)
					fprintf(stderr,"Bad file number.\n");
				else
					fprintf(stderr,"Unknown write error code %d.\n",errno);
				exit(1);
				}
			OutX += Bytes;
			if (OutX >= OutBytesPerRow)
				OutX = -OutPad;
			Q->Front += Bytes;
			if (Q->Front >= Q->End)
				Q->Front = Q->Start;
			Data -= Bytes;
			}
		}
		printf("\r%3d%% done", Pp.PercentDone);
		fflush(stdout);

#if defined(WINDOWS) & YIELD_TEST
		{
		/*
		 @	Yield test
		 */
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE))
			{
			TranslateMessage((LPMSG) &msg);
			DispatchMessage((LPMSG) &msg);
			}
		}
#endif
	}

/*
 @	Compute the cycle redundancy Check of the given string
 */
PRIVATE WORD    CRC (BYTE  *P)
{
	DWORD   sum = 0;
	WORD    n = _fstrlen((char*)P);
	while (n--) {
		sum = (sum << 1) + (DWORD) 0x2941 * *P++ + (sum >> 16);
	}
	return ((WORD) sum);
}

/*
 @	Show the copyright information.
 */
PRIVATE WORD    ShowCopyright (BOOL Visible)
	{
	char    S1[] = "(c) 1995, Pegasus Imaging Corporation, Tampa, Florida";
	char    S2[] = "Bad Code";

	Check(CRC((BYTE*)S1) == (WORD) 58266, "%s", S2);
	if (Visible)
		{
		(void) printf("\nPICZOOM (ver 1.0)  Test the Zoomer.\n");
		(void) printf("%s\n\n", S1);
		(void) printf
			(
			"    (Voice) 813-875-7575  (Fax) 813-875-7705\n\n"
			"Program Version 1.10\n\n"
			);
		(void) printf("Press 'C' to continue..\n ");
		while ('C' != getch())
			;    /* Await user response */
		printf("\n");
		}
	return strlen(S1)+2;
	}

/*
 @	Print help message.
 */
PRIVATE void    PrintHelp (void)
	{
	(void) printf
		(
		"\nPICZOOM (ver 1.0)  Test the Zoomer.\n"
		"(c) 1996, Pegasus Imaging Corporation, Tampa, Florida\n"
		"usage:  PICZOOM input_file output_file {options}\n"
		"where input_file designates an image in BMP or TARGA format and options are:\n"
		"  -D<x> <y> scale to dimensions <x>,<y>\n"
		"  -M1 -D<x> <y> scale to fraction <x>/<y> of original\n"
		"              -D option MUST be used if -M1 is\n"
		"  -B<b> new bitcount is <b>\n"
		"  -G convert to grayscale\n"
		"\ndefault is equivalent to -M1 -D2 1.\n"
		);
	}

/*
 @	Parse the command line.
 */
PRIVATE void    ParseParms (INT ArgC, char *ArgV[])
	{
	INT     I;

	strcpy(SrcName,ArgV[1]);
	strcpy(DesName,ArgV[2]);

	SkipCopyright = FALSE;
	OptWidth = OptHeight = 0;
	OptMode = 0;
	OptConvertGray = FALSE;
	OptBitCount = 0;
	for (I = 3;  I < ArgC;  ++I)
		{
		if (ArgV[I][0] == '-'  ||  ArgV[I][0] == '/')
			{
			switch (toupper(ArgV[I][1]))
				{
			case 'M':
				if (ArgV[I][2])
					OptMode = atoi(ArgV[I]+2);
				else
					{
					Check(++I<ArgC,"Invalid use of -S option");
					OptMode = atoi(ArgV[I]);
					}
				break;
			case 'B':
				if (ArgV[I][2])
					OptBitCount = atoi(ArgV[I]+2);
				else
					{
					Check(++I<ArgC,"Invalid use of -S option");
					OptBitCount = atoi(ArgV[I]);
					}
				break;
			case 'D':
				if (ArgV[I][2])
					{
					Check(I+1<ArgC,"Invalid use of -D option");
					OptWidth = atoi(ArgV[I]+2);
					}
				else
					{
					Check(++I+1<ArgC,"Invalid use of -D option");
					OptWidth = atoi(ArgV[I]);
					}
				OptHeight = atoi(ArgV[++I]);
				break;
			case 'G':
				OptConvertGray = TRUE;
				break;
			case '!':
				SkipCopyright = TRUE;
				break;
			default:
				Check(FALSE,"Unknown option %s", ArgV[I]);
				break;
				}
			}
		}
	if (!(OptWidth || OptHeight))
		{
		OptWidth = 2;
		OptHeight = 1;
		OptMode = 1;
		}
	}

/*
 @	Check that exp is TRUE.  If it is not then print the formatted
 @	message fmsg, and terminate the program.
 */
PRIVATE void    Check (BOOL Exp, char *FMsg, ... )
	{
	va_list Marker;

	if (!Exp)
		{
		va_start(Marker, FMsg);
		(void)printf("Program terminated: ");
		vprintf(FMsg, Marker);
		printf(".\n");
		if (SrcFile != -1)
			close(SrcFile);
		if (DesFile != -1)
			close(DesFile);
		exit(1);
		}
	}
