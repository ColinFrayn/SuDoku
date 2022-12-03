/*------------------- 
 * Project : SuDoku
 * File    : common.h
 *-------------------*/

/* This file contains the universal defines.
 */

#ifndef COMMON_H
#define COMMON_H

// Random defines
#define Random(a)   ((a) == (0) ? (0) : (int)((double)rand() / ((double)RAND_MAX + 1) * (a)))
#define Randomise() (srand((unsigned int)time(NULL)))

// Evolutionary algorithm definitions
#define EntityCount (1<<12)
#define EliteCount  (3)
#define Survivors   (EntityCount / 3)
#define Mutants     (EntityCount / 3)

// GUI definitions
#define WINDOW_WIDTH   (400)
#define WINDOW_HEIGHT  (400)

// Colours
#define C_BLACK    RGB(0,0,0)
#define C_RED      RGB(255,0,0)
#define C_GREEN    RGB(0,255,0)
#define C_SMALL_COLOUR RGB(0,0,255)  // Colour of the small numbers for the permissions.
#define C_BLUE     RGB(0,0,255)
#define C_WHITE    RGB(255,255,255)
#define C_GREY     RGB(170,170,170)
#define C_HIGHLIGHT RGB(200,200,200)

// GUI parameters
#define BORDERX      (10)
#define BORDERY      (10)
#define FRAME_BORDER (10)

// File formats
#define F_TEXT       (0)
#define F_POSTSCRIPT (1)
#define F_HTML       (2)

// Difficulty Levels
#define D_EASY      (0)
#define D_MILD      (1)
#define D_MEDIUM    (2)
#define D_HARD      (3)
#define D_EXPERT    (4)
#define D_FIENDISH  (5)
#define D_NIGHTMARE (6)
#define D_ILLEGAL   (7)

// Code for text update signal
#define MESSAGE_UPDATE    (55555)

#endif // COMMON_H
