/**
 * File:          proj4_wcscoll_cz.c
 * Version:       0.9 beta
 * Date:          17-12-2011
 * Last update:   18-12-2011
 *
 * Course:        IZP (winter semester, 2011)
 * Project:       #4 - Czech sorting
 *
 * Author:        David Kaspar (aka Dee'Kej), 1BIT
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies
 * E-mail:        xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   This module contains wcscoll_cz() function which is used for
 *                sorting by Czech CSN_97_6030, and also auxiliary tables used
 *                for the comparison.
 *
 * More info @:
 *      https://www.fit.vutbr.cz/study/courses/IZP/private/projekty/proj4.html
 *
 * File encoding: en_US.utf8 (United States)
 * Compiler used: gcc 4.5.2 (Ubuntu/Linaro 4.5.2-8ubuntu4)
 */


/******************************************************************************
 ***[ START OF PROJ4_WCIO.C ]**************************************************
 ******************************************************************************/

/******************************************************************************
 ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

#include <errno.h>
#include <wchar.h>

#include "proj4_wcscoll_cz.h"

/******************************************************************************
 ~~~[ LOCAL DATA TYPES DECLARATIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/*
 * Character with the highest Unicode value - represents the highest possible
 * indexing int the sorting tables (highest array bound).
 * (Actual value represents: Latin small letter z with caron.)
 */
static const wchar_t TABLE_MAX = L'\u017E';


