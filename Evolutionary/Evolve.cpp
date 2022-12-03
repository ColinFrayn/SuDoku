/************************
 * Evolve.cpp
 * Evolutionary algorithms
 * Colin Frayn
 * April 2005
 ************************/

#include "stdafx.h"
#include "common.h"
#include "SuDoku.h"
#include "resource.h"
#include "Evolve.h"

extern Individual Pop[EntityCount];
extern int Board[9][9], LastBest[9][9];
extern bool Set[9][9];
extern int Generation;
extern HWND hWndMain;
Individual NextGen[EntityCount];
extern int lastLoop;


// Randomise the initial population
void RandomisePopulation(void) {
  int n,x,y,tally[10],r;

  Randomise();

  for (n=0;n<EntityCount;n++) {
    // Loop through the 9 rows
    for (y=0;y<9;y++) {
      for (x=0;x<10;x++) tally[x] = 0;
      for (x=0;x<9;x++) {
        if (Set[x][y]) {
          Pop[n].Board[x][y] = Board[x][y];
          tally[Board[x][y]] = 1;
        }
      }
      for (x=0;x<9;x++) {
        if (Set[x][y] == false) {
          do {r=Random(9)+1;} while (tally[r] > 0);
          tally[r] = 1;
          Pop[n].Board[x][y] = r;
        }
      }
    }
  }
}

// Mutate an individual
void Mutate(Individual *ind) {
  int r,x,x1,x2,y,temp,tally[10];

  do {
    // Get a mutation coordinate
    y = Random(9);
    // Randomise a whole row?
    if (Random(4)==0) {
      for (x=0;x<10;x++) tally[x] = 0;
      for (x=0;x<9;x++) {
        if (Set[x][y]) {
          ind->Board[x][y] = Board[x][y];
          tally[Board[x][y]] = 1;
        }
      }
      for (x=0;x<9;x++) {
        if (Set[x][y] == false) {
          do {r=Random(9)+1;} while (tally[r] > 0);
          tally[r] = 1;
          ind->Board[x][y] = r;
        }
      }
    }
    // Do a swap
    else {
      do {
        x1 = Random(9);
      } while (Set[x1][y]);
      // Swap in the row
      do {
        x2 = Random(9);
      } while (Set[x2][y]);
      temp = Board[x1][y];
      Board[x1][y] = Board[x2][y];
      Board[x2][y] = temp;
    }
  } while (Random(3));
}

// Do crossover
void Crossover(Individual *child, Individual *ind1, Individual *ind2) {
  int x,y,parent;

  // Take rows from either parent, at random
  for (y=0;y<9;y++) {
    parent = Random(2);
    for (x=0;x<9;x++) {
      if (parent == 0) child->Board[x][y] = ind1->Board[x][y];
      else child->Board[x][y] = ind2->Board[x][y];
    }
  }
}

// Do the breeding stage
void Breed(void) {
  int n,elite[EliteCount],loc,i,sc;

  // Always ensure that the fittest elite survive
  for (n=0;n<EliteCount;n++) elite[n] = -1;

  for (n=0; n<EntityCount; n++) {
    sc = Pop[n].score;
    // Try to insert this score in the elite list
    loc = EliteCount-1;
    while (loc>=0) {
      if (elite[loc]>=0 && Pop[elite[loc]].score > sc) break;
      loc--;
    } 
    
    // If we found a position in the elite list then shift down the elites
    // below this position, and insert the new entry.
    if (++loc<EliteCount) {
      for (i=EliteCount-1;i>loc;i--) elite[i] = elite[i-1];
      elite[loc] = n;      
    }
  }
 
  // Copy over the surviving elite individuals
  for (n=0; n<EliteCount; n++) {  
    if (elite[n] == -1) break;
    NextGen[n] = Pop[elite[n]];
  }

  // Next select which individuals survive by right.
  for ( ; n<EliteCount+Survivors ; n++) {
    NextGen[n] = Pop[TournamentSelect()];
  }

  // Create mutants
  for ( ; n<EliteCount+Survivors+Mutants; n++) {
    NextGen[n] = Pop[TournamentSelect()];
    Mutate(&NextGen[n]);    
  }
  
  // Finally crossover
  for ( ; n<EntityCount ; n++) {
    Crossover(&NextGen[n],&Pop[TournamentSelect()],&Pop[TournamentSelect()]);    
  }
 
  // Now copy over the next generation
  for (n=0 ; n<EntityCount ; n++) {    
    if (NextGen[n].Board[0][0] != 1) {
      n=n;
    }
    Pop[n] = NextGen[n];
  }
}

