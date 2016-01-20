// @doc
// @module DocDefs.h - structures that help define a new document |
//
//  
//  <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
//

#ifndef DOCDEFS_H
#define DOCDEFS_H

#include "enums.h"

// width and height of A4 paper in TWIPS 
#define A4WIDTH  11906
#define A4HEIGHT 16838

//////////
// @struct SBDocConstants |
//         constants and recommendations used in document construction
//
// @devnote All sizes are expressed in our logical units (twips).
//
struct SBDocConstants
{
    EBProjectType eProject;     // @field type of project
    INT     nPageWidth;         // @field width of the page
    INT     nPageHeight;        // @field height of the page
    INT     nDefaultMargins;    // @field recommended margin settings
    INT     nMastheadHeight;    // @field height of masthead slab (0=not allowed)
    INT     nHeaderHeight;      // @field height of header slab (0=not allowed)
    INT     nFooterHeight;      // @field height of footer slab (0=not allowed)
    INT     nVerticalGap;       // @field vertical space between slabs
    INT     nColumnGap;         // @field horizontal space between columns
    INT     nMinColumns;        // @field minimum number of columns
    INT     nDefaultColumns;    // @field recommended number of columns
    INT     nMaxColumns;        // @field maximum number of columns
    INT     nMinPages;          // @field minimum number of pages
    INT     nDefaultPages;      // @field recommend number of pages
    INT     nMaxPages;          // @field maximum number of pages
    BOOL    bDefaultMasthead;   // @field recommend masthead TRUE or FALSE
    BOOL    bDefaultHeader;     // @field recommend header TRUE or FALSE
    BOOL    bDefaultFooter;     // @field recommend footer TRUE or FALSE
    BOOL    bDefaultLines;      // @field recommend lines between columns
};


//////////
// @struct SBDocDescription |
//         information required to create a document from scratch
//
// @devnote all sizes are expressed in our logical units (twips)
//
struct SBDocDescription
{
    EBProjectType eProject;     // @field type of project
    CSize   szPage;             // @field size of the page
    CRect   rcMargins;          // @field rectangle describing margins
    INT     nMastheadHeight;    // @field height of masthead slab
    INT     nHeaderHeight;      // @field height of header slab
    INT     nFooterHeight;      // @field height of footer slab
    INT     nVerticalGap;       // @field vertical space between slabs
    INT     nColumnGap;         // @field horizontal space between columns
    INT     nColumns;           // @field number of text columns
    INT     nPages;             // @field number of pages
    WORD    nHeaderStartPage;   // @field page number for first header (1 or 2)
    WORD    nFooterStartPage;   // @field page number for first footer (1 or 2)
    BOOL    bHasHeaders;        // @field if TRUE, document includes headers
    BOOL    bHasFooters;        // @field if TRUE, document includes footers
    BOOL    bHasMasthead;       // @field if TRUE, document includes masthead
    BOOL    bHasColumnLines;    // @field if TRUE, document has lines between columns

    // @cmember assign from a <t SBDocConstants> struct
    inline void operator =(const SBDocConstants &sInit)
    {
        eProject            = sInit.eProject;
        szPage.cx           = sInit.nPageWidth;         
        szPage.cy           = sInit.nPageHeight;
        rcMargins.top       = sInit.nDefaultMargins;
        rcMargins.left      = sInit.nDefaultMargins;
        rcMargins.right     = sInit.nPageWidth - sInit.nDefaultMargins;
        rcMargins.bottom    = sInit.nPageHeight - sInit.nDefaultMargins;
        nMastheadHeight     = sInit.nMastheadHeight;
        nHeaderHeight       = sInit.nHeaderHeight;
        nFooterHeight       = sInit.nFooterHeight;
        nVerticalGap        = sInit.nVerticalGap;
        nColumnGap          = sInit.nColumnGap;
        nColumns            = sInit.nDefaultColumns;
        nPages              = sInit.nDefaultPages;
        nHeaderStartPage    = (sInit.nHeaderHeight > 0) ? 1 : 0;
        nFooterStartPage    = (sInit.nFooterHeight > 0) ? 1 : 0;
        bHasHeaders         = sInit.bDefaultHeader;
        bHasFooters         = sInit.bDefaultFooter;
        bHasMasthead        = sInit.bDefaultMasthead;
        bHasColumnLines     = sInit.bDefaultLines;
    }
};

#endif  // DOCDEFS_H