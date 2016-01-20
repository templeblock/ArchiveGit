#ifndef _ERROR_H
#define _ERROR_H
#define SUCCESS                          1   /** Function successful        **/
#define FAILURE                          0   /** Function not successful    **/

/*     NOTE: All LEADTOOLS Function errors will return a negative number    */
#define ERROR_NO_MEMORY                 -1   /** Not enough memory available**/
#define ERROR_NO_BITMAP                 -2   /** Invalid bitmap handle      **/
#define ERROR_MEMORY_TOO_LOW            -3   /** Not enough memory available**/
#define ERROR_FILE_LSEEK                -4   /** Error seeking to position  **/
#define ERROR_FILE_WRITE                -5   /** Error writing file         **/
#define ERROR_FILE_GONE                 -6   /** File not present - abort   **/
#define ERROR_FILE_READ                 -7   /** Error reading file         **/
#define ERROR_INV_FILENAME              -8   /** Invalid filename specified **/
#define ERROR_FILE_FORMAT               -9   /** Invalid file format        **/
#define ERROR_FILE_NOT_FOUND            -10  /** File not found             **/
#define ERROR_INV_RANGE                 -11  /** Invalid width/height       **/
#define ERROR_IMAGE_TYPE                -12  /** Image format recognized,   **/
 /** but sub-type not supported **/
#define ERROR_INV_PARAMETER             -13  /** Invalid parmameter passed  **/
#define ERROR_FILE_OPEN                 -14  /** Not able to open file      **/
#define ERROR_UNKNOWN_COMP              -15  /** Unknown compression format **/
#define ERROR_NOT_SUPPORTED             -16  /** Not available for file     **/
 /** format requested           **/
#define ERROR_NOT_256_COLOR             -17  /** VGA card only supports 256 **/
 /** colors (8 bit)             **/
#define ERROR_PRINTER                   -18  /** Printer error              **/
#define ERROR_CRC_CHECK                 -19  /** Data CRC check error       **/
#define ERROR_QFACTOR                   -21  /** Invalid QFactor specified  **/
#define ERROR_TARGAINSTALL              -22  /** TARGA not installed        **/
#define ERROR_OutputType                -23  /** Invalid compression format **/
#define ERROR_XORIGIN                   -24  /** X origin specified invalid **/
#define ERROR_YORIGIN                   -25  /** Y origin specified invalid **/
#define ERROR_VIDEOTYPE                 -26  /** Invalid video mode         **/
#define ERROR_BITPERPIXEL               -27  /** Invalid bits/pixel         **/
#define ERROR_WINDOWSIZE                -28  /** Invalid window size        **/
#define ERROR_NORMAL_ABORT              -29  /** Escape key pressed         **/
#define ERROR_NOT_INITIALIZED           -30  /** Internal error - call LEAD **/
#define ERROR_CU_BUSY                   -31  /** Internal error - call LEAD **/
#define ERROR_INVALID_TABLE_TYPE        -32  /** Internal error - call LEAD **/
#define ERROR_UNEQUAL_TABLES            -33  /** Internal error - call LEAD **/
#define ERROR_INVALID_BUFFER            -34  /** Internal error - call LEAD **/
#define ERROR_MISSING_TILE_DATA         -35  /** Internal error - call LEAD **/
#define ERROR_INVALID_QVALUE            -36  /** Internal error - call LEAD **/
#define ERROR_INVALID_DATA              -37  /** Internal error - call LEAD **/
#define ERROR_INVALID_COMPRESSED_TYPE   -38  /** Internal error - call LEAD **/
#define ERROR_INVALID_COMPONENT_NUM     -39  /** Internal error - call LEAD **/
#define ERROR_INVALID_PIXEL_TYPE        -40  /** Internal error - call LEAD **/
#define ERROR_INVALID_PIXEL_SAMPLING    -41  /** Internal error - call LEAD **/
#define ERROR_INVALID_SOURCE_FILE       -42  /** Internal error - call LEAD **/
#define ERROR_INVALID_TARGET_FILE       -43  /** Internal error - call LEAD **/
#define ERROR_INVALID_IMAGE_DIMS        -44  /** Internal error - call LEAD **/
#define ERROR_INVALID_TILE_DIMS         -45  /** Internal error - call LEAD **/
#define ERROR_INVALID_PIX_BUFF_DIMS     -46  /** Internal error - call LEAD **/
#define ERROR_SEGMENT_OVERFLOW          -47  /** Internal error - call LEAD **/
#define ERROR_INVALID_SUBSAMPLING       -48  /** Internal error - call LEAD **/
#define ERROR_INVALID_Q_VIS_TABLE       -49  /** Internal error - call LEAD **/
#define ERROR_INVALID_DC_CODE_TABLE     -50  /** Internal error - call LEAD **/
#define ERROR_INVALID_AC_CODE_TABLE     -51  /** Internal error - call LEAD **/
#define ERROR_INSUFFICIENT_DATA         -52  /** Internal error - call LEAD **/
#define ERROR_MISSING_FUNC_POINTER      -53  /** Internal error - call LEAD **/
#define ERROR_TOO_MANY_DC_CODE_TABLES   -54  /** Internal error - call LEAD **/
#define ERROR_TOO_MANY_AC_CODE_TABLES   -55  /** Internal error - call LEAD **/
#define ERROR_INVALID_SUBIMAGE          -56  /** Internal error - call LEAD **/
#define ERROR_INVALID_ABORTION          -57  /** Internal error - call LEAD **/
#define ERROR_CU_NO_SUPPORT             -58  /** Internal error - call LEAD **/
#define ERROR_CU_FAILURE                -59  /** Internal error - call LEAD **/
#define ERROR_BAD_POINTER               -60  /** Internal error - call LEAD **/
#define ERROR_HEADER_DATA_FAILURE       -61  /** Internal error - call LEAD **/
#define ERROR_COMPRESSED_DATA_FAILURE   -62  /** Internal error - call LEAD **/

