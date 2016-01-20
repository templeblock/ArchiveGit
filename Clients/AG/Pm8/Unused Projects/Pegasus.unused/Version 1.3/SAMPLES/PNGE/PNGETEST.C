/*.T PNGETEST (ver 1.10)  Test the PNG Image Expand routine
 * $Header: /PEGASUS/PNGE/PNGENNW/PNGETEST.C 1     5/11/98 1:26p Jim $
 * $Nokeywords: $
 */

/***********************************************************************\
*                                                                       *
*    PNGETEST -  Test the Portable Network Graphics Expand routine      *
*                                                                       *
*   Revision History:                                                   *
*   Version 1.00  created                          5/ 6/98 Stephen Mann *
*   Version 1.10  support for all non-interlaced  10/ 7/98 Stephen Mann *
*                                                                       *
\***********************************************************************/

#pragma pack(1)

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <ctype.h>
#include    <string.h>
#include    <fcntl.h>
#include    <math.h>
#include    <dos.h>
#include    <assert.h>
#include    <io.h>
#include    <conio.h>
#include    <sys\types.h>
#include    <sys\stat.h>
#include    <time.h>

#define PNGE_OPCODE OP_PNGE

#define YIELD_TEST 0    /* 1/0  => Yield/no yield to other window tasks*/
#if defined(WINDOWS) && !defined(_CONSOLE)         /* Needed for Windows*/
        int printf(const char *f, ...) { return 0; }
        int vprintf(const char *f, va_list v) { return 0; }
        int getch(void) { return 'C'; }
        int fflush(FILE *f) { return 0; }
        #define EXPORT  __export
#else
        #define EXPORT
#endif

#define SRC_IS_C    1
#define CPUFreq     233     /* MHz */

#define GET_BUFF_SIZE   300000
#define PUT_BUFF_SIZE   1000000

#include    <stdtypes.h>
#include    <bmp.h>
#include    <errors.h>
#include    <pic.h>

#define Seconds(Time)   ((Time[0]+Time[1]*4294967296.0)/(CPUFreq*1.0E6))

/*  Globals for main*/

PRIVATE PIC_PARM  p;
PRIVATE FILE    *SrcFile = NULL, *DesFile = NULL;
PRIVATE char    SrcName[100], SrcPath[100], DesName[100], DesPath[100], BakName[100];
PRIVATE BOOL    WildCard, opt_raw, opt_bmp;

PRIVATE long    Expand (char *SrcName, char *DesName);
PRIVATE void    DisplayASCII (BYTE *q, DWORD Len);
PRIVATE void    CopyFromFileToQueue (void);
PRIVATE void    CopyToFileFromQueue (void);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (INT argc, char *argv[]);
PRIVATE void    check (BOOL exp, char *fmsg, ... );

#if SRC_IS_C
PRIVATE void    AccumTimeStampCounter (DWORD *Count);
PRIVATE void    ElapsedTimeStampCounter (DWORD *Count);
#else
extern  void    _cdecl AccumTimeStampCounter (DWORD *Count);
extern  void    _cdecl ElapsedTimeStampCounter (DWORD *Count);
#endif

/***********************************************************************\
*                                                                       *
*   Test the Pegasus routine interface.  (OP_LIP)                       *
*                                                                       *
\***********************************************************************/
#if defined(WINDOWS) && !defined(_CONSOLE)         /* Needed for Windows*/

  #define argc _argc
  #define argv _argv
  extern  INT  argc;
  extern  char **argv;

  INT     PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
                          char *szCmdLine, INT nCmdShow)
#else

  INT     main (INT argc, char *argv[])

