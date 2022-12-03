// SuDoku.cpp : Defines the entry point for the application.
// and the main window controls.

#include "stdafx.h"
#include "common.h"
#include "SuDoku.h"
#include "Reduce.h"
#include "Solve.h"
#include "resource.h"

// Global Variables:
TCHAR szTitle[]="SuDoku Solver";				// The title bar text
TCHAR szWindowClass[]="SuDoku Solver";  // The (other) title bar text
HWND hWndMain, hWndInput;
HINSTANCE g_hInstance;
HPEN WhitePen, GreyPen, RedPen, HighlightPen;
HBRUSH HighlightBrush;
HMENU g_hMenu;
int Board[9][9], History[9][9][1024], xPos=-1, yPos=-1, iHighlight = 0, iHistoryPos = 0, iHistoryMax = 0;
int BorderX, BorderY,bw,bh,width,height,gapx,gapy;
bool Set[9][9], Solved[9][9], bError = FALSE;
bool Poss[9][9][9], PossBase[9][9][9], bShow = false;
HFONT hFont,smallFont;

// WinMain function is the application entry point
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow) {
  MSG msg;

  // Set up a global
  g_hInstance = hInstance;

  // Initialise the boards
  memset(Board,0,sizeof(Board));
  memset(Set,false,sizeof(Set));
  for (int x=0;x<9;x++) for (int y=0;y<9;y++) for (int n=0;n<9;n++) Poss[x][y][n] = true;
  
  // Setup the main screen
  SetupMainWindow();

  // Display the input dialog box.
  hWndInput = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, (DLGPROC)InputWndProc);
  ShowWindow(hWndInput,SW_SHOW);

  // Set the window focus on the main window
  SetFocus(hWndMain);
  SetActiveWindow(hWndMain);
  BringWindowToTop(hWndMain);
  UpdateWindow(hWndMain);

  // Get the GUI stuff
  SetupGUI(hWndMain);

  // Draw the board
  DisplayBoard(hWndMain, Board);

  // Disable the undo/redo menu items
  EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
  EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
//    if(!IsDialogMessage(msg.hwnd, &msg)) {
 	  	TranslateMessage(&msg);
    	DispatchMessage(&msg);
//		}
    if(msg.message == WM_QUIT) {
      DestroyWindow( hWndMain );
      return (int)msg.wParam;
    }
  }

  return (int)msg.wParam;
}

