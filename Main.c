// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#pragma warning(disable:4100)
#include "Tetris.h"
///////////////////////////////////// глобальные переменные
void* Menu;
LPSTR buttonName = NULL;
HWND hwnd;
TCHAR szClassName[] = L"Window1";

///////////////////////////////////// прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, UINT, LONG);

ATOM registerMyClass(HINSTANCE hInstance);

int createMyWindow(HINSTANCE hInstance, int nCmdShow);
//////////////////////////////////////

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpszCmdParam, _In_ int nCmdShow)
{
  InitGame();


  createMyWindow(hInstance, nCmdShow);

  MSG msg;
  while (GetMessage(&msg, 0, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  MKEY key;
  switch (msg)
  {
    case WM_COMMAND:
    {
      /*switch (LOWORD(wParam))
      {
        break;
      }*/
      break;
    }
    case WM_KEYDOWN:
      switch (wParam)
      {
        case VK_DOWN:
          key = DOWN;
          break;
        case VK_UP:
          key = UP;
          break;
        case VK_LEFT:
          key = LEFT;
          break;
        case VK_RIGHT:
          key = RIGHT;
          break;
        case VK_ESCAPE:
          key = ESCAPE;
          break;
        case VK_RETURN:
          key = ENTER;
          break;
        case 88:
          key = ROTATE_RIGHT;
          break;
        case 90:
          key = ROTATE_LEFT;
          break;
      }
      break;

    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      PaintScene(hdc);

      EndPaint(hWnd, &ps);
    }
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

int createMyWindow(HINSTANCE hInstance, int nCmdShow)
{
  registerMyClass(hInstance);

  hwnd = CreateWindow(szClassName, L"task 8", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, NULL, NULL, hInstance, NULL);

  if (!hwnd) { return 0; }
  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);
  return 0;
}

ATOM registerMyClass(HINSTANCE hInstance)
{
  WNDCLASS wc;
  wc.cbClsExtra = wc.cbWndExtra = 0;
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = szClassName;

  return RegisterClass(&wc);
}