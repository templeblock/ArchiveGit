/*****************************************************************************

          Copyright (c) Micrografx, Inc., 1992.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
             from Micrografx, 1303 Arapaho, Richardson, TX. 75081.

******************************************************************************
******************************************************************************

                                   drive.c

******************************************************************************
******************************************************************************

History:

    16-Sep-92:  Created. (RSW)

*****************************************************************************/

/********************************** Includes ********************************/

#include <windows.h>
#include "types.h"
#include "data.h"
#include "routines.h"

#ifdef __cplusplus
extern "C" {
void FAR PASCAL DOS3Call (void);
}
#else
void FAR PASCAL DOS3Call (void);
#endif
/********************************* Constants ********************************/
/*********************************** Macros *********************************/
/*********************************** Types **********************************/
typedef struct
{
    BYTE    DriveNumber;
    BYTE    DeviceDriverUnit;
    WORD    BytesPerSector;
    BYTE    SectorsPerCluster;
    BYTE    ShiftFactor;
    WORD    ReservedBootSectors;
    BYTE    FatCopies;
    WORD    RootDirEntries;
    WORD    FirstDataSector;
    WORD    HighestCluster;
    BYTE    SectorsPerFat;
    WORD    RootDirStartingSector;
    DWORD   DeviceDriverAddress;
    BYTE    Media2and3;         // media descriptor here in DOS 2.x and 3.x
    BYTE    Media4;             // media descriptor here in DOS 4.x
    DWORD   NextDeviceParamBlock;
} PARAMBLOCK, far *LPPARAMBLOCK;

/********************************* Local Data *******************************/
/******************************* Exported Data ******************************/
/****************************** Local Functions *****************************/

static LPPARAMBLOCK GetParamBlockPtr (int DriveNumber)
{
#ifdef WIN32
	return NULL;
#else
    LPPARAMBLOCK lpParamBlock = NULL;

    _asm    mov     ah,032h         // Get pointer to drive parameter block
    _asm    mov     dl,byte ptr DriveNumber
    _asm    inc     dl              // 0 means current disk for int 32h
    _asm    push    ds
    DOS3Call ();
    _asm    mov     dx,ds
    _asm    pop     ds
    _asm    cmp     al,0FFh
    _asm    je      dcDone
    _asm    mov     word ptr lpParamBlock[0],bx
    _asm    mov     word ptr lpParamBlock[2],dx
dcDone:
    return lpParamBlock;
#endif	
}

static BOOL IsRamDisk (int DriveNumber)
{
    LPPARAMBLOCK lpParamBlock = GetParamBlockPtr (DriveNumber);

    return lpParamBlock && lpParamBlock->FatCopies==1;
}

static BOOL IsCDROM (int DriveNumber)
{
#ifdef WIN32
	return FALSE;
#else
    BOOL bCDfound = FALSE;

    _asm    mov     ax,01500h
    _asm    xor     bx,bx
    _asm    int     02Fh
    _asm    or      bx,bx
    _asm    jz      Done
    _asm    mov     ax,0150bh
    _asm    mov     cx,DriveNumber
    _asm    int     02Fh
    _asm    or      ax,ax
    _asm    jz      Done
    bCDfound = TRUE;
Done:
    return bCDfound;
#endif
}

/****************************** Public Functions ****************************/

/*[2*************************************************************************
 *
 *        Name:  GetExtendedDriveType
 *
 *     Purpose:  Determines the type of the specified drive.  Extends
 *               standard Windows GetDriveType by identifying RAM drives and
 *               CD ROM drives.
 *
 *  Parameters:  Drive - Drive Number to check (A=0,B=1,etc...)
 *                       If Drive is an alphabetic character,
 *                       then it will be automatically converted to
 *                       the correct drive number (e.g. 'B' is same as 1,
 *                       'c' is same as 2).
 *
 *     Returns:  0 for invalid or unknown drive, or one of the following:
 *               DRIVE_REMOVABLE        // Same as Windows
 *               DRIVE_FIXED            // Same as Windows
 *               DRIVE_REMOTE           // Same as Windows
 *               DRIVE_CDROM            // Extended Type
 *               DRIVE_RAM              // Extended Type
 *
 *************************************************************************{]*/
#ifndef STATIC16 // only in new framelib

UINT GetExtendedDriveType (int Drive)
{
    UINT DriveType;

    if (Drive>='A' && Drive<='Z')
        Drive -= 'A';
    else if (Drive>='a' && Drive<='z')
        Drive -= 'a';

    DriveType = GetDriveType (Drive);
    switch (DriveType)
    {
    case DRIVE_FIXED:
        if (IsRamDisk (Drive))  DriveType = DRIVE_RAM;
        break;
    case DRIVE_REMOTE:
        if (IsCDROM (Drive))    DriveType = DRIVE_CDROM;
        break;
    }
    return DriveType;
}

#endif

/****************************************************************************/
