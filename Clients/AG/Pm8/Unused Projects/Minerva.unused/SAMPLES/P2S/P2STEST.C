/*.T P2S test ver 3.00
 * $Header: /P2S/P2SDDW/P2STEST.C 2     2/22/97 8:05a Jim $Nokeywords:
 */

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ    Copyright (c) 1994, Pegasus Imaging Corporation                    บ
//  ณ    All rights reserved.                                               บ
//  ณ                                                                       บ
//  ณ    Use PicQueryType, PicExpandJpegSetup, PicExpandJpeg, and           บ
//  ณ    PicExpandJpegTerm to decompress a progressive JPEG image to a BMP file.        บ
//  ณ                                                                       บ
//  รฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤถ
//  ณ   Revision History:                                                   บ
//  ณ   JEXP.C                                                              บ
//  ณ   Program version 1.00  created                 5/ 4/94  Stephen Mann บ
//  ณ   Program version 2.00  add wild-card source    7/31/94  Stephen Mann บ
//  ณ   Program version 2.11  modified                8/ 4/94  Stephen Mann บ
//  ณ       Rewritten to use the NCB variable in the JEXP interface.        บ
//  ณ   Program version 2.20  modified                1/24/95  Stephen Mann บ
//  ณ       Replace TopDown with Orientation and add APP2 marker.           บ
//  ณ   Program version 3.00  modified               10/24/95  Stephen Mann บ
//  ณ       New PIC_PARM structure and version control                      บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#pragma pack(1)

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <dos.h>
#include    <string.h>
#include    <io.h>
#include    <conio.h>
#include    <ctype.h>
#include    <fcntl.h>
#include    <sys\types.h>
#include    <sys\stat.h>

#define MAX_NUM_SCANS 22
#define	PUT_BUFF_SIZE	8192

// NOTE: PegasusQuery assumes whole bmp header is in buffer, so packet
//  size must be at least large enough for that. Even worse, it assumes
// in raw JPEG case that up through frame header is all in first packet.
// It needs to be rewritten to fix this.

#define YIELD_TEST 0    // 1/0  => Yield/no yield to other window tasks
#if defined(WINDOWS) && !defined(_CONSOLE) // Needed for Windows
        int printf(const char *f, ...) { return 0; }
		int vprintf(const char *f, va_list v) { return 0; }
		int getch(void) { return 'C'; }
        int fflush(FILE *f) { return 0; }
		#define EXPORT  __export

#else
        #define EXPORT
#endif


#include    <stdtypes.h>
#include    <errors.h>
#include    <pic.h>
#include    <bmp.h>


//  Globals for main
PRIVATE PIC_PARM  p;
PRIVATE char    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE char    *des_ext;
PRIVATE int     src_file = -1, des_file = -1;
PRIVATE BOOL    wild_card, skip_copyright;


//  Local routines
PRIVATE short   expand (char *src_name, char *des_name);
PRIVATE void    read_file (PIC_PARM *p);
PRIVATE void    copy_to_file (void);
PRIVATE void    check (BOOL exp, char *fmsg, ... );
PRIVATE void    show_copyright (void);
PRIVATE void    print_help (void);
PRIVATE void    parse_parms (int argc, char *argv[]);

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ   Test the Pegasus routine interface.                                 บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
#if defined(WINDOWS) && !defined(_CONSOLE) // Needed for Windows

#define argc _argc
#define argv _argv
extern  int  argc;
extern  char **argv;

int     PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
								char *szCmdLine, int nCmdShow)
#else

int     main (int argc, char *argv[])

