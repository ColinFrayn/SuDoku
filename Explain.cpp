// Explain.cpp : Explaining the algorithms

#include "stdafx.h"
#include "common.h"
#include "Explain.h"
#include "SuDoku.h"
#include "resource.h"

extern HMENU g_hMenu;
extern HWND hWndText, hWndMain;
char strMsgText[256];

/* Message handler for the 'explanation' dialog box */
INT_PTR CALLBACK ExplanationWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  int sel;
  char strTemp[128];

  switch (message) {
	case WM_INITDIALOG:
    SendDlgItemMessage(hDlg,IDC_TEXTLIST,LB_RESETCONTENT,0,0);
    return TRUE;
	case WM_COMMAND:
    switch(LOWORD(wParam)) {
  	 case IDOK:
     case IDCANCEL:
       CheckMenuItem(g_hMenu,IDM_HELP_EXPLANATION,MF_UNCHECKED);
		   EndDialog(hDlg, LOWORD(wParam));
		   return TRUE;
     case IDC_CLEAR:
       SendDlgItemMessage(hDlg, IDC_TEXTLIST, LB_RESETCONTENT, 0, 0);
	     ShowWindow(hDlg,SW_SHOW);
       break;
     case MESSAGE_UPDATE: // Manually called signal for updating the message text
       SendDlgItemMessage(hDlg, IDC_TEXTLIST, LB_ADDSTRING, 0, (LPARAM)strMsgText);
       break;
     case IDC_DETAILS:
       sel = (int)SendDlgItemMessage(hDlg, IDC_TEXTLIST, LB_GETCURSEL, 0, 0);
       if (sel != -1) {
         SendDlgItemMessage(hDlg, IDC_TEXTLIST, LB_GETTEXT, sel, (LPARAM)strTemp);
         MessageDetails(strTemp);
       }
       break;
 	  }
	  break;
	}
  return FALSE;
}

/* Display a message in the 'explanation' window. */
void Explain(int c, int r, int n, char *strText) {
  if (!IsWindow(hWndText)) return;
  if (n >= 0) sprintf(strMsgText,"C%dR%d[%d] : %s",c+1,r+1,n+1,strText);
  else sprintf(strMsgText,"C%dR%d : %s",c+1,r+1,strText);
  SendMessage(hWndText, WM_COMMAND, MESSAGE_UPDATE, 0);
	ShowWindow(hWndText,SW_SHOW);
}