// Tournament select a new individual
int TournamentSelect(void) {
  int r1,r2;
  r1 = Random(EntityCount);
  r2 = Random(EntityCount);
  if (Pop[r1].score > Pop[r2].score) return r1;
  return r2;
}

// Run the simulation
void RunSimulation(void) {
  int n,x,y;

  // Setup the initial population
  if (lastLoop==0 && Generation == 0) RandomisePopulation();

  // Loop through a few individuals
  for (n=lastLoop;n<min(lastLoop+POP_STEP,EntityCount);n++) {
    Pop[n].score = Evaluate(&Pop[n]);
    DrawProgressBar(hWndMain,n,EntityCount);
  }
  // Next generation?
  if (n == EntityCount) {
    DrawProgressBar(hWndMain,n,n);
    Breed();
    Generation++;
    SetDlgItemInt(hWndMain,IDC_GENERATION,Generation,FALSE);
    for (y=0;y<9;y++) for (x=0;x<9;x++) LastBest[x][y] = Pop[0].Board[x][y];
    for (y=0;y<9;y++) for (x=0;x<9;x++) Board[x][y] = LastBest[x][y];
    SetDlgItemInt(hWndMain,IDC_BEST_SCORE,MAX_SCORE-Pop[0].score,FALSE);
    DisplayBoard(hWndMain,Board);
  }
  if (n >= EntityCount) lastLoop = 0;
  else lastLoop = n;
}

// Get the score for this individual
int Evaluate(Individual *ind) {
  int tally[10],x,y,score=0,old,bx,by;

  /*
  // Check the rows
  for (y=0;y<9;y++) {
    for (x=1;x<=9;x++) tally[x] = 0;
    old = score;
    for (x=0;x<9;x++) {
      score -= (tally[ind->Board[x][y]]++);
    }
    if (score==old) score += CORRECT_ROW;
  }
  */

  // Check the columns
  for (x=0;x<9;x++) {
    for (y=1;y<=9;y++) tally[y] = 0;
    old = score;
    for (y=0;y<9;y++) {
      score -= (tally[ind->Board[x][y]]++);
    }
    if (score==old) score += CORRECT_COLUMN;
  }
  // Check the boxes
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      for (x=1;x<=9;x++) tally[x] = 0;
      old = score;
      for (y=0;y<3;y++) {
        for (x=0;x<3;x++) {
          score -= (tally[ind->Board[bx*3+x][by*3+y]]++);
        }
      }
      if (score==old) score += CORRECT_BOX;
    }
  }
  return score;
}

// Setup the test problem
void SetupTest(void) {
  memset(Board,0,sizeof(Board));
  memset(Set,false,sizeof(Set));
  Board[1][0] = 6;
  Board[3][0] = 1;
  Board[5][0] = 4;
  Board[7][0] = 5;
  Board[2][1] = 8;
  Board[3][1] = 3;
  Board[5][1] = 5;
  Board[6][1] = 6;
  Board[0][2] = 2;
  Board[8][2] = 1;
  Board[0][3] = 8;
  Board[3][3] = 4;
  Board[5][3] = 7;
  Board[8][3] = 6;
  Board[2][4] = 6;
  Board[6][4] = 3;
  Board[0][5] = 7;
  Board[3][5] = 9;
  Board[5][5] = 1;
  Board[8][5] = 4;
  Board[0][6] = 5;
  Board[8][6] = 2;
  Board[2][7] = 7;
  Board[3][7] = 2;
  Board[5][7] = 6;
  Board[6][7] = 9;
  Board[1][8] = 4;
  Board[3][8] = 5;
  Board[5][8] = 8;
  Board[7][8] = 7;
  Set[1][0] = true;
  Set[3][0] = true;
  Set[5][0] = true;
  Set[7][0] = true;
  Set[2][1] = true;
  Set[3][1] = true;
  Set[5][1] = true;
  Set[6][1] = true;
  Set[0][2] = true;
  Set[8][2] = true;
  Set[0][3] = true;
  Set[3][3] = true;
  Set[5][3] = true;
  Set[8][3] = true;
  Set[2][4] = true;
  Set[6][4] = true;
  Set[0][5] = true;
  Set[3][5] = true;
  Set[5][5] = true;
  Set[8][5] = true;
  Set[0][6] = true;
  Set[8][6] = true;
  Set[2][7] = true;
  Set[3][7] = true;
  Set[5][7] = true;
  Set[6][7] = true;
  Set[1][8] = true;
  Set[3][8] = true;
  Set[5][8] = true;
  Set[7][8] = true;
}
