//op81tst.c

//QuickWin test program for opcode 81
//Opcode 80 converts bmp, tga, pcx, tif, or gif files to dibs.
//Since a dib is very close to a BMP, this program actually creates
// a bmp file as output.

//compiled using Microsoft Visual C++ 1.52c

//NOTE that there is little or no error checking on reads, writes, seeks, etc.

//NOTE that a 24-bmp file with an included palette is not properly handled
// by this sample program, as well as any weird bmp file that has extra
// information between the BITMAPINFO structure and the dib bits area.

#ifndef OPCODE_F2D
#define OPCODE_F2D OP_F2D
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>

#include <stdtypes.h>
#include <errors.h>
#include <pic.h>

//local routines
static long convert_file (char *file1, char *file2);
static long bytes_in_queue (QUEUE *q);
static long bytes_free_in_queue (QUEUE *q);
static void getdata (PIC_PARM *pp);
static void putdata (PIC_PARM *pp);
static void getdata_seek (PIC_PARM *pp);
static void putdata_seek (PIC_PARM *pp);
static void writedata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp);
static void readdata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp);
static void fail (PIC_PARM *pp, long error_code);

static PIC_PARM qq;
        //for the query -- opcode 81 needs to know the basic file type
        // before it can do the job -- query returns this
static PIC_PARM pp;
        //for the opcode

char infilelist[300], outdirectory[300], answer[50];
        //for user data entry

BYTE PICHUGE *input_queue;
        //the address of the allocated input queue
BYTE PICHUGE *output_queue;
        //the address of the allocated output queue
long inbufsize, outbufsize;
        //size of input queue and output queue

#define TEMPFILE "zguy7s2z.qub"
        //because we don't know how big the possible palette may
        // be before we start writing out the output bmp file,
        // we only write out the dib bits to this temp file,
        // then later create the real file by writing the
        // bitmapfileheader, bitmapinfoheader, palette, and
        // then copy the dib bits from the temp file

struct _find_t fileinfo;
char x_drive[_MAX_DRIVE+1];
char x_dir[_MAX_DIR+1];
char x_fname[_MAX_FNAME+1];
char x_ext[_MAX_EXT+1];
        //for _dos_findxx
char y_drive[_MAX_DRIVE+1];
char y_dir[_MAX_DIR+1];
char y_fname[_MAX_FNAME+1];
char y_ext[_MAX_EXT+1];
        //for changing extension of the output file

char infilename[300], outfilename[300];
        //the built-up input and output file names

#define QUERY_SIZE 2000
        //this is a good size
char querybuf[QUERY_SIZE];
        
int filecount;  //number of images converted

int main (int argc, char *argv[])
{
        long iresult;
        int done;
                      
        printf ("Enter source files (wildcards allowed): ");
        gets (infilelist);
        printf ("Enter destination directory: ");
        gets (outdirectory);

        //allocate the queues
        inbufsize = 10000;      //input buffer -- can be just about anything
        outbufsize = 12000;     //output buffer -- can be just about anything
        if ((input_queue = malloc (inbufsize + 1)) == NULL)
        {
                printf ("***** unable to allocate input queue\n");
                return 1;
        }
        if ((output_queue = malloc (outbufsize + 1)) == NULL)
        {
                printf ("***** unable to allocate output queue\n");
                free (input_queue);
                return 1;
        }

        //split the input file(s) argument into path info
        _splitpath (infilelist, x_drive, x_dir, x_fname, x_ext);

        filecount = 0;  //number of files processed
        done = _dos_findfirst (infilelist, _A_NORMAL, &fileinfo);
        while (done == 0)
        {
                //build the input and output file names
                strcpy (infilename, x_drive);
                strcat (infilename, x_dir);
                if (infilename[strlen(infilename)-1] != '\\')
                {
                        strcat (infilename, "\\");
                }
                strcat (infilename, fileinfo.name);
                strcpy (outfilename, outdirectory);
                if (outfilename[strlen(outfilename)-1] != '\\')
                {
                        strcat (outfilename, "\\");
                }
                _splitpath (fileinfo.name, y_drive, y_dir, y_fname, y_ext);
                strcat (outfilename, y_fname);
                strcat (outfilename, ".bmp");

                //do the work
                printf ("Converting %s to %s\n", infilename, outfilename);
                iresult = convert_file (infilename, outfilename);
                if (iresult)
                {
                        printf ("convert_file failed on file %s-- error code %ld\n",
                                infilename, iresult);
                }
                done = _dos_findnext (&fileinfo);
        }
        printf ("%d file(s) converted", filecount);
        free (input_queue);
        free (output_queue);
        return 0;
}

static int inhandle = -1;
static int temphandle = -1;     //temp output file
static int finalhandle = -1;    //final output file
        //the file handles

static long infilesize;
        //these are needed by the getdata callback