/* Array containing values for default characters comparison (1st passage). */
static const wint_t const cmp_set_def[] = {
  [L'\n'] = 79,
  [L' '] = 78,
  [L'\u00A0'] = 78,
  [L'\t'] = 77,

  [L'A'] = 76,    [L'a'] = 76,
  [L'Ā'] = 76,    [L'ā'] = 76,
  [L'Á'] = 76,    [L'á'] = 76,
  [L'À'] = 76,    [L'à'] = 76,
  [L'Â'] = 76,    [L'â'] = 76,
  [L'Ã'] = 76,    [L'ã'] = 76,
  [L'Ă'] = 76,    [L'ă'] = 76,
  [L'Å'] = 76,    [L'å'] = 76,
  [L'Ä'] = 76,    [L'ä'] = 76,
  [L'Ą'] = 76,    [L'ą'] = 76,

  [L'B'] = 75,    [L'b'] = 75,
  [L'C'] = 74,    [L'c'] = 74,
  [L'Ċ'] = 74,    [L'ċ'] = 74,
  [L'Ć'] = 74,    [L'ć'] = 74,
  [L'Ĉ'] = 74,    [L'ĉ'] = 74,
  [L'Ç'] = 74,    [L'ç'] = 74,

  [L'Č'] = 73,    [L'č'] = 73,
  [L'D'] = 72,    [L'd'] = 72,
  [L'Ď'] = 72,    [L'ď'] = 72,
  [L'Đ'] = 72,    [L'đ'] = 72,
  [L'E'] = 71,    [L'e'] = 71,
  [L'Ė'] = 71,    [L'ė'] = 71,
  [L'Ē'] = 71,    [L'ē'] = 71,
  [L'É'] = 71,    [L'é'] = 71,
  [L'Ê'] = 71,    [L'ê'] = 71,
  [L'Ě'] = 71,    [L'ě'] = 71,
  [L'Ĕ'] = 71,    [L'ĕ'] = 71,
  [L'Ë'] = 71,    [L'ë'] = 71,
  [L'Ę'] = 71,    [L'ę'] = 71,

  [L'F'] = 70,    [L'f'] = 70,
  [L'G'] = 69,    [L'g'] = 69,
  [L'Ġ'] = 69,    [L'ġ'] = 69,
                  [L'ģ'] = 69,

  [L'Ĝ'] = 69,    [L'ĝ'] = 69,
  [L'Ğ'] = 69,    [L'ğ'] = 69,
  [L'Ģ'] = 69,

  [L'H'] = 68,    [L'h'] = 68,
  [L'Ĥ'] = 68,    [L'ĥ'] = 68,
  [L'Ħ'] = 68,    [L'ħ'] = 68,
/*[L'CH'] = 67,   [L'ch'] = 67,   Space for Czech double-letter CH and ch. */
  [L'I'] = 66,    [L'ı'] = 66,
  [L'İ'] = 66,    [L'i'] = 66,
  [L'Ī'] = 66,    [L'ī'] = 66,
  [L'Í'] = 66,    [L'í'] = 66,
  [L'Ì'] = 66,    [L'ì'] = 66,
  [L'Î'] = 66,    [L'î'] = 66,
  [L'Ĩ'] = 66,    [L'ĩ'] = 66,
  [L'Ĭ'] = 66,    [L'ĭ'] = 66,
  [L'Ï'] = 66,    [L'ï'] = 66,
  [L'Į'] = 66,    [L'į'] = 66,

  [L'J'] = 65,    [L'j'] = 65,
  [L'Ĵ'] = 65,    [L'ĵ'] = 65,
  [L'K'] = 64,    [L'k'] = 64,
  [L'Ķ'] = 64,    [L'ķ'] = 64,

  [L'L'] = 63,    [L'l'] = 63,
  [L'Ĺ'] = 63,    [L'ĺ'] = 63,
  [L'Ļ'] = 63,    [L'ļ'] = 63,

  [L'Ľ'] = 63,    [L'ľ'] = 63,
  [L'Ł'] = 63,    [L'ł'] = 63,
  [L'M'] = 62,    [L'm'] = 62,
  [L'N'] = 61,    [L'n'] = 61,
  [L'Ń'] = 61,    [L'ń'] = 61,
  [L'Ň'] = 61,    [L'ň'] = 61,
  [L'Ñ'] = 61,    [L'ñ'] = 61,
  [L'Ņ'] = 61,    [L'ņ'] = 61,

  [L'O'] = 60,    [L'o'] = 60,
  [L'Ō'] = 60,    [L'ō'] = 60,
  [L'Ó'] = 60,    [L'ó'] = 60,
  [L'Ò'] = 60,    [L'ò'] = 60,
  [L'Ô'] = 60,    [L'ô'] = 60,
  [L'Õ'] = 60,    [L'õ'] = 60,
  [L'Ŏ'] = 60,    [L'ŏ'] = 60,
  [L'Ö'] = 60,    [L'ö'] = 60,
  [L'Ő'] = 60,    [L'ő'] = 60,
  [L'Ø'] = 60,    [L'ø'] = 60,
  [L'P'] = 59,    [L'p'] = 59,
  [L'Q'] = 58,    [L'q'] = 58,
  [L'R'] = 57,    [L'r'] = 57,
  [L'Ŕ'] = 57,    [L'ŕ'] = 57,
  [L'Ŗ'] = 57,    [L'ŗ'] = 57,

  [L'Ř'] = 56,    [L'ř'] = 56,
  [L'S'] = 55,    [L's'] = 55,
  [L'Ś'] = 55,    [L'ś'] = 55,
  [L'Ŝ'] = 55,    [L'ŝ'] = 55,
  [L'Ş'] = 55,    [L'ş'] = 55,

  [L'Š'] = 54,    [L'š'] = 54,
  [L'T'] = 53,    [L't'] = 53,
  [L'Ť'] = 53,    [L'ť'] = 53,
  [L'Ţ'] = 53,    [L'ţ'] = 53,
  [L'Ŧ'] = 53,    [L'ŧ'] = 53,
  [L'U'] = 52,    [L'u'] = 52,
  [L'Ū'] = 52,    [L'ū'] = 52,
  [L'Ú'] = 52,    [L'ú'] = 52,
  [L'Ù'] = 52,    [L'ù'] = 52,
  [L'Û'] = 52,    [L'û'] = 52,
  [L'Ũ'] = 52,    [L'ũ'] = 52,
  [L'Ŭ'] = 52,    [L'ŭ'] = 52,
  [L'Ů'] = 52,    [L'ů'] = 52,
  [L'Ü'] = 52,    [L'ü'] = 52,
  [L'Ű'] = 52,    [L'ű'] = 52,
  [L'Ų'] = 52,    [L'ų'] = 52,
  [L'V'] = 51,    [L'v'] = 51,
  [L'W'] = 50,    [L'w'] = 50,
  [L'Ŵ'] = 50,    [L'ŵ'] = 50,
  [L'X'] = 49,    [L'x'] = 49,
  [L'Y'] = 48,    [L'y'] = 48,
  [L'Ý'] = 48,    [L'ý'] = 48,
  [L'Ŷ'] = 48,    [L'ŷ'] = 48,
  [L'Ÿ'] = 48,    [L'ÿ'] = 48,
  [L'Z'] = 47,    [L'z'] = 47,
  [L'Ż'] = 47,    [L'ż'] = 47,
  [L'Ź'] = 47,    [L'ź'] = 47,
  [L'Ž'] = 46,    [L'ž'] = 46,
  [L'0'] = 45,
  [L'1'] = 44,
  [L'2'] = 43,
  [L'3'] = 42,
  [L'4'] = 41,
  [L'5'] = 40,
  [L'6'] = 39,
  [L'7'] = 38,
  [L'8'] = 37,
  [L'9'] = 36,
  [L'\''] = 35,
  [L'.'] = 34,
  [L','] = 33,
  [L';'] = 32,
  [L'?'] = 31,
  [L'!'] = 30,
  [L':'] = 29,
  [L'"'] = 28,
  [L'-'] = 27,
  [L'|'] = 26,
  [L'/'] = 25,
  [L'\\'] = 24,
  [L'('] = 23,
  [L')'] = 22,
/*[L'//'] = 21,   Space for double-letter //. */
  [L'['] = 20,
  [L']'] = 19,
  [L'<'] = 18,
  [L'>'] = 17,
  [L'{'] = 16,
  [L'}'] = 15,
  [L'&'] = 14,
  [L'£'] = 13,
  [L'§'] = 12,
  [L'%'] = 11,
  [L'‰'] = 10,
  [L'$'] = 9,
  [L'='] = 8,
  [L'+'] = 7,
  [L'×'] = 6,
  [L'*'] = 5,
  [L'#'] = 4,
  [L'~'] = 3,
/*[L'~~'] = 2,    Space for double-letter ~~. */
/*[L'~='] = 1,    Space for double-letter ~=. */
};


