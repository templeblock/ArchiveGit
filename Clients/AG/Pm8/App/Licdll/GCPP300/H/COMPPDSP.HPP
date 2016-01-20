//*
//* CMPPDISP.HPP
//*
//* Contains:
//*     class GFScreenDriver
//*     class GFScreenZDisp  DOS|OS/2
//*     class GFScreenTW     WINDOWS
//*     class GFScreenDW     DOS
//*     class GFScreenVid    DOS|OS/2
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*   November 5, 1993 - Removed the BackTab function from the member class
//*                      definitions.  They all now use the definition in
//*                      the base class.
//*   August 19, 1994 - Modified to support Watcom C++ 10.0.
//*

#ifndef COMPPDSP_HPP
#define COMPPDSP_HPP

#include "compplib.hpp"
#if defined( GCPP_WINDOWS )
	#if !defined( STRICT )
    #define STRICT
  #endif
  #include <windows.h>
  #include "tw.h"
#endif
#if !defined( GCPP_WINDOWS ) ||\
    ( defined( GCPP_WINDOWS ) && defined( GCPP_WIN32 ) )
  #include "video.h"
#endif
#if !defined( GCPP_WINDOWS )  &&\
    !defined( GCPP_OS2 )      &&\
    !defined( GCPP_WIN32 )    &&\
    !defined( GCPP_X32 )
  #include "_dwdisp.h"
#endif

//*
//* class GFScreenDriver definition
//*

//*
//* Notice that almost all of the functions in this class are
//* declared virtual. This must be the case because it is
//* intended to support different display methods by
//* inheriting from this class and redefining the member
//* functions.
//*
class GCPP_CLASS_TYPE GFScreenDriver
{
  public:
    GCPP_PROTO GFScreenDriver( void );
    virtual GCPP_PROTO ~GFScreenDriver( void );
    void GCPP_PROTO Origin( int ox, int oy );
    void GCPP_PROTO MaxSize( int rows, int cols );
    int GCPP_PROTO GetCols( void );
    int GCPP_PROTO GetRows( void );
    int GCPP_PROTO Printf( char GCPP_DLL_FAR *format, ... );
    virtual void GCPP_PROTO Flush( void );
    virtual void GCPP_PROTO Clear( void );
    virtual void GCPP_PROTO MoveTo( int r, int c );
    virtual void GCPP_PROTO WhereAmI( int GCPP_DLL_FAR &r, int GCPP_DLL_FAR &c );
    virtual void GCPP_PROTO CursorHome( void );
    virtual void GCPP_PROTO CursorUp( int lines );
    virtual void GCPP_PROTO CursorDown( int lines );
    virtual void GCPP_PROTO CursorRight( int lines );
    virtual void GCPP_PROTO CursorLeft( int lines );
    virtual void GCPP_PROTO Color( int fg, int bg );
    virtual void GCPP_PROTO ReadColor( int GCPP_DLL_FAR &fg,
                                       int GCPP_DLL_FAR &bg );
    virtual void GCPP_PROTO PutChar( char c );
    virtual void GCPP_PROTO PutString( char GCPP_DLL_FAR *str );
    virtual unsigned short GCPP_PROTO GetChar( void );
    virtual void GCPP_PROTO Erase2EOL( void );
    virtual void GCPP_PROTO Erase2EOP( void );
    virtual void GCPP_PROTO Erase2SOP( void );
    virtual void GCPP_PROTO Erase2SOL( void );
    virtual void GCPP_PROTO DeleteLine( void );
    virtual void GCPP_PROTO InsertLine( void );
    virtual void GCPP_PROTO InsertChar( void );
    virtual void GCPP_PROTO DeleteChar( void );
    virtual void GCPP_PROTO BackTab( void );
    virtual void GCPP_PROTO Scroll( int );
    virtual void GCPP_PROTO SetRegion( int t, int b );
    virtual void GCPP_PROTO SetColumns( int cols );
    virtual void GCPP_PROTO ShowCursor( void );
    virtual void GCPP_PROTO HideCursor( void );
    virtual void GCPP_PROTO SetTabStop( void );
    virtual void GCPP_PROTO ClearTabStop( void );
    virtual void GCPP_PROTO ClearAllTabs( void );
    virtual void GCPP_PROTO Invert( void );
    virtual void GCPP_PROTO InitTabs( void );
    virtual void GCPP_PROTO Tab( void );
    virtual unsigned int GCPP_PROTO GetKey( void );
    virtual int GCPP_PROTO KbdHit( void );

