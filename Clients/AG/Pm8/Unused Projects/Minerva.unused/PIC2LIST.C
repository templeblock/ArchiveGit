/*.T  PIC2List.C
 * $Header: /Minerva/pic2list.c 21    10/27/98 2:51p Jimbean $Nokeywords: $
 */

/*****************************************************************************/
/*      Copyright (c) 1998, Pegasus Imaging Corporation                      */
/*      All rights reserved.                                                 */
/*****************************************************************************/

#pragma pack(1)

#include "stdlib.h"
#include "stddef.h"
#include "memory.h"
#include "string.h"

/* the following are from pegasus\include directory */
#pragma warning(disable:4115 4201 4214 4001)
#include "pic.h"
#pragma warning(default:4115 4201 4214)
#include "errors.h"
#include "tiff.h"



#include "pic2file.h"   /* pic2list packet types and packet definitions */
#include "pic2list.h"   /* pic2list.c prototypes and P2LIST definition  */



#define REALLOC_BUMP    ( 4096 )

#if defined(WINDOWS) && !defined(__FLAT__) && !defined(OS2)
/* cast Win16 lengths to 16-bit */
#define realloc(x, y)   (realloc((x), (WORD)(y)))
#define memcpy(x, y, z) (memcpy((x), (y), (WORD)(z)))
#define memset(x, y, z) (memset((x), (y), (WORD)(z)))
#define strlen(x)       (lstrlen((x)))
#define strcpy(x, y)    (lstrcpy((x), (y)))
#endif




#define NOTREFERENCED(x)    ( x = x )




/*****************************************************************************
*    typedef struct
*    {
*        BYTE* list;
*        DWORD size;
*        DWORD len;
*    } P2LIST
*
*    P2LIST is defined in such a way that, if pp is PIC_PARM*, you can pass
*    (P2LIST*)&pp->PIC2List to the following functions
******************************************************************************/



/*****************************************************************************
 *  BOOL P2LValid(P2LIST* p2l)
 *
 *  validity-checking of PIC2List data
 *  packet types are not validated
 *****************************************************************************/
BOOL P2LValid(const P2LIST* p2l)
{
    P2PktGeneric* pkt;

    if ( p2l->size < 0 ||
         p2l->len < 0 ||
         p2l->size < p2l->len ||
         ( p2l->size == 0 && p2l->list != 0 ) ||
         ( p2l->size != 0 && p2l->list == 0 ) )
        return ( FALSE );
    if ( p2l->len == 0 )
        return ( TRUE );
    pkt = (P2PktGeneric*)p2l->list;
    while ( pkt->Type != P2P_EOF )
    {
        if ( (DWORD)( p2l->list + p2l->len - 1 - pkt->Data ) < pkt->Length )
            return ( FALSE );
        pkt = (P2PktGeneric*)&pkt->Data[pkt->Length];
    }
    return ( (BYTE*)pkt + 1 - p2l->list == p2l->len );
}

    

/*****************************************************************************
 *  BOOL P2LAlloc(P2LIST* p2l, DWORD size)
 *
 *  (re)allocate PIC2List to a particular, larger, size
 *****************************************************************************/
BOOL P2LAlloc(P2LIST* p2l, DWORD size)
{
    BYTE *pb;

    if ( p2l->size < (long)size )
    {
        pb = (BYTE*)realloc(p2l->list, size);
        if ( pb == 0 )
            return ( FALSE );
        p2l->size = size;
        p2l->list = pb;
    }
    return ( TRUE );
}

    

/*****************************************************************************
 *  void P2LFree(P2LIST* p2l)
 *
 *  frees PIC2List
 *****************************************************************************/
void P2LFree(P2LIST* p2l)
{
    free(p2l->list);
    p2l->list = 0;
    p2l->len = 0;
    p2l->size = 0;
}

    

/*****************************************************************************
 *  P2PktGeneric* P2LAdd(P2LIST* p2l, BYTE type, DWORD len, BYTE* data)
 *
 *  add a PIC2List packet of type with data of length len
 *  if data == 0, then the packet is allocated and the data is set to 0
 *  returns 0 if a memory allocation error occurs and on an attempt ot add
 *      a P2P_EOF packet
 *****************************************************************************/
