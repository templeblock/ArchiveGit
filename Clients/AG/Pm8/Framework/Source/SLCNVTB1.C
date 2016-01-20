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
*      SLCNVTB1.C - Character conversion tables and function for the    *
*                   Latin-1 Master Character Set (MCS).                 *
*                                                                       *
*-----------------------------------------------------------------------*


        Description:

        This file contains the character conversion tables for the Latin-1 
        Master Character Set.  For each platform that we support, there is
        are 2 conversion tables in this file.  One table converts from MCS
        Latin-1 to the platform's character set, the other converts in the
        opposite direction.  The tables are used in SLCHCNV.C.

        The tables are set up so the position (subscript, offset, index) in 
        the table corresponds to the letter to be converted and the value in 
        that location of the table is the character to convert to.

        To use tables:
            1. make sure input character is > 128
            2. subtract 128 from input character
            3. use result as index into table
            4. choose table: 
                    If input char is from a company product database, 
                    use SLToDef1ChrSet[].  
                    If input char is from an application 
                    and you need to look it up in a company product database, 
                    use SLToNat1ChrSet[].
            5. substitute character with value in that position

            SLchcnv() and SLfchcnv() will do this for you.

        All characters, even those with no particularly useful value for our
        needs in the given character set, are converted to another unique
        character.  Thus, the conversion is alway reversible, even for
        nonsense characters.

        Only one platform can be compiled at any time.

        Cautions:
        The conversion tables are only for characters 128 to 255 and don't
        contain places for 0 to 127.  Therefore, you should not try to 
        convert characters < 128 and you need to subtract 128 from characters 
        whose value is > 128 before you index into the table.
        
        Creation Date:  
        10-dec-92   PJA

        Change Record:
        09-feb-93   JMH     VAX needs to convert to itself
        09-nov-93   ARH     Included Code Page 850
        
*-----------------------------------------------------------------------*/

#ifndef SLCNVTB1_C

#define SLCNVTB1_C


#ifdef SLA_IBMCHAR_CS

/* NOTE:                                                                */
/* SLToNatChrSet[] VALUES ARE FOR MCS Latin 1 TO IBM PC EXTENDED       */
/* (Either code page 437 or 850)                                        */
/* WHILE SLToDefChrSet[] VALUES ARE FOR THE IBM PC EXTENDED TO MCS      */
/* LATIN 1                                                              */

/* THE FIRST 128 CHARACTERS ARE EXACT PAIRS SO THERE IS NO CONVERSION   */
/* THE NEXT 128 ARE THE HIGH ORDER VALUES AND THE ACCENTED CHARACTERS   */

#ifndef SL_CH850    /* defined in slfun.h */
/* Code Page 437 */
DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
    P_FIL_1         ,               /* 128 */   
    P_FIL_2         ,               /* 129 */   
    P_L9_S_QUOTE    ,               /* 130 */   
    P_FLORIN        ,               /* 131 */   
    P_L99_D_QUOTE   ,               /* 132 */   
    P_ELLIPSIS      ,               /* 133 */   
    P_FIL_3         ,               /* 134 */   
    P_FIL_4         ,               /* 135 */   
    P_FIL_5         ,               /* 136 */   
    P_FIL_6         ,               /* 137 */   
    P_FIL_7         ,               /* 138 */   
    P_LT_S_GUILL    ,               /* 139 */   
    P_MULTIPLICATION,               /* 140 */   
    P_FIL_8         ,               /* 141 */   
    P_FIL_9         ,               /* 142 */   
    P_FIL_10        ,               /* 143 */   
    P_FIL_11        ,               /* 144 */   
    P_OPEN_S_QUOTE  ,               /* 145 */   
    P_CLOSE_S_QUOTE ,               /* 146 */   
    P_OPEN_D_QUOTE  ,               /* 147 */   
    P_CLOSE_D_QUOTE ,               /* 148 */   
    P_FIL_12        ,               /* 149 */   
    P_EN_DASH       ,               /* 150 */   
    P_M_DASH        ,               /* 151 */   
    P_FIL_13        ,               /* 152 */   
    P_TRADEMARK     ,               /* 153 */   
    P_FIL_14        ,               /* 154 */   
    P_RT_S_GUILL    ,               /* 155 */   
    P_DIVISION      ,               /* 156 */   
    P_FIL_15        ,               /* 157 */   
    P_FIL_16        ,               /* 158 */   
    P_C_Y_ACUTE     ,               /* 159 */   
    P_NBSP          ,               /* 160 */   
    P_INV_EXCLAMATN ,               /* 161 */   
    P_CENT          ,               /* 162 */   
    P_POUND         ,               /* 163 */   
    P_UMLAUT        ,               /* 164 */   
    P_YEN           ,               /* 165 */   
    P_BROKEN_VBAR   ,               /* 166 */   
    P_SECTION       ,               /* 167 */   
    P_GEN_CURRENCY  ,               /* 168 */   
    P_COPYRIGHT     ,               /* 169 */   
    P_FEM_ORDINAL   ,               /* 170 */   
    P_OPEN_GUILL    ,               /* 171 */   
    P_NOT_SIGN      ,               /* 172 */   
    P_SHY           ,               /* 173 */   
    P_REG_TRADEMARK ,               /* 174 */   
    P_MACRON        ,               /* 175 */   
    P_DEGREE        ,               /* 176 */   
    P_PLUS_MINUS    ,               /* 177 */   
    P_TWO_SUPER     ,               /* 178 */   
    P_THREE_SUPER   ,               /* 179 */   
    P_ACUTE         ,               /* 180 */   
    P_MICRO_SIGN    ,               /* 181 */   
    P_PARAGRAPH     ,               /* 182 */   
    P_MIDDLE_DOT    ,               /* 183 */   
    P_CEDILLA       ,               /* 184 */   
    P_ONE_SUPER     ,               /* 185 */   
    P_MAS_ORDINAL   ,               /* 186 */   
    P_CLOS_GUILL    ,               /* 187 */   
    P_ONE_QUARTER   ,               /* 188 */   
    P_ONE_HALF      ,               /* 189 */   
    P_THREE_FOURTHS ,               /* 190 */   
    P_INV_QUESTION  ,               /* 191 */   
    P_C_A_GRAVE     ,               /* 192 */   
    P_C_A_ACUTE     ,               /* 193 */   
    P_C_A_CIRCUMFLX ,               /* 194 */   
    P_C_A_TILDE     ,               /* 195 */   
    P_C_A_UMLAUT    ,               /* 196 */   
    P_C_A_ANGSTROM  ,               /* 197 */   
    P_C_AE_LIGATURE ,               /* 198 */   
    P_C_C_CEDILLA   ,               /* 199 */   
    P_C_E_GRAVE     ,               /* 200 */   
    P_C_E_ACUTE     ,               /* 201 */   
    P_C_E_CIRCUMFLX ,               /* 202 */   
    P_C_E_UMLAUT    ,               /* 203 */   
    P_C_I_GRAVE     ,               /* 204 */   
    P_C_I_ACUTE     ,               /* 205 */   
    P_C_I_CIRCUMFLX ,               /* 206 */   
    P_C_I_UMLAUT    ,               /* 207 */   
    P_C_EDH         ,               /* 208 */   
    P_C_N_TILDE     ,               /* 209 */   
    P_C_O_GRAVE     ,               /* 210 */   
    P_C_O_ACUTE     ,               /* 211 */   
    P_C_O_CIRCUMFLX ,               /* 212 */   
    P_C_O_TILDE     ,               /* 213 */   
    P_C_O_UMLAUT    ,               /* 214 */   
    P_C_OE_LIGATURE ,               /* 215 */   
    P_C_O_SLASH     ,               /* 216 */   
    P_C_U_GRAVE     ,               /* 217 */   
    P_C_U_ACUTE     ,               /* 218 */   
    P_C_U_CIRCUMFLX ,               /* 219 */   
    P_C_U_UMLAUT    ,               /* 220 */   
    P_C_Y_UMLAUT    ,               /* 221 */   
    P_C_THORN       ,               /* 222 */   
    P_L_S_SHARP     ,               /* 223 */   
    P_L_A_GRAVE     ,               /* 224 */   
    P_L_A_ACUTE     ,               /* 225 */   
    P_L_A_CIRCUMFLX ,               /* 226 */   
    P_L_A_TILDE     ,               /* 227 */   
    P_L_A_UMLAUT    ,               /* 228 */   
    P_L_A_ANGSTROM  ,               /* 229 */   
    P_L_AE_LIGATURE ,               /* 230 */   
    P_L_C_CEDILLA   ,               /* 231 */   
    P_L_E_GRAVE     ,               /* 232 */   
    P_L_E_ACUTE     ,               /* 233 */   
    P_L_E_CIRCUMFLX ,               /* 234 */   
    P_L_E_UMLAUT    ,               /* 235 */   
    P_L_I_GRAVE     ,               /* 236 */   
    P_L_I_ACUTE     ,               /* 237 */   
    P_L_I_CIRCUMFLX ,               /* 238 */   
    P_L_I_UMLAUT    ,               /* 239 */   
    P_L_EDH         ,               /* 240 */   
    P_L_N_TILDE     ,               /* 241 */   
    P_L_O_GRAVE     ,               /* 242 */   
    P_L_O_ACUTE     ,               /* 243 */   
    P_L_O_CIRCUMFLX ,               /* 244 */   
    P_L_O_TILDE     ,               /* 245 */   
    P_L_O_UMLAUT    ,               /* 246 */   
    P_L_OE_LIGATURE ,               /* 247 */   
    P_L_O_SLASH     ,               /* 248 */   
    P_L_U_GRAVE     ,               /* 249 */   
    P_L_U_ACUTE     ,               /* 250 */   
    P_L_U_CIRCUMFLX ,               /* 251 */   
    P_L_U_UMLAUT    ,               /* 252 */   
    P_L_Y_UMLAUT    ,               /* 253 */   
    P_L_THORN       ,               /* 254 */   
    P_L_Y_ACUTE                     /* 255 */   
};                                                   



