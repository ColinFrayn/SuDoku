// Solve.cpp : The Solver functions

#include "stdafx.h"
#include "common.h"
#include "SuDoku.h"
#include "Reduce.h"
#include "Solve.h"
#include "Explain.h"
#include "resource.h"

// Global Variables:
extern int Board[9][9], History[9][9][1024], iHistoryPos, iHistoryMax;
extern bool Solved[9][9], Poss[9][9][9];
extern HMENU g_hMenu;

// Run a logical step
// Work out if there are any squares with only one possible option
int RunStep(bool bSilent) {
  int x,y,n,val,count,tally=0;

  // Check that the permission matrix is legal
  if (CheckIllegal() == true) return 0;

  // Do the logical step
  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      count=0;
      for (n=0;n<9;n++) {
        if (Poss[x][y][n] == true) {count++;val=n+1;}
        if (count>1) break;
      }
      if (count == 1 && !Solved[x][y]) {
        if (!bSilent && Board[x][y] == 0) Explain(x,y,val-1,"Only one possibility");
        Board[x][y] = val;tally++;
      }
    }
  }
  return tally;
}

// Run a box step
// For each 3*3 box, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
int BoxStep(bool bSilent) {
  int x,y,xx,yy,bx,by,count[9],n,tally=0;

  // Check that the permission matrix is legal
  if (CheckIllegal() == true) return 0;

  // Loop through all nine 3*3 boxes
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      // Reset the count
      for (n=0;n<9;n++) count[n]=0;
      // Loop through this box, tallying the possible values
      for (yy=0;yy<3;yy++) {
        y = by*3 + yy;
        for (xx=0;xx<3;xx++) {
          x = bx*3 + xx;
          if (Board[x][y] != 0) continue;
          for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
        }
      }
      // Now check if there are any 'only possible solutions'
      for (n=0;n<9;n++) {
        if (count[n] == 1) {
          // This one has only one possible solution, so fill it in
          for (yy=0;yy<3;yy++) {
            y = by*3 + yy;
            for (xx=0;xx<3;xx++) {
              x = bx*3 + xx;
              if (Poss[x][y][n] && !Solved[x][y]) {
                if (!bSilent && Board[x][y] == 0) Explain(x,y,n,"Only one location in this box");
                Board[x][y] = n+1;tally++;
              }
            }
          }
        }
      }
    }
  }
  return tally;
}

// Run a row step
// For each row, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
int RowStep(bool bSilent) {
  int x,y,count[9],n,tally = 0;

  // Check that the permission matrix is legal
  if (CheckIllegal() == true) return 0;

  // Loop through all nine rows
  for (y=0;y<9;y++) {
    // Reset the count
    for (n=0;n<9;n++) count[n]=0;
    // Loop through this row, tallying the possible values
    for (x=0;x<9;x++) {
      if (Board[x][y] != 0) continue;
      for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
    }
    // Now check if there are any 'only possible solutions'
    for (n=0;n<9;n++) {
      if (count[n] == 1) {
        for (x=0;x<9;x++) {
          // This one has only one possible solution, so fill it in
          if (Poss[x][y][n] && !Solved[x][y]) {
            if (!bSilent && Board[x][y] == 0) Explain(x,y,n,"Only one location in this row");
            Board[x][y] = n+1;tally++;
          }
        }
      }
    }
  }
  return tally;
}

// Run a column step
// For each column, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
int ColumnStep(bool bSilent) {
  int x,y,count[9],n,tally=0;

  // Check that the permission matrix is legal
  if (CheckIllegal() == true) return 0;

  // Loop through all nine columns
  for (x=0;x<9;x++) {
    // Reset the count
    for (n=0;n<9;n++) count[n]=0;
    // Loop through this column, tallying the possible values
    for (y=0;y<9;y++) {
      if (Board[x][y] != 0) continue;
      for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
    }
    // Now check if there are any 'only possible solutions'
    for (n=0;n<9;n++) {
      if (count[n] == 1) {
        for (y=0;y<9;y++) {
          // This one has only one possible solution, so fill it in
          if (Poss[x][y][n] && !Solved[x][y]) {
            if (!bSilent && Board[x][y] == 0) Explain(x,y,n,"Only one location in this column");
            Board[x][y] = n+1;tally++;
          }
        }
      }
    }
  }
  return tally;
}

// Accept proposed update changes made by one of the algorithms
void AcceptChanges(void) {
  int x,y;
  for (y=0;y<9;y++) for (x=0;x<9;x++) {
    if (Solved[x][y]) History[x][y][iHistoryPos] = Board[x][y];
    else History[x][y][iHistoryPos] = 0;
  }
  iHistoryPos++; iHistoryMax = iHistoryPos;
  for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0) Solved[x][y] = true;
  EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
  EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_ENABLED);
  SetupPermissionMatrix(false);
}

