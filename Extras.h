/********************************
 *    Extras.h                  *
 *    Colin Frayn               *
 *    Aug 2005                  *
 ********************************/

/*
  Contains the specific defines for Extras.cpp
 */

#ifndef EXTRAS_H
#define EXTRAS_H

#ifdef _WIN32
using namespace std;
#endif

// Classes required for the 'Dancing Links' algorithm
class DataObject {
  public:
    class DataObject *L, *R, *U, *D, *C;
    int data;

    DataObject();
    virtual ~DataObject();
    void CoverRow(void);
    void UncoverRow(void);
};

class ColumnObject: public DataObject {
  public:
    int size;
    ColumnObject();
    virtual ~ColumnObject();
    void CoverColumn(void);
    void UncoverColumn(void);
};

bool LoadSuDoku(void);
void SaveSuDoku(void);
bool GetFileName(char *, bool);
void ExportBoard(int);
void WriteToText(FILE *);
void WriteToPostscript(FILE *);
void WriteToHTML(FILE *);
void GenerateNew(int);
void GenerateRandom(int);
void AddRandomNumber(void);
void GenerateIncremental(int);
int  RateBoard(void);
int  SimpleSolver(void);
void SetSquare(int,int);
int  CountSolutions(int);
void GenerateDLMatrix(ColumnObject *);
int  Search(ColumnObject *, int);
INT_PTR CALLBACK AbortWndProc(HWND, UINT, WPARAM, LPARAM);

// Move types
#define MOVE_BOX_STEP         (0)
#define MOVE_ROW_STEP         (1)
#define MOVE_COLUMN_STEP      (2)
#define MOVE_RUN_STEP         (3)
#define MOVE_CONSTRICTION     (4)
#define MOVE_RC_CONSTRICTION  (5)
#define MOVE_U_SUBSETS        (6)
#define MOVE_D_SUBSETS        (7)
#define MOVE_NEIGHBOUR_PAIRS  (8)
#define MOVE_XWING            (9)
#define MOVE_SWORDFISH        (10)
#define MOVE_FORCING_CHAINS   (11)
#define MOVE_NISHIO           (12)
#define NUM_MOVES             (13)

// Postscript output values
#define PS_WIDTH          30   /* Size of each box (points) */
#define PS_MARGIN         5    /* Margin around board (points) */
#define PS_THICK          3    /* Width of thick lines (points) */
#define PS_THIN           1    /* Width of thin lines (points) */
#define PS_TOTWIDTH       (9*PS_WIDTH+2*PS_MARGIN)  /* Total board width */
#define PS_FONT_SIZE      18   /* Font size for postscript file */
#define PS_BASELINE       (((PS_WIDTH-PS_FONT_SIZE)/2)-1)    /* Offset of character base line */

/* Page size */
#define PS_A4_WIDTH       612
#define PS_A4_HEIGHT      792

#define PS_LEFT_OFFSET    ((PS_A4_WIDTH - PS_TOTWIDTH)/2)
#define PS_BASE_OFFSET    ((PS_A4_HEIGHT - PS_TOTWIDTH)/2)

#endif  // EXTRAS_H