/* Code Page 437 */
DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_C_C_CEDILLA   ,               /* 128 */ 
    D_L_U_UMLAUT    ,               /* 129 */ 
    D_L_E_ACUTE     ,               /* 130 */ 
    D_L_A_CIRCUMFLX ,               /* 131 */ 
    D_L_A_UMLAUT    ,               /* 132 */ 
    D_L_A_GRAVE     ,               /* 133 */ 
    D_L_A_ANGSTROM  ,               /* 134 */ 
    D_L_C_CEDILLA   ,               /* 135 */ 
    D_L_E_CIRCUMFLX ,               /* 136 */ 
    D_L_E_UMLAUT    ,               /* 137 */ 
    D_L_E_GRAVE     ,               /* 138 */ 
    D_L_I_UMLAUT    ,               /* 139 */ 
    D_L_I_CIRCUMFLX ,               /* 140 */ 
    D_L_I_GRAVE     ,               /* 141 */ 
    D_C_A_UMLAUT    ,               /* 142 */ 
    D_C_A_ANGSTROM  ,               /* 143 */ 
    D_C_E_ACUTE     ,               /* 144 */ 
    D_L_AE_LIGATURE ,               /* 145 */ 
    D_C_AE_LIGATURE ,               /* 146 */ 
    D_L_O_CIRCUMFLX ,               /* 147 */ 
    D_L_O_UMLAUT    ,               /* 148 */ 
    D_L_O_GRAVE     ,               /* 149 */ 
    D_L_U_CIRCUMFLX ,               /* 150 */ 
    D_L_U_GRAVE     ,               /* 151 */ 
    D_L_Y_UMLAUT    ,               /* 152 */ 
    D_C_O_UMLAUT    ,               /* 153 */ 
    D_C_U_UMLAUT    ,               /* 154 */ 
    D_CENT          ,               /* 155 */ 
    D_POUND         ,               /* 156 */ 
    D_YEN           ,               /* 157 */ 
    D_FIL_1         ,               /* 158 */ 
    D_FLORIN        ,               /* 159 */ 
    D_L_A_ACUTE     ,               /* 160 */ 
    D_L_I_ACUTE     ,               /* 161 */ 
    D_L_O_ACUTE     ,               /* 162 */ 
    D_L_U_ACUTE     ,               /* 163 */ 
    D_L_N_TILDE     ,               /* 164 */ 
    D_C_N_TILDE     ,               /* 165 */ 
    D_FEM_ORDINAL   ,               /* 166 */ 
    D_MAS_ORDINAL   ,               /* 167 */ 
    D_INV_QUESTION  ,               /* 168 */ 
    D_FIL_2         ,               /* 169 */ 
    D_NOT_SIGN      ,               /* 170 */ 
    D_ONE_HALF      ,               /* 171 */ 
    D_ONE_QUARTER   ,               /* 172 */ 
    D_INV_EXCLAMATN ,               /* 173 */ 
    D_OPEN_GUILL    ,               /* 174 */ 
    D_CLOS_GUILL    ,               /* 175 */ 
    D_OPEN_S_QUOTE  ,               /* 176 */ 
    D_CLOSE_S_QUOTE ,               /* 177 */ 
    D_OPEN_D_QUOTE  ,               /* 178 */     
    D_CLOSE_D_QUOTE ,               /* 179 */     
    D_ELLIPSIS      ,               /* 180 */ 
    D_M_DASH        ,               /* 181 */ 
    D_C_Y_ACUTE     ,               /* 182 */ 
    D_L_Y_ACUTE     ,               /* 183 */ 
    D_THREE_FOURTHS ,               /* 184 */ 
    D_GEN_CURRENCY  ,               /* 185 */ 
    D_FIL_3         ,               /* 186 */ 
    D_L9_S_QUOTE    ,               /* 187 */ 
    D_FIL_4         ,               /* 188 */ 
    D_L99_D_QUOTE   ,               /* 189 */ 
    D_UMLAUT        ,               /* 190 */ 
    D_BROKEN_VBAR   ,               /* 191 */ 
    D_C_I_GRAVE     ,               /* 192 */
    D_C_I_ACUTE     ,               /* 193 */
    D_C_I_CIRCUMFLX ,               /* 194 */
    D_C_I_UMLAUT    ,               /* 195 */
    D_C_O_GRAVE     ,               /* 196 */
    D_C_O_ACUTE     ,               /* 197 */
    D_C_O_CIRCUMFLX ,               /* 198 */
    D_C_O_TILDE     ,               /* 199 */
    D_C_OE_LIGATURE ,               /* 200 */
    D_C_U_GRAVE     ,               /* 201 */
    D_C_U_ACUTE     ,               /* 202 */
    D_C_U_CIRCUMFLX ,               /* 203 */
    D_C_Y_UMLAUT    ,               /* 204 */
    D_L_A_TILDE     ,               /* 205 */
    D_L_O_TILDE     ,               /* 206 */
    D_L_OE_LIGATURE ,               /* 207 */
    D_C_A_GRAVE     ,               /* 208 */
    D_C_A_ACUTE     ,               /* 209 */
    D_C_A_CIRCUMFLX ,               /* 210 */
    D_C_A_TILDE     ,               /* 211 */
    D_C_E_GRAVE     ,               /* 212 */
    D_C_E_CIRCUMFLX ,               /* 213 */
    D_C_E_UMLAUT    ,               /* 214 */
    D_SHY           ,               /* 215 */
    D_REG_TRADEMARK ,               /* 216 */
    D_MACRON        ,               /* 217 */
    D_ACUTE         ,               /* 218 */
    D_C_THORN       ,               /* 219 */
    D_C_EDH         ,               /* 220 */
    D_L_EDH         ,               /* 221 */
    D_CEDILLA       ,               /* 222 */
    D_EN_DASH       ,               /* 223 */
    D_FIL_5         ,               /* 224 */
    D_L_S_SHARP     ,               /* 225 */
    D_L_THORN       ,               /* 226 */
    D_FIL_6         ,               /* 227 */
    D_FIL_7         ,               /* 228 */
    D_FIL_8         ,               /* 229 */
    D_MICRO_SIGN    ,               /* 230 */
    D_FIL_9         ,               /* 231 */
    D_FIL_10        ,               /* 232 */
    D_C_O_SLASH     ,               /* 233 */
    D_FIL_11        ,               /* 234 */
    D_FIL_12        ,               /* 235 */
    D_FIL_13        ,               /* 236 */
    D_L_O_SLASH     ,               /* 237 */
    D_FIL_14        ,               /* 238 */
    D_FIL_15        ,               /* 239 */
    D_FIL_16        ,               /* 240 */
    D_PLUS_MINUS    ,               /* 241 */
    D_PARAGRAPH     ,               /* 242 */
    D_SECTION       ,               /* 243 */
    D_LT_S_GUILL    ,               /* 244 */
    D_TRADEMARK     ,               /* 245 */
    D_DIVISION      ,               /* 246 */
    D_RT_S_GUILL    ,               /* 247 */
    D_DEGREE        ,               /* 248 */
    D_MULTIPLICATION,               /* 249 */
    D_MIDDLE_DOT    ,               /* 250 */
    D_COPYRIGHT     ,               /* 251 */
    D_ONE_SUPER     ,               /* 252 */
    D_TWO_SUPER     ,               /* 253 */
    D_THREE_SUPER   ,               /* 254 */
    D_NBSP                          /* 255 */
};                                             
                                               
#else
/* Code Page 850 */
DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
    P8_FIL_1          ,               /* 128 */
    P8_FIL_2          ,               /* 129 */
    P8_L9_S_QUOTE     ,               /* 130 */
    P8_FLORIN         ,               /* 131 */
    P8_L99_D_QUOTE    ,               /* 132 */
    P8_ELLIPSIS       ,               /* 133 */
    P8_FIL_3          ,               /* 134 */
    P8_FIL_4          ,               /* 135 */
    P8_FIL_5          ,               /* 136 */
    P8_FIL_6          ,               /* 137 */
    P8_FIL_7          ,               /* 138 */
    P8_LT_S_GUILL     ,               /* 139 */
    P8_MULTIPLICATION ,               /* 140 */
    P8_FIL_8          ,               /* 141 */
    P8_FIL_9          ,               /* 142 */
    P8_FIL_10         ,               /* 143 */
    P8_FIL_11         ,               /* 144 */
    P8_OPEN_S_QUOTE   ,               /* 145 */
    P8_CLOSE_S_QUOTE  ,               /* 146 */
    P8_OPEN_D_QUOTE   ,               /* 147 */
    P8_CLOSE_D_QUOTE  ,               /* 148 */
    P8_FIL_12         ,               /* 149 */
    P8_EN_DASH        ,               /* 150 */
    P8_M_DASH         ,               /* 151 */
    P8_FIL_13         ,               /* 152 */
    P8_TRADEMARK      ,               /* 153 */
    P8_FIL_14         ,               /* 154 */
    P8_RT_S_GUILL     ,               /* 155 */
    P8_DIVISION       ,               /* 156 */
    P8_FIL_15         ,               /* 157 */
    P8_FIL_16         ,               /* 158 */
    P8_C_Y_ACUTE      ,               /* 159 */
    P8_NBSP           ,               /* 160 */
    P8_INV_EXCLAMATN  ,               /* 161 */
    P8_CENT           ,               /* 162 */
    P8_POUND          ,               /* 163 */
    P8_UMLAUT         ,               /* 164 */
    P8_YEN            ,               /* 165 */
    P8_BROKEN_VBAR    ,               /* 166 */
    P8_SECTION        ,               /* 167 */
    P8_GEN_CURRENCY   ,               /* 168 */
    P8_COPYRIGHT      ,               /* 169 */
    P8_FEM_ORDINAL    ,               /* 170 */
    P8_OPEN_GUILL     ,               /* 171 */
    P8_NOT_SIGN       ,               /* 172 */
    P8_SHY            ,               /* 173 */
    P8_REG_TRADEMARK  ,               /* 174 */
    P8_MACRON         ,               /* 175 */
    P8_DEGREE         ,               /* 176 */
    P8_PLUS_MINUS     ,               /* 177 */
    P8_TWO_SUPER      ,               /* 178 */
    P8_THREE_SUPER    ,               /* 179 */
    P8_ACUTE          ,               /* 180 */
    P8_MICRO_SIGN     ,               /* 181 */
    P8_PARAGRAPH      ,               /* 182 */
    P8_MIDDLE_DOT     ,               /* 183 */
    P8_CEDILLA        ,               /* 184 */
    P8_ONE_SUPER      ,               /* 185 */
    P8_MAS_ORDINAL    ,               /* 186 */
    P8_CLOS_GUILL     ,               /* 187 */
    P8_ONE_QUARTER    ,               /* 188 */
    P8_ONE_HALF       ,               /* 189 */
    P8_THREE_FOURTHS  ,               /* 190 */
    P8_INV_QUESTION   ,               /* 191 */
    P8_C_A_GRAVE      ,               /* 192 */
    P8_C_A_ACUTE      ,               /* 193 */
    P8_C_A_CIRCUMFLX  ,               /* 194 */
    P8_C_A_TILDE      ,               /* 195 */
    P8_C_A_UMLAUT     ,               /* 196 */
    P8_C_A_ANGSTROM   ,               /* 197 */
    P8_C_AE_LIGATURE  ,               /* 198 */
    P8_C_C_CEDILLA    ,               /* 199 */
    P8_C_E_GRAVE      ,               /* 200 */
    P8_C_E_ACUTE      ,               /* 201 */
    P8_C_E_CIRCUMFLX  ,               /* 202 */
    P8_C_E_UMLAUT     ,               /* 203 */
    P8_C_I_GRAVE      ,               /* 204 */
    P8_C_I_ACUTE      ,               /* 205 */
    P8_C_I_CIRCUMFLX  ,               /* 206 */
    P8_C_I_UMLAUT     ,               /* 207 */
    P8_C_EDH          ,               /* 208 */
    P8_C_N_TILDE      ,               /* 209 */
    P8_C_O_GRAVE      ,               /* 210 */
    P8_C_O_ACUTE      ,               /* 211 */
    P8_C_O_CIRCUMFLX  ,               /* 212 */
    P8_C_O_TILDE      ,               /* 213 */
    P8_C_O_UMLAUT     ,               /* 214 */
    P8_C_OE_LIGATURE  ,               /* 215 */
    P8_C_O_SLASH      ,               /* 216 */
    P8_C_U_GRAVE      ,               /* 217 */
    P8_C_U_ACUTE      ,               /* 218 */
    P8_C_U_CIRCUMFLX  ,               /* 219 */
    P8_C_U_UMLAUT     ,               /* 220 */
    P8_C_Y_UMLAUT     ,               /* 221 */
    P8_C_THORN        ,               /* 222 */
    P8_L_S_SHARP      ,               /* 223 */
    P8_L_A_GRAVE      ,               /* 224 */
    P8_L_A_ACUTE      ,               /* 225 */
    P8_L_A_CIRCUMFLX  ,               /* 226 */
    P8_L_A_TILDE      ,               /* 227 */
    P8_L_A_UMLAUT     ,               /* 228 */
    P8_L_A_ANGSTROM   ,               /* 229 */
    P8_L_AE_LIGATURE  ,               /* 230 */
    P8_L_C_CEDILLA    ,               /* 231 */
    P8_L_E_GRAVE      ,               /* 232 */
    P8_L_E_ACUTE      ,               /* 233 */
    P8_L_E_CIRCUMFLX  ,               /* 234 */
    P8_L_E_UMLAUT     ,               /* 235 */
    P8_L_I_GRAVE      ,               /* 236 */
    P8_L_I_ACUTE      ,               /* 237 */
    P8_L_I_CIRCUMFLX  ,               /* 238 */
    P8_L_I_UMLAUT     ,               /* 239 */
    P8_L_EDH          ,               /* 240 */
    P8_L_N_TILDE      ,               /* 241 */
    P8_L_O_GRAVE      ,               /* 242 */
    P8_L_O_ACUTE      ,               /* 243 */
    P8_L_O_CIRCUMFLX  ,               /* 244 */
    P8_L_O_TILDE      ,               /* 245 */
    P8_L_O_UMLAUT     ,               /* 246 */
    P8_L_OE_LIGATURE  ,               /* 247 */
    P8_L_O_SLASH      ,               /* 248 */
    P8_L_U_GRAVE      ,               /* 249 */
    P8_L_U_ACUTE      ,               /* 250 */
    P8_L_U_CIRCUMFLX  ,               /* 251 */
    P8_L_U_UMLAUT     ,               /* 252 */
    P8_L_Y_UMLAUT     ,               /* 253 */
    P8_L_THORN        ,               /* 254 */
    P8_L_Y_ACUTE                      /* 255 */
};                                                    

