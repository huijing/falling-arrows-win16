#include "AboutDlg.h"
#include "Globals.h"
#include "MainWnd.h"
#include "Resource.h"
#include "smprintf.h"
#include "stdlib.h"
#include "string.h"

/* Main window class and title */
static const char MainWndClass[] = "Falling Arrows";

#define TIMER_REGULAR_ID 1
#define STATUS_TEXT_LENGTH 20
#define ARROW_COORD 19
#define DROP_SPEED 1000

static char hitStatusText[STATUS_TEXT_LENGTH];

int score = 0;
int fallingArrows[ARROW_COORD] = {0};;
int arrowPositionForIndex[ARROW_COORD] = {380, 360, 340, 320, 300, 280, 260, 240, 220, 200, 180, 160, 140, 120, 100, 80, 60, 40, 20};
int stateOfLastAction = 0;

//0: Initial
//1: Press 1, correct 1
//2: Press 1, correct 0

int hitMissBlank = 0;

//0: Blank
//1: Hit
//2: Miss

/* Window procedure for our main window */
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  HDC hdc;
  PAINTSTRUCT ps;
  RECT targetRect;
  HBRUSH hBrush; 
  int i;
  int yCoord;

  char scoreString[5] = "0";
  
  
  switch (msg)
  {
    case WM_CREATE:
      printf("Created\n");
      SetTimer(hWnd, TIMER_REGULAR_ID, DROP_SPEED, NULL);
      memset(fallingArrows, 10, ARROW_COORD*sizeof(int));

      return 0;

    case WM_TIMER:
      switch(wParam){
        case TIMER_REGULAR_ID:

        if(stateOfLastAction == 2 || 
          (stateOfLastAction == 0 && (fallingArrows[1] < 4))) {
          hitMissBlank = 2;
          
          if(score > 0){
            score--;
          }
        } 

        //Score 
        if(stateOfLastAction == 1){
          score++;
          hitMissBlank = 1;
        }

        if(stateOfLastAction == 0 && (fallingArrows[1] > 3)){
          hitMissBlank = 0;
        }

        stateOfLastAction = 0;

        for(i = 0; i < ARROW_COORD - 1; i++){
          fallingArrows[i] = fallingArrows[i + 1];
        }

        fallingArrows[ARROW_COORD - 1] = rand() % 8;

        break;
      }

      InvalidateRect(hWnd, NULL, FALSE);

      return 0;
    case WM_COMMAND:
    {
      WORD id = wParam;

      switch (id)
      {
        case ID_HELP_ABOUT:
        {
          ShowAboutDialog(hWnd);
          printf("show Dialog\n");
          return 0;
        }

        case ID_FILE_EXIT:
        {
          DestroyWindow(hWnd);
          return 0;
        }
      }
      break;
    }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        switch(hitMissBlank){
          case 0: 
            TextOut(hdc, 10, 10, "        ", 8);
            break;
          case 1:
            TextOut(hdc, 10, 10, "Hit   ", 6);
            break;
          case 2:
            TextOut(hdc, 10, 10, "Miss", 4);
            break;
        }

        sprintf(scoreString, "%04d", score);

        TextOut(hdc, 140, 10, scoreString, strlen(scoreString));

        SetRect(&targetRect, 90, 360, 110, 380);
        FrameRect(hdc, &targetRect, hBrush);


        for(i = 0; i < ARROW_COORD; i++){
          yCoord = arrowPositionForIndex[i];
          SetRect(&targetRect, 90, yCoord, 110, 440);

          switch(fallingArrows[i]){
            case 0: 
              DrawText(hdc, "<", 1, &targetRect, DT_CENTER | DT_NOCLIP);
              break;
            case 1: 
              DrawText(hdc, ">", 1, &targetRect, DT_CENTER | DT_NOCLIP);
              break;
            case 2: 
              DrawText(hdc, "^", 1, &targetRect, DT_CENTER | DT_NOCLIP);
              break;
            case 3: 
              DrawText(hdc, "V", 1, &targetRect, DT_CENTER | DT_NOCLIP);
              break;
            default:
              DrawText(hdc, "   ", 3, &targetRect, DT_CENTER | DT_NOCLIP);
              break;
          }

        }

        return 0;

     case WM_KEYDOWN:

        if (wParam == VK_LEFT){
          printf("Pressed left\n");
          if(fallingArrows[1] == 0){
            printf("Left Hit");
            stateOfLastAction = 1;
          } else {
            printf("Left Miss");
            stateOfLastAction = 2;
          }

        } else if (wParam == VK_RIGHT){
          printf("Pressed right\n");

          if(fallingArrows[1] == 1){
            printf("Right Hit");
            stateOfLastAction = 1;
          } else {
            printf("Right Miss");
            stateOfLastAction = 2;
          }
        } else if (wParam == VK_UP){
           printf("Pressed up\n"); 
  
          if(fallingArrows[1] == 2){
            printf("Up Hit");
            stateOfLastAction = 1;
          } else {
            printf("Up Miss");
            stateOfLastAction = 2;
          }
        } else if (wParam == VK_DOWN) {
          printf("Pressed down\n");
          if(fallingArrows[1] == 3){
            printf("Down Hit");
            stateOfLastAction = 1;
          } else {
            printf("Down Miss");
            stateOfLastAction = 2;
          }
        } else {
          printf("Arrow keys only la");
          stateOfLastAction = 0;
        }

        InvalidateRect(hWnd, NULL, FALSE);

        return 0;

    case WM_GETMINMAXINFO:
    {
      /* Prevent our window from being sized too small */
      MINMAXINFO FAR* minMax = (MINMAXINFO FAR*)lParam;
      minMax->ptMinTrackSize.x = 200;
      minMax->ptMinTrackSize.y = 110;

      return 0;
    }

    /* Item from system menu has been invoked */
    case WM_SYSCOMMAND:
    {
      WORD id = wParam;

      switch (id)
      {
        /* Show "about" dialog on about system menu item */
        case ID_HELP_ABOUT:
        {
          ShowAboutDialog(hWnd);
          return 0;
        }
      }
      break;
    }

    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

/* Register a class for our main window */
BOOL RegisterMainWindowClass()
{
  WNDCLASS wc;

  /* Class for our main window */
  wc.style         = 0;
  wc.lpfnWndProc   = &MainWndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = g_hInstance;
  wc.hIcon         = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APPICON));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
  wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
  wc.lpszClassName = MainWndClass;

  return (RegisterClass(&wc)) ? TRUE : FALSE;
}

/* Create an instance of our main window */
HWND CreateMainWindow()
{
  /* Create instance of main window */
  HWND hWnd = CreateWindowEx(0, MainWndClass, MainWndClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 440,
                             NULL, NULL, g_hInstance, NULL);

  if (hWnd)
  {
    /* Add "about" to the system menu */
    HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
    InsertMenu(hSysMenu, 5, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenu(hSysMenu, 6, MF_BYPOSITION, ID_HELP_ABOUT, "About");
  }

  return hWnd;
}