#endif
{
    LONG    status;
    UINT    n;

    if (argc < 3) {
        print_help();   exit(1);
    }

    printf(
        "\nPNGETEST - version 1.10, Test the Portable Network Graphics Expand routine.\n"
        "(c) 1998, Pegasus Imaging Corporation, Tampa, Florida\n"
    );

    /*  sets some entries in options structure, and command line arguments.*/
    parse_parms(argc, argv);

#if defined(__WATCOMC__)
    {
        struct  find_t info;
        n = 0;
        if (_dos_findfirst(SrcName, _A_NORMAL, &info) == 0) {
            do {
                strcpy(SrcName, SrcPath);   strcat(SrcName, info.name);
                if (WildCard) {
                    strcpy(DesName, DesPath);   strcat(DesName, info.name);
                    strcpy(strchr(DesName, '.'), ".BMP");
                }
                status = Expand(SrcName, DesName);
                n++;
            } while (status == 0  &&  _dos_findnext(&info) == 0);
        }
    }
#else
    status = Expand(SrcName, DesName);
    n = 1;
#endif
    printf("\nThere were %d images Expanded.\n", n);
    return (0);
}


/***********************************************************************\
*                                                                       *
*                                                                       *
\***********************************************************************/
PRIVATE long  Expand (char *SrcName, char *DesName)
{
    REGION    Region;
    LONG      m, FileSize;
    RESPONSE  res;
    LONG      in_size, out_size, delta;
    DWORD     Time[2] = {0, 0};

    /*  Open the compressed source file*/
    SrcFile = fopen(SrcName, "rb");
    check(SrcFile != NULL, "Could not open %s", SrcName);

    /*  Important: set all default values 0 */
    memset(&p, 0, sizeof(PIC_PARM));
    p.ParmSize = sizeof(PIC_PARM);
    p.ParmVer = CURRENT_PARMVER;
    p.ParmVerMinor = 2;             /* important - needed for ver 3.0 and above */
    p.Op = PNGE_OPCODE;

    /*  Allocate Get Buff, need room for entire source file */
    FileSize = filelength(fileno(SrcFile));
    p.Get.Start = (BYTE *) malloc(FileSize);
    check(p.Get.Start != NULL, "Out of memory for Get buffer");
    p.Get.End = p.Get.Start + FileSize;
    p.Get.Front = p.Get.Rear = p.Get.Start;
    /* can choose to read into buffer now or when requested by init */

    /*  Reserve space for all parameters to be returned in PIC2List */
    p.PIC2List = malloc(1);
    check(p.PIC2List != NULL, "Out of memory");
    p.PIC2List[0] = 0;   p.PIC2ListLen = p.PIC2ListSize = 1;

    /*  Assign all expansion options */
    /* Will try to generate BMP or RAW images depending on the PNG format */
    /* provided.  opt_raw forces RAW format, opt_bmp forces BMP format,   */
    /* otherwise do BMP or RAW depending on the file characteristics. */
    if (opt_raw)  p.Flags |= F_Raw;
    if (opt_bmp)  p.Flags |= F_Bmp;

    /*  Initialize PNGE decompression, determine Region and StripSize*/
    AccumTimeStampCounter(Time);
    res = Pegasus(&p, REQ_INIT); /* does not put data*/
    while (res != RES_DONE) {
        if (res == RES_GET_NEED_DATA  ||  res == RES_GET_DATA_YIELD) {
            CopyFromFileToQueue();
        } else if (res == RES_EXTEND_PIC2LIST) {
            /* P->PacketType can be used to determine if the packet  */
            /* should be saved.  For this example, save all packets. */
            p.PIC2List = (char *) realloc(p.PIC2List, p.PIC2ListLen);
            check(p.PIC2List != NULL, "Out of memory for PIC2List");
            p.PIC2ListSize = p.PIC2ListLen;
        } else {
            check(res != RES_ERR, "Error number %d", p.Status);
            check(FALSE,"Unknown Response  %d",res);
        }
        res = Pegasus(&p, REQ_CONT);
    }
    check(p.Status == ERR_NONE, "Error %d in Pegasus", p.Status);
    ElapsedTimeStampCounter(Time);

    /*  The general image information is returned in p.u.PNG.Region.    */
    /*  For convienience most of this info is also returned in p.Head.  */
    /*  Now either F_Raw or F_Bmp has been set (but not both) */

    /*  Display any comments, watermarks, scripts, etc. */
    DisplayASCII(p.PIC2List, p.PIC2ListLen);
    p.PIC2ListLen = 0;   /* finished with the list for now */

    Region = p.u.PNG.Region;

    /*  If file is to be raw format then get region parameters from user */
    if (p.Flags & F_Raw) {
        int     Ans;
        int     Stride;
        printf("\nFile %s (%dx%d, %d-bit) is assumed raw:\n",
            DesName, Region.Width, Region.Height, Region.Bpp);
        printf("What is the image offset in bytes (was %d)? ", Region.Offset);
        scanf("%d", &Region.Offset);
        printf("What is the image stride in bytes (was %d)? ", Region.Stride);
        scanf("%d", &Stride);
        check(Stride >= Region.Stride, "Stride=%d is too small", Stride);
        Region.Stride = Stride;
#if 1
        if (Region.Bpp > 8) {
            printf("Byte-order is Motorola.\n");
        }
#else
        if (Region.Bpp > 8  &&  Region.Bpp <= 16  &&  Region.PixType != PT_RGB555) {
            printf("What is the byte-order I=Intel, M=Motorola? ");
            scanf("%s", &Ans);
            Region.PixType = (toupper(Ans) == 'M' ? PT_GRAYM : PT_GRAY);
        }
#endif
        printf("How to be stored T=TopDown, B=BottomUp?     ");
        scanf("%s", &Ans);
        if ((toupper(Ans) == 'T') != ((p.u.PNG.Region.Flags & RF_TopDown) != 0)) {
            p.Put.QFlags |= Q_REVERSE;
        }
        if (strstr(DesName, ".RAW") == NULL) {
            strcpy(strchr(DesName, '.'), ".RAW");
            printf("The destination name is now '%s'.\n", DesName);
        }
        printf("\n");
    } else {    /* assume BMP */
        /*  A file may be TopDown as opposed to BottomUp.   */
        /*  If the TopDown flag is set and the output file  */
        /*  is BMP then the image needs to be reversed.     */
        if (p.u.PNG.Region.Flags & RF_TopDown) {
            p.Put.QFlags |= Q_REVERSE;
        }
    }

    /*  I M P O R T A N T  - if Stride is different that the stride used */
    /*  when the file was created then StripSize must be recalculated.  */
    if (Region.Stride != p.u.PNG.Region.Stride) {
        int     LinesPerStrip = p.u.PNG.StripSize/p.u.PNG.Region.Stride;
        p.u.PNG.StripSize = Region.Stride * LinesPerStrip;
    }
    p.u.PNG.Region = Region;

    /* Allocate Put Buff - size is largest multiple of strip size that */
    /* will fit in PUT_BUFF_SIZE.  For this test, if there is a back   */
    /* file then the Put Buff must be large enough to hold the entire  */
    /* image.  The StripSize for an interlaced image must also be big  */
    /* enough for the entire image - if so, this was set during Init.  */
    if (BakName[0] != '\0') {
#if 0        
        int  AlphaBits = ((p.u.PNG.Region.PixType & 0x10) ? 8 : 0);
        int  S = (p.u.PNG.Region.Width * (p.u.PNG.Region.Bpp - AlphaBits) + 7) >> 3;
        if (p.Flags & F_Bmp)  S = (S + 3) & ~3;
        p.u.PNG.StripSize = S * p.u.PNG.Region.Height;
#else        
        p.u.PNG.StripSize = p.u.PNG.Region.Stride * p.u.PNG.Region.Height;
#endif                
    }
    m = max(1, PUT_BUFF_SIZE/p.u.PNG.StripSize) * p.u.PNG.StripSize;
    p.Put.Start = (BYTE *) malloc(m);
    check(p.Put.Start != NULL, "Out of memory for Put queue");
    p.Put.End = p.Put.Start + m;
    p.Put.Rear = ((p.Put.QFlags & Q_REVERSE) ? p.Put.End : p.Put.Start);
    p.Put.Front = p.Put.Rear;

    if (BakName[0] != '\0') {
        /* Read the back file into the Put Buffer (this is a RAW image). */
        FILE  *BakFile = fopen(BakName, "rb");
        check(BakFile != NULL, "Can't open file %s", BakName);
        FileSize = filelength(fileno(BakFile));
        if (FileSize != p.u.PNG.StripSize)  printf("Back file is wrong size - ignored\n");
        else {
            p.u.PNG.PicFlags |= PF_BlendWithBuffer;
            FileSize = fread(p.Put.Start, 1, p.u.PNG.StripSize, BakFile);
            check(FileSize == p.u.PNG.StripSize, "Error reading %s", BakName);
        }
        fclose(BakFile);
    }
    
    /* setup output file*/
    DesFile = fopen(DesName, "wb");
    check(DesFile != NULL, "Can't create file %s", DesName);

    if (p.Flags & F_Raw) {
        /* It is up to the user to fill in any header */
        check(fseek(DesFile, p.u.PNG.Region.Offset, SEEK_SET) == 0,
            "Bad destination seek");
    } else {
        /*  Make BMP file header.  Note, if biBitCount <= 8 the PEGASUS     */
        /*  routine will have created a gray-scale palette for the GRAY     */
        /*  file type, set biCompression to BI_RGB, and set biClrUsed and   */
        /*  biClrImportant as appropriate.                                  */
        BITMAPFILEHEADER  BF;

        check(p.Head.biWidth > 0, "Missing BITMAPINFOHEADER");
        check(p.Head.biBitCount > 8  ||  p.Head.biClrUsed > 0, "Missing color table");

        if (p.Head.biSizeImage == 0) {
            int  Stride = ((p.Head.biWidth*p.Head.biBitCount+31) >> 3) & ~3;
            p.Head.biSizeImage = labs(p.Head.biHeight)*Stride;
        }
        BF.bfType = 0x4D42;      // "BM"
        BF.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                       p.Head.biClrUsed*4;
        BF.bfSize = BF.bfOffBits + p.Head.biSizeImage;
        BF.bfReserved1 = 0;   BF.bfReserved2 = 0;
        check(fwrite(&BF, sizeof(BITMAPFILEHEADER), 1, DesFile) == 1  &&
              fwrite(&p.Head, sizeof(BITMAPINFOHEADER), 1, DesFile) == 1,
              "Error writing file header");
        if (p.Head.biClrUsed > 0) {
            check(fwrite(p.ColorTable, 4, p.Head.biClrUsed, DesFile) == p.Head.biClrUsed,
                  "Error writing color table");
        }
    }

    /*  Expand the file.*/
    AccumTimeStampCounter(Time);
    res = Pegasus(&p, REQ_EXEC);
    while (res != RES_DONE) {
        if (res == RES_GET_NEED_DATA) {
            CopyFromFileToQueue();
        } else if (res == RES_EXTEND_PIC2LIST) {
            /* P->PacketType can be used to determine if the packet  */
            /* should be saved.  For this example, save all packets. */
            p.PIC2List = (char *) realloc(p.PIC2List, p.PIC2ListLen);
            check(p.PIC2List != NULL, "Out of memory for PIC2List");
            p.PIC2ListSize = p.PIC2ListLen;
        } else if (res == RES_GET_DATA_YIELD) {
            /* do nothing */
        } else if (res == RES_PUT_NEED_SPACE) {
            CopyToFileFromQueue();
        } else if (res == RES_PUT_DATA_YIELD) {
            /* do nothing */
        } else {
            check(res != RES_ERR, "Error number %d", p.Status);
            check(FALSE,"Unknown Response  %d",res);
        }
        res = Pegasus(&p, REQ_CONT);
    }
    check(p.Status == ERR_NONE, "Error %d in Pegasus", p.Status);

    /* Flush any last output lines. */
    if (p.Put.Front != p.Put.Rear)  CopyToFileFromQueue();

    Pegasus(&p, REQ_TERM);
    ElapsedTimeStampCounter(Time);

    /*  Finished writing to file */
    printf("\r%3d%% done", p.PercentDone);
    fflush(SrcFile);
    fflush(DesFile);
    in_size  = filelength(fileno(SrcFile));
    out_size = filelength(fileno(DesFile));
    fclose(SrcFile);
    fclose(DesFile);

    /*  Display any further comments, etc. */
    printf("\n");
    DisplayASCII(p.PIC2List, p.PIC2ListLen);

    /* Show results */
    delta = out_size - in_size;

    printf("Expanded %s (%ux%u %u-bit)\n    to %s (%u-bit)\n",
        SrcName, p.Head.biWidth, labs(p.Head.biHeight), p.u.PNG.Region.Bpp,
        DesName, p.Head.biBitCount);
    if (p.Status != 0)  printf("*** Error status %d ***\n", p.Status);
    printf("Compressed size (Bits/Pixel) is %5.2f  (%5.3f sec @ %d MHz)\n",
        (in_size*8.0)/out_size, Seconds(Time), CPUFreq);
    printf("Original/Expanded size %d / %d bytes\n", in_size, out_size);
    printf("Saved %d bytes or %7.2f%%\n", delta, (100.0*delta)/out_size);

    if (p.PIC2List != NULL)  free(p.PIC2List),  p.PIC2List = NULL;
    free(p.Put.Start);   p.Put.Start = NULL;
    free(p.Get.Start);   p.Get.Start = NULL;
    return (p.Status);
}


