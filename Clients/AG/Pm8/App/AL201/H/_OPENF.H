/*
 * _OPENF.H
 *
 *  Header file for ArchiveLib 2.0
 *
 *  Copyright (c) 1994-1996 Greenleaf Software, Inc.
 *  All Rights Reserved
 *
 * DESCRIPTION
 *
 * These three classes are utility classes used inside ArchiveLib.
 * All they do is let me open a file(s) at the start of a routine and
 * then automatically close it when I exit.  It gets closed when
 * the destructor for this class gets called.  If the file was
 * already open when the routine was entered, this class leave
 * it open when the destructor gets called.  These classes will generally
 * get used like this:
 *
 *  int foo( ALStorage &input_file )
 *  {
 *       ALOpenInputFile file( input_file )
 *       ...
 *       do stuff with input file
 *       ...
 *  }    The destructor gets called here and the file is closed.
 *
 * This may seem like a lot of work, creating a class just to make
 * sure files get closed, but it replaces a *lot* of code.  Every
 * routine that uses a file can use one of these classes.
 *
 * 
 * CLASS DEFINITIONS:
 *
 *  ALOpenInputFile 
 *  ALOpenOutputFile
 *  ALOpenFiles
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 *
 */

#ifndef __OPENF_H
#define __OPENF_H

#include "arclib.h"

/*
 * class ALOpenInputFile 
 *
 * DESCRIPTION
 *
 *  This is a utility class.  The constructor opens a file for input,
 *  and keeps track of whether it was already open or not.  The destructor
 *  will automatically close the file if it was closed when the 
 *  ctor was invoked.
 *
 * DATA MEMBERS
 *
 * miFileWasOpen   : The flag that keeps track of the file's state
 *                   at the start of the routine.
 *
 * mpFile          : A pointer to the file, so we can close it in the dtor.
 *
 * MEMBER FUNCTIONS
 *
 *  ALOpenInputFile   : The constructor, opens the file.
 *  ~ALOpenInputFile  : The destructor, can close the file.
 *  operator new      : This operator is used in the Win16
 *                      DLL version of ArchiveLib.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALOpenInputFile {  /* Tag public class */
    public :
        AL_PROTO ALOpenInputFile( ALStorage AL_DLL_FAR &file );
        AL_PROTO ~ALOpenInputFile();
#if defined( AL_BUILDING_DLL ) || defined( AL_USING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Prevent the compiler from generating these members.
 */
    protected :
        AL_PROTO ALOpenInputFile( ALOpenInputFile AL_DLL_FAR &);
        ALOpenInputFile AL_DLL_FAR & operator=( ALOpenInputFile AL_DLL_FAR & );
    protected :
        int miFileWasOpen;
        ALStorage AL_DLL_FAR *mpFile;
};

/*
 * class ALOpenOutputFile 
 *
 * DESCRIPTION
 *
 *  This is a utility class.  The constructor opens a file for output,
 *  and keeps track of whether it was already open or not.  The destructor
 *  will automatically close the file if it was closed when the 
 *  ctor was invoked.
 *
 * DATA MEMBERS
 *
 * miFileWasOpen   : The flag that keeps track of the file's state
 *                   at the start of the routine.
 *
 * mpFile          : A pointer to the file, so we can close it in the dtor.
 *
 * MEMBER FUNCTIONS
 *
 *  ALOpenOutputFile   : The constructor, opens the file.
 *  ~ALOpenOutputFile  : The destructor, can close the file.
 *  operator new       : This operator is used in the Win16
 *                       DLL version of ArchiveLib.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALOpenOutputFile {  /* Tag public class */
    public :
        AL_PROTO ALOpenOutputFile( ALStorage AL_DLL_FAR &file );
        AL_PROTO ~ALOpenOutputFile();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Prevent the compiler from generating these members.
 */
    protected :
        AL_PROTO ALOpenOutputFile( ALOpenOutputFile AL_DLL_FAR &);
        ALOpenOutputFile AL_DLL_FAR & operator=( ALOpenOutputFile AL_DLL_FAR & );
    protected :
        int miFileWasOpen;
        ALStorage AL_DLL_FAR *mpFile;
};

/*
 * class ALOpenFiles
 *
 * DESCRIPTION
 *
 *  This is a utility class.  The constructor opens the first file for
 *  input, and the second for output.  It does so using the previous
 *  two classes, so it doesn't have to keep track of anything.
 *
 * DATA MEMBERS
 *
 *  mInputFile     : The input file open object.  It does all the work
 *                   related to the input file.
 *
 *  mOutputFile    : The output file open object.  It does all the work
 *                   related to the output file.
 *
 * MEMBER FUNCTIONS
 *
 *  ALOpenFiles   : The constructor, opens both files.
 *
 *  ~ALOpenFiles  : The destructor, can close one or both files.
 *
 *  operator new  : This operator is used in the Win16
 *                  DLL version of ArchiveLib.
 *
 * REVISION HISTORY
 *
 *  May 26, 1994  1.0A  : First release
 *
 *  February 14, 1996  2.0A : New release
 */

class AL_CLASS_TYPE ALOpenFiles {  /* Tag public class */
    public :
        AL_PROTO ALOpenFiles( ALStorage AL_DLL_FAR &input,
                              ALStorage AL_DLL_FAR &output );
        AL_PROTO ~ALOpenFiles();
#if defined( AL_USING_DLL ) || defined( AL_BUILDING_DLL )
        void AL_DLL_FAR * AL_PROTO operator new( size_t size );
#endif
/*
 * Prevent the compiler from generating these members.
 */
    protected :
        AL_PROTO ALOpenFiles( ALOpenFiles AL_DLL_FAR & );
        ALOpenFiles AL_DLL_FAR & operator=( ALOpenFiles AL_DLL_FAR & );
    protected :
        ALOpenInputFile mInputFile;
        ALOpenOutputFile mOutputFile;
};

#endif /* #ifndef __OPENF_H */
