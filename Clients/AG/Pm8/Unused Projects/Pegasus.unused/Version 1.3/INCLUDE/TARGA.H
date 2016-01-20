/*.T    Header to support TARGA routines   */
//  ZDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD7
//  3                                                                       :
//  3   Support for TARGA files                                             :
//  3                                                                       :
//  TMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM<

//modified 1/23/96 -- jweber -- added TARGA_FOOTER

#if !defined(_TARGA)
#define _TARGA

#include    "stdtypes.h"

typedef struct  {
			BYTE    id_size, cm_type, image_type;
			WORD    cm_org, cm_length;
			BYTE    cm_elt_size;
			WORD    x_org, y_org;
			WORD    width, height;
			BYTE    ips, idb;
		} TARGA_HEAD;

typedef struct {
	DWORD extension_offset;
		//offset of extension area, or 0
	DWORD developer_offset;
		//offset of developer directory, or 0
	BYTE signature[18];
		//must be "TRUEVISION-XFILE.\0"; else not a valid footer
} TARGA_FOOTER;

#endif
