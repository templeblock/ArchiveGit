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
