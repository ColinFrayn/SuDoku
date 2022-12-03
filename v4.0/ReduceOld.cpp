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
extern bool Poss[9][9][9], PossBase[9][9][9], Solved[9][9], bError;

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

  // Reset the error flag
  bError = false;

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

// Remove one of the permission values by a manual mouse click
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
  char strExp[64];

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
            if (Poss[xx][yy][n] == true) {
              sprintf(strExp,"All %ds in box %d are in column %d",n+1,(by*3)+bx+1,xx+1);
              Explain(xx,yy,n,strExp);
            }
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
            if (Poss[xx][yy][n] == true) {
              sprintf(strExp,"All %ds in box %d are in row %d",n+1,(by*3)+bx+1,yy+1);
              Explain(xx,yy,n,strExp);
            }
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

// Reduce permissions based on row/column constrictions.
// If all the possibilities for a given number within a row/column are also all within the
// same box, then remove all other occurrences of that number from that box
void RowColumnConstriction(void) {
  int x,y,xx,yy,bx,by,n,Box[3];
  char strExp[64];

  // Loop through all columns
  for (x=0;x<9;x++) {
    bx = x/3;
    // Loop through all numbers
    for (n=0;n<9;n++) {
      Box[0] = Box[1] = Box[2] = 0;
      // Loop through this column and tally which boxes contain this number
      for (y=0;y<9;y++) {
        if (Board[x][y] != 0) continue;
        if (Poss[x][y][n]) {by=y/3;Box[by] = 1;}
      }
      // If all occurrences of this number are within the same box
      // (and 'by' holds this box number, by logic)
      if (Box[0] + Box[1] + Box[2] == 1) {
        // Loop through the cells in this box not in column 'x', and remove the possibility
        // for the given number 'n'
        for (yy=by*3;yy<by*3+3;yy++) {
          for (xx=bx*3;xx<bx*3+3;xx++) {
            if (xx==x) continue;
            if (Poss[xx][yy][n] == true) {
              sprintf(strExp,"Any %d in box %d must be in column %d",n+1,(by*3)+bx+1,x+1);
              Explain(xx,yy,n,strExp);
            }
            Poss[xx][yy][n] = false;
          }
        }
      }
    }
  }

  // Loop through all rows
  for (y=0;y<9;y++) {
    by = y/3;
    // Loop through all numbers
    for (n=0;n<9;n++) {
      Box[0] = Box[1] = Box[2] = 0;
      // Loop through this row and tally which boxes contain this number
      for (x=0;x<9;x++) {
        if (Board[x][y] != 0) continue;
        if (Poss[x][y][n]) {bx=x/3;Box[bx] = 1;}
      }
      // If all occurrences of this number are within the same box
      // (and 'by' holds this box number, by logic)
      if (Box[0] + Box[1] + Box[2] == 1) {
        // Loop through the cells in this box not in row 'y', and remove the possibility
        // for the given number 'n'
        for (yy=by*3;yy<by*3+3;yy++) {
          for (xx=bx*3;xx<bx*3+3;xx++) {
            if (yy==y) continue;
            if (Poss[xx][yy][n] == true) {
              sprintf(strExp,"Any %d in box %d must be in row %d",n+1,(by*3)+bx+1,y+1);
              Explain(xx,yy,n,strExp);
            }
            Poss[xx][yy][n] = false;
          }
        }
      }
    }
  }
}

// Deal with unique subsets within the rows/columns/boxes
void UniqueSubsets(void) {
  int n1,n2,n3,n4;
  for (n1=0;n1<9;n1++) {
    for (n2=0;n2<n1;n2++) {
      NGroups(n1,n2,-1,-1,2);
      for (n3=0;n3<n2;n3++) {
        NGroups(n1,n2,n3,-1,3);
        for (n4=0;n4<n3;n4++) {
          NGroups(n1,n2,n3,n4,4);
        }
      }
    }
  }
}

