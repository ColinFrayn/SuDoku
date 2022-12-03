// Extras.cpp : Additional features (File I/O, printing etc)

#include "stdafx.h"
#include "common.h"
#include "Extras.h"
#include "Reduce.h"
#include "SuDoku.h"
#include "Solve.h"
#include "resource.h"

// Global Variables:
extern HWND hWndMain;
extern HMENU g_hMenu;
extern HINSTANCE g_hInstance;
extern int Board[9][9], iHistoryPos, iHistoryMax;
extern bool Set[9][9];
extern bool Poss[9][9][9], PossBase[9][9][9], Solved[9][9], bError;
int g_lastx, g_lasty, g_iDiff, g_expCount;
// Scores for using various techniques
int g_iAbort, g_SolutionCount, g_History[82], iUsed[NUM_MOVES];
DataObject DOCache[9*9*9*4];
ColumnObject COCache[324];
bool bAbortGenerate = false;

// Load in a SuDoku game setup
bool LoadSuDoku(void) {
  int x,y;
  char strFile[256], strErr[128],ch;
  FILE *fp;

  // Get a suitable target file to load
  if (!GetFileName(strFile,true)) return false;

  // Try to open the specified file
  if ((fp = fopen(strFile,"r")) == NULL) {
    sprintf(strErr,"Unable to open file %s for loading!",strFile);
    MessageBox(hWndMain,strErr,"File Error!",MB_OK);
    return false;
  }

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      fscanf(fp,"%c",&ch);
      if (ch == '.' || ch == ' ') Board[x][y] = 0;
      else Board[x][y] = (int)ch - (int)'0';
      if (Board[x][y] != 0) Set[x][y] = Solved[x][y] = true;
      else Set[x][y] = Solved[x][y] = false;
    }
    fscanf(fp,"\n");
  }
  fclose(fp);

  return true;
}

// Save a SuDoku game setup
void SaveSuDoku(void) {
  int x,y;
  char strFile[256], strErr[128];
  FILE *fp;

  // Get a suitable target file to load
  if (!GetFileName(strFile,false)) return;

  // Try to open the specified file
  if ((fp = fopen(strFile,"w")) == NULL) {
    sprintf(strErr,"Unable to open file %s for saving!",strFile);
    MessageBox(hWndMain,strErr,"File Error!",MB_OK);
    return;
  }

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Set[x][y]) fprintf(fp,"%d",Board[x][y]);
      else fprintf(fp,"0");
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}


// Select a data file for loading or saving
bool GetFileName(char *szFile, bool iOpen) {
  OPENFILENAME ofn;
  char szTemp[256]="";
  int  ErrorCode;

  strcpy(szFile,"");
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hWndMain;
  ofn.lpstrFile = szTemp;
  ofn.nMaxFile = sizeof(szTemp);
  ofn.lpstrFilter = "All\0*.*\0SuDoku Data Files\0*.dat\0";
  ofn.lpstrCustomFilter = NULL;
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  if (iOpen) ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  else ofn.Flags = OFN_PATHMUSTEXIST;
  
  if (iOpen) {
    if (GetOpenFileName(&ofn) == FALSE) {
      ErrorCode = CommDlgExtendedError();
      return false;
    }
  }
  else {
    if (GetSaveFileName(&ofn) == FALSE) {
      ErrorCode = CommDlgExtendedError();
      return false;
    }
  }
  strcpy(szFile, szTemp);
  return true;
}


// Export the board to a particular format (to be chosen)
void ExportBoard(int iFormat) {
  char strFile[256], strErr[128];
  FILE *fp;

  // Get a suitable target file to load
  if (!GetFileName(strFile,false)) return;

  // Try to open the specified file
  if ((fp = fopen(strFile,"w")) == NULL) {
    sprintf(strErr,"Unable to open file %s for export!",strFile);
    MessageBox(hWndMain,strErr,"File Error!",MB_OK);
    return;
  }

  // Perform the exporting
  switch (iFormat) {
    case (F_TEXT) : WriteToText(fp); break;
    case (F_POSTSCRIPT) : WriteToPostscript(fp); break;
    case (F_HTML) : WriteToHTML(fp); break;
  }

  fclose(fp);
}