/***********************************************************************\
*                                                                       *
*   Display any comments, watermarks, scripts, etc.                     *
*                                                                       *
\***********************************************************************/
PRIVATE void    DisplayASCII (BYTE *q, DWORD Len)
{
    if (Len > 1) {
        while (*q != 0) {
            Len = *((DWORD *) (q+1));
            switch (*q) {
              case 2  :
                printf("Script: %0.*s\n", Len, q+5);
                break;
              case 3  :
                printf("PIC2 File Description: %0.*s\n", Len, q+5);
                break;
              case 9  :
                printf("Copyright: %0.*s\n", Len, q+5);
                break;
              case 11 :
                printf("Promotion: %0.*s\n", Len, q+5);
                break;
              case 22 :
                printf("Comment: %0.*s\n", Len, q+5);
                break;
              case 23 :  /* RawData */
                if (memcmp(q+5, "APP", 3) == 0) {  /* Application Marker */
                    int    RawLen = *((int *) (q+9)),  m = min(64, RawLen);
                    char   *s = q+13;
                    printf("APP%c: ", q[8]);
                    while (m > 0) {
                        printf("%c", (isprint(*s) ? *s : 'ú'));
                        m--;   s++;
                    }
                    printf(RawLen > 64 ? " ...\n" : "\n");
                }
                break;
              case 24 :
                printf("Watermark: %0.*s\n", Len, q+5);
                break;
              default :
                break;
            }
            q += 5 + Len;
        }
    }
}


