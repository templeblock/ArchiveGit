//*
//* _ISR386.HPP
//*
//* Contains:
//* 		Definitions for the serial interrupt system for the protected mode
//* 		interrupt service routine.
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*

#ifndef _ISR386_DOT_H
#define _ISR386_DOT_H

#define MAXHANDLERS 10

class GFInterruptManager
{
	public:
		GFInterruptManager( void );
		~GFInterruptManager( void );

		int HookInterrupt( int INumber,
												GCPP_FARPTR32 PIVector,
												GCPP_FARPTR16 PRVector );
		int UnHookInterrupt( void );
		GCPP_FARPTR32 GetPreviousPVectorAddress( void );

	protected:
		GCPP_FARPTR32 GetProtectedModeInt( void );
		int SetProtectedModeInt( GCPP_FARPTR32 PIVector );
		GCPP_FARPTR16 GetRealModeInt( void );
		int SetRealModeInt( GCPP_FARPTR16 RIVector );

		int InterruptNumber;
		static int AtExitHooked;
		GCPP_FARPTR32 PreviousPVector;
		GCPP_FARPTR16 PreviousRVector;
};

#pragma pack( 1 )

struct IsrData
{
	unsigned int UartBase;									//* UART base address
	volatile unsigned char TXBlock; 				//* handshake recieved
	volatile unsigned char RXBlock; 				//* handshake sent
	unsigned char Parity; 									//* parity on if TRUE
	unsigned char TXFlow; 									//* current input handshake
	unsigned char RXFlow; 									//* current output handshake
	volatile unsigned char ModemStat; 			//* changes of state on RS232
	GCPP_FARPTR32 RXPBuffer;								//* ptr to input buffer
	GCPP_FARPTR16 RXRBuffer;
	volatile unsigned int RXHead; 					//* head of buffer
	volatile unsigned int RXTail; 					//* tail of buffer
	volatile unsigned int RXCount;					//* no of bytes in buffer
	unsigned int RXLength;									//* size of the input buffer
	unsigned int RXHighMark;								//* size - headroom
	unsigned int RXBlocking;
	GCPP_FARPTR32 TXPBuffer;								//* ptr to transmit buffer
	GCPP_FARPTR16 TXRBuffer;
	volatile unsigned int TXHead; 					//* head of buffer
	volatile unsigned int TXTail; 					//* tail of buffer
	volatile unsigned int TXCount;					//* no of bytes in buffer
	unsigned int TXLength;									//* size of the input buffer
	volatile short TXFlag;									//* tx interrupt active?
	volatile short LSError; 								//* line/overflow errors
	volatile unsigned int TXCT; 						//* Transmit interrupt counter
	volatile unsigned int RXCT; 						//* Receive interrupt counter
	volatile unsigned int MDCT; 						//* MODEM status int counter
	volatile unsigned int LSCT; 						//* Line status int counter
	unsigned char Is16550;									//* Set if 16550 is detected
  volatile unsigned int RMICT;            //* Real Mode Interrupt Counter
  volatile unsigned int PMICT;            //* Protected Mode Interrupt Counter
};


#pragma pack()

class GFSerint
{
	friend class GFI8250;
	friend class GFDigiPC;

	public:
		GFSerint( int port_num,
							unsigned char vec_num,
							unsigned char mask,
							GCPP_FARPTR32 p_handler,
							GCPP_FARPTR16 r_handler,
							GFI8250 *io_control );
		~GFSerint( void );