/*
 * Array containing values for sub ordering of the characters comparison
 * (2nd passage).
 */
static const wint_t const cmp_set_sub[] = {
  [L'\n'] = 161,
  [L' '] = 160,
  [L'\u00A0'] = 160,
  [L'\t'] = 159,

  [L'A'] = 158,   [L'a'] = 158,
  [L'Ā'] = 157,   [L'ā'] = 157,
  [L'Á'] = 156,   [L'á'] = 156,
  [L'À'] = 155,   [L'à'] = 155,
  [L'Â'] = 154,   [L'â'] = 154,
  [L'Ã'] = 153,   [L'ã'] = 153,
  [L'Ă'] = 152,   [L'ă'] = 152,
  [L'Å'] = 151,   [L'å'] = 151,
  [L'Ä'] = 150,   [L'ä'] = 150,
  [L'Ą'] = 149,   [L'ą'] = 149,

  [L'B'] = 148,   [L'b'] = 148,
  [L'C'] = 147,   [L'c'] = 147,
  [L'Ċ'] = 146,   [L'ċ'] = 146,
  [L'Ć'] = 145,   [L'ć'] = 145,
  [L'Ĉ'] = 144,   [L'ĉ'] = 144,
  [L'Ç'] = 143,   [L'ç'] = 143,

  [L'Č'] = 142,   [L'č'] = 142,
  [L'D'] = 141,   [L'd'] = 141,
  [L'Ď'] = 140,   [L'ď'] = 140,
  [L'Đ'] = 139,   [L'đ'] = 139,
  [L'E'] = 138,   [L'e'] = 138,
  [L'Ė'] = 137,   [L'ė'] = 137,
  [L'Ē'] = 136,   [L'ē'] = 136,
  [L'É'] = 135,   [L'é'] = 135,
  [L'Ê'] = 134,   [L'ê'] = 134,
  [L'Ě'] = 133,   [L'ě'] = 133,
  [L'Ĕ'] = 132,   [L'ĕ'] = 132,
  [L'Ë'] = 131,   [L'ë'] = 131,
  [L'Ę'] = 130,   [L'ę'] = 130,

  [L'F'] = 129,   [L'f'] = 129,
  [L'G'] = 128,   [L'g'] = 128,
  [L'Ġ'] = 127,   [L'ġ'] = 127,
                  [L'ģ'] = 126,

  [L'Ĝ'] = 125,   [L'ĝ'] = 125,
  [L'Ğ'] = 124,   [L'ğ'] = 124,
  [L'Ģ'] = 123,

  [L'H'] = 122,   [L'h'] = 122,
  [L'Ĥ'] = 121,   [L'ĥ'] = 121,
  [L'Ħ'] = 120,   [L'ħ'] = 120,
/*[L'CH'] = 119,  [L'ch'] = 119,   Space for Czech double-letter CH and ch. */
  [L'I'] = 118,   [L'i'] = 118,
  [L'İ'] = 117,   [L'ı'] = 117,
  [L'Ī'] = 116,   [L'ī'] = 116,
  [L'Í'] = 115,   [L'í'] = 115,
  [L'Ì'] = 114,   [L'ì'] = 114,
  [L'Î'] = 113,   [L'î'] = 113,
  [L'Ĩ'] = 112,   [L'ĩ'] = 112,
  [L'Ĭ'] = 111,   [L'ĭ'] = 111,
  [L'Ï'] = 110,   [L'ï'] = 110,
  [L'Į'] = 109,   [L'į'] = 109,

  [L'J'] = 108,   [L'j'] = 108,
  [L'Ĵ'] = 107,   [L'ĵ'] = 107,
  [L'K'] = 106,   [L'k'] = 106,
  [L'Ķ'] = 105,   [L'ķ'] = 105,

  [L'L'] = 104,   [L'l'] = 104,
  [L'Ĺ'] = 103,   [L'ĺ'] = 103,
  [L'Ļ'] = 102,   [L'ļ'] = 102,

  [L'Ľ'] = 101,   [L'ľ'] = 101,
  [L'Ł'] = 100,   [L'ł'] = 100,
  [L'M'] = 99,    [L'm'] = 99,
  [L'N'] = 98,    [L'n'] = 98,
  [L'Ń'] = 97,    [L'ń'] = 97,
  [L'Ň'] = 96,    [L'ň'] = 96,
  [L'Ñ'] = 95,    [L'ñ'] = 95,
  [L'Ņ'] = 94,    [L'ņ'] = 94,

  [L'O'] = 93,    [L'o'] = 93,
  [L'Ō'] = 92,    [L'ō'] = 92,
  [L'Ó'] = 91,    [L'ó'] = 91,
  [L'Ò'] = 90,    [L'ò'] = 90,
  [L'Ô'] = 89,    [L'ô'] = 89,
  [L'Õ'] = 88,    [L'õ'] = 88,
  [L'Ŏ'] = 87,    [L'ŏ'] = 87,
  [L'Ö'] = 86,    [L'ö'] = 86,
  [L'Ő'] = 85,    [L'ő'] = 85,
  [L'Ø'] = 84,    [L'ø'] = 84,
  [L'P'] = 83,    [L'p'] = 83,
  [L'Q'] = 82,    [L'q'] = 82,
  [L'R'] = 81,    [L'r'] = 81,
  [L'Ŕ'] = 80,    [L'ŕ'] = 80,
  [L'Ŗ'] = 79,    [L'ŗ'] = 79,

  [L'Ř'] = 78,    [L'ř'] = 78,
  [L'S'] = 77,    [L's'] = 77,
  [L'Ś'] = 76,    [L'ś'] = 76,
  [L'Ŝ'] = 75,    [L'ŝ'] = 75,
  [L'Ş'] = 74,    [L'ş'] = 74,

  [L'Š'] = 73,    [L'š'] = 73,
  [L'T'] = 72,    [L't'] = 72,
  [L'Ť'] = 71,    [L'ť'] = 71,
  [L'Ţ'] = 70,    [L'ţ'] = 70,
  [L'Ŧ'] = 69,    [L'ŧ'] = 69,
  [L'U'] = 68,    [L'u'] = 68,
  [L'Ū'] = 67,    [L'ū'] = 67,
  [L'Ú'] = 66,    [L'ú'] = 66,
  [L'Ù'] = 65,    [L'ù'] = 65,
  [L'Û'] = 64,    [L'û'] = 64,
  [L'Ũ'] = 63,    [L'ũ'] = 63,
  [L'Ŭ'] = 62,    [L'ŭ'] = 62,
  [L'Ů'] = 61,    [L'ů'] = 61,
  [L'Ü'] = 60,    [L'ü'] = 60,
  [L'Ű'] = 59,    [L'ű'] = 59,
  [L'Ų'] = 58,    [L'ų'] = 58,
  [L'V'] = 57,    [L'v'] = 57,
  [L'W'] = 56,    [L'w'] = 56,
  [L'Ŵ'] = 55,    [L'ŵ'] = 55,
  [L'X'] = 54,    [L'x'] = 54,
  [L'Y'] = 53,    [L'y'] = 53,
  [L'Ý'] = 52,    [L'ý'] = 52,
  [L'Ŷ'] = 51,    [L'ŷ'] = 51,
  [L'Ÿ'] = 50,    [L'ÿ'] = 50,
  [L'Z'] = 49,    [L'z'] = 49,
  [L'Ż'] = 48,    [L'ż'] = 48,
  [L'Ź'] = 47,    [L'ź'] = 47,
  [L'Ž'] = 46,    [L'ž'] = 46,
  [L'0'] = 45,
  [L'1'] = 44,
  [L'2'] = 43,
  [L'3'] = 42,
  [L'4'] = 41,
  [L'5'] = 40,
  [L'6'] = 39,
  [L'7'] = 38,
  [L'8'] = 37,
  [L'9'] = 36,
  [L'\''] = 35,
  [L'.'] = 34,
  [L','] = 33,
  [L';'] = 32,
  [L'?'] = 31,
  [L'!'] = 30,
  [L':'] = 29,
  [L'"'] = 28,
  [L'-'] = 27,
  [L'|'] = 26,
  [L'/'] = 25,
  [L'\\'] = 24,
  [L'('] = 23,
  [L')'] = 22,
/*[L'//'] = 21,   Space for double-letter //. */
  [L'['] = 20,
  [L']'] = 19,
  [L'<'] = 18,
  [L'>'] = 17,
  [L'{'] = 16,
  [L'}'] = 15,
  [L'&'] = 14,
  [L'£'] = 13,
  [L'§'] = 12,
  [L'%'] = 11,
  [L'‰'] = 10,
  [L'$'] = 9,
  [L'='] = 8,
  [L'+'] = 7,
  [L'×'] = 6,
  [L'*'] = 5,
  [L'#'] = 4,
  [L'~'] = 3,
/*[L'~~'] = 2,    Space for double-letter ~~. */
/*[L'~='] = 1,    Space for double-letter ~=. */
};


