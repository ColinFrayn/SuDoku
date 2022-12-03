/********************************
 *    Reduce.h                  *
 *    Colin Frayn               *
 *    May 2005                  *
 ********************************/

/*
  Contains the specific defines for Reduce.cpp
 */

#ifndef REDUCE_H
#define REDUCE_H

bool CheckIllegal(void);
void SetupPermissionMatrix(bool);
void SetupBasePermissionMatrix(void);
void RemovePermission(HWND, int, int);
void ConstrictionStep(void);
void RowColumnConstriction(void);
void NGroups(int,int,int,int,int);
void UniqueSubsets(void);
void DisjointSubsets(void);
void DSets(int,int,int,int,int);
void NeighbourPairs(void);

#endif  // REDUCE_H