// Export the board state to text
void WriteToText(FILE *fp) {
  int x,y;

  for (y=0;y<9;y++) {
    fprintf(fp,"+---+---+---+---+---+---+---+---+---+\n");
    fprintf(fp,"|   |   |   |   |   |   |   |   |   |\n|");
    for (x=0;x<9;x++) {
      if (Board[x][y] == 0) fprintf(fp,"   |");
      else fprintf(fp," %d |",Board[x][y]);
    } 
    fprintf(fp,"\n|   |   |   |   |   |   |   |   |   |\n");
  }
  fprintf(fp,"+---+---+---+---+---+---+---+---+---+\n");
}

// Export the board state to postscript
void WriteToPostscript(FILE *fp) {
  int x,y;
  time_t t;

  time(&t);
  fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(fp,"%%%%BoundingBox: %d %d %d %d\n",PS_LEFT_OFFSET,PS_BASE_OFFSET,PS_LEFT_OFFSET+PS_TOTWIDTH,PS_BASE_OFFSET+PS_TOTWIDTH);
  fprintf(fp,"%%%%Creator: Sudoku by Michael Kennett\n");
  fprintf(fp,"%%%%CreationDate: %s",ctime(&t));
  fprintf(fp,"%%Title: Saved from Col's Sudoku Helper - colin@frayn.net");
  fprintf(fp,"%%%%EndComments\n");

  /* Write the board contents as a string */
  fprintf(fp, "(" );
  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Board[x][y] > 0) fprintf(fp,"%d",Board[x][y]);
      else fprintf(fp," ");
    }
  }
  fprintf(fp,")\n" );

  /* Co-ordinate transform */
  fprintf(fp, "%d %d translate\n", PS_LEFT_OFFSET, PS_BASE_OFFSET );
  /* Draw board - thin lines first, then thick lines */
  fprintf(fp,"0 setgray\n");


  fprintf(fp,"%d setlinewidth 1 8 {dup %d mul %d add dup dup dup\n",PS_THIN,PS_WIDTH,PS_MARGIN);
  fprintf(fp,"  %d moveto %d lineto %d exch moveto %d exch lineto 1 add} repeat pop stroke\n",PS_MARGIN,PS_TOTWIDTH-PS_MARGIN,PS_MARGIN,PS_TOTWIDTH-PS_MARGIN);
  fprintf(fp,"  %d setlinewidth 1 2 {dup %d mul %d add dup dup dup\n",PS_THICK,PS_WIDTH*3,PS_MARGIN);
  fprintf(fp,"  %d moveto %d lineto %d exch moveto %d exch lineto 1 add} repeat pop stroke\n",PS_MARGIN,PS_TOTWIDTH-PS_MARGIN,PS_MARGIN,PS_TOTWIDTH-PS_MARGIN);
  fprintf(fp,"1 setlinejoin %d %d moveto %d %d lineto %d %d lineto %d %d lineto closepath stroke\n",PS_MARGIN,PS_MARGIN,PS_MARGIN,PS_TOTWIDTH-PS_MARGIN,
                    PS_TOTWIDTH-PS_MARGIN,PS_TOTWIDTH-PS_MARGIN,PS_TOTWIDTH-PS_MARGIN,PS_MARGIN);
  /* Now the code for drawing digits */
  fprintf(fp,"/Helvetica-Bold findfont %d scalefont setfont\n",PS_FONT_SIZE);
  fprintf(fp,"0 81 {2 copy 1 getinterval dup stringwidth pop\n");
  fprintf(fp,"  %d exch sub 2 div 2 index 9 mod %d mul add %d add\n",PS_WIDTH,PS_WIDTH,PS_MARGIN);
  fprintf(fp,"  8 3 index 9 idiv sub %d mul %d add\n",PS_WIDTH,PS_MARGIN+PS_BASELINE);
  fprintf(fp,"  moveto show 1 add} repeat pop pop\n");
}