static int getdata_already_called;
        //when we put the entire file in the input buffer, this makes
        // sure getdata is not called again asking for more data!
static long total_bytes_read = 0;
        //how many bytes we getdata'd from the input file
        
#define TBUF_SIZE 10000
static char tempbuf[TBUF_SIZE];
        //temp buffer for copying dib bits from temp file to output file

static long convert_file (char *file1, char *file2)
{
        long error = 1;
        long ndo, n_read, ncolors;
        RESPONSE res;
        BITMAPFILEHEADER bf;

        inhandle = -1;
        temphandle = -1;
        finalhandle = -1;

        //open the files
        inhandle = open (file1, O_RDONLY | O_BINARY);
        if (inhandle == -1)
        {
                printf ("***** unable to open input image %s\n", file1);
                goto ehandler;
        }
        temphandle = open (TEMPFILE, O_RDWR | O_BINARY | O_CREAT | O_TRUNC,
                S_IREAD | S_IWRITE);
        if (temphandle == -1)
        {
                printf ("***** unable to create temp file %s\n", TEMPFILE);
                goto ehandler;
        }
        finalhandle = open (file2, O_RDWR | O_BINARY | O_CREAT | O_TRUNC,
                S_IREAD | S_IWRITE);
        if (finalhandle == -1)
        {
                printf ("***** unable to create output file %s\n", file2);
                goto ehandler;
        }

        infilesize = filelength (inhandle);
        total_bytes_read = 0;

        //set up for the query
        memset (&qq, 0, sizeof (qq));   //VERY IMPORTANT!
        qq.ParmSize = sizeof (qq);
        qq.ParmVer = CURRENT_PARMVER;
        qq.u.QRY.BitFlagsReq = QBIT_BICOMPRESSION;      //all we care about
        qq.Get.Start = querybuf;
        qq.Get.Front = querybuf;
        qq.Get.End = querybuf + QUERY_SIZE + 1;

        //read in up to QUEYR_SIZE of the file for the query
        if (infilesize < QUERY_SIZE)
        {
                ndo = infilesize;
        }
        else
        {
                ndo = QUERY_SIZE;
        }
        read (inhandle, querybuf, (int)ndo);
        qq.Get.Rear = querybuf + ndo;
        if (PegasusQuery (&qq) == 0)
        {
                //query failed
                printf ("***** unable to query input file %s\n", file1);
                goto ehandler;
        }
        if (!(qq.u.QRY.BitFlagsAck & QBIT_BICOMPRESSION))
        {
                //could not determine file type
                printf ("***** query could not determine file type\n");
                goto ehandler;
        }

        //reset to beg. of file
        lseek (inhandle, 0, SEEK_SET);

        //set up for opcode 81
        memset (&pp, 0, sizeof (pp));   //VERY IMPORTANT!
        pp.Op = OPCODE_F2D;             //File to Dib (81)
        pp.ParmSize = sizeof (pp);
        pp.ParmVer = CURRENT_PARMVER;   //from pic.h
        pp.Head.biCompression = qq.Head.biCompression;
                //opcode 81 needs this basic file type info
        pp.u.F2D.ProgressiveMode = 2;   //for gif, if progressive
                // image, only want complete image

        pp.Get.Start = input_queue;
        pp.Get.Front = pp.Get.Rear = pp.Get.Start;
        pp.Get.End = pp.Get.Start + inbufsize + 1;

        pp.Put.Start = output_queue;
        pp.Put.Front = pp.Put.Rear = pp.Put.Start;
        pp.Put.End = pp.Put.Start + outbufsize + 1;

        getdata_already_called = 0;

        //the Pegasus loops
        res = Pegasus (&pp, REQ_INIT);
        while (res != RES_DONE)
        {
                if (res == RES_GET_NEED_DATA || res == RES_GET_DATA_YIELD)
                {
                        getdata (&pp);
                }
                else if (res == RES_PUT_DATA_YIELD || res == RES_PUT_NEED_SPACE)
                {
                        putdata (&pp);
                }
                //NOTE: RES_SEEK is never requested at init for opcode 80
                else
                {
                        if (res != RES_ERR)
                        {
                                pp.Status = ERR_UNKNOWN_RESPONSE;
                        }
                        fail (&pp, pp.Status);
                        return FALSE;
                }
                //actually, for this opcode, INIT does basicallay nothing
                res = Pegasus (&pp, REQ_CONT);
        }

        res = Pegasus (&pp, REQ_EXEC);
        while (res != RES_DONE)
        {
                if (res == RES_GET_NEED_DATA || res == RES_GET_DATA_YIELD)
                {
                        getdata (&pp);
                }
                else if (res == RES_PUT_DATA_YIELD || res == RES_PUT_NEED_SPACE)
                {
                        putdata (&pp);
                }
                else if (res == RES_SEEK)
                {
                        if ((pp.SeekInfo & SEEK_FILE) == 0)
                        {
                                getdata_seek (&pp);
                        }
                        else
                        {
                                putdata_seek (&pp);
                        }
                }
                else
                {
                        if (res != RES_ERR)
                        {
                                pp.Status = ERR_UNKNOWN_RESPONSE;
                        }
                        fail (&pp, pp.Status);
                        return FALSE;
                }
                res = Pegasus (&pp, REQ_CONT);
        }

        //flush the output
        if (pp.Put.Front != pp.Put.Rear)
        {
                putdata (&pp);
        }

        //write the bitmapfileheader out
        bf.bfType = 'B' + ('M' << 8);
        bf.bfReserved1 = 0;
        bf.bfReserved2 = 0;
        bf.bfSize = filelength (temphandle);
        //where the bitmap image is:
        bf.bfOffBits = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER)
                + (pp.u.F2D.BiOut.biClrUsed << 2);
        write (finalhandle, &bf, sizeof (BITMAPFILEHEADER));

        //write the bitmapinfoheader out
        write (finalhandle, &pp.u.F2D.BiOut, sizeof (BITMAPINFOHEADER));
        
        if (pp.u.F2D.BiOut.biClrUsed == 0)
        {
                if (pp.u.F2D.BiOut.biBitCount == 0)
                {
                        ncolors = 0;
                }
                else
                {
                        ncolors = 1 << pp.u.F2D.BiOut.biBitCount;
                }
        }
        else
        {
                ncolors = pp.u.F2D.BiOut.biClrUsed;
        }
        if (ncolors > 0)
        {
                //write the palette
                write (finalhandle, pp.ColorTable, (short)(ncolors << 2));
        }
        
        //copy the temp file
        
        lseek (temphandle, 0, SEEK_SET);
        while ((n_read = read (temphandle, tempbuf, TBUF_SIZE)) > 0)
        {
                write (finalhandle, tempbuf, (short)n_read);
        }
        
    Pegasus (&pp, REQ_TERM);
        error = 0;