// Print more details about a given technique
void MessageDetails(char *txt) {
  int x,y,z,bx,by,n,num,n1,n2,n3,n4,n5,n6;
  char strText[1024], strBit[8], strBit2[8], strComp[32], *strExp, *loc;

  x = txt[1] - '0';
  y = txt[3] - '0';
  num = txt[5] - '0';
  
  strExp = &txt[10];
  if (strstr(txt,"Only one possibility")) sprintf(strText,"There was only one number (%d) which could possibly have been placed in this cell (%d,%d).\n"
                                                          "All others would have led to a duplication on either the row, column or box.",num,x,y);
  if (strstr(txt,"Only one location in this box")) sprintf(strText,"Only this cell (%d,%d) of the 9 cells in this box could possibly contain a %d.\n"
                                                                   "All other cells are disallowed based on permissions.",x,y,num);
  if (strstr(txt,"Only one location in this row")) sprintf(strText,"Only this cell (%d,%d) of the 9 cells in this row could possibly contain a %d.\n"
                                                                   "All other cells are disallowed based on permissions.",x,y,num);
  if (strstr(txt,"Only one location in this column")) sprintf(strText,"Only this cell (%d,%d) of the 9 cells in this column could possibly contain a %d.\n"
                                                                   "All other cells are disallowed based on permissions.",x,y,num);
  if (!strncmp(strExp,"All",3)) {
    n = strExp[4] - '0';
    z = strExp[14] - '0';
    bx = (z-1)%3 + 1;
    by = (z-1)/3 + 1;
    if (strstr(strExp,"column")) {
      x = strExp[30] - '0';
      sprintf(strText,"Every possible location for a %d in box %d (%d,%d) is in column %d\n"
                      "That means that no location in column %d outside of box %d can contain a %d",
                       n,z,bx,by,x,x,z,n);
    }
    else {
      y = strExp[27] - '0';
      sprintf(strText,"Every possible location for a %d in box %d (%d,%d) is in row %d\n"
                      "That means that no location in row %d outside of box %d can contain a %d",
                       n,z,bx,by,y,y,z,n);
    }
  }
  if (!strncmp(strExp,"Any",3)) {
    n = strExp[4] - '0';
    z = strExp[13] - '0';
    bx = (z-1)%3 + 1;
    by = (z-1)/3 + 1;
    if (strstr(strExp,"column")) {
      x = strExp[33] - '0';
      sprintf(strText,"Every possible location for a %d in column %d lies in box %d (%d,%d)\n"
                      "That means that no location in box %d outside of column %d can contain a %d",
                       n,x,z,bx,by,z,x,n);
    }
    else {
      y = strExp[30] - '0';
      sprintf(strText,"Every possible location for a %d in row %d lies in box %d (%d,%d)\n"
                      "That means that no location in box %d outside of row %d can contain a %d",
                       n,y,z,bx,by,z,y,n);
    }
  }
  if (!strncmp(strExp,"Not part of",11)) {
    if (strstr(strExp,"Row")) {n = strExp[16] - '0';sprintf(strBit,"row");}
    if (strstr(strExp,"Column")) {n = strExp[19] - '0';sprintf(strBit,"column");}
    if (strstr(strExp,"Box")) {n = strExp[16] - '0';sprintf(strBit,"box");}
    loc = strstr(strExp,"Group") + 6;
    n1 = loc[0] - '0'; n2 = loc[2] - '0';
    if (n>2) n3 = loc[4] - '0';
    if (n>3) n4 = loc[6] - '0';
    if (n==2) sprintf(strComp,"%d,%d",n1,n2);
    if (n==3) sprintf(strComp,"%d,%d,%d",n1,n2,n3);
    if (n==4) sprintf(strComp,"%d,%d,%d,%d",n1,n2,n3,n4);
    sprintf(strText,"Within this %s the numbers [%s] form an unique group, with permissions occurring in exactly %d cells together, and nowhere else.\n"
                    "These %d cells must therefore contain these %d numbers in some order.\n"
                    "Any other number cannot therefore also occur in any of these %d cells, so we can remove this permission for %d at %d,%d."
                    ,strBit,strComp,n,n,n,n,num,x,y);
  }
  if (!strncmp(strExp,"Exists elsewhere in exclusive",29)) {
    if (strstr(strExp,"Row")) {n = strExp[34] - '0';sprintf(strBit,"row");}
    if (strstr(strExp,"Column")) {n = strExp[37] - '0';sprintf(strBit,"column");}
    if (strstr(strExp,"Box")) {n = strExp[34] - '0';sprintf(strBit,"box");}
    loc = strstr(strExp,"Group") + 6;
    n1 = loc[0] - '0'; n2 = loc[2] - '0';
    if (n>2) n3 = loc[4] - '0';
    if (n>3) n4 = loc[6] - '0';
    if (n==2) sprintf(strComp,"%d,%d",n1,n2);
    if (n==3) sprintf(strComp,"%d,%d,%d",n1,n2,n3);
    if (n==4) sprintf(strComp,"%d,%d,%d,%d",n1,n2,n3,n4);
    sprintf(strText,"Within this %s the numbers [%s] form an exclusive group.  They occur in exactly %d cells together with no other numbers.\n"
                    "These %d cells must therefore contain these %d numbers in some order.\n"
                    "This means that no other cell in this %s can also contain any of these %d numbers, so we can remove this permission for %d at %d,%d."
                    ,strBit,strComp,n,n,n,strBit,n,num,x,y);
  }
  if (!strncmp(strExp,"Pairing in",10)) {
    if (strstr(strExp,"columns")) {
      strcpy(strBit,"column");
      n1 = strExp[19] - '0';
      n2 = strExp[21] - '0';
    }
    else {
      strcpy(strBit,"row");
      n1 = strExp[16] - '0';
      n2 = strExp[18] - '0';
    }
    loc = strstr(strExp,"boxes") + 6;
    n3 = loc[0] - '0';
    n4 = loc[4] - '0';
    z = ((x-1)/3) + (((y-1)/3)*3) + 1;
    sprintf(strText,"All the possible locations for a %d in boxes %d and %d are in %ss %d and %d.\n"
                    "Therefore we know that the third box (%d) on this %s cannot contain a %d in either of these two %ss,"
                    ,num,n3,n4,strBit,n1,n2,z,strBit,num,strBit);
  }
  if (!strncmp(strExp,"XWing technique on",18)) {
    loc = strstr(strExp,"on") + 3;
    if (!strncmp(loc,"rows",4)) {strcpy(strBit,"rows");loc += 5;}
    if (!strncmp(loc,"columns",7)) {strcpy(strBit,"columns");loc += 8;}
    if (!strncmp(loc,"boxes",5)) {strcpy(strBit,"boxes");loc += 6;}
    n1 = loc[0] - '0';
    n2 = loc[2] - '0';
    loc += 4;
    if (!strncmp(loc,"rows",4)) {strcpy(strBit2,"row");strcpy(strComp,"rows");loc += 5;z = y;}
    if (!strncmp(loc,"columns",7)) {strcpy(strBit2,"column");strcpy(strComp,"columns");loc += 8;z = x;}
    if (!strncmp(loc,"boxes",5)) {strcpy(strBit2,"box");strcpy(strComp,"boxes");loc += 6;z = ((x-1)/3) + (((y-1)/3)*3) + 1;}
    n3 = loc[0] - '0';
    n4 = loc[2] - '0';
    sprintf(strText,"All possible locations for the number %d on %s %d and %d lie in the same 2 %s (%d & %d).\n"
                    "This means that we can rule out any other permission for a %d in those same 2 %s.\n"
                    "The cell %d,%d is in %s number %d, and not in %s %d or %d, so we can rule it out."
                    ,num,strBit,n1,n2,strComp,n3,n4,num,strComp,x,y,strBit2,z,strBit,n3,n4);
  }
  if (!strncmp(strExp,"Swordfish technique on",22)) {
    loc = strstr(strExp,"on") + 3;
    if (!strncmp(loc,"rows",4)) {strcpy(strBit,"rows");loc += 5;}
    if (!strncmp(loc,"columns",7)) {strcpy(strBit,"columns");loc += 8;}
    n1 = loc[0] - '0';
    n2 = loc[2] - '0';
    n3 = loc[4] - '0';
    loc += 6;
    if (!strncmp(loc,"rows",4)) {strcpy(strBit2,"row");strcpy(strComp,"rows");loc += 5;z = y;}
    if (!strncmp(loc,"columns",7)) {strcpy(strBit2,"column");strcpy(strComp,"columns");loc += 8;z = x;}
    n4 = loc[0] - '0';
    n5 = loc[2] - '0';
    n6 = loc[4] - '0';
    sprintf(strText,"Each of the %s %d, %d and %d contains exactly 2 possible locations for the number %d.\n"
                    "All these permissions lie in the same 3 %s (%d, %d & %d).\n"
                    "This means that we can rule out any other permission for a %d in those same 3 %s.\n"
                    "The cell %d,%d is in %s number %d, and not in %s %d, %d or %d, so we can rule it out."
                    ,strBit,n1,n2,n3,num,strComp,n4,n5,n6,num,strComp,x,y,strBit2,z,strBit,n4,n5,n6);
  }
  if (!strncmp(strExp,"Nishio technique on",19)) {
    sprintf(strText,"If we choose a %d to fill cell (%d,%d) then continue to solve the board, we end up at an illegal position\n"
                    "Therefore we know that this permission is not allowed.",num,x,y);
  }
  if (strstr(strExp,"Forcing")) {
    sprintf(strText,"Regardless which values for neighbouring permissions we choose, the value for cell (%d,%d) always ends up as a %d\n",x,y,num);
  }

  // Finally write this out to a message box
  MessageBox(hWndMain,strText,"Detailed Explanation",MB_OK);
}