/*
 * Array containing values for capital ordering of the characters comparison
 * (3rd passage).
 */
static const wint_t const cmp_set_cap[] = {
  [L'\n'] = 110,
  [L' '] = 109,
  [L'\u00A0'] = 109,
  [L'\t'] = 108,

  [L'A'] = 106,   [L'a'] = 107,
  [L'Ā'] = 106,   [L'ā'] = 107,
  [L'Á'] = 106,   [L'á'] = 107,
  [L'À'] = 106,   [L'à'] = 107,
  [L'Â'] = 106,   [L'â'] = 107,
  [L'Ã'] = 106,   [L'ã'] = 107,
  [L'Ă'] = 106,   [L'ă'] = 107,
  [L'Å'] = 106,   [L'å'] = 107,
  [L'Ä'] = 106,   [L'ä'] = 107,
  [L'Ą'] = 106,   [L'ą'] = 107,

  [L'B'] = 104,   [L'b'] = 105,
  [L'C'] = 102,   [L'c'] = 103,
  [L'Ċ'] = 102,   [L'ċ'] = 103,
  [L'Ć'] = 102,   [L'ć'] = 103,
  [L'Ĉ'] = 102,   [L'ĉ'] = 103,
  [L'Ç'] = 102,   [L'ç'] = 103,

  [L'Č'] = 100,   [L'č'] = 101,
  [L'D'] = 98,    [L'd'] = 99,
  [L'Ď'] = 98,    [L'ď'] = 99,
  [L'Đ'] = 98,    [L'đ'] = 99,
  [L'E'] = 96,    [L'e'] = 97,
  [L'Ė'] = 96,    [L'ė'] = 97,
  [L'Ē'] = 96,    [L'ē'] = 97,
  [L'É'] = 96,    [L'é'] = 97,
  [L'Ê'] = 96,    [L'ê'] = 97,
  [L'Ě'] = 96,    [L'ě'] = 97,
  [L'Ĕ'] = 96,    [L'ĕ'] = 97,
  [L'Ë'] = 96,    [L'ë'] = 97,
  [L'Ę'] = 96,    [L'ę'] = 97,

  [L'F'] = 94,    [L'f'] = 95,
  [L'G'] = 92,    [L'g'] = 93,
  [L'Ġ'] = 92,    [L'ġ'] = 93,
                  [L'ģ'] = 93,

  [L'Ĝ'] = 92,    [L'ĝ'] = 93,
  [L'Ğ'] = 92,    [L'ğ'] = 93,
  [L'Ģ'] = 92,

  [L'H'] = 90,    [L'h'] = 91,
  [L'Ĥ'] = 90,    [L'ĥ'] = 91,
  [L'Ħ'] = 90,    [L'ħ'] = 91,
/*[L'CH'] = 88,   [L'ch'] = 89,   Space for Czech double-letter CH and ch. */
  [L'I'] = 86,    [L'ı'] = 87,
  [L'İ'] = 86,    [L'i'] = 87,
  [L'Ī'] = 86,    [L'ī'] = 87,
  [L'Í'] = 86,    [L'í'] = 87,
  [L'Ì'] = 86,    [L'ì'] = 87,
  [L'Î'] = 86,    [L'î'] = 87,
  [L'Ĩ'] = 86,    [L'ĩ'] = 87,
  [L'Ĭ'] = 86,    [L'ĭ'] = 87,
  [L'Ï'] = 86,    [L'ï'] = 87,
  [L'Į'] = 86,    [L'į'] = 87,

  [L'J'] = 84,    [L'j'] = 85,
  [L'Ĵ'] = 84,    [L'ĵ'] = 85,
  [L'K'] = 82,    [L'k'] = 83,
  [L'Ķ'] = 82,    [L'ķ'] = 83,

  [L'L'] = 80,    [L'l'] = 81,
  [L'Ĺ'] = 80,    [L'ĺ'] = 81,
  [L'Ļ'] = 80,    [L'ļ'] = 81,

  [L'Ľ'] = 80,    [L'ľ'] = 81,
  [L'Ł'] = 80,    [L'ł'] = 81,
  [L'M'] = 78,    [L'm'] = 79,
  [L'N'] = 76,    [L'n'] = 77,
  [L'Ń'] = 76,    [L'ń'] = 77,
  [L'Ň'] = 76,    [L'ň'] = 77,
  [L'Ñ'] = 76,    [L'ñ'] = 77,
  [L'Ņ'] = 76,    [L'ņ'] = 77,

  [L'O'] = 74,    [L'o'] = 75,
  [L'Ō'] = 74,    [L'ō'] = 75,
  [L'Ó'] = 74,    [L'ó'] = 75,
  [L'Ò'] = 74,    [L'ò'] = 75,
  [L'Ô'] = 74,    [L'ô'] = 75,
  [L'Õ'] = 74,    [L'õ'] = 75,
  [L'Ŏ'] = 74,    [L'ŏ'] = 75,
  [L'Ö'] = 74,    [L'ö'] = 75,
  [L'Ő'] = 74,    [L'ő'] = 75,
  [L'Ø'] = 74,    [L'ø'] = 75,
  [L'P'] = 72,    [L'p'] = 73,
  [L'Q'] = 70,    [L'q'] = 71,
  [L'R'] = 68,    [L'r'] = 69,
  [L'Ŕ'] = 68,    [L'ŕ'] = 69,
  [L'Ŗ'] = 68,    [L'ŗ'] = 69,

  [L'Ř'] = 66,    [L'ř'] = 67,
  [L'S'] = 64,    [L's'] = 65,
  [L'Ś'] = 64,    [L'ś'] = 65,
  [L'Ŝ'] = 64,    [L'ŝ'] = 65,
  [L'Ş'] = 64,    [L'ş'] = 65,

  [L'Š'] = 62,    [L'š'] = 63,
  [L'T'] = 60,    [L't'] = 61,
  [L'Ť'] = 60,    [L'ť'] = 61,
  [L'Ţ'] = 60,    [L'ţ'] = 61,
  [L'Ŧ'] = 60,    [L'ŧ'] = 61,
  [L'U'] = 58,    [L'u'] = 59,
  [L'Ū'] = 58,    [L'ū'] = 59,
  [L'Ú'] = 58,    [L'ú'] = 59,
  [L'Ù'] = 58,    [L'ù'] = 59,
  [L'Û'] = 58,    [L'û'] = 59,
  [L'Ũ'] = 58,    [L'ũ'] = 59,
  [L'Ŭ'] = 58,    [L'ŭ'] = 59,
  [L'Ů'] = 58,    [L'ů'] = 59,
  [L'Ü'] = 58,    [L'ü'] = 59,
  [L'Ű'] = 58,    [L'ű'] = 59,
  [L'Ų'] = 58,    [L'ų'] = 59,
  [L'V'] = 56,    [L'v'] = 57,
  [L'W'] = 54,    [L'w'] = 55,
  [L'Ŵ'] = 54,    [L'ŵ'] = 55,
  [L'X'] = 52,    [L'x'] = 53,
  [L'Y'] = 50,    [L'y'] = 51,
  [L'Ý'] = 50,    [L'ý'] = 51,
  [L'Ŷ'] = 50,    [L'ŷ'] = 51,
  [L'Ÿ'] = 50,    [L'ÿ'] = 51,
  [L'Z'] = 48,    [L'z'] = 49,
  [L'Ż'] = 48,    [L'ż'] = 49,
  [L'Ź'] = 48,    [L'ź'] = 49,
  [L'Ž'] = 46,    [L'ž'] = 47,
  [L'0'] = 45,
  [L'1'] = 44,
  [L'2'] = 43,
  [L'3'] = 42,
  [L'4'] = 41,
  [L'5'] = 40,
  [L'6'] = 39,
  [L'7'] = 38,
  [L'8'] = 37,
  [L'9'] = 36,
  [L'\''] = 35,
  [L'.'] = 34,
  [L','] = 33,
  [L';'] = 32,
  [L'?'] = 31,
  [L'!'] = 30,
  [L':'] = 29,
  [L'"'] = 28,
  [L'-'] = 27,
  [L'|'] = 26,
  [L'/'] = 25,
  [L'\\'] = 24,
  [L'('] = 23,
  [L')'] = 22,
/*[L'//'] = 21,   Space for double-letter //. */
  [L'['] = 20,
  [L']'] = 19,
  [L'<'] = 18,
  [L'>'] = 17,
  [L'{'] = 16,
  [L'}'] = 15,
  [L'&'] = 14,
  [L'£'] = 13,
  [L'§'] = 12,
  [L'%'] = 11,
  [L'‰'] = 10,
  [L'$'] = 9,
  [L'='] = 8,
  [L'+'] = 7,
  [L'×'] = 6,
  [L'*'] = 5,
  [L'#'] = 4,
  [L'~'] = 3,
/*[L'~~'] = 2,    Space for double-letter ~~. */
/*[L'~='] = 1,    Space for double-letter ~=. */
};