#define ERROR_FIXEDPAL_DATA             -63  /** fixed palette data not found **/
#define ERROR_LOADFONT_DATA             -64  /** fixed palette data not found **/
#define ERROR_NO_STAMP                  -65  /** stamp not found **/

#define ERROR_G3CODE_INVALID            -70  /** invalid group 3 code **/
#define ERROR_G3CODE_EOF                -71  /** group3 code end of file **/
#define ERROR_G3CODE_EOL                -72  /** group3 code end of code **/
#define ERROR_PREMATURE_EOF             -73  /** premature EOF at scanline **/
#define ERROR_PREMATURE_EOL             -74  /** premature end-of-line code **/
#define ERROR_UNCOMP_EOF                -75  /** premature end-of-line code **/
#define ERROR_ACT_INCOMP                -76  /** premature end-of-line code **/
#define ERROR_BAD_DECODE_STATE          -77  /** premature end-of-line code **/
#define ERROR_VERSION_NUMBER            -78  /** bad version number **/

#define ERROR_TWAIN_NODSM               -79  /** source manager not found **/
#define ERROR_TWAIN_BUMMER              -80  /** Failure due to unknown causes **/
#define ERROR_TWAIN_LOWMEMORY           -81  /** Not enough memory to perform operation **/
#define ERROR_TWAIN_NODS                -82  /** No Data Source **/
#define ERROR_TWAIN_MAXCONNECTIONS      -83  /** DS is connected to max possible apps **/
#define ERROR_TWAIN_OPERATIONERROR      -84  /** DS or DSM reported error, app shouldn't **/
#define ERROR_TWAIN_BADCAP              -85  /** Unknown capability **/
#define ERROR_TWAIN_BADPROTOCOL         -86  /** Unrecognized MSG DG DAT combination **/
#define ERROR_TWAIN_BADVALUE            -87  /** Data parameter out of range **/
#define ERROR_TWAIN_SEQERROR            -88  /** DG DAT MSG out of expected sequence **/
#define ERROR_TWAIN_BADDEST             -89  /** Unknown destination App/Src in DSM_Entry **/
#define ERROR_TWAIN_CANCEL              -90  /** Operation was cancelled **/

#define ERROR_USER_ABORT               -100 /** User has aborted operation **/

#endif                          /* _ERROR_H */
