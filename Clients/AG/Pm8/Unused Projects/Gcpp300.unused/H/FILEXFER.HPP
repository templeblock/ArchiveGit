//*
//* FILEXFER.HPP
//*
//* DESCRIPTION
//*         class GFMonitor
//*         class GFTransfer
//*         class GFTransHelper
//*         class GFXmodem
//*         class GFXmodem1K
//*         class GFXmodem1KG
//*         class GFYmodem
//*         class GFYmodemG
//*         class GFZmodem
//*         class GFTranstxt
//*         class GFKermit
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*   November 5, 1993 - Modified to support the Symantec C++ 6.0 compiler.
//*   August 8, 1994 - Added a data member to the Zmodem class to handle
//*                    garbage collection.
//*   August 8, 1994 - The GFMointor parameter of the GFKermit constructor
//*                    was not set to default of NULL as it should have been.
//*   August 19, 1994 - Added support for the Watcom C++ 10.0 compiler.
//*   February 14, 1995
//*         Added support for several Zmodem options for 3.0 update.
//*

#ifndef FILEXFER_HPP
#define FILEXFER_HPP

#include <stdio.h>
#include "cppcomp.h"
#include "compplib.hpp"
#if !defined( GCPP_OS2 )
  #include <dos.h>
#endif

#include "_engine.hpp"

//*
//* Transfer types
//*
#define XTYPE_XMODEM     0
#define XTYPE_XMODEM_1K  1
#define XTYPE_YMODEM     2
#define XTYPE_XMODEM_1KG 3
#define XTYPE_YMODEM_G   4
#define XTYPE_KERMIT     5
#define XTYPE_ASCII      6
#define XTYPE_ZMODEM     7
#define XTYPE_BPLUS 	 8

#define XMODEM_RECEIVE_TIMEOUT        10000//* Buffer receive timeout in seconds
#define XMODEM_STARTUP_TIMEOUT        5000 //* Sender startup timeout
#define XMODEM_ACK_TIMEOUT            10000//* Time to wait for ACK after buff
#define XMODEM_MAX_ERRORS             10  //* Maximum number of block retries

#define KERMIT_MY_MAX_SIZE            94  //* My maximum kermit buffer size.
#define KERMIT_MY_PACKET_TIMEOUT      10  //* Buffer receive timeout in seconds
#define KERMIT_MY_EOL                 '\r'//* The EOL character I want.
#define KERMIT_MY_PAD_COUNT           0   //* My incoming pad character count.
#define KERMIT_MY_PAD_CHAR            0   //* My incoming pad characer.
#define KERMIT_MY_QUOTE_CHAR          '#' //* My quote character.
#define KERMIT_MY_QBIN_CHAR           '&' //* My default QBIN quoting character
#define KERMIT_MY_QBIN_DEFAULT        'Y' //* My default QBIN receive state
#define KERMIT_MY_MARK_CHARACTER      1   //* My mark character, normally ^A
#define KERMIT_MY_REPEAT_CHARACTER    '~' //* My repeat char
#define KERMIT_MAX_ERRORS             10  //* Maximum number of errors in Kermit

#define ASCII_RECEIVE_TIMEOUT         10000

//*
//* The following enumerated constants are effective only with a
//* Zmodem send session.  They are used to specify which, if any,
//* overwrite option is enabled by the sender.
//*
enum ZmodemOverwriteOptions {
        ZMODEM_NO_OVERWRITE_OPTION          = 0,
        ZMODEM_OVERWRITE_NEWER_OR_LONGER    = 1,
        ZMODEM_OVERWRITE_APPEND             = 2,
        ZMODEM_OVERWRITE_ALWAYS             = 3,
        ZMODEM_OVERWRITE_DIFF               = 4,
        ZMODEM_OVERWRITE_PROT               = 5,
        ZMODEM_OVERWRITE_NEW                = 6
};

//*
//* Class GFMonitor definitions
//*

