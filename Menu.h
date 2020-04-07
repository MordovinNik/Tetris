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
  int TriggerWindowCode;//��� ���� ������� ����� ��������� ��� ������ ��� ��������� ��� ������� (<0 ���� ��� �� ������ ����������������� � ������)
}MYBUTTON;

typedef struct
{
  BOOLEAN isActive;
  MYBUTTON *buttons;
  int numberOfButtons;
  int WindowCode;//<0 ���� ���� ������������ ����� ����� ����� ���������� ������ ���� ����� ������� ������ � ����� ����� ����
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
