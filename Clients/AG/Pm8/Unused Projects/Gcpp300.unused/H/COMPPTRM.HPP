//*
//* COMPPTRM.HPP
//*
//* Contains:
//*     class GFTerminalType
//*     class GFTeletype
//*     class GFAnsiTerminal
//*     class GFVt52
//*     class GFVt100
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//* July 2, 1992 - Removed all parameter passing modifers from member
//*                functions.
//*

#ifndef COMPPTRM_HPP
#define COMPPTRM_HPP

#include "cppcomp.h"
#include "compplib.hpp"
#include "comppdsp.hpp"

class GCPP_CLASS_TYPE GFTerminalType : public GFSerial
{
  protected:
    GCPP_PROTO GFTerminalType( GFScreenDriver GCPP_DLL_FAR *driver,
                               GFComLink GCPP_DLL_FAR *u,
                               int pbuf = 255 );
    GCPP_PROTO ~GFTerminalType( void );
    virtual void GCPP_PROTO Decode( int code,
                                    int val,
                                    int GCPP_DLL_FAR *parm );
    virtual int GCPP_PROTO Parse( unsigned char GCPP_DLL_FAR *buffer,
                                  int GCPP_DLL_FAR *parm,
                                  char sep );

    GFScreenDriver GCPP_DLL_FAR *display;
    unsigned char GCPP_DLL_FAR *parsebuf;
    unsigned char GCPP_DLL_FAR *pos;
    int parsing;
    int how_many;
    int size;
};


class GCPP_CLASS_TYPE GFTeletype : public GFTerminalType
{
  public:
    GCPP_PROTO GFTeletype( GFScreenDriver GCPP_DLL_FAR *driver,
                           GFComLink GCPP_DLL_FAR *u,
                           int bufsize = 0 );
    GCPP_PROTO ~GFTeletype( void );
    int GCPP_PROTO GetChar( unsigned msecs = 0 );
    int GCPP_PROTO GetKey( void );
};


class GCPP_CLASS_TYPE GFAnsiTerminal : public GFTerminalType
{
  public:
    GCPP_PROTO GFAnsiTerminal( GFScreenDriver GCPP_DLL_FAR *driver,
                               GFComLink GCPP_DLL_FAR *u,
                               int bufsize = 255 );
    GCPP_PROTO ~GFAnsiTerminal( void );
    int GCPP_PROTO GetChar( unsigned msecs = 0 );
    int GCPP_PROTO GetKey( void );

  protected:
    void GCPP_PROTO Decode( int code,
                            int val,
                            int GCPP_DLL_FAR *parm );
    int  GCPP_PROTO Parse( unsigned char GCPP_DLL_FAR *buffer,
                           int GCPP_DLL_FAR *parm,
                           char sep );
    virtual void GCPP_PROTO Defaults( int d0,
                                      int d1,
                                      int GCPP_DLL_FAR *p );
    int GCPP_PROTO IsFinal( int code );
    int GCPP_PROTO IsInter( int code );

    int saverow;
    int savecol;
    int saveink;
    int savepaper;
    FLAG blink;
    FLAG bright;
    FLAG conceal;
    FLAG reverse;
    char store[10];
};


class GCPP_CLASS_TYPE GFVt52 : public GFAnsiTerminal
{
  public:
    GCPP_PROTO GFVt52( GFScreenDriver GCPP_DLL_FAR *driver,
                       GFComLink GCPP_DLL_FAR *u,
                       int bufsize = 255 );
    GCPP_PROTO ~GFVt52( void );

  protected:
    void GCPP_PROTO Decode( int code, int val, int GCPP_DLL_FAR *parm );
};


#define LANSARG 10                /* Max number of ANSI arguments */

enum CharSet { ASCII, UK, SPECIAL };


struct GCPP_CLASS_TYPE GFSaveCursor {
  int col;                      //* cursor position, column
  int row;                      //* cursor position, row
  CharSet GCPP_DLL_FAR *charset;//* pointer to mapped character set
  CharSet cset0;                //* character set 0
  CharSet cset1;                //* character set 1
  int mode;                     //* origin mode
};

class GCPP_CLASS_TYPE GFVt100 : public GFTerminalType
{
  public:
    GCPP_PROTO GFVt100( GFScreenDriver GCPP_DLL_FAR *driver,
                        GFComLink GCPP_DLL_FAR *u,
                        int bufsize = 255 );
    GCPP_PROTO ~GFVt100( void );
    int GCPP_PROTO GetChar( unsigned msecs = 0 );
    int GCPP_PROTO PutChar( unsigned char c );
    int GCPP_PROTO GetKey( void );

  protected:
    void GCPP_PROTO Initialize( void );
    void GCPP_PROTO Vt100Parse( unsigned char c );
    void GCPP_PROTO DoChar( unsigned char c);
    void GCPP_PROTO DoEscape( unsigned char c );
    void GCPP_PROTO ExtParse(unsigned char c);
    void GCPP_PROTO AnsiModeSet( char c, int mode);
    void GCPP_PROTO ExtModeSet( char c, int mode );
    void GCPP_PROTO SetChar0( unsigned char c );
    void GCPP_PROTO SetChar1( unsigned char c );
    void GCPP_PROTO SetCharSet( int gset, unsigned char set );
    void GCPP_PROTO MapCharSet( int charset );
    void GCPP_PROTO SetDoubleSize( unsigned char c );
    void GCPP_PROTO TransmitId( void );
    void GCPP_PROTO SetScreenWidth( int width );
    void GCPP_PROTO SetBackGround( int mode );
    void GCPP_PROTO Bell( void );
    void GCPP_PROTO SaveCursor( void );
    void GCPP_PROTO RestoreCursor( void );
    int GCPP_PROTO Translate( unsigned key );

#if defined( GCPP_WATCOM )
    void (GCPP_PROTO GFVt100::*state) (unsigned char);
#else
    void (GCPP_DLL_FAR GFVt100::*state) (unsigned char);
#endif
    int have;
    int applkeypad;
    int backspace;
    int invert;
    CharSet GCPP_DLL_FAR *charset;
    CharSet cset0;
    CharSet cset1;
    char color;
    char normal;
    char reverse;
    unsigned char attributes;
    char cursorkey;
    GFSaveCursor save;

    int nansarg;                  //* Index for ANSI argument list
    int ansargs[LANSARG];         //* Room for 10 ANSI arguments
    unsigned originmode;          //* Origin mode, relative or absolute
    unsigned insertmode;          //* Insert mode
    unsigned autowrap;            //* Automatic wrap mode
    unsigned newline;             //* Newline mode
    unsigned cursorvisible;       //* Cursor visibility
    unsigned screenwidth;         //* Screen column width
    unsigned char lastc;
};

#endif