//*
//* Class GFMonitor is provided almost entirely in order that
//* the user can derive his own classes from it. However it
//* is not a purely abstract class since the constructor for
//* class Transfer will create an instance of it if it is
//* not provided with a pointer to an instance of a derived
//* class by the programmer.
//* Once a class is derived form it, the user can define a
//* replacement for the user_monitor, message_function,
//* and abort_function.
//*
class GCPP_CLASS_TYPE GFMonitor
{
  public:
    GCPP_PROTO GFMonitor( void );
    virtual void GCPP_PROTO Initialize( int direction );
    virtual int GCPP_PROTO UserMonitor( int code );
    virtual void GCPP_PROTO TransferMessage( char GCPP_DLL_FAR *message );
    virtual int GCPP_PROTO TransferAbort( void );
    virtual void GCPP_PROTO AsciiPrinter( char c );
    void GCPP_PROTO SetAbortKey( unsigned int newkey );

    int sending;                    //* sending or receiving?
    int transfer_type;              //* transfer protocol
    int check;                      //* error checking protocol
    char GCPP_DLL_FAR * GCPP_DLL_FAR *etype;//* descriptions for above

    char GCPP_DLL_FAR *fname;       //* file name (if known)
    int file_count;                 //* Number of files transfered
		unsigned long file_length;			//* Length of file if known
    long file_time;                 //* Time stamp of file if known

    unsigned long packno;           //* current packet number
    int block_number;               //* current block number
    int block_size;                 //* current block size 128 or 1024
    unsigned long block_checksum;   //* current checksum value
    unsigned long packex;           //* packets expected, if known
    unsigned long nbytes;           //* bytes sent/received so far

    int lasterr;                    //* last error number
    int errors;                     //* total number of errors
    int block_error_count;          //* total errors for a block

    char GCPP_DLL_FAR *pmess;       //* for special messages sent from protocol
    unsigned int transfer_abort_key;//* the abort key
};


//*
//* class GFTransfer definition
//*

//*
//* Class GFTransfer is the file transfer control class. It
//* provides a base class for the Transtxt, Xmodem and
//* Kermit classes, as well as for any other transfer
//* protocols which may be implemented. The class is not meant
//* to be used directly, and is therefore implemented as an
//* abstract class. The use of a protected constructor ensures
//* that this is the case.
//*
class GCPP_CLASS_TYPE GFTransfer
{
  protected:
    GCPP_PROTO GFTransfer( GFSerial GCPP_DLL_FAR *com,
                           GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );

  public:
    virtual GCPP_PROTO ~GFTransfer( void );
    virtual int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    virtual int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename = (char *)NULL );
    virtual int GCPP_PROTO ReplaceFileEngine( GFFileIO GCPP_DLL_FAR *new_file_engine = (GFFileIO *)NULL );
    virtual void GCPP_PROTO SetCheckCarrierFlag( int flag );

  protected:
    int use_default_file_engine;          //* use the default file system
    int file_opened;                      //* is file opened or closed?
    int clean;                            //* made our own Monitor?
    int check_cd;                         //* Check for CD signal?
    GFSerial GCPP_DLL_FAR *comport;       //* pointer to Serial
    GFLineStatus GCPP_DLL_FAR *ls;        //* pointer to line status object
    GFMonitor GCPP_DLL_FAR *usermon;      //* pointer to users monitor object
    GFFileIO GCPP_DLL_FAR *FileEngine;    //* pointer to file engine object
    unsigned char GCPP_DLL_FAR *XBuffer;  //* Transfer buffer
};


//*
//* class GFTransHelper definition
//*

//* The class GFTransHelper contains a set of functions that are used
//* by just about all the file transfer classes.  We let the GFTransHelper
//* class inherit the default structure of the GFTransfer class.
//* The other file transfer class then inherit the GFTransHelper class
//* to get the proper structure and the additional support functions.
//*
class GCPP_CLASS_TYPE GFTransHelper : public GFTransfer
{
  protected:
    GCPP_PROTO GFTransHelper( GFSerial GCPP_DLL_FAR *com,
                              GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );

    GCPP_PROTO ~GFTransHelper( void );
    virtual int GCPP_PROTO Initialize( void );
    void  GCPP_PROTO Message( const char GCPP_DLL_FAR *format, ... );
    void  GCPP_PROTO Cleanup( void );
    int   GCPP_PROTO FlushInput( void );
    int   GCPP_PROTO OpenFile( void );
    int   GCPP_PROTO CheckForAbort( void );
    char  GCPP_DLL_FAR * GCPP_PROTO StripPathComponent( unsigned char GCPP_DLL_FAR *name );
    int   GCPP_PROTO WriteCharTimed( unsigned char c );
    int   GCPP_PROTO WriteBufferTimed( unsigned char GCPP_DLL_FAR *buffer,
                                       int length );
};


//*
//* class GFXmodem definition
//*

//*
//* This is the definition for the Xmodem file transfer system.  Just
//* about all the other protocols are built using is class. Most
//* inherit this class and make minor modifications to the data
//* using the constructors.
//*
class GCPP_CLASS_TYPE GFXmodem : public GFTransHelper
{
  public:
    GCPP_PROTO GFXmodem( GFSerial GCPP_DLL_FAR *com,
                         GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFXmodem( void );
    virtual int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    virtual int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename );

  protected:
    //*
    //* Receive file routines
    //*
    void  GCPP_PROTO _ReceiveFile( void );
    int   GCPP_PROTO ReceiveBlock( void );
    int   GCPP_PROTO CheckBlockNumber( void );
    int   GCPP_PROTO VerifyChecksum( void );
    int   GCPP_PROTO GetChecksum( void );
    int   GCPP_PROTO ReadDataForBlock( void );
    int   GCPP_PROTO GetBlockNumber( void );
    int   GCPP_PROTO GetFirstCharacter( void );
    int   GCPP_PROTO SendNAK( void );
    int   GCPP_PROTO SendACK( void );

    //*
    //* Send file routines
    //*
    void  GCPP_PROTO _SendFile( void );
    int   GCPP_PROTO SendBlock( void );
    int   GCPP_PROTO SendChecksum( void );
    int   GCPP_PROTO SendBlockNumber( void );
    int   GCPP_PROTO SendStartCharacter( void );
    int   GCPP_PROTO GetFirstNAK( void );
    int   GCPP_PROTO ReadNextFileBlock( void );
    int   GCPP_PROTO GetACK( void );
    int   GCPP_PROTO SendEOT( void );

    int   GCPP_PROTO Initialize( void );

    GFChecksum GCPP_DLL_FAR *XCheck;
    int crc_mode;
    int last_control_char;
    long ack_timeout;
};


//*
//* class GFXmodem1K definition
//*
class GCPP_CLASS_TYPE GFXmodem1K : public GFXmodem
{
  public:
    GCPP_PROTO GFXmodem1K( GFSerial GCPP_DLL_FAR *com,
                           GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFXmodem1K( void );
};


//*
//* class GFXmodem1KG definition
//*
class GCPP_CLASS_TYPE GFXmodem1KG : public GFXmodem
{
  public:
    GCPP_PROTO GFXmodem1KG( GFSerial GCPP_DLL_FAR *com,
                            GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFXmodem1KG( void );
};


//*
//* class GFYmodem definition
//*
class GCPP_CLASS_TYPE GFYmodem : public GFXmodem
{
  public:
    GCPP_PROTO GFYmodem( GFSerial GCPP_DLL_FAR *com,
                         GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFYmodem( void );
    int GCPP_PROTO  ReceiveFile( char GCPP_DLL_FAR *filename = (char *)NULL );
    int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    void GCPP_PROTO SetRecurseFlag( int flag );
    void GCPP_PROTO SetStripPathOption( int flag );

  protected:
    void GCPP_PROTO _YmodemReceive( void );
    void GCPP_PROTO _YmodemSend( void );

    char strip_path_from_filename;
    int  subdirectory_flag;
    long unix_time;
};


//*
//* class GFYmodemG definition
//*
class GCPP_CLASS_TYPE GFYmodemG : public GFYmodem
{
  public:
    GCPP_PROTO GFYmodemG( GFSerial GCPP_DLL_FAR *com,
                          GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFYmodemG( void );
};


//*
//* class GFZmodem definition
//*
class GCPP_CLASS_TYPE GFZmodem : public GFTransHelper
{
  public:
    GCPP_PROTO GFZmodem( GFSerial GCPP_DLL_FAR *com,
                         GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFZmodem( void );
    int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename = (char *)NULL );
    int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    void GCPP_PROTO SetRecurseFlag( int flag );
    void GCPP_PROTO SetStripPathOption( int flag );
    void GCPP_PROTO SetCrashRecovery( int flag );
    void GCPP_PROTO SetResumeSend( int flag );
    void GCPP_PROTO SetUseWindow( int flag );
    void GCPP_PROTO SetBlockBeforeWrite( int flag );
    void GCPP_PROTO SetOverwriteOption( enum ZmodemOverwriteOptions option );
    void GCPP_PROTO SetMaxErrors( int count );

