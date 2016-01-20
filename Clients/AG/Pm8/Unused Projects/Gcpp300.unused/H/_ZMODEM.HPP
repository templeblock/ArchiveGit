//*
//* _ZMODEM.HPP
//*
//* Contains:
//*     Zmodem declarations adn defines
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#ifndef _ZMODEM_HPP
#define _ZMODEM_HPP

//*
//* Note that these constants are taken from Chuck Forsberg's
//* Public Domain code, and are therefore not copyrighted by
//* Greenleaf Software.
//*

#define ZRQINIT 0       //* Request receive init
#define ZRINIT  1       //* Receive init
#define ZSINIT 2        //* Send init sequence (optional)
#define ZACK 3          //* ACK to above
#define ZFILE 4         //* File name from sender
#define ZSKIP 5         //* To sender: skip this file
#define ZNAK 6          //* Last packet was garbled
#define ZABORT 7        //* Abort batch transfers
#define ZFIN 8          //* Finish session
#define ZRPOS 9         //* Resume data trans at this position
#define ZDATA 10        //* Data packet(s) follow
#define ZEOF 11         //* End of file
#define ZFERR 12        //* Fatal Read or Write error Detected
#define ZCRC 13         //* Request for file CRC and response
#define ZCHALLENGE 14   //* Receiver's Challenge
#define ZCOMPL 15       //* Request is complete
#define ZCAN 16         //* Other end canned session with CAN*5

#define ZPAD            '*'
#define ZDLE            CAN
#define ZBIN            'A'
#define ZHEX            'B'
#define ZVBIN           'a'
#define ZVHEX           'b'
#define ZBIN32          'C'
#define ZMAXHLEN        16
#define ZMAXSPLEN       1024

#define ZCRCE 'h'       //* CRC next, frame ends, header packet follows
#define ZCRCG 'i'       //* CRC next, frame continues nonstop
#define ZCRCQ 'j'       //* CRC next, frame continues, ZACK expected
#define ZCRCW 'k'       //* CRC next, ZACK expected, end of frame
#define ZRUB0 'l'       //* Translate to rubout 0177
#define ZRUB1 'm'       //* Translate to rubout 0377

#define CANFDX	01		//* Rx can send and receive true FDX
#define CANOVIO 02      //* Rx can receive data during disk I/O

#define ZMODEM_MAX_ERRORS   10  //* Maximum number of block retries

/*
 * File management options supported by Zmodem
 */
#define ZMRECOV   3
#define ZMNEWL    1
#define ZMAPND    3
#define ZMCLOB    4
#define ZMNEW     5
#define ZMDIFF    6
#define ZMPROT    7

#endif

