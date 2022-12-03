/***************
 * Evolve.h
 * Header file
 * Colin Frayn
 * April 2005
 ***************/

#ifndef EVOLVE_H
#define EVOLVE_H

void SetupTest(void);
void RandomisePopulation(void);
void Mutate(Individual *);
void Crossover(Individual *, Individual *, Individual *);
void Breed(void);
int  TournamentSelect(void);
void RunSimulation(void);
int  Evaluate(Individual *);

#endif // EVOLVE_H