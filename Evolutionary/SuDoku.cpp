// SuDoku.cpp : Defines the entry point for the application.
// and the main window controls.

#include "stdafx.h"
#include "common.h"
#include "SuDoku.h"
#include "resource.h"
#include "Evolve.h"

// Global Variables:
TCHAR szTitle[]="SuDoku Solver";				// The title bar text
TCHAR szWindowClass[]="SuDoku Solver"; // The (other) title bar text
HWND hWndMain;
HINSTANCE g_hInstance;
int Board[9][9], LastBest[9][9], xPos=-1, yPos=-1;
int BorderX, BorderY,width,height,bw,bh,Generation=0;
bool Set[9][9];
HFONT hFont;
Individual Pop[EntityCount];
int lastLoop = 0;

// WinMain function is the application entry point
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {
  MSG msg;
  HWND hDlg;
  RECT Rect;
  
  g_hInstance = hInstance;

  // Initialise the boards
  memset(Board,0,sizeof(Board));
  memset(LastBest,0,sizeof(LastBest));
  memset(Set,false,sizeof(Set));

  // Display the main dialog box.
  hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)WndProc);
  ShowWindow(hDlg,SW_SHOW);
  hWndMain = hDlg;

  // Set the window focus on the main window
  SetFocus(hDlg);
  SetActiveWindow(hDlg);

  // Setup the initial population
  RandomisePopulation();

  // Get a suitable font for card labels
  hFont = CreateFont(16,20,0,0,200,FALSE,FALSE,FALSE,
                     ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, NULL);

   // Get the box dimensions
  GetClientRect(GetDlgItem(hDlg,IDC_BOARD),&Rect);
  width = Rect.right - Rect.left;
  height = Rect.bottom - Rect.top;
  bw = (width  - BORDERX*2) / 9;
  bh = (height - BORDERY*2) / 9;
  // Get the more accurate border size (without rounding errors)
  BorderX  = (width  - (bw*9 + 2)) / 2;
  BorderY  = (height - (bh*9 + 2)) / 2;
  // Draw the board
  DisplayBoard(hDlg, Board);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
    if(!IsDialogMessage(hDlg, &msg)) {
 	  	TranslateMessage(&msg);
    	DispatchMessage(&msg);
		}
    if(msg.message == WM_QUIT) {
      DestroyWindow( hDlg );
      return msg.wParam;
    }
  }

  return msg.wParam;
}

