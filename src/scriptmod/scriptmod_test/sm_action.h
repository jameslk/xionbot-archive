/*
COPYRIGHT limefire/shocktart/SeLfkiLL/Jaleko/f0rk/James/etc. 2004
www.jaleko.tk
Licensed under the GPL license.
http://www.gnu.org/licenses/gpl.txt
*/

/* (1) sm_syntax.h */

#define ACT_IGNORE 1;
#define ACT_PUT 2;
#define ACT_PUSH 3;
#define ACT_ERROR 4;

typedef struct actevent_tbl {
    int start[4];
    int number[4];
    int alpha[4];
    int symbol[4];
    int white[4];
    int end[4];
    int error[4];
} actev_tb;

actev_tb actev;
