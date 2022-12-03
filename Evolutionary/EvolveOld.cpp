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

// Randomise the initial population
void RandomisePopulation(void) {
  int n,x,y;

  for (n=0;n<EntityCount;n++) {
    for (x=0;x<9;x++) {
      for (y=0;y<9;y++) {
        if (Set[x][y] == true) Pop[n].Board[x][y] = Board[x][y];
        else Pop[n].Board[x][y] = Random(9)+1;
      }
    }
  }
}

// Mutate an individual
void Mutate(Individual *ind) {
  int x1,x2,y1,y2,temp,type;
  // Get a mutation coordinate
  do {
    x1 = Random(9);
    y1 = Random(9);
  } while (Set[x1][y1]);
  // Work out the mutation type
  type = Random(2);
  // Random swap
  if (type == 0) {
    do {
      x2 = Random(9);
      y2 = Random(9);
    } while (Set[x2][y2]);
    temp = Board[x1][y1];
    Board[x1][y1] = Board[x2][y2];
    Board[x2][y2] = temp;
  }
  // Randomise location
  if (type == 1) {
    Board[x1][y1] = Random(9)+1;
  }
  // 
  if (type == 2) {
    // TODO more mutations
  }
}

// Do crossover
Individual Crossover(Individual *ind1, Individual *ind2) {
  Individual child;
  int x,y;

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Random(2) == 0) child.Board[x][y] = ind1->Board[x][y];
      else child.Board[x][y] = ind2->Board[x][y];
    }
  }
  return child;
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
    NextGen[n] = Crossover(&Pop[TournamentSelect()],&Pop[TournamentSelect()]);    
  }
 
  // Now copy over the next generation
  for (n=0 ; n<EntityCount ; n++) Pop[n] = NextGen[n];
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
  static int last = 0;

  // Loop through a few individuals
  for (n=last;n<min(last+POP_STEP,EntityCount);n++) {
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
  if (n >= EntityCount) last = 0;
  else last = n;
}

// Get the score for this individual
int Evaluate(Individual *ind) {
  int tally[10],x,y,score=0,old,bx,by;

  // Check the rows
  for (y=0;y<9;y++) {
    for (x=1;x<=9;x++) tally[x] = 0;
    old = score;
    for (x=0;x<9;x++) {
      score -= (tally[ind->Board[x][y]]++);
    }
    if (score==old) score += CORRECT_ROW;
  }

  // Check the columns
  for (x=0;x<9;x++) {
    for (y=1;y<=9;y++) tally[x] = 0;
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
