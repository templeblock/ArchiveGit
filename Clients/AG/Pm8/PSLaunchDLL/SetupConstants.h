#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PressWriter support
#include "DocDefs.h"

////////////////////////////////////////////////
// array of PROJECT_NEWSLETTER default settings,
// one for each paper size and orientation
//
const SBDocConstants kNewsletterArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_NEWSLETTER, // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        1944, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_NEWSLETTER, // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        1944, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_NEWSLETTER, // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        1944, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_NEWSLETTER, // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        1944, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_NEWSLETTER, // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        1077,               // nDefaultMargins
        1928, 567, 567,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        340, 340,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_NEWSLETTER, // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        1077,               // nDefaultMargins
        1928, 567, 567,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        340, 340,           // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        TRUE, FALSE, FALSE, // bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};


//////////////////////////////////////////////
// array of PROJECT_BROCHURE default settings,
// one for each paper size and orientation
//
const SBDocConstants kBrochureArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_BROCHURE,   // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1440,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_BROCHURE,   // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1440,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_BROCHURE,   // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1440,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_BROCHURE,   // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1440,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_BROCHURE,   // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1417,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_BROCHURE,   // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        965,                // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 1417,            // nVerticalGap, nColumnGap
        1, 3, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};

////////////////////////////////////////////
// array of PROJECT_REPORT default settings,
// one for each paper size and orientation
//
const SBDocConstants kReportArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_REPORT,     // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1944, 360, 360,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        2160, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_REPORT,     // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1944, 360, 360,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        2160, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_REPORT,     // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1944, 360, 360,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        2160, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_REPORT,     // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1944, 360, 360,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        2160, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_REPORT,     // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        1814,               // nDefaultMargins
        1928, 340, 340,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        1417, 567,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_REPORT,     // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        1814,               // nDefaultMargins
        1928, 340, 340,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        1417, 567,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};

////////////////////////////////////////////
// array of PROJECT_BOOKLET default settings,
// one for each paper size and orientation
//
const SBDocConstants kBookletArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_BOOKLET,    // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2160,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_BOOKLET,    // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2160,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_BOOKLET,    // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2160,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_BOOKLET,    // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        1080,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2160,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_BOOKLET,    // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        1077,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2154,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_BOOKLET,    // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        1077,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 2154,            // nVerticalGap, nColumnGap
        1, 2, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};

////////////////////////////////////////////
// array of PROJECT_LETTER default settings,
// one for each paper size and orientation
//
const SBDocConstants kLetterArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_LETTER,     // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        2160,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 540,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_LETTER,     // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        2160,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 540,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_LETTER,     // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        2160,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 540,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_LETTER,     // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        2160,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 540,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_LETTER,     // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        2154,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 567,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_LETTER,     // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        2154,               // nDefaultMargins
        0, 0, 0,            // nMastheadHeight, nHeaderHeight, nFooterHeight
        0, 567,             // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 2, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};

////////////////////////////////////////////
// array of PROJECT_FLYER default settings,
// one for each paper size and orientation
//
const SBDocConstants kFlyerArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_FLYER,      // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        5760, 0, 0,         // nMastheadHeight, nHeaderHeight, nFooterHeight
        1080, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_FLYER,      // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        5760, 0, 0,         // nMastheadHeight, nHeaderHeight, nFooterHeight
        1080, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_FLYER,      // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        5760, 0, 0,         // nMastheadHeight, nHeaderHeight, nFooterHeight
        1080, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_FLYER,      // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        5760, 0, 0,         // nMastheadHeight, nHeaderHeight, nFooterHeight
        1080, 540,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_FLYER,      // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        1417,               // nDefaultMargins
        227, 0, 0,          // nMastheadHeight, nHeaderHeight, nFooterHeight
        1077, 737,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_FLYER,     // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        1417,               // nDefaultMargins
        227, 0, 0,          // nMastheadHeight, nHeaderHeight, nFooterHeight
        1077, 737,          // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};

////////////////////////////////////////////
// array of PROJECT_CUSTOM default settings,
// one for each paper size and orientation
//
const SBDocConstants kCustomArray[NUMBER_OF_PAPERS * 2] =
{
    {   // letter, portrait
        PROJECT_CUSTOM,     // eProject
        12240, 15840,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1620, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // letter, landscape
        PROJECT_CUSTOM,     // eProject
        15840, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1620, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, portrait
        PROJECT_CUSTOM,     // eProject
        12240, 20160,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1620, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // legal, landscape
        PROJECT_CUSTOM,     // eProject
        20160, 12240,       // nPageWidth, nPageHeight
        1440,               // nDefaultMargins
        1620, 540, 540,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        360, 360,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, portrait (preliminary)
        PROJECT_CUSTOM,     // eProject
        A4WIDTH, A4HEIGHT,  // nPageWidth, nPageHeight
        1417,               // nDefaultMargins
        1644, 567, 567,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        340, 340,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    },
    {   // A4, landscape (preliminary)
        PROJECT_CUSTOM,     // eProject
        A4HEIGHT, A4WIDTH,  // nPageWidth, nPageHeight
        1417,               // nDefaultMargins
        1644, 567, 567,     // nMastheadHeight, nHeaderHeight, nFooterHeight
        340, 340,           // nVerticalGap, nColumnGap
        1, 1, 4,            // nMinColumns, nDefaultColumns, nMaxColumns
        1, 1, 100,          // nMinPages, nDefaultPages, nMaxPages
        FALSE, FALSE, FALSE,// bDefaultMasthead, bDefaultHeader, bDefaultFooter
        FALSE               // bDefaultLines
    }
};