// Export the board state to HTML
void WriteToHTML(FILE *fp) {
  int x,y;

  fprintf(fp,"<html><head>" );
  fprintf(fp,"<title>SuDoku Puzzle</title>");
  fprintf(fp,"</head><body>");
  fprintf(fp,"<table align=\"center\" border=\"1\" cellpadding=\"3\" cellspacing=\"1\" rules=\"all\">\n");

  for (y=0;y<9;y++) {
    fprintf(fp, " <tr>\n" );
    for (x=0;x<9;x++) {
      fprintf(fp, "  <td>" );
      if(Board[x][y] == 0) fprintf(fp,"&nbsp;");
      else fprintf(fp,"%d",Board[x][y]);
      fprintf(fp,"</td>");
    }
    fprintf(fp, "\n </tr>\n" );   
  }
  fprintf(fp,"</table>\n</body>\n</html>\n");
}

// Generate a new SuDoku puzzle
void GenerateNew(int iDiff) {
  int x,y,n,generated=0, Backup[9][9];
  bool BackupSet[9][9], PossBackup[9][9][9];

  // Create a backup of the current board state
  for (y=0;y<9;y++) for (x=0;x<9;x++) {
    Backup[x][y] = Board[x][y];
    BackupSet[x][y] = Set[x][y];
    for (n=0;n<9;n++) PossBackup[x][y][n] = Poss[x][y][n];
  }

  // Make the 'in progress, or abort' dialog box
  g_iDiff = iDiff;
  DialogBox(g_hInstance,(LPCSTR)IDD_ABORT,hWndMain,(DLGPROC) AbortWndProc);

  // Have we aborted this run?
  if (bAbortGenerate) {
    for (y=0;y<9;y++) for (x=0;x<9;x++) {
      Board[x][y] = Backup[x][y];
      if (Board[x][y] > 0) Solved[x][y] = true;
      else Solved[x][y] = false;
      Set[x][y] = BackupSet[x][y];
      for (n=0;n<9;n++) Poss[x][y][n] = PossBackup[x][y][n];
      return;
    }
  }

  // Set the new board
  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Board[x][y] > 0) Solved[x][y] = Set[x][y] = true;
      else Solved[x][y] = Set[x][y] = false;
      for (n=0;n<9;n++) Poss[x][y][n] = true;
    }
  }
  // Display the new board
  SetupPermissionMatrix(true);
  DisplayBoard(hWndMain,Board);
  iHistoryPos = iHistoryMax = 0;
  // Deactivate undo/redo menus
  EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
  EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
}

// Generate a random board with the given number of cells filled
void GenerateRandom(int iCount) {
  int n,x,y;

  // Reset the board
  for (y=0;y<9;y++) for (x=0;x<9;x++) Board[x][y] = 0;

  // Loop through the numbers
  for (n=0;n<iCount;n++) AddRandomNumber();    

  // Set the 'solved and set' matrices
  for (y=0;y<9;y++) for (x=0;x<9;x++) {
    if (Board[x][y] == 0) Solved[x][y] = Set[x][y] = false;
    else Solved[x][y] = Set[x][y] = true;
  }
}

// Add a random new (legal) number to the board
void AddRandomNumber(void) {
  int nx,ny,x,y,bx,by,add;
  bool bOK;

  do {
    add = Random(9)+1;
    bOK = true;
    // Choose a new cell
    do {
      nx = Random(9);
      ny = Random(9);
    } while (Board[nx][ny] > 0);
    // Check it's legal
    for (x=0;x<9;x++) if (Board[x][ny]==add) {bOK = false;break;}
    if (!bOK) continue;
    for (y=0;y<9;y++) if (Board[nx][y]==add) {bOK = false;break;}
    if (!bOK) continue;
    bx = nx/3;by = ny/3;
    for (y=by*3;y<by*3+3;y++) {
      for (x=bx*3;x<bx*3+3;x++) {
        if (Board[x][y]==add) {bOK = false;break;}
      }
    }
  } while (!bOK);
  Board[nx][ny] = add;
  Solved[nx][ny] = true;
  Set[nx][ny] = true;
  g_lastx = nx;
  g_lasty = ny;
}

#define MAX_TRIES  (5)
#define MAX_TESTS  (80)
#define BASE_COUNT (20)

