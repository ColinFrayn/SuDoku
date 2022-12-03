// SuDoku.cpp : Defines the entry point for the application.
// and the main window controls.

#include "stdafx.h"
#include "common.h"
#include "SuDoku.h"
#include "resource.h"

// Global Variables:
TCHAR szTitle[]="SuDoku Solver";				// The title bar text
TCHAR szWindowClass[]="SuDoku Solver"; // The (other) title bar text
HWND hWndMain, hWndInput;
HINSTANCE g_hInstance;
HPEN WhitePen, GreyPen, RedPen;
int Board[9][9], xPos=-1, yPos=-1;
int BorderX, BorderY,width,height,bw,bh;
bool Set[9][9], Solved[9][9];
bool Poss[9][9][9], bShow = false;
HFONT hFont,smallFont;

// WinMain function is the application entry point
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {
  MSG msg;
  HWND hDlg;

  // Set up a global
  g_hInstance = hInstance;

  // Initialise the boards
  memset(Board,0,sizeof(Board));
  memset(Set,false,sizeof(Set));
  for (int x=0;x<9;x++) for (int y=0;y<9;y++) for (int n=0;n<9;n++) Poss[x][y][n] = true;

  // Set up the screen for keyboard input
  // Damn this is a sucky way to have to do this.
  // Please petition Microsoft to allow proper keyboard input in dialog boxes!
//  SetupInputWindow();

  // Display the main dialog box.
  hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), hWndInput, (DLGPROC)WndProc);
  ShowWindow(hDlg,SW_SHOW);
  hWndMain = hDlg;

  // Set the window focus on the main window
  SetFocus(hDlg);
  SetActiveWindow(hDlg);

  // Get the GUI stuff
  SetupGUI(hDlg);

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
      return (int)msg.wParam;
    }
  }

  return (int)msg.wParam;
}

// Main window messaging function
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  int wmId, wmEvent, x, y;
  static int mx,my,omx,omy;
  static bool bRunning = false;
  HDC hDC;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_INITDIALOG:
      break;
    case WM_MOUSEWHEEL:
      if (xPos<0 || xPos>8 || yPos<0 || yPos>8) break;
      if (HIWORD(wParam) < 30000) Board[xPos][yPos] = (Board[xPos][yPos]+1)%10;
      if (HIWORD(wParam) > 30000) Board[xPos][yPos] = (Board[xPos][yPos]+9)%10;
      if (Board[xPos][yPos] != 0) Set[xPos][yPos] = Solved[xPos][yPos] = true;
      else Set[xPos][yPos] = Solved[xPos][yPos] = false;
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
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch (wmId) {
        case IDC_CLEAR:
          for (y=0;y<9;y++) for (x=0;x<9;x++) Set[x][y] = false;
          // Fall through
        case IDC_RESET:
          for (y=0;y<9;y++) for (x=0;x<9;x++) {
            if (Set[x][y] == false) {Solved[x][y] = false; Board[x][y] = 0;}
            else Solved[x][y] = true;
            for (int n=0;n<9;n++) Poss[x][y][n] = true;
          }
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_SHOW:
          if (bShow == true) {
            bShow = false;
            SetDlgItemText(hWnd,IDC_SHOW,"Show Possibilities");
          }
          else {
            bShow = true;
            RunStep(false);
            SetDlgItemText(hWnd,IDC_SHOW,"Hide Possibilities");
          }
          DisplayBoard(hWnd,Board);
          break;
        case IDC_BOX:
          BoxStep();
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_ROW:
          RowStep();
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_COLUMN:
          ColumnStep();
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_RUN:
          RunStep(true);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_UPDATE:
          for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0) Solved[x][y] = true;
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_LOAD:
          LoadSuDoku();
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDC_SAVE:
          SaveSuDoku();
          RunStep(false);
          DisplayBoard(hWnd,Board);
          break;
        case IDOK:
          // Fall through
        case IDCANCEL:
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
      DeleteObject(WhitePen);
      DeleteObject(GreyPen);
      DeleteObject(RedPen);
      break;
    default:
      return false;
  }
  return FALSE;
}

