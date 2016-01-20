/*----------------------------------------------------------------------*
*                                                                       *
*                   Copyright 1995 INSO Corporation                     *
*                         Common Code Library                           *
*                                                                       *
* This work is protected by United States and international copyright   *
* laws and contains certain trade secret information.  All use of this  *
* work is governed by the terms of agreements between INSO Corporation  *
* and its licensees or potential licensees.  Disclosure or disassembly  *
* of the contents of embodied programs or algorithms is prohibited.     *
* All rights are reserved worldwide.                                    *
*                                                                       *
*-----------------------------------------------------------------------*

*-----------------------------------------------------------------------*
*                                                                       *
*        slascii.h - ASCII Char defines                                 *
*                                                                       *
*-----------------------------------------------------------------------*

        Description:
            This file contains #defines for the non-extended ASCII character 
            set.  These defines maintain their ASCII value across all
            platforms and compilers.

        Cautions:
            Never quote a literal character in this file, e.g., ':' to 
            represent a colon.  Quoted characters will be translated to the
            value in the character set native to the compiler, and that char
            set won't necessarily be ASCII.  Therefore, such a define's 
            value would NOT represent the same value, the correct ASCII 
            value, in all cases, defeating the purpose of this file.

	Creation Date:
	    18-jul-89	

        23-feb-93   JCC     Represent chars w/hex (to port to non-ASCII envs)
        17-aug-93   JCC     NEWLINE is a C construct, not ASCII - moved it

*-----------------------------------------------------------------------*/
#ifndef SLASCII_H

#define SLASCII_H


/*        Commonly Used ASCII Character Codes                           */

#define BACKSPACE       0x08            /* backspace key                */
#define CR              0x0d            /* carriage return              */
#define LF              0x0a            /* linefeed                     */
#define SPACE           0x20            /* space character              */
#define TAB             0x09            /* tab character                */
#define FORMFEED        0x0c            /* formfeed                     */
#define ESC             0x1b            /* escape character             */
#define BELLCHR         0x07            /* bell character               */
#define BCKSLASH        0x5c            /* backslash character          */
#define SLASH           0x2f            /* slash character              */
#define COMMA           0x2c            /* , comma character            */
#define COLON           0x3a            /* : colon character            */
#define DOT             0x2e            /* . period (dot) chracter      */
#define SEMICOLON       0x3b            /* ; semi colon character       */
#define APOSTROPHE      0x27            /* ' apostrophe                 */
#define SINGLE_QUOTE    0x27            /* ' single quote               */
#define DOUBLE_QUOTE    0x22            /* " double quote               */
#define GRAVE_QUOTE     0x60            /* ` grave single quote         */
#define UNDERSCORE      0x5f            /* underscore charcter          */
#define HYPHEN          0x2d            /* hyphen character             */
#define LBRACK          0x5b            /* left square bracket          */
#define RBRACK          0x5d            /* right square bracket         */
#define ENDASH          0x2d            /* en-dash/hyphen mark          */
#define BANG            0x21            /* exclamation mark             */
#define QMARK           0x3f            /* quesiton mark                */
#define LPAREN          0x28            /* left parenthesis             */
#define RPAREN          0x29            /* right parenthesis            */
#define POUND           0x23            /* pound sign                   */
#define DOLLAR          0x24            /* dollar sign                  */
#define PERCENT         0x25            /* percent sign                 */
#define AMPERSAND       0x26            /* and sign                     */
#define MULT_SIGN       0x2a            /* multiply sign                */
#define PLUS            0x2b            /* addition sign                */
#define CHAR_0          0x30            /* zero character               */
#define CHAR_1          0x31            /* one character                */
#define CHAR_2          0x32            /* two character                */
#define CHAR_3          0x33            /* three character              */
#define CHAR_4          0x34            /* four character               */
#define CHAR_5          0x35            /* five character               */
#define CHAR_6          0x36            /* six character                */
#define CHAR_7          0x37            /* seven character              */
#define CHAR_8          0x38            /* eight character              */
#define CHAR_9          0x39            /* nine character               */
#define LESS_THAN       0x3c            /* less than                    */
#define GREATER_THAN    0x3e            /* greater than                 */
#define EQUAL           0x3d            /* equal sign                   */
#define AT_SIGN         0x40            /* at sign character            */
#define CARET           0x5e            /* caret character (shift 6)    */
#define TILDE           0x7e            /* tilde character              */
#define LBRACE          0x7b            /* left brace                   */
#define VBAR            0x7c            /* vertical bar                 */
#define RBRACE          0x7d            /* right brace                  */
                                                            
/*        Commonly Used ASCII Strings                                   */

#define NULLSTR         ""              /* null string                  */
#define SPACESTR        "\040"          /* space string                 */
#define DOTSTR          "\056"          /* period (dot) string          */
#define COLONSTR        "\072"          /* colon string                 */
#define SEMICSTR        "\073"          /* semi-colon string            */
#define COMMASTR        "\054"          /* comma string                 */
#define BCKSLSTR        "\134"          /* back slash string            */
#define SLASHSTR        "\057"          /* slash string                 */
#define NLINESTR        "\012"          /* new line string              */
#define SQSTR           "\047"          /* single quote string          */
#define DQSTR           "\042"          /* double quote string          */

/*        Defines for Alphabet Character Set                            */

#define U_A              0x41           /* upper case characters        */
#define U_B              0x42
#define U_C              0x43
#define U_D              0x44
#define U_E              0x45
#define U_F              0x46
#define U_G              0x47
#define U_H              0x48
#define U_I              0x49
#define U_J              0x4a
#define U_K              0x4b
#define U_L              0x4c
#define U_M              0x4d
#define U_N              0x4e
#define U_O              0x4f
#define U_P              0x50
#define U_Q              0x51
#define U_R              0x52
#define U_S              0x53
#define U_T              0x54                                     
#define U_U              0x55
#define U_V              0x56
#define U_W              0x57
#define U_X              0x58
#define U_Y              0x59
#define U_Z              0x5a

#define L_A              0x61           /* lower case characters        */
#define L_B              0x62
#define L_C              0x63
#define L_D              0x64
#define L_E              0x65
#define L_F              0x66
#define L_G              0x67
#define L_H              0x68
#define L_I              0x69
#define L_J              0x6a
#define L_K              0x6b
#define L_L              0x6c
#define L_M              0x6d
#define L_N              0x6e
#define L_O              0x6f
#define L_P              0x70
#define L_Q              0x71
#define L_R              0x72
#define L_S              0x73
#define L_T              0x74
#define L_U              0x75
#define L_V              0x76
#define L_W              0x77
#define L_X              0x78
#define L_Y              0x79
#define L_Z              0x7a

#endif /* SLASCII_H */

/*    End of file - slascii.h    */
