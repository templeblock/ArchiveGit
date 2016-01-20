//op80tst.c

//QuickWin test program for opcode 80
//Opcode 80 converts a dib to a bmp, tga, pcx, or tif file.
//Since a dib is very close to a BMP, this program actually takes
// bmp files as input.

//compiled using Microsoft Visual C++ 1.52c

//NOTE that there is little or no error checking on reads, writes, seeks, etc.

//NOTE that a 24-bmp file with an included palette is not properly handled
// by this sample program, as well as any weird bmp file that has extra
// information between the BITMAPINFO structure and the dib bits area.

#ifndef OPCODE_D2F
#define OPCODE_D2F OP_D2F
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

DWORD image_type;       //we place here the type of output file requested
        //contains BI_RGB, BI_TGA, BI_TIF, BI_GIF or BI_PCX

char infilename[300], outfilename[300];
        //the built-up input and output file names
        
int filecount;  //number of images converted

int main (int argc, char *argv[])
{
        long iresult;
        int done;
                      
        printf ("Enter source .bmp files (wildcards allowed): ");
        gets (infilelist);
        printf ("Enter destination directory: ");
        gets (outdirectory);
get_image_type:
#if OPCODE_D2F == OP_D2FPLUS
        printf ("Do you wish to create BMP, TGA, PCX, GIF or TIF output files? ");
#else
        printf ("Do you wish to create BMP, TGA, PCX, or TIF output files? ");
#endif

        gets (answer);
        if (stricmp (answer, "bmp") == 0)
        {
                image_type = BI_RGB;
        }
        else if (stricmp (answer, "tga") == 0)
        {
                image_type = BI_TGA;
        }
        else if (stricmp (answer, "pcx") == 0)
        {
                image_type = BI_PCX;
        }
        else if (stricmp (answer, "tif") == 0)
        {
                image_type = BI_TIF;
        }
#if OPCODE_D2F == OP_D2FPLUS
        else if (stricmp (answer, "gif") == 0)
        {
                image_type = BI_GIFN;
        }
#endif
        else
        {
                goto get_image_type;
        }

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
                strcat (outfilename, ".");
                strcat (outfilename, answer);   //append the new extension

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
static int outhandle = -1;
        //the file handles

static long infilesize;
        //these are needed by the getdata callback
static int getdata_already_called;
        //when we put the entire file in the input buffer, this makes
        // sure getdata is not called again asking for more data!
static long total_bytes_read = 0;
        //how many bytes we getdata'd from the input file
static long seek_adder;
        //input file offset
        
static long convert_file (char *file1, char *file2)
{
        long error = 1;
        long n_input_colors;
        RESPONSE res;

        inhandle = -1;
        outhandle = -1;

        //open the files
        inhandle = open (file1, O_RDONLY | O_BINARY);
        if (inhandle == -1)
        {
                printf ("***** unable to open input image %s\n", file1);
                goto ehandler;
        }
        outhandle = open (file2, O_RDWR | O_BINARY | O_CREAT | O_TRUNC,
                S_IREAD | S_IWRITE);
        if (outhandle == -1)
        {
                printf ("***** unable to create output file %s\n", file2);
                goto ehandler;
        }

        infilesize = filelength (inhandle);
        total_bytes_read = 0;

        //set up for opcode 80
        memset (&pp, 0, sizeof (pp));   //VERY IMPORTANT!
        pp.Op = OPCODE_D2F;         //Dib to File (80)
        pp.ParmSize = sizeof (pp);
        pp.ParmVer = CURRENT_PARMVER;   //from pic.h

        //get the input file's bitmapinfoheader into pp.Head
        // -- skip the BITMAPFILEHEADER
        lseek (inhandle, sizeof (BITMAPFILEHEADER), SEEK_SET);
        read (inhandle, &pp.Head, sizeof (BITMAPINFOHEADER));
        seek_adder = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);
        //if the input file is < 24bpp, read in that palette
        if (pp.Head.biBitCount <= 8)
        {
                if (pp.Head.biClrUsed == 0)
                {
                        //0 means max colors
                        n_input_colors = (1 << pp.Head.biBitCount);
                }
                else
                {
                        n_input_colors = pp.Head.biClrUsed;
                }
                if (n_input_colors < 2 || n_input_colors > (1 << pp.Head.biBitCount))
                {
                        printf ("***** input palette has bad color count\n");
                        goto ehandler;
                }
                read (inhandle, pp.ColorTable, (unsigned short)n_input_colors << 2);
                seek_adder += (n_input_colors << 2);
        }
        //NOTE: at this point, seek_adder is the offset in the input file of
        // the dib bits

        pp.u.D2F.OutBpp = 24;   //output bit depth for TGA when input depth
                                                        // is 24 -- this could also be set to 16
        pp.u.D2F.ImageType = image_type;

        pp.Get.Start = input_queue;
        pp.Get.Front = pp.Get.Rear = pp.Get.Start;
        pp.Get.End = pp.Get.Start + inbufsize + 1;

        pp.Put.Start = output_queue;
        pp.Put.Front = pp.Put.Rear = pp.Put.Start;
        pp.Put.End = pp.Put.Start + outbufsize + 1;

        getdata_already_called = 0;
        lseek (inhandle, seek_adder, SEEK_SET);
                //seek to the beginning of the dib bits

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
        Pegasus (&pp, REQ_TERM);
        error = 0;

ehandler:
        if (inhandle != -1)
        {
                close (inhandle);
                inhandle = -1;
        }
        if (outhandle != -1)
        {
                close (outhandle);
                outhandle = -1;
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
        if (total_bytes_read >= infilesize - seek_adder)
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
        ndo = min (ndo, infilesize - seek_adder - total_bytes_read);
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
                if (write (outhandle, address, ndo) != ndo)
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
        if (outhandle != -1)
        {
                close (outhandle);
                outhandle = -1;
        }
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
        if (pp->u.D2F.AllocType)        //if entire file there, just set pointers
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
                        if (lseek (inhandle, (pp->SeekInfo & SEEK_OFFSET) + seek_adder,
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
                if (filelength (outhandle) < (long)(pp->SeekInfo & SEEK_OFFSET))
                {
                        chsize (outhandle, (pp->SeekInfo & SEEK_OFFSET));
                }
                if (lseek (outhandle, (pp->SeekInfo & SEEK_OFFSET), SEEK_SET) < 0)
                {
                        printf ("***** putdata -- seek error # 1\n");
                        fail (pp, 1);
                }
        }
        else
        {
                if (lseek (outhandle, -(long)(pp->SeekInfo & SEEK_OFFSET), SEEK_END)
                        < 0)
                {
                        printf ("***** putdata -- seek error # 2\n");
                        fail (pp, 1);
                }
        }
        return;
}