// Generate a board incrementally
void GenerateIncremental(int iDiff) {
  int x,y,count,nnum=0,tries[81],history[81][2];
  int tests=0,generated=0,rating;

  // Start with an empty board. Add in BASE_COUNT numbers at random.
  // Keep adding new numbers until the solutions count drops beneath 2.
  // If it drops to zero then backtrack, otherwise if it drops to 1 then return
  // After a maximum of MAX_TRIES tries at adding a new number, backtrack
  // Keep going until we get a board of the desired difficulty
  do {
    // Reset the board
    for (y=0;y<9;y++) for (x=0;x<9;x++) {
      Board[x][y] = 0;
      Solved[x][y] = Set[x][y] = false;
    }
    nnum = tests = tries[0]=0;
    // Try to add a few numbers so that the board becomes a valid SuDoku
    do {
      AddRandomNumber();
      history[nnum][0] = g_lastx;
      history[nnum][1] = g_lasty;
      nnum++;
      tries[nnum]=0;
    } while (nnum<BASE_COUNT);

    // Now keep adding in new numbers and check for solutions
    do {
      tests++;
      AddRandomNumber();
      history[nnum][0] = g_lastx;
      history[nnum][1] = g_lasty;
      count = CountSolutions(2);
      tries[nnum]++;
      // Gone too far
      if (count == 0) {
        Board[g_lastx][g_lasty] = 0;
        Solved[g_lastx][g_lasty] = false;
        Set[g_lastx][g_lasty] = false;
      }
      else {
        history[nnum][0] = g_lastx;
        history[nnum][1] = g_lasty;
        nnum++;
        tries[nnum]=0;
      }
      // We're in a dead end so Backtrack
      if (tries[nnum] >= MAX_TRIES) {
        nnum--;
        Board[history[nnum][0]][history[nnum][1]] = 0;
        Solved[history[nnum][0]][history[nnum][1]] = false;
        Set[history[nnum][0]][history[nnum][1]] = false;
      }  
    } while (count != 1 && tests < MAX_TESTS);
    // If this is too easy then let's try to reduce it
    rating = RateBoard();
    while (rating < iDiff) {
      do {
        x = Random(9);
        y = Random(9);
      } while (Board[x][y] == 0);
      Board[x][y] = 0;
      Solved[x][y] = false;
      Set[x][y] = false;
      count = CountSolutions(2);
      if (count == 0) break;
      rating = RateBoard();
    }
  } while (count != 1 || rating != iDiff);
}


