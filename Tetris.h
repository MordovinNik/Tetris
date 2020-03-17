#pragma once
#include <windows.h>
#include <time.h>

void PaintScene(HDC hdc);

int InitGame(LPSTR MenuFileName);

int MoveFigure(LPARAM buttonCode);

void CloseGame();