ehandler:
        if (inhandle != -1)
        {
                close (inhandle);
                inhandle = -1;
        }
        if (temphandle != -1)
        {
                close (temphandle);
                temphandle = -1;
        }
        if (finalhandle != -1)
        {
                close (finalhandle);
                finalhandle = -1;
        }
        if (error)
        {
                remove (file2);
        }
        else
        {
                filecount++;
        }
        return error;
}

#define READ_AT_A_TIME 30000
        //max we will read at a time when a get-data is requested

static void getdata (PIC_PARM *pp)
{
        //reads more data into the Get queue
        //As written, this routine FILLs the Get queue.
        //This is not necessary; you can put in as much or as little as you like
        // each time.

        long ndo, ix;
        QUEUE *q;

        q = &pp->Get;
        if (total_bytes_read >= infilesize)
        {
                //the opcode requested too much data
                printf ("***** getdata -- attempt to read past end of file\n");
                fail (pp, 1);
        }
        ndo = bytes_free_in_queue (q);
        if (ndo <= 0)
        {
                return; //presumably a yield call
        }
        ndo = min (ndo, infilesize - total_bytes_read);
        if (ndo + q->Rear <= q->End)
        {
                //all at once
                readdata (q->Rear, ndo, pp);
                q->Rear += ndo;
                if (q->Rear >= q->End)
                {
                        q->Rear = q->Start;
                }
        }
        else
        {
                //in 2 chunks
                readdata (q->Rear, ix = (q->End - q->Rear), pp);
                readdata (q->Start, ix = (ndo - ix), pp);
                q->Rear = q->Start + ix;
        }
        total_bytes_read += ndo;
}

static void putdata (PIC_PARM *pp)
{
        //write data from the put queue to the output file

        long avail, ix;
        QUEUE *q;

        q = &pp->Put;
        if ((avail = bytes_in_queue(q)) <= 0)
        {
                return; //presumably due to yield
        }
        //something there -- it may wrap
        if (avail + q->Front <= q->End)
        {
                //all in one chunk
                writedata (q->Front, avail, pp);
                q->Front += avail;
                if (q->Front >= q->End)
                {
                        q->Front = q->Start;
                }
        }
        else
        {
                writedata (q->Front, ix = (q->End - q->Front), pp);
                writedata (q->Start, ix = (avail - ix), pp);
                q->Front = q->Start + ix;
        }
        return;
}

static void writedata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp)
{
        //do ~30K at a time
        short ndo;

        while (nbytes > 0)
        {
                if (nbytes > 30000)
                {
                        ndo = 30000;
                }
                else
                {
                        ndo = (short)nbytes;
                }
                if (write (temphandle, address, ndo) != ndo)
                {
                        printf ("***** writedata -- error writing output file\n");
                        fail (pp, 1);
                }
                nbytes -= ndo;
                address += ndo;
        }
        return;
}

