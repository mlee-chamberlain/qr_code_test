/**
 * @file font_4_22.h
 *
 * @brief File contains font definition for 4x22 lines display ported from states.c.
 * @author Ilya Mushkirov
 *
 * @copyright 2022 The Chamberlain Group, LLC. All rights reserved. All
 * information within this file and associated files, including all
 * information and files transmitted with this file are CONFIDENTIAL
 * and the proprietary property of The Chamberlain Group, LLC.
 *
 * In using the Licensed Software, Company shall not use with, combine, or
 * incorporate any viral open source software with or into any of the Licensed
 * Software in a manner that would require any portion of the Licensed software to
 * be (i) disclosed or distributed in source code form; (ii) licensed for the
 * purpose of making derivative works; or (iii) distributable or redistributable
 * at no charge.
 */

#ifndef _FONT_4_22_H_
#define _FONT_4_22_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NUM_PIX_COL_PER_ROW_BYTES (128)
#define NUM_PIX_ROW_PER_COL_BYTES (6)

#define CHARACTER_HEIGHT (8)  // Definition for characters bitmaps height

#define NUM_LANGUAGES (7)

typedef struct font_index_s
{
    uint16_t index;
} font_index_t;

typedef enum
{
  al_left=0,
  al_center,
  al_right
} alignment_t;

typedef enum
{
    GERMAN  = 0,
    DUTCH   = 1,
    ENGLISH = 2,
    ITALIAN = 3,
    FRENCH  = 4,
    SPANISH = 5,
    SWEDISH = 6,
} language_e;

#define BIG_FONT_NUM_COL (22)
#define BIG_FONT_NUM_ROW (4)

#define CHAR_PIXEL_HEIGHT (8)

typedef struct big_font_s
{
    uint8_t value[BIG_FONT_NUM_ROW][BIG_FONT_NUM_COL]; // 4 rows of 22 bytes
} big_font_t;

//memory addresses for each charater in custom character memspace:
//international
#define _cUARR 1
#define _cDARR 2
#define _UARR "\x01"
#define _DARR "\x02"

//german
#define _AE "\x03"
#define _OE "\x04"
#define _UE "\x05"
#define _SZ "\x06"

//swedish
//#define _AE "\x02"
//#define _OE "\x03"
//#define _UE "\x04"
#define _Ao "\x07"

//french
#define _EACE   "\x03"   // E with accent aigue
#define _EGRAVE "\x04"   // E with accent grave
#define _ECIRC  "\x05"   // E with accent circumflex french

//spanish
//#define _EACE  "\x03" // E with accent aigue
#define _QMINV   "\x04"    // spanish inverted question mark
#define _EMINV   "\x05"    // spanish inverted exclamation mark
#define _NTILDE  "\x06"    // N with tilde
#define _AACE    "\x07"    // A with accent aigue
#define _UACE    "\x08"    // U with accent aigue

#define _IACE    "I"    // I with accent aigue
#define _OACE    "O"    // O with accent aigue


//italian
//#define _EGRAVE "\x04"     // E with accent grave
#define _UGRAVE "\x05"     // U with accent grave italian
#define _AGRAVE "\x06"     // A with accent grave italian

#define _AP      "'"     // apostrope

//not used
#define _OCIRC "O"     // O with accent circumflex
#define _CCED  "C"     // C with cedille

typedef uint8_t lcd_character_t[CHAR_PIXEL_HEIGHT];

typedef struct
{
    uint8_t size;
    uint8_t arr[64];
    bool    is_blinking;//is blinking icon
    bool    trimmable;//should be false for aligment icons
} font_char;

extern const font_char *font_array[];



//NEW STRUCT FOR TESTING WITH MAX SIZE OF V7
typedef struct qr_code_s
{
    uint8_t value[6][45]; // 4 rows x 25
} qr_code_t;





/* ORIGINAL STRUCT
typedef struct qr_code_s
{
    uint8_t value[4][25]; // 4 rows x 25
} qr_code_t;
*/



//SPECIAL CHARACTERS
extern const lcd_character_t LCD_AE;    //�
extern const lcd_character_t LCD_OE;    //�
extern const lcd_character_t LCD_UE;    //�
extern const lcd_character_t LCD_Ao;    //�
extern const lcd_character_t LCD_SZ;    //�
extern const lcd_character_t LCD_EACE;  //�
extern const lcd_character_t LCD_OCIRC; //�
extern const lcd_character_t LCD_UARR;  // up arrow
extern const lcd_character_t LCD_DARR;  // down arrow
extern const lcd_character_t LCD_OACE;  //�
extern const lcd_character_t LCD_EGRAVE;//�
extern const lcd_character_t LCD_CCED;  //C with cedille
extern const lcd_character_t LCD_NTILDE;//N with tilde
extern const lcd_character_t LCD_AACE;  //�
extern const lcd_character_t LCD_IACE;  //�
extern const lcd_character_t LCD_UACE;  //�
extern const lcd_character_t LCD_ECIRC; //�
extern const lcd_character_t LCD_UGRAVE;//�
extern const lcd_character_t LCD_AGRAVE;//�
extern const lcd_character_t LCD_QMINV; // spanish inverted question mark
extern const lcd_character_t LCD_EMINV; // spanish inverted exclamation mark

extern const /*__flash*/ qr_code_t qr_code_myq_v3[];
extern const /*__flash*/ qr_code_t qr_code_myq_v4[];
extern const /*__flash*/ qr_code_t qr_code_myq_v5[];
extern const /*__flash*/ qr_code_t qr_code_myq_v6[];
extern const /*__flash*/ qr_code_t qr_code_myq_v7[];


#ifdef __cplusplus
}
#endif // __cplusplus

#endif