// Rate the difficulty of the current board
int RateBoard(void) {
  int x,y,n,Backup[9][9],done=81,turn,easy,tally[4];
  bool BackupSet[9][9],BackupSolved[9][9], BackupPoss[9][9][9];

  // Reset move-type-used tally
  for (n=0;n<NUM_MOVES;n++) iUsed[n] = 0;

  // Check the puzzle has a solution
  if (CountSolutions(1)==0) return D_ILLEGAL;

  // Create a backup
  for (y=0;y<9;y++) for (x=0;x<9;x++) {
    for (n=0;n<9;n++) BackupPoss[x][y][n] = Poss[x][y][n];
    Backup[x][y] = Board[x][y];
    BackupSet[x][y] = Set[x][y];
    if (Board[x][y]>0) Set[x][y]=true;
    else Set[x][y] = false;
    BackupSolved[x][y] = Solved[x][y];
    if (Board[x][y]>0) Solved[x][y]=true;
    else Solved[x][y] = false;
    if (Board[x][y]>0) done--;
  }
  SetupPermissionMatrix(true);

  // Count the number of techniques required to solve it, and tally up the total puzzle complexity

  // Firstly loop through simple strategies until done
  easy = SimpleSolver();
  done -= easy;

  // If we haven't solved it with simple strategies alone then let's use something more complicated
  if (done>0) {
    do {
      turn=0;

      // Try a constriction step
      ConstrictionStep(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_CONSTRICTION]++;
        continue;
      }

      // Try a row-column restriction
      RowColumnConstriction(true);  
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_RC_CONSTRICTION]++;
        continue;
      }

      // Try the unique subsets trick
      UniqueSubsets(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_U_SUBSETS]++;
        continue;
      }

      // Try the disjoint subsets trick
      DisjointSubsets(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_D_SUBSETS]++;
        continue;
      }

      // Check for neighbour pairs
      g_expCount=0;
      NeighbourPairs(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_NEIGHBOUR_PAIRS]++;
        continue;
      }

      // Check for XWing
      XWingRC(true);
      XWingRB(true);
      XWingCB(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_XWING]++;
        continue;
      }

      // Check for Swordfish
      Swordfish(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_SWORDFISH]++;
        continue;
      }

      // Check for forcing chains
      ForcingChains(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_FORCING_CHAINS]++;
        continue;
      }

      // Check for Nishio
      Nishio(true);
      turn = SimpleSolver();
      done -= turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_NISHIO]++;
        continue;
      }
    } while (turn>0);
  }

  // Difficulty bonus for actually using any of these algorithms
  // Easy moves
  tally[0] = iUsed[MOVE_BOX_STEP] + iUsed[MOVE_CONSTRICTION];
  // Medium moves
  tally[1] = iUsed[MOVE_COLUMN_STEP] + iUsed[MOVE_ROW_STEP];
  // Hard moves
  tally[2] = iUsed[MOVE_RUN_STEP] + iUsed[MOVE_RC_CONSTRICTION] + iUsed[MOVE_U_SUBSETS] + iUsed[MOVE_D_SUBSETS] + iUsed[MOVE_NEIGHBOUR_PAIRS];
  // Expert moves
  tally[3] = iUsed[MOVE_XWING] + iUsed[MOVE_SWORDFISH] + iUsed[MOVE_FORCING_CHAINS] + iUsed[MOVE_NISHIO];

  // Restore the board
  for (y=0;y<9;y++) for (x=0;x<9;x++) {
    Board[x][y] = Backup[x][y];
    Set[x][y] = BackupSet[x][y];
    Solved[x][y] = BackupSolved[x][y];
    for (n=0;n<9;n++) Poss[x][y][n] = BackupPoss[x][y][n];
  }

  // Penalty if this puzzle appears unsolvable
  if (done > 0) return D_NIGHTMARE;

  // Return the determined difficulty
  if (tally[1] == 0 && tally[2] == 0 && tally[3] == 0) return D_EASY;
  if (tally[1] <  3 && tally[2] == 0 && tally[3] == 0) return D_MILD;
  if (tally[1] < 10 && tally[2] == 0 && tally[3] == 0) return D_MEDIUM;
  if (tally[2] <  3 && tally[3] == 0) return D_HARD;
  if (tally[2] <  8 && tally[3] == 0) return D_EXPERT;
  if (tally[3] <  2 && iUsed[MOVE_FORCING_CHAINS] == 0 && iUsed[MOVE_NISHIO] == 0) return D_FIENDISH;
  return D_NIGHTMARE;
}

// Do simple solving steps, if possible
int SimpleSolver(void) {
  int x,y,turn,done=0;

  do {
    // Firstly, do as many simple box steps as possible
    do {
      turn = BoxStep(true);
      done += turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_BOX_STEP]++;
        continue;
      }
    } while (turn>0);

    // If we can't do a box step then do a column step
    if (turn == 0) {
      turn = ColumnStep(true);
      done += turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_COLUMN_STEP]++;
        continue;
      }
    }

    // If we can't do a box step or a column step, then do a row step
    if (turn == 0) {
      turn = RowStep(true);
      done += turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_ROW_STEP]++;
        continue;
      }
    }

    // If we can't do a box, column or row step then do a run step
    if (turn == 0) {
      turn = RunStep(true);
      done += turn;
      if (turn>0) {
        for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && !Solved[x][y]) SetSquare(x,y);
        iUsed[MOVE_RUN_STEP]++;
        continue;
      }
    }
    // Repeat as long as we have made progress
  } while (turn>0);
  return done;
}

// Set a square and sort out permissions
void SetSquare(int px, int py) {
  int x,y,n,bx,by;
  Solved[px][py] = true;
  n = Board[px][py] - 1;
  bx = px - (px%3);
  by = py - (py%3);
  for (x=0;x<9;x++) Poss[x][py][n] = false;
  for (y=0;y<9;y++) Poss[px][y][n] = false;
  for (x=bx;x<bx+3;x++) {
    for (y=by;y<by+3;y++) Poss[x][y][n] = false;
  }
}

