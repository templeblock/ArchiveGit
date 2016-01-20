/*
    pic2list.h
*/



#include "pic2file.h"



/* the fields must be ordered the same as in PIC.H PIC_PARM so
    that a pointer to PIC2List can be cast to a pointer to P2LIST */
typedef struct
{
    BYTE* list;
    LONG  size;
    LONG  len;
} P2LIST;



BOOL P2LValid(const P2LIST* p2l);
BOOL P2LAlloc(P2LIST* p2l, DWORD size);
void P2LFree(P2LIST* p2l);
P2PktGeneric* P2LAdd(P2LIST* p, BYTE type, DWORD len, const BYTE* data);
BOOL P2LDelete(P2LIST* p2l, P2PktGeneric* pkt);
#define P2LNext(p2l, p)             ( (p)->Data[(p)->Length] != P2P_EOF ? (P2PktGeneric*)&(p)->Data[(p)->Length] : 0 )
P2PktGeneric* P2LFirst(const P2LIST* p2l);
P2PktGeneric* P2LFind(const P2LIST* p2l, BYTE type);
P2PktGeneric* P2LFindNext(const P2LIST* p2l, const P2PktGeneric* pkt);

P2PktRegistration* P2LAddOrModifyRegistration(P2LIST* p2l, OPERATION Op, DWORD Code, const char* pszName);
#define P2LDeleteRegistration(p2l, pkt) ( P2LDelete(p2l, (P2PktGeneric*)(pkt)) )
P2PktRegistration* P2LFindRegistration(P2LIST *p2l, OPERATION Op);

P2PktRawData* P2LAddRawData(P2LIST* p2l, const char *desc, DWORD len, const BYTE* data);
#define P2LDeleteRawData(p2l, pkt)  ( P2LDelete(p2l, (P2PktGeneric*)(pkt)) )
#define P2LFirstRawData(p2l)        ( (P2PktRawData*)P2LFind(p2l, P2P_RawData) )
#define P2LNextRawData(p2l, pkt)    ( (P2PktRawData*)P2LFindNext(p2l, (const P2PktGeneric*)(pkt)) )
P2PktRawData* P2LFindRawData(const P2LIST* p2l, const char* desc);
P2PktRawData* P2LFindNextRawData(const P2LIST* p2l, const P2PktRawData* pkt);

P2PktTiffTag* P2LAddTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data);
P2PktTiffTag* P2LModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data);
P2PktTiffTag* P2LAddOrModifyTiffTag(P2LIST* p2l, BYTE loc, WORD tag, WORD type, DWORD count, const BYTE* data);
#define P2LDeleteTiffTag(p2l, pkt) ( P2LDelete(p2l, (P2PktGeneric*)(pkt)) )
P2PktTiffTag* P2LFirstTiffTag(const P2LIST* p2l, BYTE loc);
P2PktTiffTag* P2LNextTiffTag(const P2LIST* p2l, const P2PktTiffTag* pkt);
P2PktTiffTag* P2LFindTiffTag(const P2LIST* p2l, BYTE loc, WORD tag);
P2PktTiffTag* P2LFindNextTiffTag(const P2LIST* p2l, const P2PktTiffTag* pkt);

DWORD P2LTiffNthData(const P2PktTiffTag* pkt, DWORD n);

#define P2LTiffData(pkt)            ( P2LTiffNthData(pkt, 0) )
#define P2LTiffCount(pkt)           ( (pkt)->TiffCount )
#define P2LTiffType(pkt)            ( (pkt)->TiffType )
#define P2LTiffTag(pkt)             ( (pkt)->TiffTag )
#define P2LTiffLocation(pkt)        ( (pkt)->Location )

#define P2LTiffByte(pkt)            ( (BYTE)P2LTiffData(pkt) )
#define P2LTiffShort(pkt)           ( (WORD)P2LTiffData(pkt) )
#define P2LTiffLong(pkt)            ( (DWORD)P2LTiffData(pkt) )
#define P2LTiffRationalNumerator(pkt)    ( *(DWORD*)P2LTiffData(pkt) )
#define P2LTiffRationalDenominator(pkt)  ( *( (DWORD*)P2LTiffData(pkt) + 1 ) )

#define P2LTiffSByte(pkt)           ( (signed char)P2LTiffData(pkt) )
#define P2LTiffSShort(pkt)          ( (short)P2LTiffData(pkt) )
#define P2LTiffSLong(pkt)           ( (long)P2LTiffData(pkt) )
#define P2LTiffSRationalNumerator(pkt)   ( *(long*)P2LTiffData(pkt) )
#define P2LTiffSRationalDenominator(pkt) ( *( (long*)P2LTiffData(pkt) + 1 ) )

#define P2LTiffAscii(pkt)           ( (const char*)P2LTiffData(pkt) )
#define P2LTiffUndefined(pkt, plen) ( *plen = pkt->TiffCount, (const BYTE*)P2LTiffData(pkt) )
