#pragma once
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "Menu.h"

void PaintScene(HDC hdc);

void InitGame();

int InitMenu(LPSTR menuFileName);/*���������� ���� ���, � ����� ������ ������ ���������� ����� � 2 ����
                                 (1 - ���������, 2 - �������������)*/

int GameTick();

int MoveFigure(MKEY key);

void CloseGame();