// Use Knuth's Dancing Links algorithm to solve the current board
// Note - we assume that the current board state is legal
// Abort once we have at least g_iAbort total solutions
int CountSolutions(int iAbort) {
  int count;
  ColumnObject h;

  // Check board legality
  if (CheckIllegal()) return 0;

  // Setup the header object
  h.R = &h;
  h.L = &h;
  h.size = 0;

  // Generate the DL matrix given the current board
  GenerateDLMatrix(&h);

  // Now solve this matrix
  g_iAbort = iAbort;
  g_SolutionCount = 0;
  count = Search(&h,0);

  return count;
}

// Generate the DL matrix using the current board
void GenerateDLMatrix(ColumnObject *h) {
  int x,y,z,n,xx,yy,bx,by,data,tally=0;
  bool bOK;
  DataObject *last, *next = h, *head, *Heads[324];

  // Setup the headers & precache an array of pointers to them
  for (n=0;n<324;n++) {
    next->R = &COCache[n];
    (next->R)->L = next;
    next = next->R;
    h->L = next;
    next->R = h;
    next->U = next->D = next->C = next;
    ((ColumnObject *)next)->size = 0;
    Heads[n] = next;
  }

  // Order is nCx nRy nBz xy
  for (y=0;y<9;y++) {
    by = y/3;
    for (x=0;x<9;x++) {
      bx = x/3; 
      z = (by*3) + bx;
      if (Board[x][y] == 0) {
        for (n=0;n<9;n++) {
          // Check to see if this placement of x,y,n is legal
          bOK = true;
          for (xx=0;xx<9;xx++) if (Board[xx][y]==n+1) {bOK = false;break;}
          if (!bOK) continue;
          for (yy=0;yy<9;yy++) if (Board[x][yy]==n+1) {bOK = false;break;}
          if (!bOK) continue;
          for (yy=by*3;yy<by*3+3;yy++) {
            for (xx=bx*3;xx<bx*3+3;xx++) {
              if (Board[xx][yy]==n+1) {bOK = false;break;}
            }
          }
          if (!bOK) continue;
          // This placement of x,y,n is legal, so add it to the DL matrix
          data = x + (y*9) + (n*81);
          // Find the four columns.  For each one, add a new element.
          // -------- First column --------
          head = Heads[n*9 + x];
          ((ColumnObject *)head)->size++;
          last = head->U;
          head->U = &DOCache[tally++];
          next = head->U;
          next->U = last;
          next->D = next->C = head;
          last->D = next;
          next->L = next;
          next->data = data;
          // -------- Second column --------
          head = Heads[n*9 + y + 81];
          ((ColumnObject *)head)->size++;
          next->R = &DOCache[tally++];
          (next->R)->L = next;
          (next->R)->R = next;
          next = next->R;
          next->U = head->U;
          (next->U)->D = next;
          head->U = next;
          next->D = next->C = head;
          next->data = data;
          // -------- Third column --------
          bx = x/3; by = y/3;
          z = (by*3)+bx;
          head = Heads[n*9 + z + 162];
          ((ColumnObject *)head)->size++;
          next->R = &DOCache[tally++];
          (next->R)->L = next;
          (next->R)->R = next->L;
          next = next->R;
          next->U = head->U;
          (next->U)->D = next;
          head->U = next;
          next->D = next->C = head;
          next->data = data;
          // -------- Fourth column --------
          head = Heads[x*9 + y + 243];
          ((ColumnObject *)head)->size++;
          next->R = &DOCache[tally++];
          (next->R)->L = next;
          (next->R)->R = next->L->L;
          next = next->R;
          (next->R)->L = next;
          next->U = head->U;
          (next->U)->D = next;
          head->U = next;
          next->D = next->C = head;
          next->data = data;
        }
      }
    }
  }

  // Now cover all columns that correspond to the existing numbers
  for (y=0;y<9;y++) {
    by = y/3;
    for (x=0;x<9;x++) {
      if (Board[x][y] > 0) {
        bx = x/3; 
        z = (by*3) + bx;
        n = Board[x][y] - 1;
        ((ColumnObject *)Heads[n*9 + x])->CoverColumn();
        ((ColumnObject *)Heads[n*9 + y + 81])->CoverColumn();
        ((ColumnObject *)Heads[n*9 + z + 162])->CoverColumn();
        ((ColumnObject *)Heads[x*9 + y + 243])->CoverColumn();
      }
    }
  }
}