#if SRC_IS_C
/***********************************************************************\
*                                                                       *
*   Routines to generate timing (more accurate in assembly).            *
*                                                                       *
\***********************************************************************/
PRIVATE void    AccumTimeStampCounter (DWORD *Count)
{
    /* Count[1]:Count[0] += 64-bit counter */
    Count[0] = (DWORD) (clock() * ((CPUFreq*1.0E6)/CLK_TCK));
}

PRIVATE void    ElapsedTimeStampCounter (DWORD *Count)
{
    /* Count[1]:Count[0] = 64-bit counter - Count[1]:Count[0] */
    Count[0] = (DWORD) (clock() * ((CPUFreq*1.0E6)/CLK_TCK)) - Count[0];
}
#endif


/***********************************************************************\
*                                                                       *
*   Level 3)  This is an example of a simple routine "deferred" to by   *
*   level 2 routines to read in any bytes from the Get queue when       *
*   treated as a linear buffer. (Typically when the RES_GET_NEED_DATA   *
*   response is returned.)  This routine "resumes" setting the Q_IO_ERR *
*   flag in QFlags to reflect any read error condition.  An error causes*
*   the invoking level 2 routine to terminate with status ERR_BAD_READ. *
*                                                                       *
\***********************************************************************/
PRIVATE void   CopyFromFileToQueue (void)
{
    LONG    AskLen = p.Get.End - p.Get.Rear, GotLen;

    GotLen = fread(p.Get.Rear, 1, AskLen, SrcFile);
    if (GotLen < AskLen)  p.Get.QFlags |= Q_EOF;   /* no more data left to read */
    p.Get.Rear += GotLen;
}


