//*
//* _ISR88.HPP
//*
//* Contains:
//* 		Definitions for the serial interrupt system for the real mode
//* 		interrupt service routine.
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*

#ifndef _ISR88_DOT_H
#define _ISR88_DOT_H

#define MAXHANDLERS 10

typedef void (GCPP_CFUN GCPP_FAR *GCPP_Interrupt)(void);

class GFInterruptManager
{
  int InterruptNumber;
  static int AtExitHooked;
	GCPP_Interrupt InterruptVector;
	GCPP_Interrupt PreviousVector;

  public:
    GFInterruptManager( void );
    ~GFInterruptManager( void );

		int HookInterrupt( int INumber, GCPP_Interrupt IVector );
    int UnHookInterrupt( void );
		GCPP_Interrupt GetPreviousVectorAddress( void );
		GCPP_Interrupt GetInterruptVectorAddress( void );
};

#pragma pack(1)

struct IsrData
{
  unsigned int UartBase;                  //* UART base address
  volatile unsigned char TXBlock;         //* handshake recieved
  volatile unsigned char RXBlock;         //* handshake sent
  unsigned char Parity;                   //* parity on if TRUE
  unsigned char TXFlow;                   //* current input handshake
  unsigned char RXFlow;                   //* current output handshake
  volatile unsigned char ModemStat;       //* changes of state on RS232
  unsigned char *RXBuffer;                //* ptr to input buffer
  volatile unsigned int RXHead;           //* head of buffer
  volatile unsigned int RXTail;           //* tail of buffer
  volatile unsigned int RXCount;          //* no of bytes in buffer
  unsigned int RXLength;                  //* size of the input buffer
	unsigned int RXHighMark;								//* size - headroom
	unsigned int RXBlocking;								//* blocking flag
	unsigned char *TXBuffer;								//* ptr to transmit buffer
  volatile unsigned int TXHead;           //* head of buffer
  volatile unsigned int TXTail;           //* tail of buffer
  volatile unsigned int TXCount;          //* no of bytes in buffer
  unsigned int TXLength;                  //* size of the input buffer
  volatile short TXFlag;                  //* tx interrupt active?
  volatile short LSError;                 //* line/overflow errors
  volatile unsigned int TXCT;             //* Transmit interrupt counter
  volatile unsigned int RXCT;             //* Receive interrupt counter
  volatile unsigned int MDCT;             //* MODEM status int counter
  volatile unsigned int LSCT;             //* Line status int counter
  unsigned char Is16550;                  //* Set if 16550 is detected
};

class GFSerint
{
  friend class GFI8250;
  friend class GFDigiPC;

  public:
    GFSerint( int port_num,
              unsigned char vec_num,
              unsigned char mask,
							GCPP_Interrupt isr,
							GFI8250 *io_control );
    ~GFSerint( void );

  protected:
    //*
    //* values to here accessed by interrupt service routines
    //*
    struct IsrData *IsrDataPtr;               //* Address of port data
    unsigned char *RXBuffer;                  //* Pointer to RXBuffer
    unsigned char *TXBuffer;                  //* Pointer to TXBuffer
    unsigned int UartBase;                    //* Uart base address
    int PortNumber;                           //* Port number
		GCPP_Interrupt PreviousIsr; 							//* Address of previous isr
		GCPP_Interrupt IsrStub; 									//* Pointer to isr stub
    GFInterruptManager *InterruptHandler;
		unsigned char PortInitialized;						//* Initialization flag
    unsigned char InterruptVectorNumber;      //* vector to intercept
    unsigned char Mask8259;                   //* mask for PIC
		GFI8250 *IOControl;

    int SetHand( unsigned char inhand, unsigned char outhand );
    int GetHand( unsigned char &inhand, unsigned char &outhand );
    void Update( unsigned char *vregs );
    int Install( unsigned base = 0x3f8,
                 unsigned ibsize = GCPP_RXSIZE,
                 unsigned obsize = GCPP_TXSIZE,
                 unsigned room = GCPP_RXROOM );
    void Clear( int dir );
    void Remove( void );
    int GetChar( void );
    unsigned Stuff( unsigned char outc );
    int PutChar( unsigned char outc );
    int PutImm( unsigned char c );
		int SetRXBuffer( unsigned int rx_length, unsigned int headroom );
		int SetTXBuffer( unsigned int tx_length );
		int CheckForUart( void );
		int BlockingOn( void );
		int BlockingOff( void );
    unsigned char GetTXBlockStatusElement( void );
    unsigned char GetRXBlockStatusElement( void );
    unsigned char GetTXFlowStatusElement( void );
    unsigned char GetRXFlowStatusElement( void );
    unsigned int GetRXCountStatusElement( void );
    unsigned int GetTXCountStatusElement( void );
    unsigned int GetRXLengthStatusElement( void );
    unsigned int GetTXLengthStatusElement( void );
    short GetLSErrorStatusElement( void );
    unsigned char GetModemStatusElement( void );
    int Set16550StatusElement( TRIGGER_LEVEL level );
    void SetTXBlockStatusElement( unsigned char status );
    void SetRXBlockStatusElement( unsigned char status );
    void SetModemStatusElement( unsigned char status );
    void SetParityStatusElement( char selected_parity );
    void SetRXFlowStatusElement( unsigned char status );
    void SetTXFlowStatusElement( unsigned char status );
    void SetTXFlagStatusElement( short int status );
    void SetLSErrorStatusElement( short LSError );
    void SetTXHeadStatusElement( unsigned int status );
    void SetTXTailStatusElement( unsigned int status );
    void SetRXHeadStatusElement( unsigned int status );
    void SetRXTailStatusElement( unsigned int status );
    void SetRXCountStatusElement( unsigned int status );
    void SetTXCountStatusElement( unsigned int status );

    static int IrqInUse( int irq_number );
};

#pragma pack()

extern "C" {
extern struct IsrData *PortDataTables[ GCPP_NUMPORTS+1];
extern short int IrqToPortData[ GCPP_NUMIRQS ];
extern unsigned int BoardStartPort;
extern unsigned int BoardStatusRegister;
extern GCPP_Interrupt IRQTable[6];
extern GCPP_Interrupt SharedIRQTable[6];
}

#endif
