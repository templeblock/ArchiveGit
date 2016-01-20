/*.T    JEXP.C Ver. 3.00 -  Demo the PicExpandJpeg Interface
 * $Header: /14TST/14TSTDDW/J32TEST.C 2     4/24/96 7:40p Jim $
 * $Nokeywords: $
 */

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ    Copyright (c) 1995, Pegasus Imaging Corporation                    บ
//  ณ    All rights reserved.                                               บ
//  ณ                                                                       บ
//  ณ    Use PicQueryType, PicExpandJpegSetup, PicExpandJpeg, and           บ
//  ณ    PicExpandJpegTerm to decompress a sequential JPEG image to a BMP file.        บ
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
//  ณ   Program version 3.00  modified               10/25/95  Stephen Mann บ
//  ณ       Adjust Copyright, clean-up, test App2 marker.                   บ
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


#define YIELD_TEST 0    // 1/0  => Yield/no yield to other window tasks
#if defined(WINDOWS) && !defined(_CONSOLE)
		int printf(const char *f, ...) { return 0; }
		int vprintf(const char *f, va_list v) { return 0; }
		int getch(void) { return 'C'; }
		int fflush(FILE *f) { return 0; }
		#define EXPORT  __export

#else
		#define EXPORT
#endif

#define	PUT_BUFF_SIZE	8192
#define MAX_NUM_SCANS 22   // for progressive JPEG

#include    <stdtypes.h>
#include    <errors.h>
#include    <pic.h>
#include    <bmp.h>


//  Globals for main
PRIVATE PIC_PARM  p;
PRIVATE SCAN_PARM scan_parms[MAX_NUM_SCANS];
PRIVATE char    src_name[100], src_path[100], des_name[100], des_path[100];
PRIVATE char    *des_ext;
PRIVATE int     src_file = -1, des_file = -1;
PRIVATE BOOL    wild_card, skip_copyright;
PRIVATE LONG    num_prog_scans;
PRIVATE CHAR    key[9];
PRIVATE	DWORD	get_buff_size = 10000;

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
#if defined(WINDOWS) && !defined(_CONSOLE)

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
				"\nJ32TEST - convert sequential JPEG images to progressive.\n"
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
		if(status < 0)
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
	long		len;

		/****/

	memset(&p, 0, sizeof(PIC_PARM));    // important: set all default values 0
    // ALSO important to set the following
	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;

	//  Allocate GetBuff and place into it the information to be queried
	p.Get.Start = (BYTE *) malloc(get_buff_size);
	check(p.Get.Start != NULL, "Out of memory for get buffer");
	p.Get.End = p.Get.Start + get_buff_size;

	//  Open the JPEG file, read in header data
	src_file = open(src_name, O_BINARY | O_RDONLY);
	check(src_file != -1, "Could not open %s", src_name);
	len = read(src_file, p.Get.Start, get_buff_size);
	check(len != -1, "Error reading input file");
	if(len < get_buff_size)
		p.Get.QFlags |= Q_EOF;
	p.Get.Rear = p.Get.Start + len;
	p.Get.Front = p.Get.Start;

	p.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;
	check(PegasusQuery(&p), "Invalid PegasusQuery");
	memset(&p.u.QRY, 0, sizeof(p.u.QRY));
	check(p.Head.biCompression == BI_PICJ  ||
				p.Head.biCompression == BI_JPEG, "Must be JPEG compressed");


	//  setup output buff
	p.Put.Start = (BYTE  *) malloc(PUT_BUFF_SIZE);
	check(p.Put.Start != NULL, "Out of memory for put buff");
	p.Put.Front = p.Put.Rear = p.Put.Start;
	p.Put.End = p.Put.Start + PUT_BUFF_SIZE;

	// setup output file
	des_file = open(des_name, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IWRITE);
	check(des_file != -1, "Can't create file %s", des_name);


	p.ParmSize = sizeof(PIC_PARM);
	p.ParmVer = CURRENT_PARMVER;   p.ParmVerMinor = 1;
	p.Op = OP_S2P;
	memcpy(p.KeyField, key, 8);

	p.u.S2P.PicFlags |= PF_YieldPut;

	p.u.S2P.NumProgScans = num_prog_scans;
	if(num_prog_scans != 0)
	{
		check(1 <= num_prog_scans && num_prog_scans <= MAX_NUM_SCANS, "num scans out of range");
		p.u.S2P.ScanParms = scan_parms;
	}

	//  Initialize Jpeg decompression
	res = Pegasus(&p, REQ_INIT);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA) {
			read_file(&p);
		} else if (res == RES_PUT_DATA_YIELD) {
			copy_to_file();
		} else if (res == RES_ERR) {
			check(FALSE, "Error number %d ",p.Status);
		} else {
			check(FALSE, "Unexpected or unknown response %d ", res);
		}
		check(p.Status >= 0, "Error %d in Pegasus", p.Status);
		if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);
		res = Pegasus(&p, REQ_CONT);
	}
	check(p.Status >= 0, "Error %d in Pegasus", p.Status);
	if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);


	//  Decompress the file.
	res = Pegasus(&p, REQ_EXEC);
	while (res != RES_DONE)
	{
		if (res == RES_GET_NEED_DATA)
			read_file(&p);
		else if (res == RES_PUT_DATA_YIELD)
			copy_to_file();
		else if (res == RES_YIELD)
			;
		else if (res == RES_ERR)
			check(FALSE, "Error number %d ",p.Status);
		else
			check(FALSE, "Unexpected or unknown response %d ", res);
		check(p.Status >= 0, "Error %d in Pegasus", p.Status);
		if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);
		res = Pegasus(&p, REQ_CONT);
	}
	check(p.Status >= 0, "Error %d in Pegasus", p.Status);
	if (p.Status > 0)  printf("Status %d in Pegasus.\n", p.Status);
	Pegasus(&p, REQ_TERM);


	printf("\n");
	printf("Converted file %s into %s.\n", src_name, des_name);


	//  Close file, free memory.
	close(src_file);
	close(des_file);

	free(p.Put.Start);   p.Put.Start = NULL;
	free(p.Get.Start);   p.Get.Start = NULL;

	return (p.Status);
}

