#pragma once
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "Menu.h"

void PaintScene(HDC hdc);

void InitGame();

int InitMenu(LPSTR menuFileName);/*вызывается один раз, в файле должна лежать информация сразу о 2 меню
                                 (1 - стартовое, 2 - внутриигровое)*/

int GameTick();

int MoveFigure(MKEY key);

void CloseGame();