  protected:
    //*
    //* Receive file routines
    //*
    void GCPP_PROTO _ReceiveFile( void );
    int  GCPP_PROTO GetFileName( void );
		int  GCPP_PROTO SetRXFileOptions( void );
    void GCPP_PROTO SendFileOffset( int frame_type );
    long GCPP_PROTO ConvertHeaderToOffset( void );
    void GCPP_PROTO ReadFileData( void );

    //*
    //* Send file routines
    //*
    void GCPP_PROTO _SendFile( void );
    int GCPP_PROTO OpenLink( void );
    int GCPP_PROTO OpenNextFile( void );
    int GCPP_PROTO CloseLink( void );
    int GCPP_PROTO ReadBlock( void );
    int GCPP_PROTO BackChannelActivity( void );
		int GCPP_PROTO SendDataFrame( void );
    int GCPP_PROTO StartFileTransfer( void );
    void GCPP_PROTO SendFileData( void );


    //*
    //* Common transfer routines
    //*
    int GCPP_PROTO ReadFrameHeader( int immediate_return );
    int GCPP_PROTO GetPad( void );
    int GCPP_PROTO GetHeaderType( void );
    int GCPP_PROTO ReadHexFrameHeaderData( void );
    int GCPP_PROTO ReadBinaryHeaderData16( void );
    int GCPP_PROTO ReadBinaryHeaderData32( void );
    int GCPP_PROTO ReadHexByte( void );
    int GCPP_PROTO GetCookedChar( void );
    int GCPP_PROTO SendBinaryHeader( char frame_type,
                                     char GCPP_DLL_FAR *header_data );
    int GCPP_PROTO SendHexHeader( char frame_type,
                                  unsigned char GCPP_DLL_FAR *header_data );
    int GCPP_PROTO SendBinaryPacket( int length, char terminator );
    int GCPP_PROTO EncodeCharacter( char *buf, unsigned char c );
    int GCPP_PROTO SendEncodedCharacter( char c );
    int GCPP_PROTO ReadBinaryByte( void );
    int GCPP_PROTO ReadCrc16( int terminator );
    int GCPP_PROTO ReadCrc32( int terminator );
    int GCPP_PROTO ReadBinaryPacket( void );
    int GCPP_PROTO OpenFile( void );

    unsigned char *IBuffer;
    int ibuf_size;
    long unix_time;

    int cancel_count;
    int garbage_count;
    int rx_flags;
    int error_recovery;
    unsigned int rx_buffer_size;
    unsigned int rx_frame_output_count;
    unsigned char header[ 4 ];
    int header_type;
    int crash_recovery;
    int block_before_write;
    char strip_path_from_filename;
    int  subdirectory_flag;
    int  crc_mode;
    int  read_full_packet;
    int use_window;
		unsigned int window_size;
    int resume_send;
    int conversion_options;
    int management_options;
    int max_errors;
    int subpacket_size;
    GFCrc16 GCPP_DLL_FAR *XCheck16;
    GFCrc32 GCPP_DLL_FAR *XCheck32;
};


//*
//* class GFTranstxt definition
//*
class GCPP_CLASS_TYPE GFTranstxt : public GFTransHelper
{
  public:
    GCPP_PROTO GFTranstxt( GFSerial GCPP_DLL_FAR *com,
                           GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL,
                           unsigned delay = 1000 );
    GCPP_PROTO ~GFTranstxt( void );
    int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename );
    void GCPP_PROTO SetRecurseFlag( int flag );
    void GCPP_PROTO SetLineFeedStripping( int flag );

