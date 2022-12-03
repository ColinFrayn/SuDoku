/********************************
 *    SuDoku.h                  *
 *    Colin Frayn               *
 *    April 2005                *
 ********************************/

/*
  Contains the specific defines for SuDoku.cpp
 */

#ifndef SUDOKU_H
#define SUDOKU_H

INT_PTR CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DisplayBoard(HWND,int [9][9]);
void HighlightBox(HWND,int,int,int,int);
void SelectBox(HWND, LPARAM);
void RunStep(bool);
void BoxStep(void);
void ColumnStep(void);
void RowStep(void);
void LoadSuDoku(void);
void SaveSuDoku(void);
bool GetFileName(char *, bool);
void SetupGUI(HWND);
void SetupInputWindow(void);
INT_PTR CALLBACK InputWndProc(HWND, UINT, WPARAM, LPARAM);

#endif  // SUDOKU_H
