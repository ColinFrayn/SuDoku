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
void ConstrictionStep(bool);
void RowColumnConstriction(bool);
void NGroups(int,int,int,int,int,bool);
void UniqueSubsets(bool);
void DisjointSubsets(bool);
void DSets(int,int,int,int,int,bool);
void NeighbourPairs(bool);
void XWingRC(bool);
void XWingRB(bool);
void XWingCB(bool);
void Swordfish(bool);
void Nishio(bool);
void ForcingChains(bool);

#endif  // REDUCE_H