/******************************************************************************
 ~~~[ FUNCTIONAL PROTOTYPES OF LOCAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

static inline int get_defval(const wchar_t **p_wstr);
static inline int get_subval(const wchar_t *p_wstr, wint_t wch_val);
static inline int get_capval(const wchar_t *p_wstr, wint_t wch_val);


/******************************************************************************
 ~~~[ GLOBAL FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Same function as standard wcscoll() function, however this function compares
 * more correctly compared to Czech CSN_97_6030 norm. Also doesn't fail
 * (Segmentation fault) if one of the given wide strings is NULL. (In this case
 * the errno is set to EINVAL). Function doesn't fail if any of the strings
 * character is with its value outside of the comparison tables. In this case
 * this character has the highest sorting value.
 */
int wcscoll_cz(const wchar_t *ws1, const wchar_t *ws2)
{{{
  errno = EINVAL;

  /* Test for correct strings to prevent Segmentation fault. */
  if (ws1 == NULL && ws2 == NULL)
    return 0;
  else if (ws1 == NULL)
    return -1;
  else if (ws2 == NULL)
    return 1;

  errno = 0;                    /* Reseting errno. */

  /* Auxiliary strings. The given strings can't be changed!!! */
  const wchar_t *wstr1 = ws1;
  const wchar_t *wstr2 = ws2;

  /* Auxiliary variables containing sorting values of the characters. */
  int wch1_val;
  int wch2_val;

  /* Another auxiliary variables for the sub and capital ordering. */
  int wch1_auxval;
  int wch2_auxval;

  /* Auxiliary variables representing result of sub and capital ordering. */
  int sub_cmp = 0;
  int cap_cmp = 0;


  /*
   * Compare characters to each other until difference is found or the end of
   * any string is reached.
   */
  while (*wstr1 != L'\0' && *wstr2 != L'\0') {

    /* Get the characters values for the default sorting. */
    wch1_val = get_defval(&wstr1);
    wch2_val = get_defval(&wstr2);

    /* Different characters? */
    if (wch1_val != wch2_val) {
      
      /* Which one has higher sorting value? */
      if ((wch1_val - wch2_val) > 0)
        return -1;
      else
        return 1;

    }
    /* Same characters. Sub ordering testing? */
    else if (sub_cmp == 0) {
    
      /* Getting values for the sub ordering. */
      wch1_auxval = get_subval(wstr1, wch1_val);
      wch2_auxval = get_subval(wstr2, wch2_val);

      /* Same characters? */
      if (wch1_auxval == wch2_auxval) {

        /* Capital ordering? */
        if (cap_cmp == 0) {
          
          /* Getting values for the capital ordering. */
          wch1_auxval = get_capval(wstr1, wch1_val);
          wch2_auxval = get_capval(wstr2, wch2_val);

          /* Different characters? */
          if (wch1_auxval != wch2_auxval) {
            
            /* Which one has higher sorting value? */
            if ((wch1_auxval - wch2_auxval) > 0)
              cap_cmp = -1;
            else
              cap_cmp = 1;

          }
        }

      }
      /* Different characters. */
      else {
        
        /* Which one has the higher sorting value? */
        if ((wch1_auxval - wch2_auxval) > 0)
          sub_cmp = -1;
        else
          sub_cmp = 1;
      }

    }
    
    /* Pointers shifting to next characters. */
    wstr1++;
    wstr2++;
  }

  /* End of cycle. Different last characters? */
  if (*wstr1 != *wstr2) {
    
    /* Which string is shorter? */
    if (*wstr1 == L'\0')
      return -1;
    else
      return 1;
  }
  
  /* 
   * Returns result of sub ordering, if it was successful, otherwise returns
   * result of capital ordering.
   */
  return (sub_cmp != 0) ? sub_cmp : cap_cmp;
}}}

