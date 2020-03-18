#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
  ERR, ESCAPE, RIGHT, LEFT, DOWN, UP, ROTATE_RIGHT, ROTATE_LEFT, ENTER
}MKEY;

void* GetMenuProperties(FILE* stream);

LPSTR SetActiveElement(void* menuPtr, MKEY key);

int PaintMenu(void* menuPtr, HDC hdc);

void DeleteMyMenu(void* menuPtr);
