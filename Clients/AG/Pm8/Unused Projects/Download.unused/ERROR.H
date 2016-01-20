#ifndef _ERROR_H
#define _ERROR_H
/*
// Possible errors returned by a routine.
*/

enum ERRORCODE
{
	ERRORCODE_None = 0,				/* No error. All is well. */

	ERRORCODE_NotInitialized	= -1,		/* Not init'd. Can't do anything. */
	ERRORCODE_BadParameter		= -2,		/* Some parameter is not valid. */
	ERRORCODE_IntError			= -3,		/* Internal error. */

	ERRORCODE_Open					= -4,		/* Error opening device. */
	ERRORCODE_Access				= -5,		/* Error accessing device (priviledge). */
	ERRORCODE_AccessLimit		= -6,		/* Error accessing device (I/O limit). */

	ERRORCODE_Read					= -7,		/* Error reading from device. */
	ERRORCODE_Write				= -8,		/* Error writing to device. */
// ERRORCODE_Seek             is defined later in this file

	ERRORCODE_Memory				= -9,		/* Unable to allocate some memory. */

	ERRORCODE_ZeroRecord			= -10,	/* Passed a zero record # */
	ERRORCODE_InvalidRecord		= -11,	/* Invalid record # */

	ERRORCODE_NoType				= -12,	/* Passed DB_NO_TYPE */
	ERRORCODE_TypeMismatch		= -13,	/* Types don't match */
	ERRORCODE_IllegalType		= -14,	/* Type passed is not a good one */

	ERRORCODE_TooManyLocks		= -15,	/* Locked too many times */
	ERRORCODE_NotLocked			= -16,	/* Unlocked too many times */

	ERRORCODE_NotAllocated		= -17,	/* Entity is not allocated */
	ERRORCODE_Locked				= -18,	/* Entity is locked */
	ERRORCODE_Full					= -19,	/* Entity is full */

	ERRORCODE_DoesNotExist		= -20,	/* The requested thing does not exist */

	ERRORCODE_NotRegistered		= -21,	/* A function was not registered */

	ERRORCODE_IoError				= -22,	/* Some kind of I/O error */

	ERRORCODE_NoHandles			= -23,	/* No file handles available */

	ERRORCODE_Busy					= -24,	/* Entity is busy */

	ERRORCODE_Corrupt				= -25,	/* Something smells fishy! */

	ERRORCODE_VersionMismatch	= -26,	/* Different versions. */

	ERRORCODE_Fail             = -27,	/* Operation Failed. */
	ERRORCODE_OutOfSequence    = -28,	/* Routine called of of proper order. */

	ERRORCODE_Seek					= -29,	/* Error reading from device. */
	ERRORCODE_LZWNotSupported	= -30,	/* We don't support LZW. */

	ERRORCODE_Abort            = -31,   // First used for add on install

// Collection Specific
	ERRORCODE_InvalidINIEntry  = -1000,	/* An INI entry is invalid or empty. */
	ERRORCODE_NoINIFile        = -1001,	/* An INI entry is invalid or empty. */
	ERRORCODE_Done             = -1002,	/* Operation/Process completed. */
	ERRORCODE_Exists           = -1003,	/* Item already exists. */
	ERRORCODE_DataSequenceError = -1004,/* Attempt to add data in incorrect order. */
	ERRORCODE_StopListIOError  = -1005, /* Stoplist file i/o (open, read) error. */
	ERRORCODE_KeyGreaterFound  = -1006, /* Found greater than. */

   LAST_ERRORCODE
};

typedef ERRORCODE far *LPERRORCODE;

class CErrorcodeException : public CException
{
	DECLARE_DYNAMIC(CErrorcodeException);
public:
	ERRORCODE m_error;
	CErrorcodeException(ERRORCODE error)
		{ m_error = error; }
};

extern void ThrowErrorcodeException(ERRORCODE error);

#endif						/* #ifndef _ERROR_H */