    virtual void GCPP_PROTO Index( void );
    virtual void GCPP_PROTO ReverseIndex( void );

  protected:
    int row;
    int column;
    int numrows;
    int numcols;
    int xorigin;
    int yorigin;
    int ink;
    int paper;
    int top;
    int bottom;
    char GCPP_DLL_FAR *tabs;
};

//*
//* class GFScreenZDisp definition
//*
#if defined( GCPP_SYMANTEC ) && !defined( GCPP_WINDOWS )

class GFScreenZDisp : public GFScreenDriver
{
  public:
    GFScreenZDisp( void );
    ~GFScreenZDisp( void );
    void Flush( void );
    void Clear( void );
    void MoveTo( int r, int c );
    void WhereAmI( int &r, int &c );
    void CursorHome( void );
    void CursorUp( int lines );
    void CursorDown( int lines );
    void CursorRight( int lines );
    void CursorLeft( int lines );
    void Color( int fg, int bg );
    void ReadColor( int &fg, int &bg );
    void PutChar( char c );
    void PutString( char *str );
    unsigned short GetChar( void );
    void Erase2EOL( void );
    void Erase2EOP( void );
    void Erase2SOP( void );
    void Erase2SOL( void );
    void DeleteLine( void );
    void InsertLine( void );
    void InsertChar( void );
    void DeleteChar( void );
    void Scroll( int );
    unsigned int GetKey( void );
    int KbdHit( void );
    void ShowCursor( void );
    void HideCursor( void );
    void Invert( void );

  protected:
    unsigned short oldrow;
    unsigned short oldcol;
    unsigned short *oldscreen;
};
#endif

//*
//* class GFScreenTW definition
//*
#if defined( GCPP_WINDOWS )

class GCPP_CLASS_TYPE GFScreenTW : public GFScreenDriver
{
  public:
    GCPP_PROTO GFScreenTW( HINSTANCE hInstance,
                           HINSTANCE hPrevInstance,
                           int nCmdShow,
                           char GCPP_DLL_FAR *title1,
                           char GCPP_DLL_FAR *title2,
                           int init_system = 1,
                           int init_window = 1 );
    virtual GCPP_PROTO ~GFScreenTW( void );
    void GCPP_PROTO Flush( void );
    void GCPP_PROTO Clear( void );
    void GCPP_PROTO MoveTo( int r, int c );
    void GCPP_PROTO WhereAmI( int GCPP_DLL_FAR &r, int GCPP_DLL_FAR &c );
    void GCPP_PROTO CursorHome( void );
    void GCPP_PROTO CursorUp( int lines );
    void GCPP_PROTO CursorDown( int lines );
    void GCPP_PROTO CursorRight( int lines );
    void GCPP_PROTO CursorLeft( int lines );
    void GCPP_PROTO Color( int fg, int bg );
    void GCPP_PROTO ReadColor( int GCPP_DLL_FAR &fg, int GCPP_DLL_FAR &bg );
    void GCPP_PROTO PutChar( char c );
    void GCPP_PROTO PutString( char GCPP_DLL_FAR *str );
    unsigned short GCPP_PROTO GetChar( void );
    void GCPP_PROTO Erase2EOL( void );
    void GCPP_PROTO Erase2EOP( void );
    void GCPP_PROTO Erase2SOP( void );
    void GCPP_PROTO Erase2SOL( void );
    void GCPP_PROTO DeleteLine( void );
    void GCPP_PROTO InsertLine( void );
    void GCPP_PROTO InsertChar( void );
    void GCPP_PROTO DeleteChar( void );
    void GCPP_PROTO Scroll( int direction );
    void GCPP_PROTO wrap( int swtch );
    void GCPP_PROTO ShowCursor( void );
    void GCPP_PROTO HideCursor( void );
    void GCPP_PROTO Invert( void );
    unsigned int GCPP_PROTO GetKey( void );
    int GCPP_PROTO KbdHit( void );

