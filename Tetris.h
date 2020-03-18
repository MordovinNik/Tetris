#pragma once
#include <windows.h>
#include <time.h>
#include "Menu.h"

void PaintScene(HDC hdc);

int InitGame();

int InitMenu(LPSTR menuFileName);/*������ ���� ������� 2 ���� ����� ������������������� ��������� ���� � �������������,
                                 ��� � ����� ����� ������ ���������� ��� ����, ����������� �����-���� ��������*/
int GameTick();

int MoveFigure(MKEY key);

void CloseGame();