// The master function for the unique subsets algorithm
void NGroups(int n1, int n2, int n3, int n4, int NN) {
  int x,y,bx,by,count,n,nvec[4] = {n1,n2,n3,n4},nfit;
  bool bOK;
  char strExp[64];

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
          for (n=0;n<9;n++) {
            if (n==n1 || n==n2 || n==n3 || n==n4) Poss[x][y][n] = true;
            else {
              if (Poss[x][y][n] == true) {
                if (NN==2) sprintf(strExp,"Not part of Row-2Group %d,%d",n1+1,n2+1);
                if (NN==3) sprintf(strExp,"Not part of Row-3Group %d,%d,%d",n1+1,n2+1,n3+1);
                if (NN==4) sprintf(strExp,"Not part of Row-4Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
                Explain(x,y,n,strExp);
              }
              Poss[x][y][n] = false;
            }
          }
        }
        // End this row loop
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
          for (n=0;n<9;n++) {
            if (n==n1 || n==n2 || n==n3 || n==n4) Poss[x][y][n] = true;
            else {
              if (Poss[x][y][n] == true) {
                if (NN==2) sprintf(strExp,"Not part of Column-2Group %d,%d",n1+1,n2+1);
                if (NN==3) sprintf(strExp,"Not part of Column-3Group %d,%d,%d",n1+1,n2+1,n3+1);
                if (NN==4) sprintf(strExp,"Not part of Column-4Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
                Explain(x,y,n,strExp);
              }
              Poss[x][y][n] = false;
            }
          }        
        }
        // End this column loop
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
              for (n=0;n<9;n++) {
                if (n==n1 || n==n2 || n==n3 || n==n4) Poss[x][y][n] = true;
                else {
                  if (Poss[x][y][n] == true) {
                    if (NN==2) sprintf(strExp,"Not part of Box-2Group %d,%d",n1+1,n2+1);
                    if (NN==3) sprintf(strExp,"Not part of Box-3Group %d,%d,%d",n1+1,n2+1,n3+1);
                    if (NN==4) sprintf(strExp,"Not part of Box-4Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
                    Explain(x,y,n,strExp);
                  }
                  Poss[x][y][n] = false;
                }
              }        
            }
            // End this box loop
          }
        }
      }
    }
  }
}

// Work out disjoint subsets
// If we have a set of N numbers which occur together and alone exactly N times
// in any R/C/B then we can eliminate those numbers from the rest of the R/C/B
void DisjointSubsets(void) {
  int n1,n2,n3,n4;

  for (n1=0;n1<9;n1++) {
    for (n2=0;n2<n1;n2++) {
      DSets(n1,n2,-1,-1,2);
      for (n3=0;n3<n2;n3++) {
        DSets(n1,n2,n3,-1,3);
        for (n4=0;n4<n3;n4++) {
          DSets(n1,n2,n3,n4,4);
        }
      }
    }
  }
}

