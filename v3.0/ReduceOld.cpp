// Recude.cpp : Permission reduction techniques

#include "stdafx.h"
#include "common.h"
#include "Reduce.h"
#include "SuDoku.h"
#include "resource.h"

// Global Variables:
extern HWND hWndMain;
extern int Board[9][9];
extern int BorderX, BorderY,bw,bh;
extern bool Poss[9][9][9], PossBase[9][9][9], Solved[9][9];

// Set up the permission matrix to its default state
void SetupPermissionMatrix(bool bErase) {
  int x,y,bx,by,xx,yy,n;

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Board[x][y] != 0) {
        for (n=0;n<9;n++) Poss[x][y][n] = false;
        Poss[x][y][Board[x][y]-1] = true;
        continue;
      }
      else if (bErase) {
        for (n=0;n<9;n++) Poss[x][y][n] = true;
      }
      // Parse this column
      for (yy=0;yy<9;yy++) {
        if (Board[x][yy] != 0) Poss[x][y][Board[x][yy]-1] = false;
      }
      // Parse this row
      for (xx=0;xx<9;xx++) {
        if (Board[xx][y] != 0) Poss[x][y][Board[xx][y]-1] = false;
      }
      // Parse this block
      bx = x/3;
      by = y/3;
      for (yy=0;yy<3;yy++) {
        for (xx=0;xx<3;xx++) {
          if (Board[bx*3 + xx][by*3 + yy] != 0) Poss[x][y][Board[bx*3 + xx][by*3 + yy]-1] = false;
        }
      }
    }
  }
}

// Set up the base permission matrix to its default state
void SetupBasePermissionMatrix(void) {
  int x,y,bx,by,xx,yy,n;

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Board[x][y] != 0) {
        for (n=0;n<9;n++) PossBase[x][y][n] = false;
        PossBase[x][y][Board[x][y]-1] = true;
        continue;
      }
      else for (n=0;n<9;n++) PossBase[x][y][n] = true;
      // Parse this column
      for (yy=0;yy<9;yy++) if (Board[x][yy] != 0) PossBase[x][y][Board[x][yy]-1] = false;
      // Parse this row
      for (xx=0;xx<9;xx++) if (Board[xx][y] != 0) PossBase[x][y][Board[xx][y]-1] = false;
      // Parse this block
      bx = x/3; by = y/3;
      for (yy=0;yy<3;yy++) {
        for (xx=0;xx<3;xx++) {
          if (Board[bx*3 + xx][by*3 + yy] != 0) PossBase[x][y][Board[bx*3 + xx][by*3 + yy]-1] = false;
        }
      }
    }
  }
}

// Check to see if the permission matrix is currently legal
bool CheckIllegal(void) {
  int x,y,xx,yy,bx,by;
  bool bError = false;

  // Check for illegal positions
  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Solved[x][y]) {
        // Check this column
        for (yy=0;yy<9;yy++) if (yy != y && Solved[x][yy] && Board[x][yy] == Board[x][y]) bError = true;
        // Check this row
        for (xx=0;xx<9;xx++) if (xx != x && Solved[xx][y] && Board[xx][y] == Board[x][y]) bError = true;
        // Check this box
        bx = x/3;
        by = y/3;
        // Loop through this box, checking for illegal values
        for (yy=by*3;yy<(by*3) + 3;yy++) {
          for (xx=bx*3;xx<(bx*3)+3;xx++) {
            if ((xx!=x || yy!=y) && Solved[xx][yy] && Board[xx][yy] == Board[x][y]) bError = true;
          }
        }
      }
    }
  }
  if (bError) {
    MessageBox(hWndMain,"Oh Dear - you seem to have reached an illegal board state!\nAre you sure you typed it in correctly?","Illegal Board State!",MB_OK);
  }
  return bError;
}

// Remove one of the permission values
void RemovePermission(HWND hWnd, int mx, int my) {
  int x,y,xx,yy,bx,by,n,nx,ny;

  // Setup the base permission matrix
  SetupBasePermissionMatrix();

  // Loop through the boxes
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy++;
    if (y>5) yy++;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx++;
      if (x>5) xx++;
      // Draw the text
      if (Board[x][y] == 0) {
        for (by=0;by<3;by++) {
          for (bx=0;bx<3;bx++) {
            n = by*3 + bx;
            if (PossBase[x][y][n]) {
              nx = xx + (bw/4) + ((bx*2 - 1)*bw)/8;
              ny = yy + (bh/11) + (by*(bh+2))/4;
              if (mx >= nx && mx < (nx+(bw/4)) && my >= ny && my < (ny+((bh+2)/4))) {
                if (Poss[x][y][n] == true) Poss[x][y][n] = false;
                else Poss[x][y][n] = true;
              }
            }
          }
        }
      }
    }
  }
}

