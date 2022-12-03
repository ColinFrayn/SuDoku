/********************************
 *    Solve.h                   *
 *    Colin Frayn               *
 *    May 2005                  *
 ********************************/

/*
  Contains the specific defines for Solve.cpp
 */

#ifndef SOLVE_H
#define SOLVE_H

int  RunStep(bool);
int  BoxStep(bool);
int  ColumnStep(bool);
int  RowStep(bool);
void AcceptChanges(void);

#endif  // SOLVE_H