//  ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤท
//  ณ                                                                       บ
//  ณ   This is a callback routine invoked by PicExpandJpegSetup and by     บ
//  ณ   PicExpandJpeg to load a buffer pointed to by BuffPtr from the next  บ
//  ณ   successive  bytes in the src_file.                                  บ
//  ณ                                                                       บ
//  ิอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
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
			len = read(src_file, p->Get.Rear, avail);
			if (len == -1)
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
		len = read(src_file, p->Get.Rear, avail);
		if (len == -1)
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

/**********************************************************************/

PRIVATE void   copy_to_file (void)
{
	long	len;

	/****/

	len = p.Put.Rear - p.Put.Front;
	if (write(des_file, p.Put.Front, len) < len) {
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

/**********************************************************************/


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
		printf("\nJ32TEST - Test converting JPEG images to progressive.\n"
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
	"\nJ32TEST - Test converting JPEG images to progressive JPEG.\n"
	"(c) 1995, Pegasus Imaging Corporation, Tampa, Florida\n"
	"\nusage:  J32TEST  file_name[.PIC]  [destination[.XPG]]  {options}\n"
	"where options are:\n"
	"    -E[#]  input buffer size, min. 1024(default 10000).\n"
	"    -Kkey  Decrypts the image file with the given key, key is up to 8 chars.\n"
//	"     (press any key) \n"
//	);
//	getch();
//	printf(
	"The file_name may be a wild card name - if so, all images are converted\n"
	"sharing the same options and the destination name is the destination path\n"
	"followed by the file_name with .XPG extension.  If the source is not a wild\n"
	"card name, the destination name is the destination with default .XPG extension."
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
	INT     i;
	BOOL    have_name;

	wild_card = (strchr(argv[1], '*') != NULL  ||
							 strchr(argv[1], '?') != NULL);
	_fullpath(src_name, argv[1], sizeof(src_name));   strupr(src_name);
	_splitpath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	if (ci_ext[0] == '\0')  strcpy(ci_ext, ".PIC");
	_makepath(src_name, ci_drive, ci_dir, ci_fname, ci_ext);
	_makepath(src_path, ci_drive, ci_dir, "", "");

	des_ext = ".XPG";
	skip_copyright = FALSE;
	get_buff_size = 10000;
	memset(key, 0, sizeof(key));

	for (i = 2;  i < argc;  ++i) {
		if (argv[i][0] == '-'  ||  argv[i][0] == '/') {
			switch (toupper(argv[i][1])) {
				case 'K' :
						strncpy(key, argv[i]+2, 8);
						break;
				case 'E' :
						get_buff_size = atoi(argv[i]+2);
						check(get_buff_size >= 1024, "Bad input buff size");
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


		num_prog_scans = 10;
		scan_parms[0].Co = 3; // indicates interleaved YCbCr for dc scans
		scan_parms[0].Ss = 0;
		scan_parms[0].Se = 0;
		scan_parms[0].Al = 1;
		scan_parms[0].Ah = 0;
		scan_parms[1].Co = 0; // y
		scan_parms[1].Ss = 1;
		scan_parms[1].Se = 5;
		scan_parms[1].Al = 2;
		scan_parms[1].Ah = 0;
		scan_parms[2].Co = 1; // Cb
		scan_parms[2].Ss = 1;
		scan_parms[2].Se = 63;
		scan_parms[2].Al = 1;
		scan_parms[2].Ah = 0;
		scan_parms[3].Co = 2; // Cr
		scan_parms[3].Ss = 1;
		scan_parms[3].Se = 63;
		scan_parms[3].Al = 1;
		scan_parms[3].Ah = 0;
		scan_parms[4].Co = 0; // y
		scan_parms[4].Ss = 6;
		scan_parms[4].Se = 63;
		scan_parms[4].Al = 2;
		scan_parms[4].Ah = 0;
		scan_parms[5].Co = 0; // y
		scan_parms[5].Ss = 1;
		scan_parms[5].Se = 63;
		scan_parms[5].Al = 1;
		scan_parms[5].Ah = 2;
		scan_parms[6].Co = 3; // interleaved YCbCr
		scan_parms[6].Ss = 0;
		scan_parms[6].Se = 0;
		scan_parms[6].Al = 0;
		scan_parms[6].Ah = 1;
		scan_parms[7].Co = 1; // Cb
		scan_parms[7].Ss = 1;
		scan_parms[7].Se = 63;
		scan_parms[7].Al = 0;
		scan_parms[7].Ah = 1;
		scan_parms[8].Co = 2; // Cr
		scan_parms[8].Ss = 1;
		scan_parms[8].Se = 63;
		scan_parms[8].Al = 0;
		scan_parms[8].Ah = 1;
		scan_parms[9].Co = 0; // y
		scan_parms[9].Ss = 1;
		scan_parms[9].Se = 63;
		scan_parms[9].Al = 0;
		scan_parms[9].Ah = 1;




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