/***********************************************************************\
*                                                                       *
*   Level 3)  This is an example of a simple routine "deferred" to by   *
*   level 2 routines to write ALL bytes from the Put queue when treated *
*   as a linear buffer. (Typically when the RES_PUT_NEED_SPACE response *
*   is returned.)  At least one line (Stride bytes) must be availible   *
*   in the queue on return to level 2 with Put.Front <= Put.Rear.       *
*   This routine "resumes" setting the Q_IO_ERR flag in QFlags to       *
*   reflect any write error condition. An error causes the invoking     *
*   level 2 routine to terminate with status ERR_BAD_WRITE.             *
*                                                                       *
\***********************************************************************/
PRIVATE void   CopyToFileFromQueue (void)
{
    static int  ImageStart, ImageEnd, FilePos;
    static BOOL FirstWrite = TRUE;
    int         GotLen, AskLen = p.Put.Rear - p.Put.Front;

    printf("\r%3d%% done", p.PercentDone);
    fflush(stdout);

    if (AskLen == 0) return;       /* buffer is empty*/

    /*  Handle the case when the file lines are to be reversed */
    if (FirstWrite) {
        FirstWrite = FALSE;
        FilePos = ImageStart = ftell(DesFile);
        ImageEnd = ImageStart + p.u.PNG.Region.Height * p.u.PNG.Region.Stride;
        if (p.Put.QFlags & Q_REVERSE) {
            check(fseek(DesFile, ImageEnd, SEEK_SET) != -1, "Bad Seek");
            FilePos = ImageEnd;
        }
    }
    if (p.Put.QFlags & Q_REVERSE) {
        AskLen = min(-AskLen, FilePos - ImageStart);
        FilePos -= AskLen;
        check(fseek(DesFile, FilePos, SEEK_SET) != -1, "Bad Seek");
        while (TRUE) {
            GotLen = fwrite(p.Put.Rear, 1, AskLen, DesFile);
            if (GotLen == 0) {
                /* write error or trying to write before ImageStart */
                p.Put.QFlags |= Q_IO_ERR;               /* "sticky" error*/
                return;
            }
            if (GotLen == AskLen)  break;
            AskLen -= GotLen;
            p.Put.Rear += GotLen;
        }
        check(fseek(DesFile, FilePos, SEEK_SET) != -1, "Bad Seek");
        if (ImageStart == FilePos)  p.Put.QFlags |= Q_EOF;
        p.Put.Rear = p.Put.Front = p.Put.End;
    } else {
        AskLen = min(AskLen, ImageEnd - FilePos);
        FilePos += AskLen;
        while (TRUE) {
            GotLen = fwrite(p.Put.Front, 1, AskLen, DesFile);
            if (GotLen == 0) {
                /* write error or trying to write after ImageEnd */
                p.Put.QFlags |= Q_IO_ERR;               /* "sticky" error*/
                return;
            }
            if (GotLen == AskLen)  break;
            AskLen -= GotLen;
            p.Put.Front += GotLen;
        }
        if (ImageEnd == FilePos)  p.Put.QFlags |= Q_EOF;
        p.Put.Rear = p.Put.Front = p.Put.Start;
    }
}


