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
*   SLPREDEF.H - ifdefs & undefs to prepare SL.H for pre-attribute      *
*                section definitions.                                   *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:
            slpredef.h undefines (if defined) all token strings that might
            conflict before or within platform attributes definition.
            It should therefore be included in SL.H *before* any #define or
            #ifdef statements.

            The following token strings have been defined in some platforms 
            by default (internal to a compiler) or on the compiler command
            line or in standard .h files (slundef.h, in contrast, contains 
            token strings defined in platform-proprietary .h files).  We must 
            guarantee that only one platform name will be defined in SL.H.  
            If we want to define a string prior to the platform attributes 
            section, we must #undef it here to avoid redefinition warnings 
            from the compiler.

        Cautions:
            This file should be updated if new conflicts arise, or if a
            define moves from pre-attribute section to post-, or vice versa.

        Creation Date:
            08-aug-91       JCC

        Change Record:

*-----------------------------------------------------------------------*/

#ifndef SLPREDEF_H

#define SLPREDEF_H

/* Define the platform if Borland C compiler.
*/

#ifdef __BORLANDC__

#ifdef __MSDOS__
#define  MSDOS
#elif defined( __OS2__ )
#define OS2
#endif

#endif

/* Define the platform if IBM C compiler
*/
#ifdef __IBMC__
#define OS2
#endif

/* WINDOWS and MSDOS are treated as separate environments in SL.H.  However,
 * Microsoft treats WINDOWS as a mutant of MSDOS & therefore defines both
 * of them simultaneously.  Not good for our attribute section.
*/
#if defined(WINDOWS) || defined(WIN32)
#undef MSDOS
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

/* Think C and Apple's MPW compiler both need to define MAC
*/

#ifdef THINK_C
#define  MAC
#endif

#ifdef applec
#define  MAC
#endif

/* Power Macintosh.  Currently we support only the Code Warrior compiler.
*/

#ifdef __MWERKS__ 
#ifdef powerc
#define POWERMAC
#else 
#define MAC
#endif
#endif


#endif /* SLPREDEF_H */

/*  End of file - SLpredef.h */