P2PktGeneric* P2LAdd(P2LIST* p2l, BYTE type, DWORD len, const BYTE* data)
{
    P2PktGeneric *pkt;
    LONG p2len;

    if ( type == P2P_EOF )
        return ( 0 );
    p2len = p2l->len;
    if ( p2len != 0 )
        p2len--;
    if ( (DWORD)( p2l->size - p2l->len ) < sizeof(P2PktNull) + len )
    {
        if ( !P2LAlloc(p2l, p2l->len + sizeof(P2PktNull) + len + REALLOC_BUMP) )
            return ( 0 );
    }
    pkt = (P2PktGeneric*)( (BYTE*)p2l->list + p2len );
    pkt->Type = type;
    pkt->Length = len;
    if ( data != 0 )
        memcpy(pkt->Data, data, len);
    else
        memset(pkt->Data, 0, len);
    pkt->Data[len] = 0;     /* P2P_EOF */
    p2l->len = &pkt->Data[len + 1] - p2l->list;
    return ( pkt );
}



/*****************************************************************************
 *  BOOL P2LDelete(P2LIST* p2l, P2PktGeneric *pkt)
 *
 *  delete a packet from the PIC2List
 *  returns 0 on an attempt to delete a P2P_EOF packet
 *****************************************************************************/
BOOL P2LDelete(P2LIST* p2l, P2PktGeneric* pkt)
{
    DWORD pktlen;
    BYTE* pb = (BYTE*)pkt;

    if ( pkt->Type == P2P_EOF )
        return ( FALSE );
    pktlen = sizeof(P2PktNull) + pkt->Length;
    memcpy(pb, pb + pktlen, ( p2l->list + p2l->len ) - ( pb + pktlen ));
    p2l->len -= pktlen;
    return ( TRUE );
}



/*****************************************************************************
 *  P2PktGeneric* P2LFirst(P2LIST* p2l)
 *
 *  return a pointer to the first packet in the PIC2List
 *****************************************************************************/
P2PktGeneric* P2LFirst(const P2LIST* p2l)
{
    if ( p2l->len == 0 || ((P2PktGeneric*)p2l->list)->Type == P2P_EOF )
        return ( 0 );
    return ( (P2PktGeneric*)p2l->list );
}



/*****************************************************************************
 *  P2PktGeneric* P2LNext(P2LIST* p2l, P2PktGeneric* pkt)
 *
 *  return a pointer to the next packet after pkt in the PIC2List
 *****************************************************************************/



/*****************************************************************************
 *  P2PktGeneric* P2LFind(P2LIST* p2l, BYTE type)
 *
 *  return a pointer to the first type packet in the PIC2List
 *****************************************************************************/
P2PktGeneric* P2LFind(const P2LIST* p2l, BYTE type)
{
    P2PktGeneric* pkt;

    if ( type == P2P_EOF )
        return ( 0 );
    pkt = P2LFirst(p2l);
    while ( pkt != 0 && pkt->Type != type )
        pkt = P2LNext(p2l, pkt);
    return ( pkt );
}



/*****************************************************************************
 *  P2PktGeneric* P2LFindNext(P2LIST* p2l, P2PktGeneric* pkt)
 *
 *  return a pointer to the next type packet after pkt in the PIC2List
 *****************************************************************************/
P2PktGeneric* P2LFindNext(const P2LIST* p2l, const P2PktGeneric* pkt)
{
    BYTE type = pkt->Type;

    NOTREFERENCED(p2l);
    do
    {
        pkt = P2LNext(p2l, pkt);
    }
    while ( pkt != 0 && pkt->Type != type );
    return ( (P2PktGeneric*)pkt );
}



/*****************************************************************************
 *  P2PktRegistration* P2LAddOrModifyRegistration(P2LIST* p2l, OPERATION Op, DWORD Code, const char* pszName)
 *
 *  add or modify registration packet for Op
 *  returns 0 if a memory allocation error occurs
 *****************************************************************************/
P2PktRegistration* P2LAddOrModifyRegistration(P2LIST* p2l, OPERATION Op, DWORD Code, const char* pszName)
{
    P2PktRegistration* pkt = P2LFindRegistration(p2l, Op);

    if ( pkt != 0 && !P2LDeleteRegistration(p2l, pkt) )
        return ( 0 );
    pkt = (P2PktRegistration*)P2LAdd(p2l, P2P_Registration, sizeof(P2PktRegistration) + strlen(pszName), 0);
    if ( pkt == 0 )
        return ( 0 );
    pkt->Op = Op;
    pkt->Code = Code;
    strcpy(pkt->Name, pszName);
    return ( pkt );
}