// Main window messaging function
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  int wmId, wmEvent, x, y;
  static int mx,my,omx,omy;
  static bool bRunning = false;
  HDC hDC;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_CREATE:
      return TRUE;
    case WM_INITDIALOG:
      // Setup the necessary GUI elements
      SetDlgItemInt(hWnd,IDC_GENERATION,0,FALSE);
      SetTimer(hWnd,UPDATE_TIMER,UPDATE_RATE,NULL);
      SetDlgItemInt(hWnd,IDC_GENERATION,0,FALSE);
      SetDlgItemText(hWnd,IDC_BEST_SCORE,"None");
      break;
    case WM_MOUSEWHEEL:
      if (xPos<0 || xPos>8 || yPos<0 || yPos>8) break;
      if (HIWORD(wParam) < 30000) Board[xPos][yPos] = (Board[xPos][yPos]+1)%10;
      if (HIWORD(wParam) > 30000) Board[xPos][yPos] = (Board[xPos][yPos]+9)%10;
      if (Board[xPos][yPos] != 0) Set[xPos][yPos] = true;
      else Set[xPos][yPos] = false;
      DisplayBoard(hWnd, Board);
      break;
    case WM_MOUSEMOVE:  // Mouse has moved
      omx=mx;
      omy=my;
      mx = LOWORD(lParam); 
      my = HIWORD(lParam); 
      HighlightBox(hWnd,mx,my,omx,omy);
      break;
    case WM_LBUTTONUP:  // L-mouse click.  Check for new box selection
      SelectBox(hWnd, lParam);
      DisplayBoard(hWnd,Board);
      HighlightBox(hWnd,mx,my,omx,omy);
      break;
    case WM_TIMER:
      if (bRunning) {
        RunSimulation();
      }
      break;
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch (wmId) {
        case IDC_CLEAR:
          for (y=0;y<9;y++) for (x=0;x<9;x++) Set[x][y] = false;
          // Fall through
        case IDC_RESET:
          for (y=0;y<9;y++) for (x=0;x<9;x++) if (Set[x][y] == false) Board[x][y] = 0;
          Generation = 0;
          lastLoop = 0;
          DisplayBoard(hWnd,Board);
          SetDlgItemInt(hWnd,IDC_GENERATION,0,FALSE);
          SetDlgItemText(hWnd,IDC_BEST_SCORE,"None");
          break;
        case IDC_RUN:
          if (bRunning == false) {
            bRunning = true;
            SetDlgItemText(hWnd,IDC_RUN,"Stop");
          }
          else {
            bRunning = false;
            SetDlgItemText(hWnd,IDC_RUN,"Run");
          }
          break;
        case IDC_TEST:
          SetupTest();
          Generation = 0;
          lastLoop = 0;
          DisplayBoard(hWnd,Board);
          SetDlgItemInt(hWnd,IDC_GENERATION,0,FALSE);
          SetDlgItemText(hWnd,IDC_BEST_SCORE,"None");
          break;
        case IDOK:
          // Fall through
        case IDCANCEL:
         KillTimer(hWnd, UPDATE_TIMER);
         if (MessageBox(hWndMain, "Really Quit?", "Quit Program", MB_YESNO) == IDYES) {
            PostQuitMessage(IDCANCEL);
          }
          return TRUE;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
    case WM_PAINT:
      hDC = BeginPaint(hWnd, &ps);
      ReleaseDC(hWnd, hDC);
      DisplayBoard(hWnd, Board);
      HighlightBox(hWnd,mx,my,0,0);
      break;
    case WM_DESTROY:
      break;
    default:
      return false;
  }
  return FALSE;
}

// Display the given board
void DisplayBoard(HWND hWnd, int Board[9][9]) {
  HWND hWndFrame = GetDlgItem(hWnd,IDC_BOARD);
  int x,y,xx,yy;
  char strNum[2];
  HPEN WhitePen, GreyPen, RedPen;
  HDC hDC = GetDC(hWndFrame);

   // Get a suitable pen for drawing outlines
  WhitePen = CreatePen(PS_SOLID,1,C_WHITE);
  GreyPen = CreatePen(PS_SOLID,1,C_GREY);
  RedPen = CreatePen(PS_SOLID,1,C_RED);

  // Clear the window
  SelectObject(hDC,GetStockObject(BLACK_BRUSH));
  Rectangle(hDC,1,1,width,height);
  SelectObject(hDC,GetStockObject(WHITE_BRUSH));
  SelectObject(hDC,GreyPen);
  SetBkColor(hDC, C_WHITE);
  SetTextAlign(hDC,TA_CENTER | TA_TOP);

  // Draw the framework
  // First the board outline
  MoveToEx(hDC,BorderX -1,BorderY-1,NULL);
  LineTo(hDC,BorderX +(bw*9)+2,BorderY-1);
  LineTo(hDC,BorderX +(bw*9)+2,BorderY+(bh*9)+2);
  LineTo(hDC,BorderX -1,       BorderY+(bh*9)+2);
  LineTo(hDC,BorderX -1,       BorderY-1);
  // Now the 3*3 box separators
  // TODO, if necessary

  // Print the boxes
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy++;
    if (y>5) yy++;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx++;
      if (x>5) xx++;
      // Check if this box is set
      if (Set[x][y] == true) SetTextColor(hDC, C_RED);
      else SetTextColor(hDC, C_GREY);
      // If this is the selected box then draw the border in red
      if (x == xPos && y == yPos) SelectObject(hDC,RedPen);
      else SelectObject(hDC,GreyPen);
      // Draw the box
      Rectangle(hDC,xx,yy,xx+bw,yy+bh);
      // Draw the text
      if (Board[x][y] != 0) {
        sprintf(strNum,"%d",Board[x][y]);
        TextOut(hDC,xx+bw/2,yy+bh/2 - 9,strNum,1);
      }
    }
  }

  DeleteObject(WhitePen);
  DeleteObject(GreyPen);
  DeleteObject(RedPen);
  ReleaseDC(hWnd, hDC);
}

