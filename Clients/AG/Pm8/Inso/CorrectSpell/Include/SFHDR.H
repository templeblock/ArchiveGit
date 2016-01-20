/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*
 
*-----------------------------------------------------------------------*
*                                                                       *
*        SFhdr.h - Standard Function Includes                           *
*                                                                       *
*-----------------------------------------------------------------------*
*                                                                       *
Description:
        
    This header contains all Includes for the Standard Functions 
    including decoding and correction functions.

    This header should be included by all modules of the SF, all 
    products and by the User Application.


    SFhdr.h
     |
     |___SLlibhdr.h
     |    |
     |    |___SL*.h
     |
     |___SFfun.h
          |
          |___ICfun.h (if built for the IC product)
          |___REfun.h (if built for the RE product)
          |___IHfun.h (if built for the IH product)
          |
          |___SFdef.h
          |___SFclitic.h (if the product(s) use clitics)
          |___SFctrl.h"
          |___SFexthdr.h


Cautions:
    None.

Creation Date:
    02-jun-92   ECH

Change Record:

*-----------------------------------------------------------------------*/

#ifndef SFHDR_H

#define SFHDR_H

#include "sllibhdr.h"
#include "sffun.h"

#endif /* SFHDR_H */

/* End of file - SFhdr.h */
