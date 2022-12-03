/********************************
 *    Explain.h                 *
 *    Colin Frayn               *
 *    Aug 2005                  *
 ********************************/

/*
  Contains the specific defines for Explain.cpp
 */

#ifndef EXPLAIN_H
#define EXPLAIN_H

INT_PTR CALLBACK ExplanationWndProc(HWND, UINT, WPARAM, LPARAM);
void Explain(int,int,int,char *);
void MessageDetails(char *);

#endif // EXPLAIN_H
