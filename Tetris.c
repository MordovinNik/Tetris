#include "Tetris.h"

typedef struct
{
  POINT leftTop;
  COLORREF color;
}MSQUARE;//квадратики, из которых состоят фигуры

typedef struct
{
  MSQUARE squares[4];
  POINT center;
}MFIGURE;

struct MSCENE
{
  MSQUARE squares[20][10];
  MFIGURE figure;
  BOOLEAN isEmpty[20][10];
  int score;
}scene;

#define MBLOCK_SIZE 21

void* startMenu = NULL;
void* inGameMenu = NULL;

void InitGame()
{

  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++)
      scene.isEmpty[i][j] = TRUE;

  scene.score = 0;

}

int InitMenu(LPSTR menuFileName)
{
  //открываем файл и передаем поток в функцию получающую параметрвы меню из файла
  FILE* stream;
  fopen_s(&stream, menuFileName, "r");

  if (!stream)
    return -1;

  startMenu = GetMenuProperties(stream);
  inGameMenu = GetMenuProperties(stream);

  if (!(startMenu && inGameMenu))
    return -2;

  return 0;
}

void CloseGame()
{
  DeleteMyMenu(startMenu);
  DeleteMyMenu(inGameMenu);
}

void CreateFigure()
{
  MFIGURE figure;
  BOOLEAN correct;
  int index = 2;
  enum { TOP, RIGHT, BOTTOM, LEFT } direction;
  COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
  int length = 1, height = 2;
  int isLength = -1;

  for (int i = 0; i < 4; i++)
    figure.squares[i].color = color;

  //инициализируем положение первых 2 квадратов всегда одинаково,чтобы ускорить и упростить процесс поиска центра

  figure.squares[0].leftTop.x = 100;
  figure.squares[0].leftTop.y = 50;

  figure.squares[1].leftTop = figure.squares[0].leftTop;
  figure.squares[1].leftTop.y += MBLOCK_SIZE + 1;

  while (index < 4)
  {
    isLength = -1;
    correct = TRUE;
    direction = rand() % 4;

    if (index == 2)
      figure.squares[index].leftTop = figure.squares[1].leftTop;
    else
      figure.squares[index].leftTop = figure.squares[1 + rand() % 2].leftTop;

    switch (direction)//смещаем блок относительно предыдущего в случайную сторону
    {
      case TOP:
        figure.squares[index].leftTop.y -= MBLOCK_SIZE + 1;
        isLength = -1;
        break;
      case RIGHT:
        figure.squares[index].leftTop.x += MBLOCK_SIZE + 1;
        length++;
        isLength = 1;
        break;
      case BOTTOM:
        figure.squares[index].leftTop.y += MBLOCK_SIZE + 1;
        height++;
        isLength = 0;
        break;
      case LEFT:
        figure.squares[index].leftTop.x -= MBLOCK_SIZE + 1;
        length++;
        isLength = 1;
    }

    //проверяем, что блоки в фигуре не пересеклись

    for (int i = 0; i < index; i++)
      for (int j = i + 1; j <= index; j++)
        if (figure.squares[i].leftTop.x == figure.squares[j].leftTop.x &&
          figure.squares[i].leftTop.y == figure.squares[j].leftTop.y)
          correct = FALSE;

    if (correct)
      index++;
    else
    {
      if (isLength == 1)
        length--;
      if (!isLength)
        height--;
    }

  }

  //поиск центра фигуры, относительно которого будет производиться поворот
  figure.center.x = figure.squares[1].leftTop.x + MBLOCK_SIZE / 2;
  figure.center.y = figure.squares[1].leftTop.y + MBLOCK_SIZE / 2;

  POINT point = figure.squares[3].leftTop;

  if (point.x < figure.squares[1].leftTop.x)//отражаем квадрат относительно центра, чтобы он был справа
  {
    point.x += 2 * (MBLOCK_SIZE + 1);

    if (point.x == figure.squares[1].leftTop.x)
      point.x += 2 * (MBLOCK_SIZE + 1);
  }

  if (point.x - figure.center.x > MBLOCK_SIZE || abs(point.y - figure.center.y) > MBLOCK_SIZE)
  {
    figure.center.x = figure.squares[2].leftTop.x + MBLOCK_SIZE / 2;
    figure.center.y = figure.squares[2].leftTop.y + MBLOCK_SIZE / 2;
  }

  if (length == height)
  {
    if (figure.squares[2].leftTop.x > figure.squares[1].leftTop.x)
    {
      figure.center.x = figure.squares[2].leftTop.x - 1;
      figure.center.y = figure.squares[2].leftTop.y - 1;
    }
    else
    {
      figure.center.x = figure.squares[1].leftTop.x - 1;
      figure.center.y = figure.squares[1].leftTop.y - 1;
    }
  }

  scene.figure = figure;
}

void PaintSquare(HDC hdc, MSQUARE square)
{
  HBRUSH hBrush = CreateSolidBrush(square.color);
  HBRUSH hOldBrush = SelectObject(hdc, hBrush);

  Rectangle(hdc, square.leftTop.x, square.leftTop.y, square.leftTop.x + MBLOCK_SIZE, square.leftTop.y + MBLOCK_SIZE);
  DeleteObject(hBrush);
  DeleteObject(hOldBrush);
}

void PaintScene(HDC hdc)
{
  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++)
      if (!scene.isEmpty)
        PaintSquare(hdc, scene.squares[i][j]);

  for (int i = 0; i < 4; i++)
    PaintSquare(hdc, scene.figure.squares[i]);
}
