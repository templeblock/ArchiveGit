/*
PKWARE Data Compression Library
Copyright 1990,1991 by PKWARE Inc.  All Rights Reserved.
*/

#ifdef __cplusplus
	extern "C" {
#endif

unsigned short far pascal implode(
  unsigned (far pascal * read_buff)(char far *buffer,unsigned short far *size),
  unsigned (far pascal * write_buff)(char far *buffer,unsigned short far *size),
  char far *work_buff,
  unsigned short int far *type,
  unsigned short int far *dsize);


unsigned far pascal explode(
  unsigned (far pascal * read_buff)(char far *buffer,unsigned short far *size),
  unsigned (far pascal * write_buff)(char far *buffer,unsigned short far *size),
  char far *work_buff);

unsigned long far pascal crc32(
  unsigned char far *buffer,
  unsigned short int far *size,
  unsigned long far *old_crc);
#ifdef __cplusplus
   }                         // End of 'extern "C"' declaration
#endif

#define CMP_BINARY             0
#define CMP_ASCII              1

#define CMP_NO_ERROR           0
#define CMP_INVALID_DICTSIZE   1
#define CMP_INVALID_MODE       2
#define CMP_BAD_DATA           3
#define CMP_ABORT              4