  protected:
    int GCPP_PROTO _ReceiveFile( void );
    int GCPP_PROTO AsciiReceiveReadData( void );
    int GCPP_PROTO AsciiReceiveWaitForRxData( void );

    int GCPP_PROTO _SendFile( void );
    int GCPP_PROTO AsciiSendExecuteLineDelay( void );
    int GCPP_PROTO AsciiSendLoadUpBuffer( void );
    int GCPP_PROTO AsciiSendTheBuffer( void );
    int GCPP_PROTO AsciiSendEchoSomeCharacters( void );

    long timeout_time;
    int  delay_milliseconds_per_line;
    int  subdirectory_flag;
    int  strip_lf;
};


//*
//* class GFKermit definition
//*

//*
//* The GFKermit class uses the GFTransHelper class but does not
//* inherit anything from the Xmodem type transfers.
//*

#define tochar( x )   ( ( x ) + 32 )
#define unchar( x )   ( ( x ) - 32 )
#define ctl( x )      ( ( x ) ^ (unsigned char) 64 )


class GCPP_CLASS_TYPE GFKermit : public GFTransHelper
{
  public:
    GCPP_PROTO GFKermit( GFSerial GCPP_DLL_FAR *com,
                         GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
    GCPP_PROTO ~GFKermit( void );
    int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename = (char *)NULL );
    int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
    void GCPP_PROTO SetRecurseFlag( int flag );
    void GCPP_PROTO SetStripPathOption( int flag );
    void GCPP_PROTO SetDataType( int flag );

  protected:
    //*
    //* Receive file routines
    //*
    int  GCPP_PROTO _ReceiveFile( void );
    int  GCPP_PROTO GetInitPacket( void );
    int  GCPP_PROTO GetBorF( void );
    void GCPP_PROTO SetDate( char GCPP_DLL_FAR *data );
    void GCPP_PROTO ProcessCapability( void );
    int  GCPP_PROTO ReadFileData( void );
    int  GCPP_PROTO SendInitialAck( void );
    int  GCPP_PROTO SendAck( void );
    int  GCPP_PROTO ResendAck( void );
    int  GCPP_PROTO ResendInitialAck( void );
    int  GCPP_PROTO SaveBuffer( void );

    //*
    //* Send file routines
    //*
    int GCPP_PROTO _SendFile( void );
    int GCPP_PROTO SendFileBlocks( void );
    int GCPP_PROTO SendFileName( void );
    int GCPP_PROTO FillBuffer( char *buffer );
    int GCPP_PROTO SendPacketAndWaitForACK( char type,
                                            int buffer_length,
                                            char GCPP_DLL_FAR *buffer );

    //*
    //* Common routines
    //*
    int  GCPP_PROTO Initialize( void );
    int  GCPP_PROTO SendPacket( char type,
                                int length,
                                char GCPP_DLL_FAR *buffer );
    char GCPP_PROTO ReceivePacket( void );
    int  GCPP_PROTO ChecksumCalculate( unsigned char GCPP_DLL_FAR *buffer );
    int  GCPP_PROTO InitParameters( char data[ 10 ] );
    int  GCPP_PROTO UpdateErrorCount( void );
    void GCPP_PROTO LoadInitParameters( void );
    void GCPP_PROTO KermitCleanup( void );

