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
#define C_BLUE     RGB(0,0,255)
#define C_WHITE    RGB(255,255,255)
#define C_GREY     RGB(170,170,170)
#define C_HIGHLIGHT RGB(200,200,200)

// GUI parameters
#define BORDERX      (10)
#define BORDERY      (10)
#define FRAME_BORDER (10)

#endif // COMMON_H