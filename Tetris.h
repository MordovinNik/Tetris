#pragma once
#include <windows.h>
#include <time.h>
#include "Menu.h"

void PaintScene(HDC hdc);

int InitGame();

int InitMenu(LPSTR menuFileName);/*должна быть вызвана 2 раза чтобы проинициализировать начальное меню и внутриигровое,
                                 или в одном файле должно находиться оба меню, разделенных каким-либо символом*/
int GameTick();

int MoveFigure(MKEY key);

void CloseGame();