// Main window messaging function
INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  int wmId, wmEvent, x, y, n;
  static int mx,my,omx,omy;
  static bool bRunning = false;
  HDC hDC;
  PAINTSTRUCT ps;

  switch (message) {
    case WM_INITDIALOG:
      return TRUE;
    case WM_MOUSEWHEEL:
      if (xPos<0 || xPos>8 || yPos<0 || yPos>8) break;
      if (HIWORD(wParam) < 30000) Board[xPos][yPos] = (Board[xPos][yPos]+1)%10;
      if (HIWORD(wParam) > 30000) Board[xPos][yPos] = (Board[xPos][yPos]+9)%10;
      if (Board[xPos][yPos] != 0) Set[xPos][yPos] = Solved[xPos][yPos] = true;
      else Set[xPos][yPos] = Solved[xPos][yPos] = false;
      SetupPermissionMatrix(true);
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
    case WM_RBUTTONUP:  // R-mouse click.  Check for removing a permission value
      if (bShow) {
        RemovePermission(hWnd, mx, my);
        DisplayBoard(hWnd,Board);
      }
      break;
    case WM_SIZE:
      SetupGUI(hWnd);
      DisplayBoard(hWnd,Board);
      break;
    case WM_KEYUP:
      switch(wParam) {
        case '0':
        case '1': 
        case '2': 
        case '3': 
        case '4': 
        case '5': 
        case '6': 
        case '7': 
        case '8': 
        case '9': 
          if (xPos<0 || xPos>8 || yPos<0 || yPos>8) break;
          n = (int)(wParam - '0');
          Board[xPos][yPos] = n;
          if (Board[xPos][yPos] != 0) Set[xPos][yPos] = Solved[xPos][yPos] = true;
          else Set[xPos][yPos] = Solved[xPos][yPos] = false;
          SetupPermissionMatrix(true);
          DisplayBoard(hWnd, Board);
          break;
        case VK_CLEAR:
        case VK_DELETE:
          if (xPos<0 || xPos>8 || yPos<0 || yPos>8) break;
          Board[xPos][yPos] = 0;
          Set[xPos][yPos] = Solved[xPos][yPos] = false;
          SetupPermissionMatrix(true);
          DisplayBoard(hWnd, Board);
          break;
      }
      break;
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch (wmId) {
        case IDM_FILE_NEW:
          for (y=0;y<9;y++) for (x=0;x<9;x++) Set[x][y] = false;
          for (y=0;y<9;y++) for (x=0;x<9;x++) {
            if (Set[x][y] == false) {Solved[x][y] = false; Board[x][y] = 0;}
            else Solved[x][y] = true;
            for (int n=0;n<9;n++) Poss[x][y][n] = true;
          }
          SetupPermissionMatrix(true);
          DisplayBoard(hWnd,Board);
          iHistoryPos = iHistoryMax = 0;
          // TODO : Deactivate undo/redo menus
          break;
        case IDM_FILE_LOAD:
          if (LoadSuDoku() == true) {
            SetupPermissionMatrix(true);
            DisplayBoard(hWnd,Board);
            iHistoryPos = iHistoryMax = 0;
            // Deactivate undo/redo menus
            EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
            EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
          }
          break;
        case IDM_FILE_SAVE:
          SaveSuDoku();
          DisplayBoard(hWnd,Board);
          break;
        case IDM_HELP_ABOUT:
          MessageBox(hWndMain,"SuDoku Solver v3.0 by Colin Frayn, 2005\ncmf@cercia.ac.uk\nhttp://www.cs.bham.ac.uk/~cmf/index.php","About SuDoku Solver v3.0",MB_OK);
          break;
        case IDM_EDIT_RESETALL:
          for (y=0;y<9;y++) for (x=0;x<9;x++) {
            if (Set[x][y] == false) {Solved[x][y] = false; Board[x][y] = 0;}
            else Solved[x][y] = true;
            for (int n=0;n<9;n++) Poss[x][y][n] = true;
          }
          SetupPermissionMatrix(true);
          DisplayBoard(hWndMain,Board);
          iHistoryPos = iHistoryMax = 0;
          // Deactivate undo/redo menus
          EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
          EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
          break;
        case IDM_EDIT_UNDO:
          if (iHistoryPos > 0) {
            // Store the present position
            for (y=0;y<9;y++) for (x=0;x<9;x++) {
              if (Solved[x][y]) History[x][y][iHistoryPos] = Board[x][y];
              else History[x][y][iHistoryPos] = 0;
            }
            // Retract the last move
            iHistoryPos--;
            // Set the board & solved state
            for (y=0;y<9;y++) for (x=0;x<9;x++) Board[x][y] = History[x][y][iHistoryPos];
            for (y=0;y<9;y++) for (x=0;x<9;x++) Solved[x][y] = ((Board[x][y] > 0) ? true : false) ;
            // Reset the permission matrix
            SetupPermissionMatrix(true);
            // Display the board
            DisplayBoard(hWndMain,Board);
            EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_ENABLED);
          }
          if (iHistoryPos == 0) {
            // Deactivate 'undo' menu
            EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
          }
          break;
        case IDM_EDIT_REDO:
          if (iHistoryPos < iHistoryMax) {
            iHistoryPos++;
            // Set the board & solved state
            for (y=0;y<9;y++) for (x=0;x<9;x++) Board[x][y] = History[x][y][iHistoryPos];
            for (y=0;y<9;y++) for (x=0;x<9;x++) Solved[x][y] = ((Board[x][y] > 0) ? true : false) ;
            // Reset the permission matrix
            SetupPermissionMatrix(true);
            // Display the board
            DisplayBoard(hWndMain,Board);
            EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_ENABLED);
          }
          if (iHistoryPos == iHistoryMax) {
            // Deactivate 'redo' menu
            EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
          }
          break;
        // Quit the programme
        case IDM_FILE_EXIT:
        case IDOK:
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
      DeleteObject(HighlightBrush);
      DeleteObject(HighlightPen);
      DeleteObject(hFont);
      DeleteObject(smallFont);
      PostQuitMessage(IDCANCEL);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return FALSE;
}

