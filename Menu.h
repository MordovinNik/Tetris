#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

void* GetMenuProperties(FILE* stream);

LPSTR SetActiveElement(void* menuPtr, WPARAM key);

int PaintMenu(void* menuPtr, HDC hdc);

void DeleteMyMenu(void* menuPtr);