// Run a constriction step
// If, in any one box, all possible locations for a given number are on the same
// row or column, then delete all other occurrences of those numbers elsewhere in that
// same row or column.
void ConstrictionStep(void) {
  int xx,yy,x,y,bx,by,t,n,col[3],row[3],con;

  // Loop through all boxes
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      // Loop through all numbers
      for (n=0;n<9;n++) {
        col[0] = col[1] = col[2] = 0;
        row[0] = row[1] = row[2] = 0;
        // Loop through all cells and tally the count in each row,column
        for (y=0;y<3;y++) {
          yy = by*3 + y;
          for (x=0;x<3;x++) {
            xx = bx*3 + x;
            // If this cell does not have a number in it
            if (Board[xx][yy] == 0) {
              // Update the column, row counts
              if (Poss[xx][yy][n]) {row[y]++;col[x]++;}
            }
          }
        }
        // Loop through column, row counts. If one of them is > 0 and the other two are 0 then
        // this is a certain placement
        // First try the columns  
        con = -1;
        for (t=0;t<3;t++) {
          if (col[t]>0) {
            if (con == -1) con = t;
            else con = 3;
          }
        }
        // We have an unique column, so remove all other duplicates in this column outside the present box
        if (con >= 0 && con < 3) {
          xx = bx*3 + con;
          for (yy=0;yy<9;yy++) {
            if (yy/3 == by) continue;
            Poss[xx][yy][n] = false;
          }
        }
        // Now the rows
        con = -1;
        for (t=0;t<3;t++) {
          if (row[t]>0) {
            if (con == -1) con = t;
            else con = 3;
          }
        }
        // We have a unique row, so remove all other duplicates in this row outside the present box
        if (con >= 0 && con < 3) {
          yy = by*3 + con;
          for (xx=0;xx<9;xx++) {
            if (xx/3 == bx) continue;
            Poss[xx][yy][n] = false;
          }
        }
        // And we're done for this number
      }
      // And now we're done for this box
    }
  }
  // The end!
}

// The master function for the Ngroups loop
void NGroups(int n1, int n2, int n3, int n4, int NN) {
  int x,y,bx,by,count,n,nvec[4] = {n1,n2,n3,n4},nfit;
  bool bOK;

  // Loop through each row
  for (y=0;y<9;y++) {
    // Check to see if this group only occurs together, and does so N times
    count=0;
    bOK = true;
    for (x=0;x<9;x++) {
      if (Board[x][y] != 0) continue;
      nfit=0;
      for (n=0;n<NN;n++) if (Poss[x][y][nvec[n]]) nfit++;
      if (nfit==NN) count++;
      else if (nfit>0) {bOK = false;break;}
    }
    // We have a group
    if (bOK && count==NN) {
      // Remove all other possibilities from these squares
      for (x=0;x<9;x++) {
        // If Poss[x][y][n1] then we know also that this is true for n2,n3,n4
        if (Poss[x][y][n1]) {
          for (n=0;n<9;n++) Poss[x][y][n] = false;
          for (n=0;n<NN;n++) Poss[x][y][nvec[n]] = true;
        }
      }
    }
  }
  // Loop through each column
  for (x=0;x<9;x++) {
    // Check to see if this group only occurs together, and does so N times
    count=0;
    bOK = true;
    for (y=0;y<9;y++) {
      if (Board[x][y] != 0) continue;
      nfit=0;
      for (n=0;n<NN;n++) if (Poss[x][y][nvec[n]]) nfit++;
      if (nfit==NN) count++;
      else if (nfit>0) {bOK = false;break;}
    }
    // We have a group
    if (bOK && count==NN) {
      // Remove all other possibilities from these squares
      for (y=0;y<9;y++) {
        // If Poss[x][y][n1] then we know also that this is true for n2,n3,n4
        if (Poss[x][y][n1]) {
          for (n=0;n<9;n++) Poss[x][y][n] = false;
          for (n=0;n<NN;n++) Poss[x][y][nvec[n]] = true;
        }
      }
    }
  }
  // Loop through each box
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      // Check to see if this group only occurs together, and does so N times
      count=0;
      bOK = true;
      for (y=by*3;y<by*3+3;y++) {
        for (x=bx*3;x<bx*3+3;x++) {
          if (Board[x][y] != 0) continue;
          nfit=0;
          for (n=0;n<NN;n++) if (Poss[x][y][nvec[n]]) nfit++;
          if (nfit==NN) count++;
          else if (nfit>0) {bOK = false;break;}
        }
        if (bOK == false) break;
      }
      // We have a group
      if (bOK && count==NN) {
        // Remove all other possibilities from these squares
        for (y=by*3;y<by*3+3;y++) {
          for (x=bx*3;x<bx*3+3;x++) {
            // If Poss[x][y][n1] then we know also that this is true for n2,n3,n4
            if (Poss[x][y][n1]) {
              for (n=0;n<9;n++) Poss[x][y][n] = false;
              for (n=0;n<NN;n++) Poss[x][y][nvec[n]] = true;
            }
          }
        }
      }
    }
  }
}

// Co-occurring groups of 2 numbers
void N2Groups(void) {
  int n1,n2;
  // Loop through each possible pair
  for (n1=0;n1<8;n1++) {
    for (n2=n1+1;n2<9;n2++) {
      NGroups(n1,n2,0,0,2);
    }
  }
}

// Co-occurring groups of 3 numbers
void N3Groups(void) {
  int n1,n2,n3;
  // Loop through each possible pair
  for (n1=0;n1<7;n1++) {
    for (n2=n1+1;n2<8;n2++) {
      for (n3=n2+1;n3<9;n3++) {
        NGroups(n1,n2,n3,0,3);
      }
    }
  }
}

// Co-occurring groups of 4 numbers
void N4Groups(void) {
  int n1,n2,n3,n4;
  // Loop through each possible pair
  for (n1=0;n1<6;n1++) {
    for (n2=n1+1;n2<7;n2++) {
      for (n3=n2+1;n3<8;n3++) {
        for (n4=n3+1;n4<9;n4++) {
          NGroups(n1,n2,n3,n4,4);
        }
      }
    }
  }
}

// Work out disjoint subsets
// If we have a set of N numbers which occur together and alone exactly N times
// in any R/C/B then we can eliminate those numbers from the rest of the R/C/B
void DisjointSubsets(void) {

}

