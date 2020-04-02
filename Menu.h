#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
  BOOLEAN isActive;
  POINT leftPos, rightPos;
  COLORREF Color, HiglightColor;
  LPSTR name;
  int TriggerWindowCode;//код окна которое будет открывать эта кнопка при наведении или нажатии (<0 если она не должна взаимодействовать с окнами)
}MYBUTTON;

typedef struct
{
  BOOLEAN isActive;
  MYBUTTON *buttons;
  int numberOfButtons;
  int WindowCode;//<0 если окно отображается сразу иначе будет отображено только если будет активна кнопка с кодом этого окна
  COLORREF Color;
  POINT leftPos, rightPos;
}MYWINDOW;

typedef struct
{
  MYWINDOW *windows;
  int numberOfWindows;
}MYMENU;

typedef enum
{
  ERR, ESCAPE, RIGHT, LEFT, DOWN, UP, ROTATE_RIGHT, ROTATE_LEFT, ENTER
}MKEY;

MYMENU* GetMenuProperties(FILE* stream);

LPSTR SetActiveElement(MYMENU* menuPtr, MKEY key);

int PaintMenu(MYMENU* menuPtr, HDC hdc);

void DeleteMyMenu(MYMENU* menuPtr);
