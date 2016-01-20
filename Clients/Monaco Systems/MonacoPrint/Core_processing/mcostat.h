
#ifndef MON_STATUS
#define MON_STATUS
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//
//	monstat.h
//
//	Defines Monacco status codes
//
//	Create Date:	7/18/93
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

							// Numerical value of the status
typedef enum {
	MCO_SUCCESS = 							0,
	MCO_FAILURE = 							1,
	MCO_MEM_ALLOC_ERROR =					2,
	MCO_SYSTEM_ERROR =						3,
	MCO_UNSUPPORTED_OPERATION =				4,
	MCO_UNSUPPORTED_MODE =					5,
	MCO_INVALID_LAYOUT =					6,
	MCO_UNSUPPORTED_LAYOUT =				7,
	MCO_UNSUPPORTED_NUM_COMPONENTS =		8,
	MCO_FILE_ERROR =						9,
	MCO_FILE_OPEN_ERROR =					10,
	MCO_FILE_CREATE_ERROR =					11,
	MCO_FILE_READ_ERROR =					12,
	MCO_FILE_WRITE_ERROR =					13,
	MCO_FILE_NOT_OPEN =						14,
	MCO_FILE_DATA_ERROR =					15,
	MCO_FILE_EOF_ERROR =					16,
	MCO_FILE_BOF_ERROR =					17,
	MCO_NOT_IMPLEMENTED =					18,
	MCO_BAD_VALUE =							19,
	MCO_BAD_PARAMETER =						20,
	MCO_OBJECT_NOT_INITIALIZED =			21,
	MCO_OBJECT_ALREADY_INITIALIZED =		22,
	MCO_OUT_OF_RANGE =						23,
	MCO_WRONG_NUM_COMP = 					24,		 //Added by James Vogh
	MCO_NOT_CALIB =							25,      //Added by James Vogh
	MCO_BADTARGET =							26,      //Added by James Vogh
	MCO_BADEVE = 							27,		 //Added by James Vogh
	MCO_BADCOUNT = 							28,		 //Added by James Vogh
	MCO_NOUSE = 							29,		 //Added by James Vogh
	MCO_RSRC_ERROR = 						30,		 //Added by James Vogh
	MCO_NOT_PRESENT = 						31,		 //Added by James Vogh
	MCO_FILE_LOCKED = 						32,		 //Added by James Vogh
	MCO_ABORTED =							33,
	MCO_NOT_TIFF_FILE = 					34,		 //Added by Peter Zhang
	MCO_NOT_PHOTO25_FILE = 					35, 	 //Added by Peter Zhang
	MCO_PARSE_TIFF_ERROR =					36,      //Added by Peter Zhang
	MCO_READ_TIFF_ERROR	=					37,		 //Added by Peter Zhang
	MCO_WRITE_TIFF_ERROR = 					38,		 //Added by Peter Zhang
	MCO_COMPRESSION_TIFF_NOTSUPPORTED = 	39,		 //Added by Peter Zhang
	MCO_PARSE_PHOTO25_ERROR = 				40, 	 //Added by Peter Zhang
	MCO_READ_PHOTO25_ERROR	= 				41,		 //Added by Peter Zhang
	MCO_WRITE_PHOTO25_ERROR	= 				42,		 //Added by Peter Zhang
	MCO_COMPRESSION_PHOTO25_NOTSUPPORTED = 	43,		 //Added by Peter Zhang
	MCO_INVALID_CMYK_TABLE	=				44,		 //Added by Peter Zhang
	MCO_CMYK_TABEL_NOT_LOADED =				45,		 //Added by Peter Zhang
	MCO_READ_CMYK_TABLE_ERROR = 			46,		 //Added by Peter Zhang
	MCO_OPEN_CMYK_TABLE_ERROR = 			47, 	 //Added by Peter Zhang
	MCO_NOT_A_RGB_IMAGE	=					48,		 //Added by Peter Zhang
	MCO_INVALID_FILE_FORMAT = 				49,		 //Added by Peter Zhang
	MCO_DISK_FULL = 						50,		 //Added by Peter Zhang
	MCO_SOURCE_FOLDER_EMPTY = 				51,		 //Added by Peter Zhang
	MCO_SOURCE_FOLDER_FULL	=				52,		 //Added by Peter Zhang
	MCO_DESTINATION_FOLDER_FULL = 			53,		 //Added by Peter Zhang
	MCO_WRITE_EPS_ERROR			=			54,		 //Added by Peter Zhang
	MCO_OPEN_JOB_ERROR			=			55,		 //Added by Peter Zhang
	MCO_SAVE_JOB_ERROR			=			56,	     //Added by Peter Zhang
	MCO_WAIT					=			57,		 //Added by Peter Zhang
	MCO_FILE_ALREADY_OPEN		= 			58,  	 //Added by Peter Zhang
	MCO_OPEN_CMYK_ERROR			=			59,  	 //Added by Peter Zhang
	MCO_OPEN_SRC_ERROR			=			60,  	 //Added by Peter Zhang
	MCO_OPEN_DEST_ERROR			=			61,  	 //Added by Peter Zhang
	MCO_NOT_A_GRAY_IMAGE		=			62,  	 //Added by Peter Zhang
	MCO_OPEN_SET_ERROR			=			63,		 //Added by Peter Zhang
	MCO_FILE_NAME_ERROR			=			64,		 //Added by Peter Zhang
	MCO_COR_SET_ERROR			=			65,		 //Added by Peter Zhang
	MCO_CANCEL					=			66,		 //Added by Peter Zhang
	MCO_DELETE					=			67,		 //Added by Peter Zhang
	MCO_QUIT					=			68,		 //Added by Peter Zhang
	MCO_START					=			69,		 //Added by Peter Zhang
	MCO_SINGULAR				=			70,		 //Added by Peter Zhang
	MCO_NO_QD3D					= 			71,		 //Added by James
	MCO_QD3D_ERROR				= 			72,		 //Added by James
	MCO_SERIAL_ERROR			=			73,
	MCO_MAX_MEMORY				= 			74,
	MCO_INVALID_PTR				=			75,
	MCO_SERIAL_DEVICE_ERROR		=			76,
	MCO_DEVICE_NOT_FOUND		= 			77,
	MCO_NOT_ICC_LINK			= 			78
} McoStatus;

#endif