    TW_WINDOW GCPP_DLL_FAR *window;
    int wrap_flag;
};
#endif



#if !defined( GCPP_WINDOWS )  &&\
    !defined( GCPP_OS2 )      &&\
    !defined( GCPP_WIN32 )    &&\
    !defined( GCPP_X32 )

//*
//* class GFScreenDW defintion
//*
class GFScreenDW : public GFScreenDriver
{
  public:
    GFScreenDW( int init_window = 1 );
    virtual ~GFScreenDW( void );
    void Flush( void );
    void Clear( void );
    void MoveTo( int r, int c );
    void WhereAmI( int &r, int &c );
    void CursorHome( void );
    void CursorUp( int lines );
    void CursorDown( int lines );
    void CursorRight( int lines );
    void CursorLeft( int lines );
    void Color( int fg, int bg );
    void ReadColor( int &fg, int &bg );
    void PutChar( char c );
    void PutString( char *str );
    unsigned short GetChar( void );
    void Erase2EOL( void );
    void Erase2EOP( void );
    void Erase2SOP( void );
    void Erase2SOL( void );
    void DeleteLine( void );
    void InsertLine( void );
    void InsertChar( void );
    void DeleteChar( void );
    void Scroll( int direction );
    void Wrap( int swtch );
    unsigned int GetKey( void );
    int KbdHit( void );
    void HideCursor( void );
    void ShowCursor( void );
    void Invert( void );

    HWND window;
    int attribute;
    int wrap_flag;
};

#endif

#if !defined( GCPP_WINDOWS ) ||\
    ( defined( GCPP_WINDOWS ) && defined( GCPP_WIN32 ) )

//*
//* class GFScreenVid definition
//*
class GFScreenVid : public GFScreenDriver
{
  public:
    GCPP_PROTO GFScreenVid( int init_system = 1, int init_window = 1 );
    virtual GCPP_PROTO ~GFScreenVid( void );
    void GCPP_PROTO Flush( void );
    void GCPP_PROTO Clear( void );
    void GCPP_PROTO MoveTo( int r, int c );
    void GCPP_PROTO WhereAmI( int GCPP_DLL_FAR &r, int GCPP_DLL_FAR &c );
    void GCPP_PROTO CursorHome( void );
    void GCPP_PROTO CursorUp( int lines );
    void GCPP_PROTO CursorDown( int lines );
    void GCPP_PROTO CursorRight( int lines );
    void GCPP_PROTO CursorLeft( int lines );
    void GCPP_PROTO Color( int fg, int bg );
    void GCPP_PROTO ReadColor( int GCPP_DLL_FAR &fg, int GCPP_DLL_FAR &bg );
    void GCPP_PROTO PutChar( char c );
    void GCPP_PROTO PutString( char GCPP_DLL_FAR *str );
    unsigned short GCPP_PROTO GetChar( void );
    void GCPP_PROTO Erase2EOL( void );
    void GCPP_PROTO Erase2EOP( void );
    void GCPP_PROTO Erase2SOP( void );
    void GCPP_PROTO Erase2SOL( void );
    void GCPP_PROTO DeleteLine( void );
    void GCPP_PROTO InsertLine( void );
    void GCPP_PROTO InsertChar( void );
    void GCPP_PROTO DeleteChar( void );
    void GCPP_PROTO Scroll( int direction );
    unsigned int GCPP_PROTO GetKey( void );
    int GCPP_PROTO KbdHit( void );
    void GCPP_PROTO HideCursor( void );
    void GCPP_PROTO ShowCursor( void );
    void GCPP_PROTO Invert( void );

    void GCPP_PROTO SetRegion( int rt, int bt );
    void GCPP_PROTO Index( void );
    void GCPP_PROTO ReverseIndex( void );

    VID_WINDOW GCPP_DLL_FAR *window;
};
#endif

#endif