// Display the given board
void DisplayBoard(HWND hWnd, int Board[9][9]) {
  HWND hWndFrame = GetDlgItem(hWnd,IDC_BOARD);
  int x,y,xx,yy,bx,by,n;
  char strNum[2];
  HDC hDC = GetDC(hWndFrame);

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

  // Print the boxes
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy++;
    if (y>5) yy++;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx++;
      if (x>5) xx++;
      // If this is the selected box then draw the border in red
      if (x == xPos && y == yPos) SelectObject(hDC,RedPen);
      else SelectObject(hDC,GreyPen);
      // Draw the box
      Rectangle(hDC,xx,yy,xx+bw,yy+bh);
      // Draw the text
      if (Board[x][y] != 0) {
        SelectObject(hDC,hFont);
        // Check if this box is set
        if (Solved[x][y]) {
          if (Set[x][y]) SetTextColor(hDC, C_RED);
          else SetTextColor(hDC, C_BLUE);
        }
        else SetTextColor(hDC, C_GREY);
        sprintf(strNum,"%d",Board[x][y]);
        TextOut(hDC,xx+bw/2,yy+bh/2 - 16,strNum,1);
      }
      else if (bShow) {
        SelectObject(hDC,smallFont);
        SetTextColor(hDC, C_GREEN);
        for (by=0;by<3;by++) {
          for (bx=0;bx<3;bx++) {
            n = by*3 + bx;
            if (Poss[x][y][n]) {
              sprintf(strNum,"%d",n+1);
              TextOut(hDC,xx+10+bx*14,yy+3+by*14,strNum,1);
            }
          }
        }
      }
    }
  }

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

// Run a logical step
// Work out if there are any squares with only one possible option
void RunStep(bool bUpdate) {
  int x,y,bx,by,xx,yy,n,val,count;

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      if (Solved[x][y]) {
        for (n=0;n<9;n++) Poss[x][y][n] = false;
        Poss[x][y][Board[x][y]-1] = true;
        continue;
      }
      for (n=0;n<9;n++) Poss[x][y][n] = true;
      // Parse this column
      for (yy=0;yy<9;yy++) {
        if (Solved[x][yy]) Poss[x][y][Board[x][yy]-1] = false;
      }
      // Parse this row
      for (xx=0;xx<9;xx++) {
        if (Solved[xx][y]) Poss[x][y][Board[xx][y]-1] = false;
      }
      // Parse this block
      bx = x/3;
      by = y/3;
      for (yy=0;yy<3;yy++) {
        for (xx=0;xx<3;xx++) {
          if (Solved[bx*3 + xx][by*3 + yy]) Poss[x][y][Board[bx*3 + xx][by*3 + yy]-1] = false;
        }
      }
      if (bUpdate) {
        if (!Solved[x][y]) Board[x][y] = 0;
        count=0;
        for (n=0;n<9;n++) {
          if (Poss[x][y][n] == true) {count++;val=n+1;}
          if (count>1) break;
        }
        if (count == 1) Board[x][y] = val;
      }
    }
  }
}

// Run a box step
// For each 3*3 box, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
void BoxStep(void) {
  int x,y,xx,yy,bx,by,count[9],n;

  // Setup the possibility values
  RunStep(false);

  // Loop through all nine 3*3 boxes
  for (by=0;by<3;by++) {
    for (bx=0;bx<3;bx++) {
      // Reset the count
      for (n=0;n<9;n++) count[n]=0;
      // Loop through this box, tallying the possible values
      for (yy=0;yy<3;yy++) {
        y = by*3 + yy;
        for (xx=0;xx<3;xx++) {
          x = bx*3 + xx;
          if (Solved[x][y]) continue;
          Board[x][y] = 0;
          for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
        }
      }
      // Now check if there are any 'only possible solutions'
      for (n=0;n<9;n++) {
        if (count[n] == 1) {
          // This one has only one possible solution, so fill it in
          for (yy=0;yy<3;yy++) {
            y = by*3 + yy;
            for (xx=0;xx<3;xx++) {
              x = bx*3 + xx;
              if (Poss[x][y][n]) Board[x][y] = n+1;
            }
          }
        }
      }
    }
  }
}