	protected:
		//*
		//* values to here accessed by interrupt service routines
		//*
		GFInterruptManager *InterruptHandler; 	//* Pointer to Interrupt manager
		GCPP_FARPTR32 RXBuffer; 								//* Pointer to RXBuffer
		GCPP_FARPTR32 TXBuffer; 								//* Pointer to TXBuffer
		GCPP_FARPTR32 IsrDataPtr; 							//* Pointer to isr data structure
		GCPP_FARPTR32 ProtectedStub;						//* Address of Protected mode stub
		GCPP_FARPTR16 RealStub; 								//* Address of Real mode stub
		unsigned int UartBase;									//* Uart base address
		unsigned char PortInitialized;					//* Initialization flag
		unsigned char InterruptVectorNumber;		//* Vector to intercept
		unsigned char Mask8259; 								//* Mask for PIC
		int PortNumber; 												//* Port number
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
		void SetModemStatusElement( unsigned char status );
		void SetParityStatusElement( char selected_parity );
		void SetTXFlowStatusElement( unsigned char status );
		void SetRXFlowStatusElement( unsigned char status );
		void SetTXBlockStatusElement( unsigned char status );
		void SetRXBlockStatusElement( unsigned char status );
		void SetTXFlagStatusElement( short int status );
		void SetLSErrorStatusElement( short error );
		void SetRXPBufferStatusElement( GCPP_FARPTR32 status );
		void SetRXHighMarkStatusElement( unsigned int status );
		int Set16550StatusElement( TRIGGER_LEVEL level );
		void SetTXHeadStatusElement( unsigned int status );
		void SetTXTailStatusElement( unsigned int status );
		void SetRXHeadStatusElement( unsigned int status );
		void SetRXTailStatusElement( unsigned int status );
		void SetRXCountStatusElement( unsigned int status );
		void SetTXCountStatusElement( unsigned int status );
		void SetTXLengthStatusElement( unsigned int status );
		void SetRXLengthStatusElement( unsigned int status );
		void SetUartBaseStatusElement( unsigned int status );
		void SetRXBlockingStatusElement( unsigned int status );
		void SetRXRBufferStatusElement( GCPP_FARPTR16 status );
		void SetTXRBufferStatusElement( GCPP_FARPTR16 status );
		void SetTXPBufferStatusElement( GCPP_FARPTR32 status );
		void GetIsrDataCopy( struct IsrData &TempIsrDataPtr );
		unsigned char GetTXFlowStatusElement( void );
		unsigned char GetRXFlowStatusElement( void );
		unsigned char GetTXBlockStatusElement( void );
		unsigned char GetRXBlockStatusElement( void );
		unsigned char GetModemStatusElement( void );
		unsigned int GetTXCountStatusElement( void );
		unsigned int GetRXCountStatusElement( void );
		unsigned int GetTXLengthStatusElement( void );
		unsigned int GetRXLengthStatusElement( void );
		unsigned int GetRXBlockingStatusElement( void );
		unsigned int GetRXHeadStatusElement( void );
		short GetLSErrorStatusElement( void );
		void IncrementRXHeadStatusElement( void );
		void IncremenetRXCountStatusElement( void );

		static int IrqInUse( int irq_number );
	public:
		static int RelocateIsrData( void );
};

extern GCPP_FARPTR32 RealBlockPtr32;
extern GCPP_FARPTR16 RealBlockPtr16;

extern "C" {


//*
//* The next four pointers are used to locate the start and end
//* of the protected isr code and data segments.	I need to know where
//* these are so that the memory they occupy can be locked.
//*
extern char SegmentStartPC32;
extern char SegmentEndPC32;
extern char SegmentStartPD32;
extern char SegmentEndPD32;

//*
//* Anything that is stored in the reloacted real section of memory
//* can't be addressed directly.  Instead, we calculate an offset
//* into the real mode segment by subtracting SegmentStartRR16.  It
//* is the first address in the real segment.  The next value defined
//* here marks the end of the relocated real segment.  We only need it
//* so we can figure out how much memory to relocate.
//*

extern char SegmentStartRR16;
extern char SegmentEndRR16;

//*
//* Deep inside the ISR, I sometimes need to look up the value of
//* this programs DGROUP Selector.	It gets stored in this short
//* int for lookup by the ISR.
//*
extern short int _DgroupSelectorPD32;

int GCPP_CFUN _isrp_putchar( unsigned char data, GCPP_FARPTR32 isr_data_ptr );
int GCPP_CFUN _isrp_getchar( GCPP_FARPTR32 isr_data_ptr );
void GCPP_CFUN _RealIret( void );

extern GCPP_FARPTR32 PPortDataTables[ GCPP_NUMPORTS+1 ];
extern GCPP_FARPTR16 RPortDataTables[ GCPP_NUMPORTS+1 ];
extern short int PIrqToPortData[ GCPP_NUMIRQS ];
extern short int RIrqToPortData[ GCPP_NUMIRQS ];
extern unsigned int PBoardStartPort;
extern unsigned int PBoardStatusRegister;
extern unsigned int IRQPTable[ GCPP_NUMIRQS ];
extern unsigned int IRQRTable[ GCPP_NUMIRQS ];
extern unsigned int SharedIRQPTable[ GCPP_NUMIRQS ];
extern unsigned int SharedIRQRTable[ GCPP_NUMIRQS ];

}

#endif