// Select a box after a left mouse click
void SelectBox(HWND hWnd, LPARAM loc) {
  HWND hWndFrame = GetDlgItem(hWnd,IDC_BOARD);
  int mx,my,x,y,xx,yy;

  // Get the coordinates of the mouse click
  // Correcting for the border.
  mx = LOWORD(loc) - FRAME_BORDER; 
  my = HIWORD(loc) - FRAME_BORDER; 

  // Get the mouse click position
  xPos = yPos = -1;
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy++;
    if (y>5) yy++;
    if (my >= yy && my < (yy+bh)) yPos=y;
  }
  for (x=0;x<9;x++) {
    xx = BorderX  + bw*x; 
    if (x>2) xx++;
    if (x>5) xx++;
    if (mx >= xx && mx < (xx+bw)) xPos=x;
  }
}

// Highlight the box over which we're hovering
void HighlightBox(HWND hWnd, int mx, int my, int omx, int omy) {
  HWND hWndFrame = GetDlgItem(hWnd,IDC_BOARD);
  int x,y,xx,yy;
  HPEN WhitePen, GreenPen;
  HDC hDC = GetDC(hWndFrame);

  // Correct for the border
  mx -= FRAME_BORDER; omx -= FRAME_BORDER;
  my -= FRAME_BORDER; omy -= FRAME_BORDER;

   // Get a suitable pen for drawing outlines
  WhitePen = CreatePen(PS_SOLID,1,C_WHITE);
  GreenPen = CreatePen(PS_SOLID,1,C_GREEN);
  SelectObject(hDC,WhitePen);

  // Print the boxes
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy++;
    if (y>5) yy++;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx++;
      if (x>5) xx++;
      // Check if we should highlight this one
      if (mx >= xx && mx <= (xx+bw) && my >= yy && my <= (yy+bh)) SelectObject(hDC,GreenPen);
      else if (omx >= xx && omx <= (xx+bw) && omy >= yy && omy <= (yy+bh)) SelectObject(hDC,WhitePen);
      else continue;
      // Draw the frame, or erase the old one (if necessary)
      MoveToEx(hDC,xx+1,yy+1,NULL);
      LineTo(hDC,xx+1,yy+bh-2);
      LineTo(hDC,xx+bw-2,yy+bh-2);
      LineTo(hDC,xx+bw-2,yy+1);
      LineTo(hDC,xx+1,yy+1);
    }
  }

  DeleteObject(WhitePen);
  DeleteObject(GreenPen);
  ReleaseDC(hWnd, hDC);
}

// Draw the progress bar
void DrawProgressBar(HWND hWnd, int progress, int target) {
  int width, height, BarLength;
  static int last = -1;
  static int lastprogress=0, lasttarget=1;
  BOOL bOverwrite = FALSE;
  HBRUSH RedBrush;
  HDC hDC;
  RECT Rect;
  HWND hWndBar;
  
  // Get the bar window dimensions
  hWndBar = GetDlgItem(hWnd, IDC_PROGRESS);
  GetClientRect(hWndBar,&Rect);
  width = Rect.right - Rect.left;
  height = Rect.bottom - Rect.top;
  
  // Calculate the bar length
  if (progress > target) progress = target;
  if (target == 0) BarLength = 0;
  else BarLength = (progress * width) / target;
  
  if (BarLength == last) return;
  last = BarLength;
  
  // Draw the progress bar
  hDC = GetDC(hWndBar);
  
  SelectObject(hDC,GetStockObject(BLACK_BRUSH));
  Rectangle(hDC,BarLength,1,width,height);
  
  RedBrush = CreateSolidBrush(C_RED);
  SelectObject(hDC,RedBrush);
  Rectangle(hDC,0,0,BarLength,height);
  DeleteObject(RedBrush);
  ReleaseDC(hWndBar,hDC);
}