/* Code Page 850 */
DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_C_C_CEDILLA    ,               /* 128 */
    D_L_U_UMLAUT     ,               /* 129 */
    D_L_E_ACUTE      ,               /* 130 */
    D_L_A_CIRCUMFLX  ,               /* 131 */
    D_L_A_UMLAUT     ,               /* 132 */
    D_L_A_GRAVE      ,               /* 133 */
    D_L_A_ANGSTROM   ,               /* 134 */
    D_L_C_CEDILLA    ,               /* 135 */
    D_L_E_CIRCUMFLX  ,               /* 136 */
    D_L_E_UMLAUT     ,               /* 137 */
    D_L_E_GRAVE      ,               /* 138 */
    D_L_I_UMLAUT     ,               /* 139 */
    D_L_I_CIRCUMFLX  ,               /* 140 */
    D_L_I_GRAVE      ,               /* 141 */
    D_C_A_UMLAUT     ,               /* 142 */
    D_C_A_ANGSTROM   ,               /* 143 */
    D_C_E_ACUTE      ,               /* 144 */
    D_L_AE_LIGATURE  ,               /* 145 */
    D_C_AE_LIGATURE  ,               /* 146 */
    D_L_O_CIRCUMFLX  ,               /* 147 */
    D_L_O_UMLAUT     ,               /* 148 */
    D_L_O_GRAVE      ,               /* 149 */
    D_L_U_CIRCUMFLX  ,               /* 150 */
    D_L_U_GRAVE      ,               /* 151 */
    D_L_Y_UMLAUT     ,               /* 152 */
    D_C_O_UMLAUT     ,               /* 153 */
    D_C_U_UMLAUT     ,               /* 154 */
    D_L_O_SLASH      ,               /* 155 */
    D_POUND          ,               /* 156 */
    D_C_O_SLASH      ,               /* 157 */
    D_MULTIPLICATION ,               /* 158 */
    D_FLORIN         ,               /* 159 */
    D_L_A_ACUTE      ,               /* 160 */
    D_L_I_ACUTE      ,               /* 161 */
    D_L_O_ACUTE      ,               /* 162 */
    D_L_U_ACUTE      ,               /* 163 */
    D_L_N_TILDE      ,               /* 164 */
    D_C_N_TILDE      ,               /* 165 */
    D_FEM_ORDINAL    ,               /* 166 */
    D_MAS_ORDINAL    ,               /* 167 */
    D_INV_QUESTION   ,               /* 168 */
    D_REG_TRADEMARK  ,               /* 169 */
    D_NOT_SIGN       ,               /* 170 */
    D_ONE_HALF       ,               /* 171 */
    D_ONE_QUARTER    ,               /* 172 */
    D_INV_EXCLAMATN  ,               /* 173 */
    D_OPEN_GUILL     ,               /* 174 */
    D_CLOS_GUILL     ,               /* 175 */
    D_OPEN_S_QUOTE   ,               /* 176 */
    D_CLOSE_S_QUOTE  ,               /* 177 */
    D_OPEN_D_QUOTE   ,               /* 178 */
    D_CLOSE_D_QUOTE  ,               /* 179 */
    D_ELLIPSIS       ,               /* 180 */
    D_C_A_ACUTE      ,               /* 181 */
    D_C_A_CIRCUMFLX  ,               /* 182 */
    D_C_A_GRAVE      ,               /* 183 */
    D_COPYRIGHT      ,               /* 184 */
    D_M_DASH         ,               /* 185 */
    D_FIL_1          ,               /* 186 */
    D_L9_S_QUOTE     ,               /* 187 */
    D_FIL_2          ,               /* 188 */
    D_CENT           ,               /* 189 */
    D_YEN            ,               /* 190 */
    D_TRADEMARK      ,               /* 191 */
    D_L_OE_LIGATURE  ,               /* 192 */
    D_L99_D_QUOTE    ,               /* 193 */
    D_FIL_3          ,               /* 194 */
    D_FIL_4          ,               /* 195 */
    D_FIL_5          ,               /* 196 */
    D_FIL_6          ,               /* 197 */
    D_L_A_TILDE      ,               /* 198 */
    D_C_A_TILDE      ,               /* 199 */
    D_C_OE_LIGATURE  ,               /* 200 */
    D_FIL_7          ,               /* 201 */
    D_FIL_8          ,               /* 202 */
    D_FIL_9          ,               /* 203 */
    D_C_Y_UMLAUT     ,               /* 204 */
    D_FIL_10         ,               /* 205 */
    D_FIL_11         ,               /* 206 */
    D_GEN_CURRENCY   ,               /* 207 */
    D_L_EDH          ,               /* 208 */
    D_C_EDH          ,               /* 209 */
    D_C_E_CIRCUMFLX  ,               /* 210 */
    D_C_E_UMLAUT     ,               /* 211 */
    D_C_E_GRAVE      ,               /* 212 */
    D_FIL_12         ,               /* 213 */
    D_C_I_ACUTE      ,               /* 214 */
    D_C_I_CIRCUMFLX  ,               /* 215 */
    D_C_I_UMLAUT     ,               /* 216 */
    D_FIL_13         ,               /* 217 */
    D_FIL_14         ,               /* 218 */
    D_FIL_15         ,               /* 219 */
    D_FIL_16         ,               /* 220 */
    D_BROKEN_VBAR    ,               /* 221 */
    D_C_I_GRAVE      ,               /* 222 */
    D_EN_DASH        ,               /* 223 */
    D_C_O_ACUTE      ,               /* 224 */
    D_L_S_SHARP      ,               /* 225 */
    D_C_O_CIRCUMFLX  ,               /* 226 */
    D_C_O_GRAVE      ,               /* 227 */
    D_L_O_TILDE      ,               /* 228 */
    D_C_O_TILDE      ,               /* 229 */
    D_MICRO_SIGN     ,               /* 230 */
    D_L_THORN        ,               /* 231 */
    D_C_THORN        ,               /* 232 */
    D_C_U_ACUTE      ,               /* 233 */
    D_C_U_CIRCUMFLX  ,               /* 234 */
    D_C_U_GRAVE      ,               /* 235 */
    D_L_Y_ACUTE      ,               /* 236 */
    D_C_Y_ACUTE      ,               /* 237 */
    D_MACRON         ,               /* 238 */
    D_ACUTE          ,               /* 239 */
    D_SHY            ,               /* 240 */
    D_PLUS_MINUS     ,               /* 241 */
    D_LT_S_GUILL     ,               /* 242 */
    D_THREE_FOURTHS  ,               /* 243 */
    D_PARAGRAPH      ,               /* 244 */
    D_SECTION        ,               /* 245 */
    D_DIVISION       ,               /* 246 */
    D_CEDILLA        ,               /* 247 */
    D_DEGREE         ,               /* 248 */
    D_UMLAUT         ,               /* 249 */
    D_MIDDLE_DOT     ,               /* 250 */
    D_ONE_SUPER      ,               /* 251 */
    D_THREE_SUPER    ,               /* 252 */
    D_TWO_SUPER      ,               /* 253 */
    D_RT_S_GUILL     ,               /* 254 */
    D_NBSP                           /* 255 */
};

#endif        /* SL_CH850 */
#endif        /* MSDOS - End of IBM PC Conversion Table */


#if defined(WINDOWS) || defined(WIN32)

/* NOTE:                                                                */
/* SLToNatChrSet[] VALUES ARE FOR MCS Latin 1 TO WINDOWS EXTENDED */
/* WHILE SLToDefChrSet[] VALUES ARE FOR THE WINDOWS EXTENDED TO MCS     */
/* LATIN 1                                                              */

/* THE FIRST 128 CHARACTERS ARE EXACT PAIRS SO THERE IS NO CONVERSION   */
/* THE NEXT 128 ARE THE HIGH ORDER VALUES AND THE ACCENTED CHARACTERS   */

DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
   /* The MS Windows character set is almost identical to our           */
   /* Master Character Set.  The differences are so slight, some        */
   /* applications may not need to perform conversion for MS Windows.   */

    W_FIL_1          ,               /* 128 */
    W_FIL_2          ,               /* 129 */
    W_L9_S_QUOTE     ,               /* 130 */
    W_FLORIN         ,               /* 131 */
    W_L99_D_QUOTE    ,               /* 132 */
    W_ELLIPSIS       ,               /* 133 */
    W_FIL_3          ,               /* 134 */
    W_FIL_4          ,               /* 135 */
    W_FIL_5          ,               /* 136 */
    W_PER_MILLE      ,               /* 137 */
    W_FIL_6          ,               /* 138 */
    W_LT_S_GUILL     ,               /* 139 */
    W_MULTIPLICATION ,               /* 140 */
    W_FIL_7          ,               /* 141 */
    W_FIL_8          ,               /* 142 */
    W_FIL_9          ,               /* 143 */
    W_FIL_10         ,               /* 144 */
    W_OPEN_S_QUOTE   ,               /* 145 */
    W_CLOSE_S_QUOTE  ,               /* 146 */
    W_OPEN_D_QUOTE   ,               /* 147 */
    W_CLOSE_D_QUOTE  ,               /* 148 */
    W_FIL_11         ,               /* 149 */
    W_EN_DASH        ,               /* 150 */
    W_M_DASH         ,               /* 151 */
    W_FIL_12         ,               /* 152 */
    W_TRADEMARK      ,               /* 153 */
    W_FIL_13         ,               /* 154 */
    W_RT_S_GUILL     ,               /* 155 */
    W_DIVISION       ,               /* 156 */
    W_FIL_14         ,               /* 157 */
    W_FIL_15         ,               /* 158 */
    W_C_Y_ACUTE      ,               /* 159 */
    W_NBSP           ,               /* 160 */
    W_INV_EXCLAMATN  ,               /* 161 */
    W_CENT           ,               /* 162 */
    W_POUND          ,               /* 163 */
    W_UMLAUT         ,               /* 164 */
    W_YEN            ,               /* 165 */
    W_BROKEN_VBAR    ,               /* 166 */
    W_SECTION        ,               /* 167 */
    W_GEN_CURRENCY   ,               /* 168 */
    W_COPYRIGHT      ,               /* 169 */
    W_FEM_ORDINAL    ,               /* 170 */
    W_OPEN_GUILL     ,               /* 171 */
    W_NOT_SIGN       ,               /* 172 */
    W_SHY            ,               /* 173 */
    W_REG_TRADEMARK  ,               /* 174 */
    W_MACRON         ,               /* 175 */
    W_DEGREE         ,               /* 176 */
    W_PLUS_MINUS     ,               /* 177 */
    W_TWO_SUPER      ,               /* 178 */
    W_THREE_SUPER    ,               /* 179 */
    W_ACUTE          ,               /* 180 */
    W_MICRO_SIGN     ,               /* 181 */
    W_PARAGRAPH      ,               /* 182 */
    W_MIDDLE_DOT     ,               /* 183 */
    W_CEDILLA        ,               /* 184 */
    W_ONE_SUPER      ,               /* 185 */
    W_MAS_ORDINAL    ,               /* 186 */
    W_CLOS_GUILL     ,               /* 187 */
    W_ONE_QUARTER    ,               /* 188 */
    W_ONE_HALF       ,               /* 189 */
    W_THREE_FOURTHS  ,               /* 190 */
    W_INV_QUESTION   ,               /* 191 */
    W_C_A_GRAVE      ,               /* 192 */
    W_C_A_ACUTE      ,               /* 193 */
    W_C_A_CIRCUMFLX  ,               /* 194 */
    W_C_A_TILDE      ,               /* 195 */
    W_C_A_UMLAUT     ,               /* 196 */
    W_C_A_ANGSTROM   ,               /* 197 */
    W_C_AE_LIGATURE  ,               /* 198 */
    W_C_C_CEDILLA    ,               /* 199 */
    W_C_E_GRAVE      ,               /* 200 */
    W_C_E_ACUTE      ,               /* 201 */
    W_C_E_CIRCUMFLX  ,               /* 202 */
    W_C_E_UMLAUT     ,               /* 203 */
    W_C_I_GRAVE      ,               /* 204 */
    W_C_I_ACUTE      ,               /* 205 */
    W_C_I_CIRCUMFLX  ,               /* 206 */
    W_C_I_UMLAUT     ,               /* 207 */
    W_C_EDH          ,               /* 208 */
    W_C_N_TILDE      ,               /* 209 */
    W_C_O_GRAVE      ,               /* 210 */
    W_C_O_ACUTE      ,               /* 211 */
    W_C_O_CIRCUMFLX  ,               /* 212 */
    W_C_O_TILDE      ,               /* 213 */
    W_C_O_UMLAUT     ,               /* 214 */
    W_C_OE_LIGATURE  ,               /* 215 */
    W_C_O_SLASH      ,               /* 216 */
    W_C_U_GRAVE      ,               /* 217 */
    W_C_U_ACUTE      ,               /* 218 */
    W_C_U_CIRCUMFLX  ,               /* 219 */
    W_C_U_UMLAUT     ,               /* 220 */
    W_C_Y_UMLAUT     ,               /* 221 */
    W_C_THORN        ,               /* 222 */
    W_L_S_SHARP      ,               /* 223 */
    W_L_A_GRAVE      ,               /* 224 */
    W_L_A_ACUTE      ,               /* 225 */
    W_L_A_CIRCUMFLX  ,               /* 226 */
    W_L_A_TILDE      ,               /* 227 */
    W_L_A_UMLAUT     ,               /* 228 */
    W_L_A_ANGSTROM   ,               /* 229 */
    W_L_AE_LIGATURE  ,               /* 230 */
    W_L_C_CEDILLA    ,               /* 231 */
    W_L_E_GRAVE      ,               /* 232 */
    W_L_E_ACUTE      ,               /* 233 */
    W_L_E_CIRCUMFLX  ,               /* 234 */
    W_L_E_UMLAUT     ,               /* 235 */
    W_L_I_GRAVE      ,               /* 236 */
    W_L_I_ACUTE      ,               /* 237 */
    W_L_I_CIRCUMFLX  ,               /* 238 */
    W_L_I_UMLAUT     ,               /* 239 */
    W_L_EDH          ,               /* 240 */
    W_L_N_TILDE      ,               /* 241 */
    W_L_O_GRAVE      ,               /* 242 */
    W_L_O_ACUTE      ,               /* 243 */
    W_L_O_CIRCUMFLX  ,               /* 244 */
    W_L_O_TILDE      ,               /* 245 */
    W_L_O_UMLAUT     ,               /* 246 */
    W_L_OE_LIGATURE  ,               /* 247 */
    W_L_O_SLASH      ,               /* 248 */
    W_L_U_GRAVE      ,               /* 249 */
    W_L_U_ACUTE      ,               /* 250 */
    W_L_U_CIRCUMFLX  ,               /* 251 */
    W_L_U_UMLAUT     ,               /* 252 */
    W_L_Y_UMLAUT     ,               /* 253 */
    W_L_THORN        ,               /* 254 */
    W_L_Y_ACUTE                      /* 255 */
};



DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_FIL_1          ,               /* 128 */
    D_FIL_2          ,               /* 129 */
    D_L9_S_QUOTE     ,               /* 130 */
    D_FLORIN         ,               /* 131 */
    D_L99_D_QUOTE    ,               /* 132 */
    D_ELLIPSIS       ,               /* 133 */
    D_FIL_3          ,               /* 134 */
    D_FIL_4          ,               /* 135 */
    D_FIL_5          ,               /* 136 */
    D_FIL_6          ,               /* 137 */
    D_FIL_7          ,               /* 138 */
    D_LT_S_GUILL     ,               /* 139 */
    D_C_OE_LIGATURE  ,               /* 140 */
    D_FIL_8          ,               /* 141 */
    D_FIL_9          ,               /* 142 */
    D_FIL_10         ,               /* 143 */
    D_FIL_11         ,               /* 144 */
    D_OPEN_S_QUOTE   ,               /* 145 */
    D_CLOSE_S_QUOTE  ,               /* 146 */
    D_OPEN_D_QUOTE   ,               /* 147 */
    D_CLOSE_D_QUOTE  ,               /* 148 */
    D_FIL_12         ,               /* 149 */
    D_EN_DASH        ,               /* 150 */
    D_M_DASH         ,               /* 151 */
    D_FIL_13         ,               /* 152 */
    D_TRADEMARK      ,               /* 153 */
    D_FIL_14         ,               /* 154 */
    D_RT_S_GUILL     ,               /* 155 */
    D_L_OE_LIGATURE  ,               /* 156 */
    D_FIL_15         ,               /* 157 */
    D_FIL_16         ,               /* 158 */
    D_C_Y_UMLAUT     ,               /* 159 */
    D_NBSP           ,               /* 160 */
    D_INV_EXCLAMATN  ,               /* 161 */
    D_CENT           ,               /* 162 */
    D_POUND          ,               /* 163 */
    D_GEN_CURRENCY   ,               /* 164 */
    D_YEN            ,               /* 165 */
    D_BROKEN_VBAR    ,               /* 166 */
    D_SECTION        ,               /* 167 */
    D_UMLAUT         ,               /* 168 */
    D_COPYRIGHT      ,               /* 169 */
    D_FEM_ORDINAL    ,               /* 170 */
    D_OPEN_GUILL     ,               /* 171 */
    D_NOT_SIGN       ,               /* 172 */
    D_SHY            ,               /* 173 */
    D_REG_TRADEMARK  ,               /* 174 */
    D_MACRON         ,               /* 175 */
    D_DEGREE         ,               /* 176 */
    D_PLUS_MINUS     ,               /* 177 */
    D_TWO_SUPER      ,               /* 178 */
    D_THREE_SUPER    ,               /* 179 */
    D_ACUTE          ,               /* 180 */
    D_MICRO_SIGN     ,               /* 181 */
    D_PARAGRAPH      ,               /* 182 */
    D_MIDDLE_DOT     ,               /* 183 */
    D_CEDILLA        ,               /* 184 */
    D_ONE_SUPER      ,               /* 185 */
    D_MAS_ORDINAL    ,               /* 186 */
    D_CLOS_GUILL     ,               /* 187 */
    D_ONE_QUARTER    ,               /* 188 */
    D_ONE_HALF       ,               /* 189 */
    D_THREE_FOURTHS  ,               /* 190 */
    D_INV_QUESTION   ,               /* 191 */
    D_C_A_GRAVE      ,               /* 192 */
    D_C_A_ACUTE      ,               /* 193 */
    D_C_A_CIRCUMFLX  ,               /* 194 */
    D_C_A_TILDE      ,               /* 195 */
    D_C_A_UMLAUT     ,               /* 196 */
    D_C_A_ANGSTROM   ,               /* 197 */
    D_C_AE_LIGATURE  ,               /* 198 */
    D_C_C_CEDILLA    ,               /* 199 */
    D_C_E_GRAVE      ,               /* 200 */
    D_C_E_ACUTE      ,               /* 201 */
    D_C_E_CIRCUMFLX  ,               /* 202 */
    D_C_E_UMLAUT     ,               /* 203 */
    D_C_I_GRAVE      ,               /* 204 */
    D_C_I_ACUTE      ,               /* 205 */
    D_C_I_CIRCUMFLX  ,               /* 206 */
    D_C_I_UMLAUT     ,               /* 207 */
    D_C_EDH          ,               /* 208 */
    D_C_N_TILDE      ,               /* 209 */
    D_C_O_GRAVE      ,               /* 210 */
    D_C_O_ACUTE      ,               /* 211 */
    D_C_O_CIRCUMFLX  ,               /* 212 */
    D_C_O_TILDE      ,               /* 213 */
    D_C_O_UMLAUT     ,               /* 214 */
    D_MULTIPLICATION ,               /* 215 */
    D_C_O_SLASH      ,               /* 216 */
    D_C_U_GRAVE      ,               /* 217 */
    D_C_U_ACUTE      ,               /* 218 */
    D_C_U_CIRCUMFLX  ,               /* 219 */
    D_C_U_UMLAUT     ,               /* 220 */
    D_C_Y_ACUTE      ,               /* 221 */
    D_C_THORN        ,               /* 222 */
    D_L_S_SHARP      ,               /* 223 */
    D_L_A_GRAVE      ,               /* 224 */
    D_L_A_ACUTE      ,               /* 225 */
    D_L_A_CIRCUMFLX  ,               /* 226 */
    D_L_A_TILDE      ,               /* 227 */
    D_L_A_UMLAUT     ,               /* 228 */
    D_L_A_ANGSTROM   ,               /* 229 */
    D_L_AE_LIGATURE  ,               /* 230 */
    D_L_C_CEDILLA    ,               /* 231 */
    D_L_E_GRAVE      ,               /* 232 */
    D_L_E_ACUTE      ,               /* 233 */
    D_L_E_CIRCUMFLX  ,               /* 234 */
    D_L_E_UMLAUT     ,               /* 235 */
    D_L_I_GRAVE      ,               /* 236 */
    D_L_I_ACUTE      ,               /* 237 */
    D_L_I_CIRCUMFLX  ,               /* 238 */
    D_L_I_UMLAUT     ,               /* 239 */
    D_L_EDH          ,               /* 240 */
    D_L_N_TILDE      ,               /* 241 */
    D_L_O_GRAVE      ,               /* 242 */
    D_L_O_ACUTE      ,               /* 243 */
    D_L_O_CIRCUMFLX  ,               /* 244 */
    D_L_O_TILDE      ,               /* 245 */
    D_L_O_UMLAUT     ,               /* 246 */
    D_DIVISION       ,               /* 247 */
    D_L_O_SLASH      ,               /* 248 */
    D_L_U_GRAVE      ,               /* 249 */
    D_L_U_ACUTE      ,               /* 250 */
    D_L_U_CIRCUMFLX  ,               /* 251 */
    D_L_U_UMLAUT     ,               /* 252 */
    D_L_Y_ACUTE      ,               /* 253 */
    D_L_THORN        ,               /* 254 */
    D_L_Y_UMLAUT                     /* 255 */
};  
    
