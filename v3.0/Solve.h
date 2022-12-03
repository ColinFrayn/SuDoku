/********************************
 *    Solve.h                   *
 *    Colin Frayn               *
 *    May 2005                  *
 ********************************/

/*
  Contains the specific defines for SuDoku.cpp
 */

#ifndef SOLVE_H
#define SOLVE_H

int  RunStep(void);
int  BoxStep(void);
int  ColumnStep(void);
int  RowStep(void);
void AcceptChanges(void);

#endif  // SOLVE_H