/*****************************************************************************
 *  BOOL P2LDeleteRegistration(P2LIST* p2l, P2PktRegistration *pkt)
 *
 *  delete a registration packet from the PIC2List
 *****************************************************************************/



/*****************************************************************************
 *  P2PktRegistration* P2LFindRegistration(P2LIST* p2l, OPERATION Op)
 *
 *  return a pointer to the first registration packet for Op
 *****************************************************************************/
P2PktRegistration* P2LFindRegistration(P2LIST *p2l, OPERATION Op)
{
    P2PktRegistration* pkt = (P2PktRegistration*)P2LFind(p2l, P2P_Registration);
    while ( pkt != 0 && pkt->Op != Op )
        pkt = (P2PktRegistration*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    return ( pkt );
}



/*****************************************************************************
 *  P2PktRawData* P2LAddRawData(P2LIST* p2l, char *desc, DWORD len, BYTE* data)
 *
 *  add a RawData PIC2List packet of type desc[4] with data of length len
 *  if data == 0, then the packet is allocated and the data is set to 0
 *  returns 0 if a memory allocation error occurs and on an attempt ot add
 *      a P2P_EOF packet
 *****************************************************************************/
P2PktRawData* P2LAddRawData(P2LIST* p2l, const char *desc, DWORD len, const BYTE* data)
{
    P2PktRawData* pkt = (P2PktRawData*)P2LAdd(p2l, P2P_RawData, sizeof(P2PktRawData) - sizeof(P2PktGeneric) + len, 0);
    if ( pkt != 0 )
    {
        memcpy(pkt->RawDescription, desc, sizeof(pkt->RawDescription));
        pkt->RawLength = len;
        if ( data != 0 )
            memcpy(pkt->RawData, data, len);
        else
            memset(pkt->RawData, 0, len);
    }
    return ( pkt );
}



/*****************************************************************************
 *  BOOL P2LDeleteRawData(P2LIST* p2l, P2PktRawData *pkt)
 *
 *  delete a raw data packet from the PIC2List
 *****************************************************************************/

 
 
/*****************************************************************************
 *  P2PktRawData* P2LFindRawData(P2LIST* p2l, char* desc)
 *
 *  return a pointer to the first RawData PIC2List packet of type desc[4]
 *****************************************************************************/
P2PktRawData* P2LFindRawData(const P2LIST* p2l, const char* desc)
{
    DWORD xdesc = *(DWORD*)desc;
    P2PktRawData* pkt = (P2PktRawData*)P2LFind(p2l, P2P_RawData);
    while ( pkt != 0 && xdesc != *(DWORD*)pkt->RawDescription )
        pkt = (P2PktRawData*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    return ( pkt );
}



/*****************************************************************************
 *  P2PktRawData* P2LFindNextRawData(P2LIST* p2l, P2PktRawData* pkt)
 *
 *  return a pointer to the next RawData PIC2List packet of type desc[4] and
 *      following pkt
 *****************************************************************************/
P2PktRawData* P2LFindNextRawData(const P2LIST* p2l, const P2PktRawData* pkt)
{
    DWORD xdesc = *(DWORD*)pkt->RawDescription;
    do
    {
        pkt = (P2PktRawData*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    }   
    while ( pkt != 0 && xdesc != *(DWORD*)pkt->RawDescription );
    return ( (P2PktRawData*)pkt );
}



static BYTE TypeSize[] = { 1, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8 };



/*****************************************************************************
 *  P2PktTiffTag* P2LAddTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, BYTE* data)
 *
 *  add a TiffTag PIC2List packet for IFD location loc
 *  if data == 0, then the packet is allocated and the data is set to 0
 *  returns 0 if a memory allocation error occurs or if a packet with the same
 *      tag is already in the PIC2List for IFD location loc
 *****************************************************************************/
P2PktTiffTag* P2LAddTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data)
{
    P2PktTiffTag* pkt;
    DWORD len;
    if ( type > sizeof(TypeSize) / sizeof(TypeSize[0]) )
        return ( 0 );
    if ( P2LFindTiffTag(p2l, loc, tag) != 0 )
        return ( 0 );
    if ( count == (DWORD)-1 && TypeSize[type] == 1 )
    {
        if ( data == 0 )
            return ( 0 );
        count = len = strlen((const char*)data) + 1;
    }
    else
        len = count * TypeSize[type];
    pkt = (P2PktTiffTag*)P2LAdd(p2l, P2P_TiffTag, sizeof(P2PktTiffTag) - sizeof(P2PktGeneric) + len, 0);
    if ( pkt != 0 )
    {
        pkt->Location = loc;
        pkt->TiffTag = tag;
        pkt->TiffType = type;
        pkt->TiffCount = count;
        if ( data != 0 )
            memcpy(pkt->TiffData, data, len);
        else
            memset(pkt->TiffData, 0, len);
    }
    return ( pkt );
}



/*****************************************************************************
 *  P2PktTiffTag* P2LModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, BYTE* data)
 *
 *  modify a TiffTag PIC2List packet for IFD location loc
 *  if data == 0, then the packet is allocated and the data is set to 0
 *  returns 0 if a memory allocation error occurs or if a packet with the same tag
 *      not found in the PIC2List for IFD location loc
 *****************************************************************************/
P2PktTiffTag* P2LModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data)
{
    P2PktTiffTag *pold = P2LFindTiffTag(p2l, loc, tag);
    if ( pold == 0 || !P2LDeleteTiffTag(p2l, pold) )
        return ( 0 );
    return ( P2LAddTiffTag(p2l, loc, tag, type, count, data) );
}



/*****************************************************************************
 *  P2PktTiffTag* P2LAddOrModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, BYTE* data)
 *
 *  add or modify a TiffTag PIC2List packet for IFD location loc
 *  if data == 0, then the packet is allocated and the data is set to 0
 *  returns 0 if a memory allocation error occurs
 *****************************************************************************/
P2PktTiffTag* P2LAddOrModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data)
{
    P2PktTiffTag *pold = P2LFindTiffTag(p2l, loc, tag);
    if ( pold != 0 && !P2LDeleteTiffTag(p2l, pold) )
        return ( 0 );
    return ( P2LAddTiffTag(p2l, loc, tag, type, count, data) );
}