// Run a row step
// For each row, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
void RowStep(void) {
  int x,y,count[9],n;

  // Setup the possibility values
  RunStep(false);

  // Loop through all nine rows
  for (y=0;y<9;y++) {
    // Reset the count
    for (n=0;n<9;n++) count[n]=0;
    // Loop through this row, tallying the possible values
    for (x=0;x<9;x++) {
      if (Solved[x][y]) continue;
      Board[x][y] = 0;
      for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
    }
    // Now check if there are any 'only possible solutions'
    for (n=0;n<9;n++) {
      if (count[n] == 1) {
        for (x=0;x<9;x++) {
          // This one has only one possible solution, so fill it in
          if (Poss[x][y][n]) Board[x][y] = n+1;
        }
      }
    }
  }
}

// Run a column step
// For each column, work out which possibilities each square has, and if there is only
// one square which could be a certain number, then set it
void ColumnStep(void) {
  int x,y,count[9],n;

  // Setup the possibility values
  RunStep(false);

  // Loop through all nine columns
  for (x=0;x<9;x++) {
    // Reset the count
    for (n=0;n<9;n++) count[n]=0;
    // Loop through this column, tallying the possible values
    for (y=0;y<9;y++) {
      if (Solved[x][y]) continue;
      Board[x][y] = 0;
      for (n=0;n<9;n++) if (Poss[x][y][n]) count[n]++;
    }
    // Now check if there are any 'only possible solutions'
    for (n=0;n<9;n++) {
      if (count[n] == 1) {
        for (y=0;y<9;y++) {
          // This one has only one possible solution, so fill it in
          if (Poss[x][y][n]) Board[x][y] = n+1;
        }
      }
    }
  }
}

// Load in a SuDoku game setup
void LoadSuDoku(void) {
  int x,y;
  char strFile[256], strErr[128],ch;
  FILE *fp;

  // Get a suitable target file to load
  if (!GetFileName(strFile,true)) return;

  // Try to open the specified file
  if ((fp = fopen(strFile,"r")) == NULL) {
    sprintf(strErr,"Unable to open file %s for loading!",strFile);
    MessageBox(hWndMain,strErr,"File Error!",MB_OK);
    return;
  }

  for (y=0;y<9;y++) {
    for (x=0;x<9;x++) {
      fscanf(fp,"%c",&ch);
      Board[x][y] = (int)ch - (int)'0';
      if (Board[x][y] != 0) Set[x][y] = Solved[x][y] = true;
      else Set[x][y] = Solved[x][y] = false;
    }
    fscanf(fp,"\n");
  }
  fclose(fp);
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

// Setup the GUI elements
void SetupGUI(HWND hDlg) {
  RECT Rect;

  // Get a suitable font for box labels
  hFont = CreateFont(36,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                     ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, NULL);
  smallFont = CreateFont(14,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
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
 
  // Get a suitable pen for drawing outlines
  WhitePen = CreatePen(PS_SOLID,1,C_WHITE);
  GreyPen = CreatePen(PS_SOLID,1,C_GREY);
  RedPen = CreatePen(PS_SOLID,1,C_RED);
}

// Set up the (invisible) input window5
void SetupInputWindow(void) {
  WNDCLASSEX wcex;
  CHAR szWindow[]="Input Window";
  static bool bRegistered = false;
  ATOM at;
  
  if (!bRegistered) {
    // Setup the window class & register it
    wcex.cbSize = sizeof(WNDCLASSEX); 
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)InputWndProc;
    wcex.cbClsExtra = wcex.cbWndExtra = 0;
    wcex.hInstance = g_hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindow;
    wcex.hIconSm = NULL;
    
    at = RegisterClassEx(&wcex);
    
    bRegistered = true;
  }

  hWndInput = CreateWindow(szWindow,"Input Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 50, 50, NULL, NULL, g_hInstance, NULL);
  EnableWindow(hWndInput,true);
  ShowWindow(hWndInput,SW_SHOW);
}

// Main window messaging function
INT_PTR CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC hDC;
  PAINTSTRUCT ps;

  switch (message) {
  case WM_CREATE:
    return TRUE;
  case WM_INITDIALOG:
    return TRUE;
  case WM_SIZE:
    break;
  case WM_MOUSEMOVE:
    break;
  case WM_PAINT:
    // Make it look as if we've drawn stuff, just to keep Windows happy. *sigh*
    hDC = BeginPaint( hWnd, &ps );
    EndPaint(hWnd, &ps);
    break;
  case WM_KEYUP:
    switch(wParam) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      wParam = wParam;
      break;
    }
    case WM_TIMER:
      break;
    case WM_DESTROY:
      return FALSE;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return FALSE;
}