// Display the given board
void DisplayBoard(HWND hWndFrame, int Board[9][9]) {
  int x,y,xx,yy,bx,by,n;
  char strNum[2];
  HDC hDC = GetDC(hWndFrame);

  // Setup the base permission matrix
  SetupBasePermissionMatrix();

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
  LineTo(hDC,BorderX +(bw*9)+gapx*2,BorderY-1);
  LineTo(hDC,BorderX +(bw*9)+gapx*2,BorderY+(bh*9)+gapy*2);
  LineTo(hDC,BorderX -1,       BorderY+(bh*9)+gapy*2);
  LineTo(hDC,BorderX -1,       BorderY-1);

  // Print the boxes
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy+=gapy;
    if (y>5) yy+=gapy;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx+=gapx;
      if (x>5) xx+=gapx;
      // If this is the selected box then draw the border in red
      if (x == xPos && y == yPos) SelectObject(hDC,RedPen);
      else SelectObject(hDC,GreyPen);
      // Get the background colour
      if (iHighlight == 0 || (!Solved[x][y] && Poss[x][y][iHighlight-1])) {
        SelectObject(hDC,GetStockObject(WHITE_BRUSH));
        SetBkColor(hDC,C_WHITE);
      }
      else {
        SelectObject(hDC,HighlightBrush);
        SetBkColor(hDC,C_HIGHLIGHT);
      }
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
        TextOut(hDC,xx+bw/2,yy+bh/12,strNum,1);
      }
      else if (bShow) {
        SelectObject(hDC,smallFont);
        SetTextColor(hDC, C_GREEN);
        for (by=0;by<3;by++) {
          for (bx=0;bx<3;bx++) {
            n = by*3 + bx;
            if (Poss[x][y][n]) {
              sprintf(strNum,"%d",n+1);
              TextOut(hDC,xx+(bw/4)+(bx*bw)/4,yy+(bh/11)+(by*(bh+2))/4,strNum,1);
            }
            else {
              // If this should be a permission, but it's been manually removed
              if (PossBase[x][y][n]) {
                SetTextColor(hDC, C_RED);
                sprintf(strNum,"x");
                TextOut(hDC,xx+(bw/4)+(bx*bw)/4,yy+(bh/11)+(by*(bh+2))/4,strNum,1);
                SetTextColor(hDC, C_GREEN);
              }
            }
          }
        }
      }
    }
  }

  ReleaseDC(hWndFrame, hDC);
}

// Select a box after a left mouse click
void SelectBox(HWND hWndFrame, LPARAM loc) {
  int mx,my,x,y,xx,yy;

  // Get the coordinates of the mouse click
  // Correcting for the border.
  mx = LOWORD(loc) - FRAME_BORDER; 
  my = HIWORD(loc) - FRAME_BORDER; 

  // Get the mouse click position
  xPos = yPos = -1;
  for (y=0;y<9;y++) {
    yy = BorderY + bh*y; 
    if (y>2) yy+=gapy;
    if (y>5) yy+=gapy;
    if (my >= yy && my < (yy+bh)) yPos=y;
  }
  for (x=0;x<9;x++) {
    xx = BorderX  + bw*x; 
    if (x>2) xx+=gapx;
    if (x>5) xx+=gapx;
    if (mx >= xx && mx < (xx+bw)) xPos=x;
  }
}