#endif
{
		struct  find_t info;
		short   status;
		UINT    n;

		if (argc < 2) {
				print_help();   exit(1);
		}

		printf(
				"\nJ32TEST - Test prgressive JPEG by converting images to .BMP files.\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
		);

		//  Get command line arguments.
		parse_parms(argc, argv);

		if (!skip_copyright)  show_copyright();

		n = 0;
		if (_dos_findfirst(src_name, _A_NORMAL, &info) == 0) {
				do {
						strcpy(src_name, src_path);   strcat(src_name, info.name);
						if (wild_card) {
								strcpy(des_name, des_path);   strcat(des_name, info.name);
								strcpy(strchr(des_name, '.'), des_ext);
						}
						status = expand(src_name, des_name);
						if(status != 0)
							break;
						n++;
				} while ( _dos_findnext(&info) == 0);
		}
		printf("\nThere were %d images converted.\n", n);
		if(status)
			printf("status = %d\n",status);
		return (0);
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE short   expand (char *src_name, char *des_name)
{
	RESPONSE    res;
	DWORD		get_buff_size;
	long		len,siz;



	memset(&p, 0, sizeof(PIC_PARM));    // important: set all default values 0
    // ALSO important to set the following
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;


		//  Open the PICX file
	src_file = open(src_name, O_BINARY | O_RDONLY);
	check(src_file != -1, "Could not open %s", src_name);

	//  Allocate GetBuff and place into it the information to be queried
	get_buff_size = filelength(src_file);
	p.Get.Start = (BYTE *) malloc(get_buff_size);
	check(p.Get.Start != NULL, "Out of memory for get buffer");
	p.Get.End = p.Get.Start + get_buff_size;
	siz = get_buff_size;
	len = (unsigned)read(src_file, p.Get.Start, siz);
	check(len != (unsigned)-1, "Error reading input file");
	if(len < siz)
		p.Get.QFlags |= Q_EOF;
	p.Get.Rear = p.Get.Start + len;
	p.Get.Front = p.Get.Start;

	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
	check(PegasusQuery(&p), "Invalid PegasusQuery");
	memset(&p.u.QRY, 0, sizeof(p.u.QRY));
	check(p.Head.biCompression == BI_PJPG, "Must be progressive JPEG");

		//  Set up the uncompressed image characteristics.

	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
	p.Op = OP_P2S;
	p.u.P2S.PicFlags = 0;
	p.u.P2S.PicFlags |= PF_YieldPut;


	// Top Down flag must be set BEFORE jpeg init is done,because it resets biHeight
	if(p.Head.biHeight < 1) {
		p.Head.biHeight = -p.Head.biHeight;
	}
	// from now on,p.Head.biHeight is positive.

	//  setup output buff
	p.Put.Start = (BYTE  *) malloc(PUT_BUFF_SIZE);
	check(p.Put.Start != NULL, "Out of memory for put buff");
	p.Put.Front = p.Put.Rear = p.Put.Start;
	p.Put.End = p.Put.Start + PUT_BUFF_SIZE;

	// setup output file
	des_file = open(des_name, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IWRITE);
	check(des_file != -1, "Can't create file %s", des_name);


	//  Initialize Jpeg decompression
	res = Pegasus(&p, REQ_INIT);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
			read_file(&p);
		else if (res == RES_PUT_NEED_SPACE)
			copy_to_file();
		else if (res == RES_PUT_DATA_YIELD)
			copy_to_file();
		else if (res == RES_ERR)
			check(FALSE, "Error number %d ",p.Status);
		else
			check(FALSE, "Unexpected or unknown response %d ", res);
		check(p.Status >= 0, "Error %d in Pegasus", p.Status);
		res = Pegasus(&p, REQ_CONT);
	}
	check(p.Status >= 0, "Error %d in Pegasus", p.Status);


  	// Decompress and recompress
		p.u.P2S.NumScansReq = 0; // signal to do all scans
		res = Pegasus(&p, REQ_EXEC);
		while (res != RES_DONE)
		{
			if (res == RES_GET_NEED_DATA)
				read_file(&p);
			else if (res == RES_PUT_NEED_SPACE)
				copy_to_file();
			else if (res == RES_PUT_DATA_YIELD)
				copy_to_file();
			else if (res == RES_YIELD)
				;
			else if (res == RES_ERR)
				check(FALSE, "Error number %d ",p.Status);
			else
				check(FALSE, "Unexpected or unknown response %d ", res);
			check(p.Status >= 0, "Error %d in Pegasus", p.Status);
			res = Pegasus(&p, REQ_CONT);
		}

		check(p.Status >= 0, "Error %d in Pegasus", p.Status);
		if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);


	Pegasus(&p, REQ_TERM);

			//  Close out file, free out memory.
	close(des_file);
	free(p.Put.Start);   p.Put.Start = NULL;

		// Close src file and memory

	close(src_file);
	free(p.Get.Start);   p.Get.Start = NULL;
	return (p.Status);
}

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   This is a callback routine invoked by PicExpandJpegStartup and by   บ
//  ณ   PicExpandJpeg to load a buffer pointed to by BuffPtr from the next  บ
//  ณ   successive *BuffLen bytes in the src_file.  In the case that        บ
//  ณ   fewer than *BuffLen bytes can be read, *BuffLen is set to the       บ
//  ณ   actual number of bytes read.  An error is returned if read-error    บ
//  ณ   or end-of-file is encountered, otherwise 0 is returned.             บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ

#if 1

PRIVATE void   read_file (PIC_PARM *p)
{
	long    avail,len,siz;

		/****/

	p->Status = ERR_NONE;
	if ( p->Get.Rear >= p->Get.Front)
	{
		if( (avail = p->Get.End - p->Get.Rear) == 0)
		{
			p->Get.QFlags |= Q_EOF;
			return;
		}
		siz = avail;
		len = (unsigned)read(src_file, p->Get.Rear, siz);
		if (len == (unsigned)-1)     // read error
			p->Status = ERR_BAD_READ;
		else
		{
			p->Get.Rear += len;
			if(len < siz)
				p->Get.QFlags |= Q_EOF;
		}
	}
	else  // Front not before Rear for PICX
		p->Status = ERR_EXPAND_INTERNAL_ERR;


#if defined( WINDOWS) & YIELD_TEST
	// yield test
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage((LPMSG) &msg);
				DispatchMessage((LPMSG) &msg);
	}
#endif
}

#endif

#if 0

PRIVATE void   read_file (PIC_PARM *p)
{
	long    avail,len;

		/****/

	p->Status = ERR_NONE;
	if ( p->Get.Rear >= p->Get.Front)
	{
		avail = p->Get.End - p->Get.Rear;
		if(avail > 0)
		{
			len = (unsigned)read(src_file, p->Get.Rear, avail);
			if (len == (unsigned)-1)
			{    // read error
				p->Status = ERR_BAD_READ;
				return;
			}
			if(len < avail)
			{
				p->Get.QFlags |= Q_EOF;
				p->Get.Rear += len;
			}
			else
			{
				if(p->Get.Front > p->Get.Start)
					p->Get.Rear = p->Get.Start;
				else
					p->Get.Rear = p->Get.End;
			}
		}
		else
		{
			if(p->Get.Front > p->Get.Start)
				p->Get.Rear = p->Get.Start;
		}
	}
	if(p->Get.Rear < p->Get.Front)
	{
		avail = p->Get.Front - p->Get.Rear - 1;
		len = (unsigned)read(src_file, p->Get.Rear, avail);
		if (len == (unsigned)-1)
		{    // read error
			p->Status = ERR_BAD_READ;
			return;
		}
		p->Get.Rear += len;
		if(len < avail)
			p->Get.QFlags |= Q_EOF;
	}


#if defined( WINDOWS) & YIELD_TEST
	// yield test
		MSG msg;
		if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage((LPMSG) &msg);
				DispatchMessage((LPMSG) &msg);
	}
#endif
}

#endif

PRIVATE void   copy_to_file (void)
{
	long	len;


	len = p.Put.Rear - p.Put.Front;
	if ((unsigned)write(des_file, p.Put.Front, len) < len) {
		p.Status = ERR_BAD_WRITE;
		return;
	}
	p.Status = ERR_NONE;
	p.Put.Rear = p.Put.Front;
#if defined(WINDOWS) & YIELD_TEST
		// yield test
				MSG msg;
				if (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE)) {
						TranslateMessage((LPMSG) &msg);
						DispatchMessage((LPMSG) &msg);
		}