// Now solve the matrix recursively using the Dancing Links algorithm
// Quit when we've found enough solutions
int Search(ColumnObject *h, int depth) {
  int count=0,least=1000;
  DataObject *r;
  ColumnObject *c, *best;

  g_History[depth] = -1;
  if (h->R == h) {g_SolutionCount++;return 1;}

  // Choose a column object 'C' (with the smallest size)
  c = (ColumnObject *)h->R;
  while (c != h) {
    if (c->size < least) {
      best = c;
      least = c->size;
      if (least == 0) return 0; // We have a non-solution
    }
    c = (ColumnObject *)c->R;
  }
  c = best;

  // Cover column 'C'
  c->CoverColumn();

  // Loop through column 'C'
  r = c->D;
  while (r != c) {
    // Store this row as part of the potential solution 
    g_History[depth] = r->data;
    // Loop through this row and cover all elements
    r->CoverRow();
    // Recurse
    count += Search(h,depth+1);
    // Loop back through this row and uncover all elements
    r->UncoverRow();
    // Go to the next row
    r = r->D;
    // Abort if we're only interested in checking for a limited number of solutions
    if (g_SolutionCount >= g_iAbort) break;
  }
  // Uncover column 'C'
  c->UncoverColumn();

  return count;
}

// Cover a column
void ColumnObject::CoverColumn(void) {
  DataObject *r, *j;

  R->L = L;
  L->R = R;

  r = D;
  while (r->C != r) {
    j = r->R;
    while (j != r) {
      // Cover this element
      (j->U)->D = j->D;
      (j->D)->U = j->U;
      (((ColumnObject *)(j->C))->size)--;
      j = j->R;
    }
    r = r->D;
  }
}

// Uncover a column
void ColumnObject::UncoverColumn(void) {
  DataObject *r, *j;

  r = U;
  while (r->C != r) {
    j = r->L;
    while (j != r) {
      // Uncover this element
      (j->D)->U = j;
      (j->U)->D = j;
      (((ColumnObject *)(j->C))->size)++;
      j = j->L;
    }
    r = r->U;
  }

  R->L = this;
  L->R = this;
}

// Cover a row
void DataObject::CoverRow(void) {
  DataObject *j;

  j = R;
  while (j != this) {
    ((ColumnObject *)(j->C))->CoverColumn();
    j = j->R;
  }
}

// Uncover a row
void DataObject::UncoverRow(void) {
  DataObject *j;

  j = L;
  while (j != this) {
    ((ColumnObject *)(j->C))->UncoverColumn();   
    j = j->L;
  }
}

// Constructors/Destructors
DataObject::DataObject() {}
DataObject::~DataObject() {}
ColumnObject::ColumnObject() {}
ColumnObject::~ColumnObject() {}

#define TIMER_ID (1)

/* Message handler for the 'in progress or abort' dialog box */
INT_PTR CALLBACK AbortWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
	case WM_INITDIALOG:
    // Keep randomly generating boards until we get one that fits our criteria
    SetTimer(hDlg,TIMER_ID,1,NULL);
    bAbortGenerate = false;
    return TRUE;
  case WM_TIMER:
    do {
      GenerateRandom(25 - (g_iDiff/2) + Random(10 - ((g_iDiff+1)/2)));
    } while (CountSolutions(2) != 1);
    if (RateBoard() == g_iDiff) {
      KillTimer(hDlg,TIMER_ID);
      EndDialog(hDlg, LOWORD(wParam));
    }
    break;
	case WM_COMMAND:
    switch(LOWORD(wParam)) {
  	 case IDOK:
     case IDCANCEL:
       bAbortGenerate = true;
       KillTimer(hDlg,TIMER_ID);
		   EndDialog(hDlg, LOWORD(wParam));
		   return TRUE;
 	  }
	  break;
	}
  return FALSE;
}