// Highlight the box over which we're hovering
void HighlightBox(HWND hWndFrame, int mx, int my, int omx, int omy) {
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
    if (y>2) yy+=gapy;
    if (y>5) yy+=gapy;
    for (x=0;x<9;x++) {
      xx = BorderX  + bw*x; 
      if (x>2) xx+=gapx;
      if (x>5) xx+=gapx;
      // Check if we should highlight this one
      if (mx >= xx && mx <= (xx+bw) && my >= yy && my <= (yy+bh)) SelectObject(hDC,GreenPen);
      else if (omx >= xx && omx <= (xx+bw) && omy >= yy && omy <= (yy+bh)) {
        if (iHighlight == 0 || (!Solved[x][y] && Poss[x][y][iHighlight-1])) SelectObject(hDC,WhitePen);
        else SelectObject(hDC,HighlightPen);
      }
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
  ReleaseDC(hWndFrame, hDC);
}

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
      Board[x][y] = (int)ch - (int)'0';
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

// Setup the GUI elements
void SetupGUI(HWND hDlg) {
  RECT Rect;
  static bool bSetup = FALSE;

   // Get the box dimensions
  GetClientRect(hDlg,&Rect);
  width = Rect.right - Rect.left;
  height = Rect.bottom - Rect.top;
  bw = (width  - BORDERX*2) / 9;
  bh = (height - BORDERY*2) / 9;

  // The gap between boxes
  gapx = max(width/150,1);
  gapy = max(height/150,1);

  // Get the more accurate border size (without rounding errors)
  BorderX  = (width  - (bw*9 + gapx*2)) / 2 + 1;
  BorderY  = (height - (bh*9 + gapy*2)) / 2 + 1;

  // Get rid of old fonts if we have any
  if (bSetup) {
    DeleteObject(hFont);
    DeleteObject(smallFont);
  }
  // Get a suitable font for box labels
  hFont = CreateFont(((bh*9)/10) - 2,((bw*3)/4) - 2,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                     ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, NULL);
  smallFont = CreateFont(bh/4,bw/6,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                     ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, NULL);

  // Set up drawing objects
  if (!bSetup) {
    // Get a suitable pen for drawing outlines
    HighlightPen = CreatePen(PS_SOLID,1,C_HIGHLIGHT);
    WhitePen = CreatePen(PS_SOLID,1,C_WHITE);
    GreyPen = CreatePen(PS_SOLID,1,C_GREY);
    RedPen = CreatePen(PS_SOLID,1,C_RED);
    HighlightBrush = CreateSolidBrush(C_HIGHLIGHT);
  }
  // Only set this up once!
  bSetup = TRUE;
}

// Set up the main window
void SetupMainWindow(void) {
  WNDCLASSEX wcex;
  char szWindow[] = "Main Window";
  ATOM at;
  
  // Setup the window class & register it
	wcex.style       = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra  = 0;
	wcex.cbWndExtra  = 0;
	wcex.hInstance   = g_hInstance;
	wcex.hIcon       = NULL;
	wcex.hCursor     = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	 = (LPCSTR)IDR_MENU;
	wcex.lpszClassName = szWindow;
	wcex.hIconSm       = NULL;
  wcex.cbSize        = sizeof(WNDCLASSEX); 
    
  at = RegisterClassEx(&wcex);
    
  hWndMain = CreateWindow(szWindow,"Col's SuDoku Solver v3.0    (c) 2005 Colin Frayn   colin@frayn.net", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                          CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, g_hInstance, NULL);

  EnableWindow(hWndMain,true);
  ShowWindow(hWndMain,SW_SHOW);
  UpdateWindow(hWndMain);

  // Get a handle to the main menu
  g_hMenu = GetMenu(hWndMain);
}

// Main window messaging function
INT_PTR CALLBACK InputWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  HDC hDC;
  int wmId, wmEvent, x, y, tally;
  bool bChanged = false;
  PAINTSTRUCT ps;

  switch (message) {
  case WM_CREATE:
    return TRUE;
  case WM_INITDIALOG:
    return TRUE;
    case WM_COMMAND:
      wmId    = LOWORD(wParam); 
      wmEvent = HIWORD(wParam); 
      // Parse the menu selections:
      switch (wmId) {
        case IDC_RESET:
          for (y=0;y<9;y++) for (x=0;x<9;x++) {
            if (Set[x][y] == false) {Solved[x][y] = false; Board[x][y] = 0;}
            else Solved[x][y] = true;
            for (int n=0;n<9;n++) Poss[x][y][n] = true;
          }
          SetupPermissionMatrix(true);
          DisplayBoard(hWndMain,Board);
          iHistoryPos = iHistoryMax = 0;
          // Deactivate undo/redo menus          
          EnableMenuItem(g_hMenu,IDM_EDIT_UNDO,MF_GRAYED);
          EnableMenuItem(g_hMenu,IDM_EDIT_REDO,MF_GRAYED);
          break;
        case IDC_SHOW:
          if (bShow == true) {
            bShow = false;
            SetDlgItemText(hWnd,IDC_SHOW,"Show Possibilities");
          }
          else {
            bShow = true;
            SetDlgItemText(hWnd,IDC_SHOW,"Hide Possibilities");
          }
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_RESET_PERMISSIONS:
          SetupPermissionMatrix(true);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_BOX:
          BoxStep();
          SetupPermissionMatrix(false);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_ROW:
          RowStep();
          SetupPermissionMatrix(false);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_COLUMN:
          ColumnStep();
          SetupPermissionMatrix(false);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_RUN:
          RunStep();
          SetupPermissionMatrix(false);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_CONSTRICTIONS:
          ConstrictionStep();
          RowColumnConstriction();
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_DISJOINT:
          DisjointSubsets();
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_NEIGHBOUR_PAIRS:
          NeighbourPairs();
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_UPDATE:
          tally=0;
          for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && Solved[x][y] == false) tally++;
          // Update the history, if necessary
          if (tally > 0) {
            AcceptChanges();
            DisplayBoard(hWndMain,Board);
          }
          break;
        case IDC_CLEAR:
          for (y=0;y<9;y++) for (x=0;x<9;x++) if (Board[x][y] != 0 && Solved[x][y] == false) Board[x][y] = 0;
          SetupPermissionMatrix(true);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_UNIQUE:
          UniqueSubsets();
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_DO_ALL:
          // Loop through simple strategies until done
          do {
            bChanged = false;
            if (RunStep() > 0) {
              AcceptChanges();
              bChanged = true;
            }
            if (!bError && BoxStep() > 0) {
              AcceptChanges();
              bChanged = true;
            }
            if (!bError && RowStep() > 0) {
              AcceptChanges();
              bChanged = true;
            }
            if (!bError && ColumnStep() > 0) {
              AcceptChanges();
              bChanged = true;
            }
          } while (bChanged && !bError);
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_0:
          iHighlight = 0;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_1:
          iHighlight = 1;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_2:
          iHighlight = 2;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_3:
          iHighlight = 3;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_4:
          iHighlight = 4;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_5:
          iHighlight = 5;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_6:
          iHighlight = 6;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_7:
          iHighlight = 7;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_8:
          iHighlight = 8;
          DisplayBoard(hWndMain,Board);
          break;
        case IDC_9:
          iHighlight = 9;
          DisplayBoard(hWndMain,Board);
          break;
        default:
          return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
    case WM_PAINT:
      // Make it look as if we've drawn stuff, just to keep Windows happy. *sigh*
      hDC = BeginPaint( hWnd, &ps );
      EndPaint(hWnd, &ps);
      break;
    case WM_DESTROY:
      return FALSE;
    default:
      return FALSE;
  }
  return FALSE;
}