/***********************************************************************\
*                                                                       *
*       Print help message.                                             *
*                                                                       *
\***********************************************************************/
PRIVATE void    print_help (void)
{
    printf(
"\nPNGETEST (ver 1.10)  Test the Portable Network Graphics Expand routine.\n"
"(c) 1998, Pegasus Imaging Corporation, Tampa, Florida\n\n"
"usage:  PNGETEST  source_name[.PNG]  destination[.BMP]  [back.RAW] {options}\n\n"
"where source_name designates a PNG file and options are:\n"
"  -B      BMP file forced - convert to BMP if necessary.\n"
"  -R      RAW file forced - ask format for each file.\n"
"The source name may be a wild card name - if so, all images are expanded\n"
"and share the same options.  If the source is not a wild card name, the\n"
"destination is the name of a file with the .BMP extension.  If the source\n"
"is a wild card, the destination is a path name; the destination name is the\n"
"destination path followed by the source name with .BMP extension.\n"
"If the back image is availible (it must be the same size as the destination\n"
"and have the same bit depth) then it is blended with any transparent or alpha\n"
"decompressed image(s).\n"
    );
}


/***********************************************************************\
*                                                                       *
*       Parse the parameters.                                           *
*                                                                       *
\***********************************************************************/
PRIVATE void    parse_parms (int argc, char *argv[])
{
    char    ci_drive[5], ci_dir[60], ci_fname[20], ci_ext[10], *p;
    int     i;

    WildCard = (strchr(argv[1], '*') != NULL  ||
                strchr(argv[1], '?') != NULL);
    _fullpath(SrcName, argv[1], 100);   strupr(SrcName);
    _splitpath(SrcName, ci_drive, ci_dir, ci_fname, ci_ext);
    if (ci_ext[0] == '\0')  strcpy(ci_ext, ".PNG");
    _makepath(SrcName, ci_drive, ci_dir, ci_fname, ci_ext);
    _makepath(SrcPath, ci_drive, ci_dir, "", "");

    _fullpath(DesName, argv[2], 100);   strupr(DesName);
    if (WildCard) {
        p = DesName + strlen(DesName);
        if (p[-1] != '\\'  &&  p[-1] != ':')  *((WORD *) p) = 0x005C;
        _splitpath(DesName, ci_drive, ci_dir, ci_fname, ci_ext);
        _makepath(DesPath, ci_drive, ci_dir, "", "");
    } else {
        p = strchr(DesName, '.');
        if (p == NULL)  strcat(DesName, ".BMP");
    }

    opt_raw = FALSE;   opt_bmp = FALSE;   BakName[0] = '\0';
    i = 3;
    if (argc >= 4  &&  argv[3][0] != '-') {
        _fullpath(BakName, argv[3], 100);   strupr(BakName);
        _splitpath(BakName, ci_drive, ci_dir, ci_fname, ci_ext);
        if (ci_ext[0] == '\0')  strcpy(ci_ext, ".RAW");
        _makepath(BakName, ci_drive, ci_dir, ci_fname, ci_ext);
        i = 4;
    }
    
    for (;  i < argc;  ++i) {
        if (argv[i][0] == '-'  ||  argv[i][0] == '/') {
            switch (toupper(argv[i][1])) {
              case 'B' :  opt_bmp = TRUE;
                          break;
              case 'R' :  opt_raw = TRUE;
                          break;
              default  :  check(FALSE, "Unknown option %s", argv[i]);
                          break;
            }
        }
    }
    check(opt_bmp == FALSE  ||  opt_raw == FALSE,
        "Can't have both RAW and BMP options");
}


/***********************************************************************\
*                                                                       *
*       Check that exp is TRUE.  If it is not then print the formatted  *
*   message fmsg, and terminate the program.                            *
*                                                                       *
\***********************************************************************/
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
    va_list marker;

    if (!exp) {
        va_start(marker, fmsg);
        printf("Program terminated: ");  vprintf(fmsg, marker);  printf(".\n");
        exit(1);
    }
}
