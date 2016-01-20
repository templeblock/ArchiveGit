/***********************************************************************
 TWAIN Sample Source 
 Copyright (C) '91-'92 TWAIN Working Group: 
 Aldus, Caere, Eastman-Kodak, Logitech,
 Hewlett-Packard Corporations.
 All rights reserved.
*************************************************************************/

// Source file: dcd_com.c
VOID ExtractEnumerationValue (pTW_CAPABILITY pData, LPVOID pVoid, int j);
VOID BuildUpEnumerationType (pTW_CAPABILITY pData, pTW_ENUMERATION pE, void *pList);
VOID BuildUpArrayType (pTW_CAPABILITY pData, pTW_ARRAY pA, void *pList);
VOID BuildUpOneValue (pTW_CAPABILITY pData, TW_UINT16 ItemType, TW_UINT32 Item);
VOID ExtractOneValue (pTW_CAPABILITY pData, LPVOID pVoid);
TW_UINT16 AltDCItemSize(TW_INT16 ItemType);