#endif
}

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Check that exp is TRUE.  If it is not then print the formatted  บ
//  ณ   message fmsg, and terminate the program.                            บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    check (BOOL exp, char *fmsg, ... )
{
        va_list marker;

		if (!exp) {
                va_start(marker, fmsg);
				printf("Program terminated: ");  vprintf(fmsg, marker);  printf(".\n");
                if (src_file != -1)  close(src_file);
                if (des_file != -1)  close(des_file);
                exit(1);
        }
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   Show the copyright information.                                     บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    show_copyright (void)
{
		printf("\nJ32TEST - Convert progressive JPEG images to sequential JPG\n"
				   "(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n\n"
				   "    (Voice) 813-875-7575  (Fax) 813-875-7705\n\n"
				   "Program Version 3.00\n\n"
                );
        printf("Press 'C' to continue.. ");
        fflush(stdout);
        while ('C' != getch()) ;    // Await user response
        printf("\n");
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Print help message.                                             บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    print_help (void)
{
        printf(
				"\nJ32TEST - Convert Progressive JPEG images to sequential JPG\n"
				"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
				"\nusage:  J32TEST  file_name [destination[.JPG]]  {options}\n"
				"where options are:\n"
		);
		printf(
				"The file_name may be a wild card name - if so, all images are compressed\n"
				"sharing the same options and the destination name is the destination path\n"
		"followed by the file_name with .JPG extension.  If the source is not a wild\n"
		"card name, the destination name is the destination with default .JPG extension."
		);
}


//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ       Parse the parameters.                                           บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
PRIVATE void    parse_parms (int argc, char *argv[])
{
		char    ci_drive[5], ci_dir[60], ci_fname[20], ci_ext[10], *p;
		//INT     i;
		BOOL    have_name;

		wild_card = (strchr(argv[1], '*') != NULL  ||
								 strchr(argv[1], '?') != NULL);
		_fullpath(src_name, argv[1], sizeof(src_name));   strupr(src_name);
		_splitpath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
		if (ci_ext[0] == '\0')  strcpy(ci_ext, ".PIC");
		_makepath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
		_makepath(src_path, ci_drive, ci_dir, "", "");

		des_ext = ".JPG";
		skip_copyright = FALSE;

#if 0
		for (i = 2;  i < argc;  ++i) {
				if (argv[i][0] == '-'  ||  argv[i][0] == '/') {
						switch (toupper(argv[i][1])) {
								case 'B' :
										bit_count = atoi(argv[i]+2);
										break;
								case 'G' :
										make_gray = TRUE;
										gray_levels = atoi(argv[i]+2);
										if(gray_levels == 0)
											gray_levels = 256;
										break;
								case 'M' :
										make_colors = TRUE;
										break;
								case 'N' :
										dither = FALSE;
										break;
								case 'D' :
										no_gets = TRUE;
										break;
								case 'E' :
										decode_to_data_end = TRUE;
										break;
								case 'C' :
										no_smooth = TRUE;
										break;
								case 'S' :
										multiple_scans = TRUE;
										break;
								case 'Z' :
										test_strips = TRUE;
										break;
								case 'P' :
										nc = atoi(argv[i]+2);
										if(nc > 0)
											nprimcolors = nc;
										else
											nseccolors = nc;
										break;
								case 'A' :
										packet_size = atoi(argv[i]+2);
										if(packet_size < 128)
											packet_size = 128;
										break;
								case 'T' :
										thumbnail = atoi(argv[i]+2);
										if (thumbnail == 0)  thumbnail = 3;
										check(thumbnail >= 1  &&  thumbnail <= 4, "Bad thumbnail");
										break;
								case 'F' :
										num_scans = atoi(argv[i]+2);
										check(num_scans <= MAX_NUM_SCANS, "Bad num scans");
										break;
								case '!' :
					skip_copyright = TRUE;
										break;
								default  :
										check(FALSE, "Unknown option %s", argv[i]);
										break;
						}
				}
		}
#endif

		have_name = (argc > 2  &&  argv[2][0] != '-'  &&  argv[2][0] != '/');
		if (have_name) {
				_fullpath(des_name, argv[2], 100);   strupr(des_name);
		} else {
				strcpy(des_name, src_name);
		}
		if (wild_card) {
                if (have_name) {
						p = des_name + strlen(des_name);
                        if (p[-1] != '\\'  &&  p[-1] != ':')  *((WORD *) p) = 0x005C;
                }
                _splitpath(des_name, ci_drive, ci_dir, ci_fname, ci_ext);
                _makepath(des_path, ci_drive, ci_dir, "", "");
        } else {
                p = strchr(des_name, '.');
                if (have_name) {
                        if (p == NULL)  strcat(des_name, des_ext);
                } else {
                        strcpy(p, des_ext);
                }
        }
}