#endif  /* WINDOWS - End of MS Windows Conversion Table */


#if defined(MAC) || defined(POWERMAC)

/* NOTE:                                                                */
/* SLToNatChrSet[] VALUES ARE FOR MCS Latin 1 TO MAC EXTENDED          */
/* WHILE SLToDefChrSet[] VALUES ARE FOR THE MAC EXTENDED TO MCS         */
/* LATIN 1                                                              */

/* THE FIRST 128 CHARACTERS ARE EXACT PAIRS SO THERE IS NO CONVERSION   */
/* THE NEXT 128 ARE THE HIGH ORDER VALUES AND THE ACCENTED CHARACTERS   */

DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
    M_FIL_1          ,               /* 128 */
    M_FIL_2          ,               /* 129 */
    M_L9_S_QUOTE     ,               /* 130 */
    M_FLORIN         ,               /* 131 */
    M_L99_D_QUOTE    ,               /* 132 */
    M_ELLIPSIS       ,               /* 133 */
    M_FIL_3          ,               /* 134 */
    M_FIL_4          ,               /* 135 */
    M_FIL_5          ,               /* 136 */
    M_FIL_6          ,               /* 137 */
    M_FIL_7          ,               /* 138 */
    M_LT_S_GUILL     ,               /* 139 */
    M_MULTIPLICATION ,               /* 140 */
    M_FIL_8          ,               /* 141 */
    M_FIL_9          ,               /* 142 */
    M_FIL_10         ,               /* 143 */
    M_FIL_11         ,               /* 144 */
    M_OPEN_S_QUOTE   ,               /* 145 */
    M_CLOSE_S_QUOTE  ,               /* 146 */
    M_OPEN_D_QUOTE   ,               /* 147 */
    M_CLOSE_D_QUOTE  ,               /* 148 */
    M_FIL_12         ,               /* 149 */
    M_EN_DASH        ,               /* 150 */
    M_M_DASH         ,               /* 151 */
    M_FIL_13         ,               /* 152 */
    M_TRADEMARK      ,               /* 153 */
    M_FIL_14         ,               /* 154 */
    M_RT_S_GUILL     ,               /* 155 */
    M_DIVISION       ,               /* 156 */
    M_FIL_15         ,               /* 157 */
    M_FIL_16         ,               /* 158 */
    M_C_Y_ACUTE      ,               /* 159 */
    M_NBSP           ,               /* 160 */
    M_INV_EXCLAMATN  ,               /* 161 */
    M_CENT           ,               /* 162 */
    M_POUND          ,               /* 163 */
    M_UMLAUT         ,               /* 164 */
    M_YEN            ,               /* 165 */
    M_BROKEN_VBAR    ,               /* 166 */
    M_SECTION        ,               /* 167 */
    M_GEN_CURRENCY   ,               /* 168 */
    M_COPYRIGHT      ,               /* 169 */
    M_FEM_ORDINAL    ,               /* 170 */
    M_OPEN_GUILL     ,               /* 171 */
    M_NOT_SIGN       ,               /* 172 */
    M_SHY            ,               /* 173 */
    M_REG_TRADEMARK  ,               /* 174 */
    M_MACRON         ,               /* 175 */
    M_DEGREE         ,               /* 176 */
    M_PLUS_MINUS     ,               /* 177 */
    M_TWO_SUPER      ,               /* 178 */
    M_THREE_SUPER    ,               /* 179 */
    M_ACUTE          ,               /* 180 */
    M_MICRO_SIGN     ,               /* 181 */
    M_PARAGRAPH      ,               /* 182 */
    M_MIDDLE_DOT     ,               /* 183 */
    M_CEDILLA        ,               /* 184 */
    M_ONE_SUPER      ,               /* 185 */
    M_MAS_ORDINAL    ,               /* 186 */
    M_CLOS_GUILL     ,               /* 187 */
    M_ONE_QUARTER    ,               /* 188 */
    M_ONE_HALF       ,               /* 189 */
    M_THREE_FOURTHS  ,               /* 190 */
    M_INV_QUESTION   ,               /* 191 */
    M_C_A_GRAVE      ,               /* 192 */
    M_C_A_ACUTE      ,               /* 193 */
    M_C_A_CIRCUMFLX  ,               /* 194 */
    M_C_A_TILDE      ,               /* 195 */
    M_C_A_UMLAUT     ,               /* 196 */
    M_C_A_ANGSTROM   ,               /* 197 */
    M_C_AE_LIGATURE  ,               /* 198 */
    M_C_C_CEDILLA    ,               /* 199 */
    M_C_E_GRAVE      ,               /* 200 */
    M_C_E_ACUTE      ,               /* 201 */
    M_C_E_CIRCUMFLX  ,               /* 202 */
    M_C_E_UMLAUT     ,               /* 203 */
    M_C_I_GRAVE      ,               /* 204 */
    M_C_I_ACUTE      ,               /* 205 */
    M_C_I_CIRCUMFLX  ,               /* 206 */
    M_C_I_UMLAUT     ,               /* 207 */
    M_C_EDH          ,               /* 208 */
    M_C_N_TILDE      ,               /* 209 */
    M_C_O_GRAVE      ,               /* 210 */
    M_C_O_ACUTE      ,               /* 211 */
    M_C_O_CIRCUMFLX  ,               /* 212 */
    M_C_O_TILDE      ,               /* 213 */
    M_C_O_UMLAUT     ,               /* 214 */
    M_C_OE_LIGATURE  ,               /* 215 */
    M_C_O_SLASH      ,               /* 216 */
    M_C_U_GRAVE      ,               /* 217 */
    M_C_U_ACUTE      ,               /* 218 */
    M_C_U_CIRCUMFLX  ,               /* 219 */
    M_C_U_UMLAUT     ,               /* 220 */
    M_C_Y_UMLAUT     ,               /* 221 */
    M_C_THORN        ,               /* 222 */
    M_L_S_SHARP      ,               /* 223 */
    M_L_A_GRAVE      ,               /* 224 */
    M_L_A_ACUTE      ,               /* 225 */
    M_L_A_CIRCUMFLX  ,               /* 226 */
    M_L_A_TILDE      ,               /* 227 */
    M_L_A_UMLAUT     ,               /* 228 */
    M_L_A_ANGSTROM   ,               /* 229 */
    M_L_AE_LIGATURE  ,               /* 230 */
    M_L_C_CEDILLA    ,               /* 231 */
    M_L_E_GRAVE      ,               /* 232 */
    M_L_E_ACUTE      ,               /* 233 */
    M_L_E_CIRCUMFLX  ,               /* 234 */
    M_L_E_UMLAUT     ,               /* 235 */
    M_L_I_GRAVE      ,               /* 236 */
    M_L_I_ACUTE      ,               /* 237 */
    M_L_I_CIRCUMFLX  ,               /* 238 */
    M_L_I_UMLAUT     ,               /* 239 */
    M_L_EDH          ,               /* 240 */
    M_L_N_TILDE      ,               /* 241 */
    M_L_O_GRAVE      ,               /* 242 */
    M_L_O_ACUTE      ,               /* 243 */
    M_L_O_CIRCUMFLX  ,               /* 244 */
    M_L_O_TILDE      ,               /* 245 */
    M_L_O_UMLAUT     ,               /* 246 */
    M_L_OE_LIGATURE  ,               /* 247 */
    M_L_O_SLASH      ,               /* 248 */
    M_L_U_GRAVE      ,               /* 249 */
    M_L_U_ACUTE      ,               /* 250 */
    M_L_U_CIRCUMFLX  ,               /* 251 */
    M_L_U_UMLAUT     ,               /* 252 */
    M_L_Y_UMLAUT     ,               /* 253 */
    M_L_THORN        ,               /* 254 */
    M_L_Y_ACUTE                      /* 255 */
};


DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_C_A_UMLAUT      ,               /* 128 */
    D_C_A_ANGSTROM    ,               /* 129 */
    D_C_C_CEDILLA     ,               /* 130 */
    D_C_E_ACUTE       ,               /* 131 */
    D_C_N_TILDE       ,               /* 132 */
    D_C_O_UMLAUT      ,               /* 133 */
    D_C_U_UMLAUT      ,               /* 134 */
    D_L_A_ACUTE       ,               /* 135 */
    D_L_A_GRAVE       ,               /* 136 */
    D_L_A_CIRCUMFLX   ,               /* 137 */
    D_L_A_UMLAUT      ,               /* 138 */
    D_L_A_TILDE       ,               /* 139 */
    D_L_A_ANGSTROM    ,               /* 140 */
    D_L_C_CEDILLA     ,               /* 141 */
    D_L_E_ACUTE       ,               /* 142 */
    D_L_E_GRAVE       ,               /* 143 */
    D_L_E_CIRCUMFLX   ,               /* 144 */
    D_L_E_UMLAUT      ,               /* 145 */
    D_L_I_ACUTE       ,               /* 146 */
    D_L_I_GRAVE       ,               /* 147 */
    D_L_I_CIRCUMFLX   ,               /* 148 */
    D_L_I_UMLAUT      ,               /* 149 */
    D_L_N_TILDE       ,               /* 150 */
    D_L_O_ACUTE       ,               /* 151 */
    D_L_O_GRAVE       ,               /* 152 */
    D_L_O_CIRCUMFLX   ,               /* 153 */
    D_L_O_UMLAUT      ,               /* 154 */
    D_L_O_TILDE       ,               /* 155 */
    D_L_U_ACUTE       ,               /* 156 */
    D_L_U_GRAVE       ,               /* 157 */
    D_L_U_CIRCUMFLX   ,               /* 158 */
    D_L_U_UMLAUT      ,               /* 159 */
    D_FIL_1           ,               /* 160 */
    D_DEGREE          ,               /* 161 */
    D_CENT            ,               /* 162 */
    D_POUND           ,               /* 163 */
    D_SECTION         ,               /* 164 */
    D_MIDDLE_DOT      ,               /* 165 */
    D_PARAGRAPH       ,               /* 166 */
    D_L_S_SHARP       ,               /* 167 */
    D_REG_TRADEMARK   ,               /* 168 */
    D_COPYRIGHT       ,               /* 169 */
    D_TRADEMARK       ,               /* 170 */
    D_ACUTE           ,               /* 171 */
    D_UMLAUT          ,               /* 172 */
    D_FIL_2           ,               /* 173 */
    D_C_AE_LIGATURE   ,               /* 174 */
    D_C_O_SLASH       ,               /* 175 */
    D_TWO_SUPER       ,               /* 176 */
    D_PLUS_MINUS      ,               /* 177 */
    D_THREE_SUPER     ,               /* 178 */
    D_FIL_3           ,               /* 179 */
    D_YEN             ,               /* 180 */
    D_MICRO_SIGN      ,               /* 181 */
    D_ONE_SUPER       ,               /* 182 */
    D_ONE_QUARTER     ,               /* 183 */
    D_ONE_HALF        ,               /* 184 */
    D_THREE_FOURTHS   ,               /* 185 */
    D_FIL_4           ,               /* 186 */
    D_FEM_ORDINAL     ,               /* 187 */
    D_MAS_ORDINAL     ,               /* 188 */
    D_FIL_5           ,               /* 189 */
    D_L_AE_LIGATURE   ,               /* 190 */
    D_L_O_SLASH       ,               /* 191 */
    D_INV_QUESTION    ,               /* 192 */
    D_INV_EXCLAMATN   ,               /* 193 */
    D_NOT_SIGN        ,               /* 194 */
    D_FIL_6           ,               /* 195 */
    D_FLORIN          ,               /* 196 */
    D_FIL_7           ,               /* 197 */
    D_FIL_8           ,               /* 198 */
    D_OPEN_GUILL      ,               /* 199 */
    D_CLOS_GUILL      ,               /* 200 */
    D_ELLIPSIS        ,               /* 201 */
    D_NBSP            ,               /* 202 */
    D_C_A_GRAVE       ,               /* 203 */
    D_C_A_TILDE       ,               /* 204 */
    D_C_O_TILDE       ,               /* 205 */
    D_C_OE_LIGATURE   ,               /* 206 */
    D_L_OE_LIGATURE   ,               /* 207 */
    D_SHY             ,               /* 208 */
    D_M_DASH          ,               /* 209 */
    D_OPEN_D_QUOTE    ,               /* 210 */
    D_CLOSE_D_QUOTE   ,               /* 211 */
    D_OPEN_S_QUOTE    ,               /* 212 */
    D_CLOSE_S_QUOTE   ,               /* 213 */
    D_DIVISION        ,               /* 214 */
    D_FIL_9           ,               /* 215 */
    D_L_Y_UMLAUT      ,               /* 216 */
    D_C_Y_UMLAUT      ,               /* 217 */
    D_FIL_10          ,               /* 218 */
    D_GEN_CURRENCY    ,               /* 219 */
    D_FIL_11          ,               /* 220 */
    D_FIL_12          ,               /* 221 */
    D_FIL_13          ,               /* 222 */
    D_MULTIPLICATION  ,               /* 223 */
    D_FIL_14          ,               /* 224 */
    D_EN_DASH         ,               /* 225 */
    D_FIL_15          ,               /* 226 */
    D_FIL_16          ,               /* 227 */
    D_LT_S_GUILL      ,               /* 228 */
    D_C_A_CIRCUMFLX   ,               /* 229 */
    D_C_E_CIRCUMFLX   ,               /* 230 */
    D_C_A_ACUTE       ,               /* 231 */
    D_C_E_UMLAUT      ,               /* 232 */
    D_C_E_GRAVE       ,               /* 233 */
    D_C_I_ACUTE       ,               /* 234 */
    D_C_I_CIRCUMFLX   ,               /* 235 */
    D_C_I_UMLAUT      ,               /* 236 */
    D_C_I_GRAVE       ,               /* 237 */
    D_C_O_ACUTE       ,               /* 238 */
    D_C_O_CIRCUMFLX   ,               /* 239 */
    D_RT_S_GUILL      ,               /* 240 */
    D_C_O_GRAVE       ,               /* 241 */
    D_C_U_ACUTE       ,               /* 242 */
    D_C_U_CIRCUMFLX   ,               /* 243 */
    D_C_U_GRAVE       ,               /* 244 */
    D_C_EDH           ,               /* 245 */
    D_C_Y_ACUTE       ,               /* 246 */
    D_L_THORN         ,               /* 247 */
    D_MACRON          ,               /* 248 */
    D_L_EDH           ,               /* 249 */
    D_L_Y_ACUTE       ,               /* 250 */
    D_C_THORN         ,               /* 251 */
    D_CEDILLA         ,               /* 252 */
    D_L9_S_QUOTE      ,               /* 253 */
    D_L99_D_QUOTE     ,               /* 254 */
    D_BROKEN_VBAR                     /* 255 */
};

#endif        /* End of Macintosh Conversion Table */


#ifdef  UNIX

   /* The ISO Latin-1 Character Set is almost identical to our          */
   /* Master Character Set.  The differences are so slight, some        */
   /* applications may not need to perform conversion for UNIX.         */

/* NOTE:                                                                */
/* SLToNatChrSet[] VALUES ARE FOR MCS Latin 1 TO UNIX EXTENDED          */
/* WHILE SLToDefChrSet[] VALUES ARE FOR THE UNIX EXTENDED TO MCS        */
/* LATIN 1                                                              */

/* THE FIRST 128 CHARACTERS ARE EXACT PAIRS SO THERE IS NO CONVERSION   */
/* THE NEXT 128 ARE THE HIGH ORDER VALUES AND THE ACCENTED CHARACTERS   */

DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
    U_FIL_1          ,               /* 128 */
    U_FIL_2          ,               /* 129 */
    U_L9_S_QUOTE     ,               /* 130 */
    U_FLORIN         ,               /* 131 */
    U_L99_D_QUOTE    ,               /* 132 */
    U_ELLIPSIS       ,               /* 133 */
    U_FIL_3          ,               /* 134 */
    U_FIL_4          ,               /* 135 */
    U_FIL_5          ,               /* 136 */
    U_FIL_6          ,               /* 137 */
    U_FIL_7          ,               /* 138 */
    U_LT_S_GUILL     ,               /* 139 */
    U_MULTIPLICATION ,               /* 140 */
    U_FIL_8          ,               /* 141 */
    U_FIL_9          ,               /* 142 */
    U_FIL_10         ,               /* 143 */
    U_FIL_11         ,               /* 144 */
    U_OPEN_S_QUOTE   ,               /* 145 */
    U_CLOSE_S_QUOTE  ,               /* 146 */
    U_OPEN_D_QUOTE   ,               /* 147 */
    U_CLOSE_D_QUOTE  ,               /* 148 */
    U_FIL_12         ,               /* 149 */
    U_EN_DASH        ,               /* 150 */
    U_M_DASH         ,               /* 151 */
    U_FIL_13         ,               /* 152 */
    U_TRADEMARK      ,               /* 153 */
    U_FIL_14         ,               /* 154 */
    U_RT_S_GUILL     ,               /* 155 */
    U_DIVISION       ,               /* 156 */
    U_FIL_15         ,               /* 157 */
    U_FIL_16         ,               /* 158 */
    U_C_Y_ACUTE      ,               /* 159 */
    U_NBSP           ,               /* 160 */
    U_INV_EXCLAMATN  ,               /* 161 */
    U_CENT           ,               /* 162 */
    U_POUND          ,               /* 163 */
    U_UMLAUT         ,               /* 164 */
    U_YEN            ,               /* 165 */
    U_BROKEN_VBAR    ,               /* 166 */
    U_SECTION        ,               /* 167 */
    U_GEN_CURRENCY   ,               /* 168 */
    U_COPYRIGHT      ,               /* 169 */
    U_FEM_ORDINAL    ,               /* 170 */
    U_OPEN_GUILL     ,               /* 171 */
    U_NOT_SIGN       ,               /* 172 */
    U_SHY            ,               /* 173 */
    U_REG_TRADEMARK  ,               /* 174 */
    U_MACRON         ,               /* 175 */
    U_DEGREE         ,               /* 176 */
    U_PLUS_MINUS     ,               /* 177 */
    U_TWO_SUPER      ,               /* 178 */
    U_THREE_SUPER    ,               /* 179 */
    U_ACUTE          ,               /* 180 */
    U_MICRO_SIGN     ,               /* 181 */
    U_PARAGRAPH      ,               /* 182 */
    U_MIDDLE_DOT     ,               /* 183 */
    U_CEDILLA        ,               /* 184 */
    U_ONE_SUPER      ,               /* 185 */
    U_MAS_ORDINAL    ,               /* 186 */
    U_CLOS_GUILL     ,               /* 187 */
    U_ONE_QUARTER    ,               /* 188 */
    U_ONE_HALF       ,               /* 189 */
    U_THREE_FOURTHS  ,               /* 190 */
    U_INV_QUESTION   ,               /* 191 */
    U_C_A_GRAVE      ,               /* 192 */
    U_C_A_ACUTE      ,               /* 193 */
    U_C_A_CIRCUMFLX  ,               /* 194 */
    U_C_A_TILDE      ,               /* 195 */
    U_C_A_UMLAUT     ,               /* 196 */
    U_C_A_ANGSTROM   ,               /* 197 */
    U_C_AE_LIGATURE  ,               /* 198 */
    U_C_C_CEDILLA    ,               /* 199 */
    U_C_E_GRAVE      ,               /* 200 */
    U_C_E_ACUTE      ,               /* 201 */
    U_C_E_CIRCUMFLX  ,               /* 202 */
    U_C_E_UMLAUT     ,               /* 203 */
    U_C_I_GRAVE      ,               /* 204 */
    U_C_I_ACUTE      ,               /* 205 */
    U_C_I_CIRCUMFLX  ,               /* 206 */
    U_C_I_UMLAUT     ,               /* 207 */
    U_C_EDH          ,               /* 208 */
    U_C_N_TILDE      ,               /* 209 */
    U_C_O_GRAVE      ,               /* 210 */
    U_C_O_ACUTE      ,               /* 211 */
    U_C_O_CIRCUMFLX  ,               /* 212 */
    U_C_O_TILDE      ,               /* 213 */
    U_C_O_UMLAUT     ,               /* 214 */
    U_C_OE_LIGATURE  ,               /* 215 */
    U_C_O_SLASH      ,               /* 216 */
    U_C_U_GRAVE      ,               /* 217 */
    U_C_U_ACUTE      ,               /* 218 */
    U_C_U_CIRCUMFLX  ,               /* 219 */
    U_C_U_UMLAUT     ,               /* 220 */
    U_C_Y_UMLAUT     ,               /* 221 */
    U_C_THORN        ,               /* 222 */
    U_L_S_SHARP      ,               /* 223 */
    U_L_A_GRAVE      ,               /* 224 */
    U_L_A_ACUTE      ,               /* 225 */
    U_L_A_CIRCUMFLX  ,               /* 226 */
    U_L_A_TILDE      ,               /* 227 */
    U_L_A_UMLAUT     ,               /* 228 */
    U_L_A_ANGSTROM   ,               /* 229 */
    U_L_AE_LIGATURE  ,               /* 230 */
    U_L_C_CEDILLA    ,               /* 231 */
    U_L_E_GRAVE      ,               /* 232 */
    U_L_E_ACUTE      ,               /* 233 */
    U_L_E_CIRCUMFLX  ,               /* 234 */
    U_L_E_UMLAUT     ,               /* 235 */
    U_L_I_GRAVE      ,               /* 236 */
    U_L_I_ACUTE      ,               /* 237 */
    U_L_I_CIRCUMFLX  ,               /* 238 */
    U_L_I_UMLAUT     ,               /* 239 */
    U_L_EDH          ,               /* 240 */
    U_L_N_TILDE      ,               /* 241 */
    U_L_O_GRAVE      ,               /* 242 */
    U_L_O_ACUTE      ,               /* 243 */
    U_L_O_CIRCUMFLX  ,               /* 244 */
    U_L_O_TILDE      ,               /* 245 */
    U_L_O_UMLAUT     ,               /* 246 */
    U_L_OE_LIGATURE  ,               /* 247 */
    U_L_O_SLASH      ,               /* 248 */
    U_L_U_GRAVE      ,               /* 249 */
    U_L_U_ACUTE      ,               /* 250 */
    U_L_U_CIRCUMFLX  ,               /* 251 */
    U_L_U_UMLAUT     ,               /* 252 */
    U_L_Y_UMLAUT     ,               /* 253 */
    U_L_THORN        ,               /* 254 */
    U_L_Y_ACUTE                      /* 255 */
};

DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_FIL_1         ,               /* 128 */
    D_FIL_2         ,               /* 129 */
    D_L9_S_QUOTE    ,               /* 130 */
    D_FLORIN        ,               /* 131 */
    D_L99_D_QUOTE   ,               /* 132 */
    D_ELLIPSIS      ,               /* 133 */
    D_FIL_3         ,               /* 134 */
    D_FIL_4         ,               /* 135 */
    D_FIL_5         ,               /* 136 */
    D_FIL_6         ,               /* 137 */
    D_FIL_7         ,               /* 138 */
    D_LT_S_GUILL    ,               /* 139 */
    D_C_OE_LIGATURE ,               /* 140 */
    D_FIL_8         ,               /* 141 */
    D_FIL_9         ,               /* 142 */
    D_FIL_10        ,               /* 143 */
    D_FIL_11        ,               /* 144 */
    D_OPEN_S_QUOTE  ,               /* 145 */
    D_CLOSE_S_QUOTE ,               /* 146 */
    D_OPEN_D_QUOTE  ,               /* 147 */
    D_CLOSE_D_QUOTE ,               /* 148 */
    D_FIL_12        ,               /* 149 */
    D_EN_DASH       ,               /* 150 */
    D_M_DASH        ,               /* 151 */
    D_FIL_13        ,               /* 152 */
    D_TRADEMARK     ,               /* 153 */
    D_FIL_14        ,               /* 154 */
    D_RT_S_GUILL    ,               /* 155 */
    D_L_OE_LIGATURE ,               /* 156 */
    D_FIL_15        ,               /* 157 */
    D_FIL_16        ,               /* 158 */
    D_C_Y_UMLAUT    ,               /* 159 */
    D_NBSP          ,               /* 160 */
    D_INV_EXCLAMATN ,               /* 161 */
    D_CENT          ,               /* 162 */
    D_POUND         ,               /* 163 */
    D_GEN_CURRENCY  ,               /* 164 */
    D_YEN           ,               /* 165 */
    D_BROKEN_VBAR   ,               /* 166 */
    D_SECTION       ,               /* 167 */
    D_UMLAUT        ,               /* 168 */
    D_COPYRIGHT     ,               /* 169 */
    D_FEM_ORDINAL   ,               /* 170 */
    D_OPEN_GUILL    ,               /* 171 */
    D_NOT_SIGN      ,               /* 172 */
    D_SHY           ,               /* 173 */
    D_REG_TRADEMARK ,               /* 174 */
    D_MACRON        ,               /* 175 */
    D_DEGREE        ,               /* 176 */
    D_PLUS_MINUS    ,               /* 177 */
    D_TWO_SUPER     ,               /* 178 */
    D_THREE_SUPER   ,               /* 179 */
    D_ACUTE         ,               /* 180 */
    D_MICRO_SIGN    ,               /* 181 */
    D_PARAGRAPH     ,               /* 182 */
    D_MIDDLE_DOT    ,               /* 183 */
    D_CEDILLA       ,               /* 184 */
    D_ONE_SUPER     ,               /* 185 */
    D_MAS_ORDINAL   ,               /* 186 */
    D_CLOS_GUILL    ,               /* 187 */
    D_ONE_QUARTER   ,               /* 188 */
    D_ONE_HALF      ,               /* 189 */
    D_THREE_FOURTHS ,               /* 190 */
    D_INV_QUESTION  ,               /* 191 */
    D_C_A_GRAVE     ,               /* 192 */
    D_C_A_ACUTE     ,               /* 193 */
    D_C_A_CIRCUMFLX ,               /* 194 */
    D_C_A_TILDE     ,               /* 195 */
    D_C_A_UMLAUT    ,               /* 196 */
    D_C_A_ANGSTROM  ,               /* 197 */
    D_C_AE_LIGATURE ,               /* 198 */
    D_C_C_CEDILLA   ,               /* 199 */
    D_C_E_GRAVE     ,               /* 200 */
    D_C_E_ACUTE     ,               /* 201 */
    D_C_E_CIRCUMFLX ,               /* 202 */
    D_C_E_UMLAUT    ,               /* 203 */
    D_C_I_GRAVE     ,               /* 204 */
    D_C_I_ACUTE     ,               /* 205 */
    D_C_I_CIRCUMFLX ,               /* 206 */
    D_C_I_UMLAUT    ,               /* 207 */
    D_C_EDH         ,               /* 208 */
    D_C_N_TILDE     ,               /* 209 */
    D_C_O_GRAVE     ,               /* 210 */
    D_C_O_ACUTE     ,               /* 211 */
    D_C_O_CIRCUMFLX ,               /* 212 */
    D_C_O_TILDE     ,               /* 213 */
    D_C_O_UMLAUT    ,               /* 214 */
    D_MULTIPLICATION,               /* 215 */
    D_C_O_SLASH     ,               /* 216 */
    D_C_U_GRAVE     ,               /* 217 */
    D_C_U_ACUTE     ,               /* 218 */
    D_C_U_CIRCUMFLX ,               /* 219 */
    D_C_U_UMLAUT    ,               /* 220 */
    D_C_Y_ACUTE     ,               /* 221 */
    D_C_THORN       ,               /* 222 */
    D_L_S_SHARP     ,               /* 223 */
    D_L_A_GRAVE     ,               /* 224 */
    D_L_A_ACUTE     ,               /* 225 */
    D_L_A_CIRCUMFLX ,               /* 226 */
    D_L_A_TILDE     ,               /* 227 */
    D_L_A_UMLAUT    ,               /* 228 */
    D_L_A_ANGSTROM  ,               /* 229 */
    D_L_AE_LIGATURE ,               /* 230 */
    D_L_C_CEDILLA   ,               /* 231 */
    D_L_E_GRAVE     ,               /* 232 */
    D_L_E_ACUTE     ,               /* 233 */
    D_L_E_CIRCUMFLX ,               /* 234 */
    D_L_E_UMLAUT    ,               /* 235 */
    D_L_I_GRAVE     ,               /* 236 */
    D_L_I_ACUTE     ,               /* 237 */
    D_L_I_CIRCUMFLX ,               /* 238 */
    D_L_I_UMLAUT    ,               /* 239 */
    D_L_EDH         ,               /* 240 */
    D_L_N_TILDE     ,               /* 241 */
    D_L_O_GRAVE     ,               /* 242 */
    D_L_O_ACUTE     ,               /* 243 */
    D_L_O_CIRCUMFLX ,               /* 244 */
    D_L_O_TILDE     ,               /* 245 */
    D_L_O_UMLAUT    ,               /* 246 */
    D_DIVISION      ,               /* 247 */
    D_L_O_SLASH     ,               /* 248 */
    D_L_U_GRAVE     ,               /* 249 */
    D_L_U_ACUTE     ,               /* 250 */
    D_L_U_CIRCUMFLX ,               /* 251 */
    D_L_U_UMLAUT    ,               /* 252 */
    D_L_Y_ACUTE     ,               /* 253 */
    D_L_THORN       ,               /* 254 */
    D_L_Y_UMLAUT                    /* 255 */
};

#endif  /* End of UNIX conversion table */

#ifdef  VAX

/* DEC Multinational and our MCS Latin 1 character sets are identical     */
/* throughout the scope of used characters, so conversion is not necessary*/
/* A dummy table is provided, however, for the cases in which Latin 2 or  */
/* Latin 5 character sets are defined, and for this reason, the CONVERT   */
/* flag must be set.                                                      */