    int  max_length;
    int  qbin;
    int  text_mode;
    char timeout;
    char pad_count;
    char pad_char;
    char eol;
    char quote_char;
    char his_qbin_char;
    char my_qbin_char;
    char repeat_flag;
    char his_repeat_char;
    char my_repeat_char;
    char his_capabilities;
    char strip_path_from_filename;
    int  subdirectory_flag;
};

#define BPLUS_SEND_AHEAD_BUFFERS	5
#define BPLUS_MAX_ERRORS					10

typedef struct GCPP_CLASS_TYPE pending_info {
  int seq_num;
  int packet_size;
  unsigned char *packet;
} PENDING_PACKET;

//*
//* class GFCompuServeBPlus definition
//*
class GCPP_CLASS_TYPE GFCompuServeBPlus : public GFTransHelper
{
  public:
		GCPP_PROTO GFCompuServeBPlus( GFSerial GCPP_DLL_FAR *com,
                         GFMonitor GCPP_DLL_FAR *mon = (GFMonitor *)NULL );
		GCPP_PROTO ~GFCompuServeBPlus( void );
    static int GCPP_PROTO Respond( GFSerial GCPP_DLL_FAR *cp, int use_b_plus = 1 );
		int GCPP_PROTO TransferFile( void );
		int GCPP_PROTO ReceiveFile( char GCPP_DLL_FAR *filename = (char *)NULL );
    int GCPP_PROTO SendFile( char GCPP_DLL_FAR *filename );
		void GCPP_PROTO SetSendFileInfoFlag( int flag );
    void GCPP_PROTO SetStripPathOption( int flag );
		void GCPP_PROTO SetChecksumMethod( int method );
		void GCPP_PROTO SetResumeDownloadFlag( int flag );

  protected:
    //*
    //* Common transfer routines
    //*
		int GCPP_PROTO Initialize( void );
		void GCPP_PROTO InitCheck( void );
		void GCPP_PROTO DoChecksum( unsigned short ch );
		void GCPP_PROTO DiscardAckedPackets( void );
		int GCPP_PROTO WriteCharTimed( unsigned char c );
		void GCPP_PROTO SendAck( void );
		void GCPP_PROTO SendMaskedByte( unsigned short ch );
		int GCPP_PROTO ReadByte( int timeout_opt );
		int GCPP_PROTO ReadMaskedByte( void );
		void GCPP_PROTO TransmitPacket( int size, int seq, unsigned char *packet );
		int GCPP_PROTO WaitForAck( int timeout_opt,
															int rec_idle,
															int send_ack,
															int resend );
		int GCPP_PROTO FlushPending( void );
		void GCPP_PROTO Fail( unsigned char fail_code );
		void GCPP_PROTO SendUnexpectedPacket( void );
		void GCPP_PROTO SendAbort( void );
		int GCPP_PROTO ReadPacket( int rec_dle, int timeout_opt, int send_ack );
    int GCPP_PROTO SendPacket( int size, unsigned char *buffer );
		int GCPP_PROTO RespondToPlus( void );
		int GCPP_PROTO _ReceiveFile( void );
		int GCPP_PROTO _SendFile( void );
		int GCPP_PROTO HandleProtocol( void );

		unsigned char *IBuffer;
    int ibuf_size;

		int strip_path_from_filename;
		int transfer_complete;
		int s_seq_num;
		int buffer_size;
		int char_timeout;
		int last_character;
		int not_masked;
		int packets_not_acked;
		PENDING_PACKET pending[ BPLUS_SEND_AHEAD_BUFFERS ];
		int pending_count;
		int next_packet;
		int data_packet_size;
		int sent_enq;
		int last_ack;
		int quoting;
		int use_bplus;
		int want_7_bit;
		char resume_download;
		char resume_upload;
		char send_file_info;
		char check_method;
		char check_crc;
		char window_size;
		char rec_window_size;
		char use_transport;
		char mask[ 32 ];
		GFCrc16 GCPP_DLL_FAR *XCheck16;
		GFCrc32 GCPP_DLL_FAR *XCheck32;
		GFCCITTCrc16 GCPP_DLL_FAR *XCCITT16;
};

#endif
