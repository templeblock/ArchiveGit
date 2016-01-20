/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
/********
        Copyright (c) 1993 Microsoft Corporation.

        avihapp.h --

        Include this in your standalone application.  You MUST also define
        an exported procedure that takes two LPSTR as parameters, such
        as the following:

        BOOL __export CALLBACK hspAVICallback(LPSTR pszHotspotID, 
                                                LPSTR pszCommand)

        Pass the address of this function as the third parameter when you
        call hspPlayAVI, by using MakeProcAddress.
********/

LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR szAVIFile, LPSTR szIniFile, FARPROC);