DSTATIC  UCHAR  SLToNat1ChrSet[128] =
{
    D_FIL_1          ,               /* 128 */
    D_FIL_2          ,               /* 129 */
    D_L9_S_QUOTE     ,               /* 130 */
    D_FLORIN         ,               /* 131 */
    D_L99_D_QUOTE    ,               /* 132 */
    D_ELLIPSIS       ,               /* 133 */
    D_FIL_3          ,               /* 134 */
    D_FIL_4          ,               /* 135 */
    D_FIL_5          ,               /* 136 */
    D_FIL_6          ,               /* 137 */
    D_FIL_7          ,               /* 138 */
    D_LT_S_GUILL     ,               /* 139 */
    D_MULTIPLICATION ,               /* 140 */
    D_FIL_8          ,               /* 141 */
    D_FIL_9          ,               /* 142 */
    D_FIL_10         ,               /* 143 */
    D_FIL_11         ,               /* 144 */
    D_OPEN_S_QUOTE   ,               /* 145 */
    D_CLOSE_S_QUOTE  ,               /* 146 */
    D_OPEN_D_QUOTE   ,               /* 147 */
    D_CLOSE_D_QUOTE  ,               /* 148 */
    D_FIL_12         ,               /* 149 */
    D_EN_DASH        ,               /* 150 */
    D_M_DASH         ,               /* 151 */
    D_FIL_13         ,               /* 152 */
    D_TRADEMARK      ,               /* 153 */
    D_FIL_14         ,               /* 154 */
    D_RT_S_GUILL     ,               /* 155 */
    D_DIVISION       ,               /* 156 */
    D_FIL_15         ,               /* 157 */
    D_FIL_16         ,               /* 158 */
    D_C_Y_ACUTE      ,               /* 159 */
    D_NBSP           ,               /* 160 */
    D_INV_EXCLAMATN  ,               /* 161 */
    D_CENT           ,               /* 162 */
    D_POUND          ,               /* 163 */
    D_UMLAUT         ,               /* 164 */
    D_YEN            ,               /* 165 */
    D_BROKEN_VBAR    ,               /* 166 */
    D_SECTION        ,               /* 167 */
    D_GEN_CURRENCY   ,               /* 168 */
    D_COPYRIGHT      ,               /* 169 */
    D_FEM_ORDINAL    ,               /* 170 */
    D_OPEN_GUILL     ,               /* 171 */
    D_NOT_SIGN       ,               /* 172 */
    D_SHY            ,               /* 173 */
    D_REG_TRADEMARK  ,               /* 174 */
    D_MACRON         ,               /* 175 */
    D_DEGREE         ,               /* 176 */
    D_PLUS_MINUS     ,               /* 177 */
    D_TWO_SUPER      ,               /* 178 */
    D_THREE_SUPER    ,               /* 179 */
    D_ACUTE          ,               /* 180 */
    D_MICRO_SIGN     ,               /* 181 */
    D_PARAGRAPH      ,               /* 182 */
    D_MIDDLE_DOT     ,               /* 183 */
    D_CEDILLA        ,               /* 184 */
    D_ONE_SUPER      ,               /* 185 */
    D_MAS_ORDINAL    ,               /* 186 */
    D_CLOS_GUILL     ,               /* 187 */
    D_ONE_QUARTER    ,               /* 188 */
    D_ONE_HALF       ,               /* 189 */
    D_THREE_FOURTHS  ,               /* 190 */
    D_INV_QUESTION   ,               /* 191 */
    D_C_A_GRAVE      ,               /* 192 */
    D_C_A_ACUTE      ,               /* 193 */
    D_C_A_CIRCUMFLX  ,               /* 194 */
    D_C_A_TILDE      ,               /* 195 */
    D_C_A_UMLAUT     ,               /* 196 */
    D_C_A_ANGSTROM   ,               /* 197 */
    D_C_AE_LIGATURE  ,               /* 198 */
    D_C_C_CEDILLA    ,               /* 199 */
    D_C_E_GRAVE      ,               /* 200 */
    D_C_E_ACUTE      ,               /* 201 */
    D_C_E_CIRCUMFLX  ,               /* 202 */
    D_C_E_UMLAUT     ,               /* 203 */
    D_C_I_GRAVE      ,               /* 204 */
    D_C_I_ACUTE      ,               /* 205 */
    D_C_I_CIRCUMFLX  ,               /* 206 */
    D_C_I_UMLAUT     ,               /* 207 */
    D_C_EDH          ,               /* 208 */
    D_C_N_TILDE      ,               /* 209 */
    D_C_O_GRAVE      ,               /* 210 */
    D_C_O_ACUTE      ,               /* 211 */
    D_C_O_CIRCUMFLX  ,               /* 212 */
    D_C_O_TILDE      ,               /* 213 */
    D_C_O_UMLAUT     ,               /* 214 */
    D_C_OE_LIGATURE  ,               /* 215 */
    D_C_O_SLASH      ,               /* 216 */
    D_C_U_GRAVE      ,               /* 217 */
    D_C_U_ACUTE      ,               /* 218 */
    D_C_U_CIRCUMFLX  ,               /* 219 */
    D_C_U_UMLAUT     ,               /* 220 */
    D_C_Y_UMLAUT     ,               /* 221 */
    D_C_THORN        ,               /* 222 */
    D_L_S_SHARP      ,               /* 223 */
    D_L_A_GRAVE      ,               /* 224 */
    D_L_A_ACUTE      ,               /* 225 */
    D_L_A_CIRCUMFLX  ,               /* 226 */
    D_L_A_TILDE      ,               /* 227 */
    D_L_A_UMLAUT     ,               /* 228 */
    D_L_A_ANGSTROM   ,               /* 229 */
    D_L_AE_LIGATURE  ,               /* 230 */
    D_L_C_CEDILLA    ,               /* 231 */
    D_L_E_GRAVE      ,               /* 232 */
    D_L_E_ACUTE      ,               /* 233 */
    D_L_E_CIRCUMFLX  ,               /* 234 */
    D_L_E_UMLAUT     ,               /* 235 */
    D_L_I_GRAVE      ,               /* 236 */
    D_L_I_ACUTE      ,               /* 237 */
    D_L_I_CIRCUMFLX  ,               /* 238 */
    D_L_I_UMLAUT     ,               /* 239 */
    D_L_EDH          ,               /* 240 */
    D_L_N_TILDE      ,               /* 241 */
    D_L_O_GRAVE      ,               /* 242 */
    D_L_O_ACUTE      ,               /* 243 */
    D_L_O_CIRCUMFLX  ,               /* 244 */
    D_L_O_TILDE      ,               /* 245 */
    D_L_O_UMLAUT     ,               /* 246 */
    D_L_OE_LIGATURE  ,               /* 247 */
    D_L_O_SLASH      ,               /* 248 */
    D_L_U_GRAVE      ,               /* 249 */
    D_L_U_ACUTE      ,               /* 250 */
    D_L_U_CIRCUMFLX  ,               /* 251 */
    D_L_U_UMLAUT     ,               /* 252 */
    D_L_Y_UMLAUT     ,               /* 253 */
    D_L_THORN        ,               /* 254 */
    D_L_Y_ACUTE                      /* 255 */
};

DSTATIC  UCHAR  SLToDef1ChrSet[128] =
{
    D_FIL_1         ,               /* 128 */
    D_FIL_2         ,               /* 129 */
    D_L9_S_QUOTE    ,               /* 130 */
    D_FLORIN        ,               /* 131 */
    D_L99_D_QUOTE   ,               /* 132 */
    D_ELLIPSIS      ,               /* 133 */
    D_FIL_3         ,               /* 134 */
    D_FIL_4         ,               /* 135 */
    D_FIL_5         ,               /* 136 */
    D_FIL_6         ,               /* 137 */
    D_FIL_7         ,               /* 138 */
    D_LT_S_GUILL    ,               /* 139 */
    D_MULTIPLICATION,               /* 140 */
    D_FIL_8         ,               /* 141 */
    D_FIL_9         ,               /* 142 */
    D_FIL_10        ,               /* 143 */
    D_FIL_11        ,               /* 144 */
    D_OPEN_S_QUOTE  ,               /* 145 */
    D_CLOSE_S_QUOTE ,               /* 146 */
    D_OPEN_D_QUOTE  ,               /* 147 */
    D_CLOSE_D_QUOTE ,               /* 148 */
    D_FIL_12        ,               /* 149 */
    D_EN_DASH       ,               /* 150 */
    D_M_DASH        ,               /* 151 */
    D_FIL_13        ,               /* 152 */
    D_TRADEMARK     ,               /* 153 */
    D_FIL_14        ,               /* 154 */
    D_RT_S_GUILL    ,               /* 155 */
    D_DIVISION      ,               /* 156 */
    D_FIL_15        ,               /* 157 */
    D_FIL_16        ,               /* 158 */
    D_C_Y_ACUTE     ,               /* 159 */
    D_NBSP          ,               /* 160 */
    D_INV_EXCLAMATN ,               /* 161 */
    D_CENT          ,               /* 162 */
    D_POUND         ,               /* 163 */
    D_UMLAUT        ,               /* 164 */
    D_YEN           ,               /* 165 */
    D_BROKEN_VBAR   ,               /* 166 */
    D_SECTION       ,               /* 167 */
    D_GEN_CURRENCY  ,               /* 168 */
    D_COPYRIGHT     ,               /* 169 */
    D_FEM_ORDINAL   ,               /* 170 */
    D_OPEN_GUILL    ,               /* 171 */
    D_NOT_SIGN      ,               /* 172 */
    D_SHY           ,               /* 173 */
    D_REG_TRADEMARK ,               /* 174 */
    D_MACRON        ,               /* 175 */
    D_DEGREE        ,               /* 176 */
    D_PLUS_MINUS    ,               /* 177 */
    D_TWO_SUPER     ,               /* 178 */
    D_THREE_SUPER   ,               /* 179 */
    D_ACUTE         ,               /* 180 */
    D_MICRO_SIGN    ,               /* 181 */
    D_PARAGRAPH     ,               /* 182 */
    D_MIDDLE_DOT    ,               /* 183 */
    D_CEDILLA       ,               /* 184 */
    D_ONE_SUPER     ,               /* 185 */
    D_MAS_ORDINAL   ,               /* 186 */
    D_CLOS_GUILL    ,               /* 187 */
    D_ONE_QUARTER   ,               /* 188 */
    D_ONE_HALF      ,               /* 189 */
    D_THREE_FOURTHS ,               /* 190 */
    D_INV_QUESTION  ,               /* 191 */
    D_C_A_GRAVE     ,               /* 192 */
    D_C_A_ACUTE     ,               /* 193 */
    D_C_A_CIRCUMFLX ,               /* 194 */
    D_C_A_TILDE     ,               /* 195 */
    D_C_A_UMLAUT    ,               /* 196 */
    D_C_A_ANGSTROM  ,               /* 197 */
    D_C_AE_LIGATURE ,               /* 198 */
    D_C_C_CEDILLA   ,               /* 199 */
    D_C_E_GRAVE     ,               /* 200 */
    D_C_E_ACUTE     ,               /* 201 */
    D_C_E_CIRCUMFLX ,               /* 202 */
    D_C_E_UMLAUT    ,               /* 203 */
    D_C_I_GRAVE     ,               /* 204 */
    D_C_I_ACUTE     ,               /* 205 */
    D_C_I_CIRCUMFLX ,               /* 206 */
    D_C_I_UMLAUT    ,               /* 207 */
    D_C_EDH         ,               /* 208 */
    D_C_N_TILDE     ,               /* 209 */
    D_C_O_GRAVE     ,               /* 210 */
    D_C_O_ACUTE     ,               /* 211 */
    D_C_O_CIRCUMFLX ,               /* 212 */
    D_C_O_TILDE     ,               /* 213 */
    D_C_O_UMLAUT    ,               /* 214 */
    D_C_OE_LIGATURE ,               /* 215 */
    D_C_O_SLASH     ,               /* 216 */
    D_C_U_GRAVE     ,               /* 217 */
    D_C_U_ACUTE     ,               /* 218 */
    D_C_U_CIRCUMFLX ,               /* 219 */
    D_C_U_UMLAUT    ,               /* 220 */
    D_C_Y_UMLAUT    ,               /* 221 */
    D_C_THORN       ,               /* 222 */
    D_L_S_SHARP     ,               /* 223 */
    D_L_A_GRAVE     ,               /* 224 */
    D_L_A_ACUTE     ,               /* 225 */
    D_L_A_CIRCUMFLX ,               /* 226 */
    D_L_A_TILDE     ,               /* 227 */
    D_L_A_UMLAUT    ,               /* 228 */
    D_L_A_ANGSTROM  ,               /* 229 */
    D_L_AE_LIGATURE ,               /* 230 */
    D_L_C_CEDILLA   ,               /* 231 */
    D_L_E_GRAVE     ,               /* 232 */
    D_L_E_ACUTE     ,               /* 233 */
    D_L_E_CIRCUMFLX ,               /* 234 */
    D_L_E_UMLAUT    ,               /* 235 */
    D_L_I_GRAVE     ,               /* 236 */
    D_L_I_ACUTE     ,               /* 237 */
    D_L_I_CIRCUMFLX ,               /* 238 */
    D_L_I_UMLAUT    ,               /* 239 */
    D_L_EDH         ,               /* 240 */
    D_L_N_TILDE     ,               /* 241 */
    D_L_O_GRAVE     ,               /* 242 */
    D_L_O_ACUTE     ,               /* 243 */
    D_L_O_CIRCUMFLX ,               /* 244 */
    D_L_O_TILDE     ,               /* 245 */
    D_L_O_UMLAUT    ,               /* 246 */
    D_L_OE_LIGATURE ,               /* 247 */
    D_L_O_SLASH     ,               /* 248 */
    D_L_U_GRAVE     ,               /* 249 */
    D_L_U_ACUTE     ,               /* 250 */
    D_L_U_CIRCUMFLX ,               /* 251 */
    D_L_U_UMLAUT    ,               /* 252 */
    D_L_Y_UMLAUT    ,               /* 253 */
    D_L_THORN       ,               /* 254 */
    D_L_Y_ACUTE                     /* 255 */
};

#endif  /* End of VAX conversion table */

#endif /* SLCNVTB1_C */

/* End of file - slcnvtb1.c */