static void readdata (BYTE PICHUGE *address, long nbytes, PIC_PARM *pp)
{
        //do ~30K at a time
        short ndo;

        while (nbytes > 0)
        {
                if (nbytes > 30000)
                {
                        ndo = 30000;
                }
                else
                {
                        ndo = (short)nbytes;
                }
                if (read (inhandle, address, ndo) != ndo)
                {
                        printf ("***** readdata -- error reading input file\n");
                        fail (pp, 1);
                }
                nbytes -= ndo;
                address += ndo;
        }
        return;
}

static void fail (PIC_PARM *pp, long error_code)
{
        //terminates Pegasus and sets error code

        Pegasus (pp, REQ_TERM);
        pp->Status = error_code;
        if (inhandle != -1)
        {
                close (inhandle);
                inhandle = -1;
        }
        if (temphandle != -1)
        {
                close (temphandle);
                temphandle = -1;
        }
        if (finalhandle != -1)
        {
                close (finalhandle);
                finalhandle = -1;
        }
        remove (TEMPFILE);
        remove (outfilename);
        printf ("!!!!Error -- code %ld\n", error_code);
        return;
}

//FIXUP is necessary because all routines in this module always assume
// that the Front and Rear pointers point to a location WITHIN the ring,
// but it is not terribly hard to imagine someone leaving one of those
// pointers pointing just past the end of the buffer

#define FIXUP(q) \
        if (q->Front >= q->End) \
        { \
                q->Front = q->Start; \
        } \
        if (q->Rear >= q->End) \
        { \
                q->Rear = q->Start; \
        }

PUBLIC long bytes_in_queue (QUEUE *q)
{
        //returns number of bytes in a queue

        long ix;

        FIXUP (q);
        if (q->Front == q->Rear)
        {
                return 0;
        }
        if ((ix = (q->Rear - q->Front)) > 0)
        {
                return ix;
        }
        return (q->End - q->Front) + (q->Rear - q->Start);
}

PUBLIC long bytes_free_in_queue (QUEUE *q)
{
        //returns amount of space available in a queue

        long ix;

        FIXUP (q);
        ix = q->Rear - q->Front;
        if (ix >= 0)
        {
                return (q->End - q->Start) - ix - 1;
        }
        else
        {
                return -ix - 1;
        }
}

PRIVATE void getdata_seek (PIC_PARM *pp)
{
        QUEUE *q;

        q = &pp->Get;
        if (pp->u.F2D.AllocType)        //if entire file there, just set pointers
        {
                if (pp->SeekInfo & SEEK_DIRECTION)
                {
                        //from end of file
                        q->Front = q->End - (pp->SeekInfo & SEEK_OFFSET);
                }
                else
                {
                        //from beginning of file
                        q->Front = q->Start + (pp->SeekInfo & SEEK_OFFSET);
                }
        }
        else
        {
                //file was in chunks -- empty the buffer and seek the file
                q->Front = q->Rear = q->Start;
                if ((pp->SeekInfo & SEEK_DIRECTION) == 0)
                {
                        if (lseek (inhandle, (pp->SeekInfo & SEEK_OFFSET),
                                SEEK_SET) < 0)
                        {
                                printf ("***** getdata -- seek error # 1\n");
                                fail (pp, 1);
                        }
                        total_bytes_read = pp->SeekInfo & SEEK_OFFSET;
                }
                else
                {
                        if (lseek (inhandle, -(long)(pp->SeekInfo & SEEK_OFFSET),
                                SEEK_END) < 0)
                        {
                                printf ("***** getdata -- seek error # 2\n");
                                fail (pp, 1);
                        }
                        total_bytes_read = infilesize - (pp->SeekInfo & SEEK_OFFSET);
                }
        }
        return;
}

PRIVATE void putdata_seek (PIC_PARM *pp)
{       
        //flush output data
        putdata (pp);

        //do a seek
        if ((pp->SeekInfo & SEEK_DIRECTION) == 0)
        {
                //we may be asked to seek beyond the current end of the file.
                // if so, need to extend the file
                if (filelength (temphandle) < (long)(pp->SeekInfo & SEEK_OFFSET))
                {
                        chsize (temphandle, (pp->SeekInfo & SEEK_OFFSET));
                }
                if (lseek (temphandle, (pp->SeekInfo & SEEK_OFFSET), SEEK_SET) < 0)
                {
                        printf ("***** putdata -- seek error # 1\n");
                        fail (pp, 1);
                }
        }
        else
        {
                if (lseek (temphandle, -(long)(pp->SeekInfo & SEEK_OFFSET), SEEK_END)
                        < 0)
                {
                        printf ("***** putdata -- seek error # 2\n");
                        fail (pp, 1);
                }
        }
        return;
}