// The master function for the disjoint subsets algorithm
void DSets(int n1, int n2, int n3, int n4, int NN) {
  int x,y,bx,by,c,count,nvec[4] = {n1,n2,n3,n4},nfit,celno;
  bool bOK,dset[9];
  char strExp[64];

  // Loop through each row
  for (y=0;y<9;y++) {
    // Check to see if this group occurs together exactly N times without other numbers included
    count=0;
    bOK = true;
    for (x=0;x<9;x++) {
      if (Board[x][y] != 0) continue;
      nfit=0;
      for (c=0;c<9;c++) {
        if (Poss[x][y][c]) {
          if (c==n1 || c==n2 || c==n3 || c==n4) nfit++;
          else {nfit=0;break;}
        }
      }
      if (nfit==NN) {dset[x] = true;count++;}
      else dset[x] = false;
    }
    // We have a group
    if (bOK && count==NN) {
      // Remove all other occurrences of these numbers from other squares in this row
      for (x=0;x<9;x++) {
        if (Board[x][y] != 0) continue;
        if (dset[x]) continue;
        if (NN==2) sprintf(strExp,"Exists elsewhere in exclusive Row 2-Group %d,%d",n1+1,n2+1);
        if (NN==3) sprintf(strExp,"Exists elsewhere in exclusive Row 3-Group %d,%d,%d",n1+1,n2+1,n3+1);
        if (NN==4) sprintf(strExp,"Exists elsewhere in exclusive Row 4-Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
        if (Poss[x][y][n1] == true) Explain(x,y,n1,strExp);
        if (Poss[x][y][n2] == true) Explain(x,y,n2,strExp);
        if (NN>2 && Poss[x][y][n3] == true) Explain(x,y,n3,strExp);
        if (NN>3 && Poss[x][y][n4] == true) Explain(x,y,n4,strExp);
        Poss[x][y][n1] = false;
        Poss[x][y][n2] = false;
        if (NN>2) Poss[x][y][n3] = false;
        if (NN>3) Poss[x][y][n4] = false;
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
      for (c=0;c<9;c++) {
        if (Poss[x][y][c]) {
          if (c==n1 || c==n2 || c==n3 || c==n4) nfit++;
          else {nfit=0;break;}
        }
      }
      if (nfit==NN) {dset[y] = true;count++;}
      else dset[y] = false;
    }
    // We have a group
    if (bOK && count==NN) {
      // Remove all other possibilities from these squares
      for (y=0;y<9;y++) {
        if (Board[x][y] != 0) continue;
        if (dset[y]) continue;
        if (NN==2) sprintf(strExp,"Exists elsewhere in exclusive Column 2-Group %d,%d",n1+1,n2+1);
        if (NN==3) sprintf(strExp,"Exists elsewhere in exclusive Column 3-Group %d,%d,%d",n1+1,n2+1,n3+1);
        if (NN==4) sprintf(strExp,"Exists elsewhere in exclusive Column 4-Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
        if (Poss[x][y][n1] == true) Explain(x,y,n1,strExp);
        if (Poss[x][y][n2] == true) Explain(x,y,n2,strExp);
        if (NN>2 && Poss[x][y][n3] == true) Explain(x,y,n3,strExp);
        if (NN>3 && Poss[x][y][n4] == true) Explain(x,y,n4,strExp);
        Poss[x][y][n1] = false;
        Poss[x][y][n2] = false;
        if (NN>2) Poss[x][y][n3] = false;
        if (NN>3) Poss[x][y][n4] = false;
       }
    }
  }
  // Loop through each box
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      // Check to see if this group only occurs together, and does so N times
      count=0;
      bOK = true;
      celno=0;
      for (y=by*3;y<by*3+3;y++) {
        for (x=bx*3;x<bx*3+3;x++) {
          if (Board[x][y] != 0) continue;
          nfit=0;
          for (c=0;c<9;c++) {
            if (Poss[x][y][c]) {
              if (c==n1 || c==n2 || c==n3 || c==n4) nfit++;
              else {nfit=0;break;}
            }
          }
          if (nfit==NN) {dset[celno] = true;count++;}
          else dset[celno] = false;
          celno++;
        }
      }
      // We have a group
      if (bOK && count==NN) {
        // Remove all other possibilities from these squares
        celno=0;
        for (y=by*3;y<by*3+3;y++) {
          for (x=bx*3;x<bx*3+3;x++) {
            if (Board[x][y] != 0) continue;
            if (dset[celno++]) continue;
            if (NN==2) sprintf(strExp,"Exists elsewhere in exclusive Box 2-Group %d,%d",n1+1,n2+1);
            if (NN==3) sprintf(strExp,"Exists elsewhere in exclusive Box 3-Group %d,%d,%d",n1+1,n2+1,n3+1);
            if (NN==4) sprintf(strExp,"Exists elsewhere in exclusive Box 4-Group %d,%d,%d,%d",n1+1,n2+1,n3+1,n4+1);
            if (Poss[x][y][n1] == true) Explain(x,y,n1,strExp);
            if (Poss[x][y][n2] == true) Explain(x,y,n2,strExp);
            if (NN>2 && Poss[x][y][n3] == true) Explain(x,y,n3,strExp);
            if (NN>3 && Poss[x][y][n4] == true) Explain(x,y,n4,strExp);
            Poss[x][y][n1] = false;
            Poss[x][y][n2] = false;
            if (NN>2) Poss[x][y][n3] = false;
            if (NN>3) Poss[x][y][n4] = false;
          }
        }
      }
    }
  }
}

// Remove permissions based on neighbouring pairs.
// If all the possibilities for number 'n' in a certain box share the same exact two columns
// as the possibilities for that number in another box in th esame column, then remove the possibilities
// for number 'n' in both those columns in the third box.  Ditto for boxes in the same row, and numbers
// sharing the same two rows.
void NeighbourPairs(void) {
  int x,y,bx,by,b1,b2,n,Box1[3],Box2[3];
  char strExp[64];

  // Loop through all box columns (1-3)
  for (bx=0;bx<3;bx++) {
    // Loop through all pairs of boxes in this column
    for (b1=0;b1<2;b1++) {
      for (b2=b1+1;b2<3;b2++) {
        // Loop through all numbers
        for (n=0;n<9;n++) {
          // Check which columns this number exists in within the first box
          Box1[0] = Box1[1] = Box1[2] = 0;
          for (y=0;y<3;y++) {
            for (x=0;x<3;x++) {
              if (Poss[bx*3+x][b1*3+y][n]) Box1[x]=1;
            }
          }
          // Check that the given number occurs in exactly two columns in box 1
          if (Box1[0] + Box1[1] + Box1[2] != 2) continue;
          // Check which columns this number exists in within the second box
          Box2[0] = Box2[1] = Box2[2] = 0;
          for (y=0;y<3;y++) {
            for (x=0;x<3;x++) {
              if (Poss[bx*3+x][b2*3+y][n]) Box2[x]=1;
            }
          }
          // Check that the given number occurs in exactly two columns in box 2
          if (Box2[0] + Box2[1] + Box2[2] != 2) continue;
          // Check that the columns are the same in the two boxes
          if (Box1[0] != Box2[0] || Box1[1] != Box2[1] || Box1[2] != Box2[2]) continue;
          // We have the required situation. So remove all occurrences of 'n' from the
          // permission matrix for the remaining box and the two specified columns
          // First get the last box's y coordinate
          by = 3-(b1+b2);
          if (Box1[0] == 0) sprintf(strExp,"Pairing in columns %d,%d in boxes %d & %d",(bx*3)+2,(bx*3)+3,(b1*3)+bx+1,(b2*3)+bx+1);
          if (Box1[1] == 0) sprintf(strExp,"Pairing in columns %d,%d in boxes %d & %d",(bx*3)+1,(bx*3)+3,(b1*3)+bx+1,(b2*3)+bx+1);
          if (Box1[2] == 0) sprintf(strExp,"Pairing in columns %d,%d in boxes %d & %d",(bx*3)+1,(bx*3)+2,(b1*3)+bx+1,(b2*3)+bx+1);
          for (y=by*3;y<by*3+3;y++) {
            if (Box1[0] == 1) {
              if (Poss[bx*3+0][y][n]) Explain(bx*3,y,n,strExp);
              Poss[bx*3+0][y][n] = false;
            }
            if (Box1[1] == 1) {
              if (Poss[bx*3+1][y][n]) Explain(bx*3+1,y,n,strExp);
              Poss[bx*3+1][y][n] = false;
            }
            if (Box1[2] == 1) {
              if (Poss[bx*3+2][y][n]) Explain(bx*3+2,y,n,strExp);
              Poss[bx*3+2][y][n] = false;
            }
          }
        }
      }
    }
  }
  // Loop through all box rows (1-3)
  for (by=0;by<3;by++) {
    // Loop through all pairs of boxes in this row
    for (b1=0;b1<2;b1++) {
      for (b2=b1+1;b2<3;b2++) {
        // Loop through all numbers
        for (n=0;n<9;n++) {
          // Check which rows this number exists in within the first box
          Box1[0] = Box1[1] = Box1[2] = 0;
          for (y=0;y<3;y++) {
            for (x=0;x<3;x++) {
              if (Poss[b1*3+x][by*3+y][n]) Box1[y]=1;
            }
          }
          // Check that the given number occurs in exactly two columns in box 1
          if (Box1[0] + Box1[1] + Box1[2] != 2) continue;
          // Check which columns this number exists in within the second box
          Box2[0] = Box2[1] = Box2[2] = 0;
          for (y=0;y<3;y++) {
            for (x=0;x<3;x++) {
              if (Poss[b2*3+x][by*3+y][n]) Box2[y]=1;
            }
          }
          // Check that the given number occurs in exactly two columns in box 2
          if (Box2[0] + Box2[1] + Box2[2] != 2) continue;
          // Check that the columns are the same in the two boxes
          if (Box1[0] != Box2[0] || Box1[1] != Box2[1] || Box1[2] != Box2[2]) continue;
          // We have the required situation. So remove all occurrences of 'n' from the
          // permission matrix for the remaining box and the two specified columns
          // First get the last box's y coordinate
          bx = 3-(b1+b2);
          if (Box1[0] == 0) sprintf(strExp,"Pairing in rows %d,%d in boxes %d & %d",(by*3)+2,(by*3)+3,(b1*3)+bx+1,(b2*3)+bx+1);
          if (Box1[1] == 0) sprintf(strExp,"Pairing in rows %d,%d in boxes %d & %d",(by*3)+1,(by*3)+3,(b1*3)+bx+1,(b2*3)+bx+1);
          if (Box1[2] == 0) sprintf(strExp,"Pairing in rows %d,%d in boxes %d & %d",(by*3)+1,(by*3)+2,(b1*3)+bx+1,(b2*3)+bx+1);
          for (x=bx*3;x<bx*3+3;x++) {
            if (Box1[0] == 1) {
              if (Poss[x][by*3+0][n]) Explain(x,by*3,n,strExp);
              Poss[x][by*3+0][n] = false;
            }
            if (Box1[1] == 1) {
              if (Poss[x][by*3+1][n]) Explain(x,by*3+1,n,strExp);
              Poss[x][by*3+1][n] = false;
            }
            if (Box1[2] == 1) {
              if (Poss[x][by*3+2][n]) Explain(x,by*3+2,n,strExp);
              Poss[x][by*3+2][n] = false;
            }
          }
        }
      }
    }
  }
}


// Perform an X-Wing reduction.
// First, find two rows or columns which have the same options for a particular number
// (in terms of position from the beginning ot the R/C)
// Second, cross out all other possibilities for that number in the same two rows & columns
void XWing(void) {
  int n,x,y,r1,r2,c1,c2,count,p1,p2;
  bool bOK;
  char strExp[64];

  // Loop through all possible numbers
  for (n=0;n<9;n++) {
    // Loop through all first rows
    for (r1=0;r1<9;r1++) {
      // Check that there are exactly two permissions for 'n' in this row
      count=0;
      for (x=0;x<9;x++) {
        count += (int)Poss[x][r1][n];
        if (count>2) break;
        if (Poss[x][r1][n] && count==1) p1=x;
        if (Poss[x][r1][n] && count==2) p2=x;
      }
      if (count!=2) continue;
      // Loop through all second rows
      for (r2=r1+1;r2<9;r2++) {
        // Check exactly the same two positions contain the same 'n' (and only those two)
        bOK = true;
        for (x=0;x<9;x++) {
          if (Poss[x][r1][n] != Poss[x][r2][n]) {bOK = false;break;}
        }
        // We have an XWing situation
        if (bOK) {
          // Choose these two rows in turn
          for (x=p1;x<=p2;x+=(p2-p1)) {
            // Loop through each row
            for (y=0;y<9;y++) {
              // Set all other possibility values for 'n' to false except those in r1,r2
              if (y!=r1 && y!=r2) {
                if (Poss[x][y][n]) {
                  sprintf(strExp,"XWing technique on rows %d,%d positions %d,%d",r1+1,r2+1,p1+1,p2+1);
                  Explain(x,y,n,strExp);
                }
                Poss[x][y][n] = false;
              }
            }
          }
        }
      }
    }

    // Loop through all first columns
    for (c1=0;c1<9;c1++) {
      // Check that there are exactly two permissions for 'n' in this column
      count=0;
      for (y=0;y<9;y++) {
        count += (int)Poss[c1][y][n];
        if (count>2) break;
        if (Poss[c1][y][n] && count==1) p1=y;
        if (Poss[c1][y][n] && count==2) p2=y;
      }
      if (count!=2) continue;
      // Loop through all second columns
      for (c2=c1+1;c2<9;c2++) {
        // Check exactly the same two positions contain the same 'n' (and only those two)
        bOK = true;
        for (y=0;y<9;y++) {
          if (Poss[c1][y][n] != Poss[c2][y][n]) {bOK = false;break;}
        }
        // We have an XWing situation
        if (bOK) {
          // Choose these two columns in turn
          for (y=p1;y<=p2;y+=(p2-p1)) {
            // Loop through each column
            for (x=0;x<9;x++) {
              // Set all other possibility values for 'n' to false except those in c1,c2
              if (x!=c1 && x!=c2) {
                if (Poss[x][y][n]) {
                  sprintf(strExp,"XWing technique on columns %d,%d positions %d,%d",c1+1,c2+1,p1+1,p2+1);
                  Explain(x,y,n,strExp);
                }
                Poss[x][y][n] = false;
              }
            }
          }
        }
      }
    }
  }
}