/*****************************************************************************
 *  BOOL P2LDeleteTiffTag(P2LIST* p2l, P2PktRawData *pkt)
 *
 *  delete a tiff tag packet from the PIC2List
 *****************************************************************************/

 
 
/*****************************************************************************
 *  P2PktTiffTag* P2LFirstTiffTag(P2LIST* p2l, BYTE loc)
 *
 *  returns a pointer to the first TiffTag packet for IFD location loc in the PIC2List
 *****************************************************************************/
P2PktTiffTag* P2LFirstTiffTag(const P2LIST* p2l, BYTE loc)
{
    P2PktTiffTag* pkt = (P2PktTiffTag*)P2LFind(p2l, P2P_TiffTag);
    while ( pkt != 0 && pkt->Location != loc )
        pkt = (P2PktTiffTag*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    return ( pkt );
}



/*****************************************************************************
 *  P2PktTiffTag* P2LNextTiffTag(P2LIST* p2l, P2PktTiffTag* pkt)
 *
 *  returns a pointer to the next TiffTag packet for IFD location loc in the PIC2List
 *      and following pkt
 *****************************************************************************/
P2PktTiffTag* P2LNextTiffTag(const P2LIST* p2l, const P2PktTiffTag* pkt)
{
    BYTE loc = pkt->Location;
    do
    {
        pkt = (P2PktTiffTag*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    }
    while ( pkt != 0 && pkt->Location != loc );
    return ( (P2PktTiffTag*)pkt );
}



/*****************************************************************************
 *  P2PktTiffTag* P2LFindTiffTag(P2LIST* p2l, BYTE loc, WORD tag)
 *
 *  returns a pointer to the first TiffTag packet with tag for IFD location loc
 *      in the PIC2List
 *****************************************************************************/
P2PktTiffTag* P2LFindTiffTag(const P2LIST* p2l, BYTE loc, WORD tag)
{
    P2PktTiffTag* pkt = (P2PktTiffTag*)P2LFind(p2l, P2P_TiffTag);
    while ( pkt != 0 && ( pkt->TiffTag != tag || pkt->Location != loc ) )
        pkt = (P2PktTiffTag*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    return ( pkt );
}



/*****************************************************************************
 *  P2PktTiffTag* P2LFindNextTiffTag(P2LIST* p2l, P2PktTiffTag* pkt)
 *
 *  returns a pointer to the next TiffTag packet PIC2List which follows pkt
 *      and has the same IFD location and tag as packet
 *****************************************************************************/
P2PktTiffTag* P2LFindNextTiffTag(const P2LIST* p2l, const P2PktTiffTag* pkt)
{
    BYTE loc = pkt->Location;
    WORD tag = pkt->TiffTag;
    do
    {
        pkt = (P2PktTiffTag*)P2LFindNext(p2l, (P2PktGeneric*)pkt);
    }   
    while ( pkt != 0 && ( pkt->TiffTag != tag || pkt->Location != loc ) );
    return ( (P2PktTiffTag*)pkt );
}



/*****************************************************************************
 *  DWORD P2LTiffNthData(P2PktTiffTag* pkt, DWORD n)
 *
 *  returns TiffTag data for the Nth TiffTag array element
 *****************************************************************************/
DWORD P2LTiffNthData(const P2PktTiffTag* pkt, DWORD n)
{
    if ( n > pkt->TiffCount )
        return ( (DWORD)-1 );

    switch ( pkt->TiffType )
    {
    case TIF_BYTE:
        return ( pkt->TiffData[n] );

    case TIF_SBYTE:
                  return ( (signed char)pkt->TiffData[n] );

    case TIF_ASCII:
    case TIF_UNDEFINED:
        return ( (DWORD)(BYTE PICHUGE*)pkt->TiffData );

    case TIF_SHORT:
        return ( *( (WORD*)pkt->TiffData + n ) );

    case TIF_SSHORT:
        return ( *( (short*)pkt->TiffData + n ) );

    case TIF_LONG:
        return ( *( (DWORD*)pkt->TiffData + n ) );

    case TIF_SLONG:
        return ( *( (long*)pkt->TiffData + n ) );

    case TIF_RATIONAL:
    case TIF_SRATIONAL:
    case TIF_FLOAT:
    case TIF_DOUBLE:
        return ( (DWORD)(BYTE PICHUGE*)pkt->TiffData );
    }
         return ( 0 );
}



/*****************************************************************************
 *  DWORD P2LTiffCount(P2PktTiffTag* pkt)
 *
 *  returns TiffTag data count
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffType(P2PktTiffTag* pkt)
 *
 *  returns TiffTag data type
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffTag(P2PktTiffTag* pkt)
 *
 *  returns TiffTag tag
 *****************************************************************************/

 
 
/*****************************************************************************
 *  DWORD P2LTiffLocation(P2PktTiffTag* pkt)
 *
 *  returns TiffTag data location
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffData(P2PktTiffTag* pkt)
 *
 *  returns TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  BYTE P2LTiffByte(P2PktTiffTag* pkt)
 *
 *  returns BYTE TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  WORD P2LTiffShort(P2PktTiffTag* pkt)
 *
 *  returns SHORT TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffLong(P2PktTiffTag* pkt)
 *
 *  returns LONG TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffRationalNumerator(P2PktTiffTag* pkt)
 *
 *  returns Numerator for RATIONAL TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  DWORD P2LTiffRationalDenominator(P2PktTiffTag* pkt)
 *
 *  returns Denominator for RATIONAL TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  signed char P2LTiffSByte(P2PktTiffTag* pkt)
 *
 *  returns SBYTE TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  short P2LTiffSShort(P2PktTiffTag* pkt)
 *
 *  returns SSHORT TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  long P2LTiffSLong(P2PktTiffTag* pkt)
 *
 *  returns SLONG TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  long P2LTiffSRationalNumerator(P2PktTiffTag* pkt)
 *
 *  returns Numerator for SRATIONAL TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  long P2LTiffSRationalDenominator(P2PktTiffTag* pkt)
 *
 *  returns Denominator for SRATIONAL TiffTag data
 *****************************************************************************/



/*****************************************************************************
 *  const char* P2LTiffAscii(P2PktTiffTag* pkt)
 *
 *  returns pointer to TiffTag asciiz string data
 *****************************************************************************/



/*****************************************************************************
 *  const BYTE* P2LTiffUndefined(P2PktTiffTag* pkt, DWORD &len)
 *
 *  returns pointer to TiffTag undefined data and returns length of the data
 *****************************************************************************/



#pragma warning(disable:4514)
