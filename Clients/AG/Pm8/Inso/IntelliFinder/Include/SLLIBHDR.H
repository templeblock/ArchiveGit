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
*    SLlibhdr.h - Header for the base level library.                    *
*                                                                       *
*-----------------------------------------------------------------------*
                                
    Description:                             
        This file includes all of the header files in the InfoSoft
        Base Level Library.  Any functions with references
        to the InfoSoft Library should include this file.
                                    
    Cautions:
        None

    Creation Date:
        18-jul-89    Created by InfoSoft Software Engineering
*----------------------------------------------------------------------*/
#ifndef SLLIBHDR_H

#define SLLIBHDR_H


/*    lower level includes        */

#define SL_INTERNAL /* Windows specific defines in windows.h not included. */ 

#include "slfun.h"
#include "sl.h"
#include "slcalls.h"
#include "slascii.h"
#include "slmacros.h"
#include "sllibext.h"
#include "slmemman.h"
#include "slmemtyp.h"
#include "sllang.h"
#include "slchrmcs.h"
#include "slatt.h"
#include "slerror.h"

/* Include slch2mcs.h if Latin-2 (Eastern European) characters are needed */

#ifdef SL_HMCH2
#include "slch2mcs.h"
#endif

/* Include slch5mcs.h if Latin-5 (Cyrillic) characters are needed */

#ifdef SL_HMCH5
#include "slch5mcs.h"
#endif

/* Include slch7mcs.h if Latin-7 (Greek) characters are needed */
#ifdef SL_HMCH7
#include "slch7mcs.h"
#endif

#endif /* SLLIBHDR_H */

/* End of file - sllibhdr.h */