/******************************************************************************
 ~~~[ AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ******************************************************************************/

/**
 * Auxiliary function which returns the sorting value of the character. Also
 * merges the horizontal tab and spaces for the comparison purpose.
 */
static inline int get_defval(const wchar_t **p_wstr)
{{{
  /* Character outside the comparison table? */
  if (**p_wstr > TABLE_MAX)
    return 0;

  wint_t ret_val;

  ret_val = cmp_set_def[**p_wstr];    /* Accessing the comparison value. */

  /* Testing for double characters, which are taken as one. */
  switch (ret_val) {

    /* ~= or ~~ ? */
    case 3 :
      if (*(*p_wstr + 1) == L'=') {
        p_wstr++;
        return 1;
      }
      else if (*(*p_wstr + 1) == L'~') {
        p_wstr++;
        return 2;
      }

      break;

    /* // ? */
    case 25 :
      if (*(*p_wstr + 1) == L'/') {
        p_wstr++;
        return 21;
      }

      break;

    /* Czech double letter CH? */
    case 74 :
      if (*(*p_wstr + 1) == L'H' || *(*p_wstr + 1) == L'h') {
        (*p_wstr)++;
        return 67;
      }

      break;

    /* Horizontal tab "merging". */
    case 78 :
      while (*(*p_wstr + 1) == L'\t')
        (*p_wstr)++;

      break;

    /* Spaces or non-breakable spaces "merging". */
    case 79 :
      while (*(*p_wstr + 1) == L' ' || *(*p_wstr + 1) == L'\u00A0')
        (*p_wstr)++;

      break;

    /* None of above. */
    default :
      break;
  }

  return ret_val;     /* Return of the comparison value of the character. */
}}}


/**
 * Auxiliary function which returns the sub ordering value of the character.
 * Also uses value computed by get_defval() function for easier comparison value
 * determining.
 */
static inline int get_subval(const wchar_t *p_wstr, wint_t wch_val)
{{{
  /* Same value for all tables? */
  if (wch_val < 46)
    return wch_val;
  /* Czech CH letter? */
  else if (wch_val == 67)
    return 119;
  
  return cmp_set_sub[*p_wstr];    /* Return of new comparison value. */
}}}


/**
 * Auxiliary function which returns the capital ordering value of the character.
 * Also uses value computed by get_defval() function for easier comparison value
 * determining.
 */
static inline int get_capval(const wchar_t *p_wstr, wint_t wch_val)
{{{
  /* Same value for all tables? */
  if (wch_val < 46)
    return wch_val;
  /* Czech CH letter? */
  else if (wch_val == 67) {

    /* Big or small letter? */
    if (*(p_wstr - 1) == L'C')
      return 88;
    else
      return 89;
  }
  
  return cmp_set_cap[*p_wstr];    /* Return of new comparison value. */
}}}

/******************************************************************************
 ***[ END OF PROJ4_WCIO.C ]****************************************************
 ******************************************************************************/
 
