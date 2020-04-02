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

typedef struct
{
  MSQUARE square;
  int isEmpty;
}SCENESQUARE;

struct MSCENE
{
  int rightWall, downWall;
  SCENESQUARE sceneSquares[20][10];
  MFIGURE figure;
  int score;
}scene;

#define MBLOCK_SIZE 21

static MYMENU* startMenu = NULL;
static MYMENU* inGameMenu = NULL;
static BOOLEAN isPause = FALSE;///////////////////////////////////////////////////////////
static BOOLEAN gameStarted = TRUE;
static int record = 0;

BOOLEAN CheckSquaresPlacement(MFIGURE figure,int currentSquareIndex)
{
  for (int i = 0; i < currentSquareIndex; i++)
    for (int j = i + 1; j <= currentSquareIndex; j++)
      if (figure.squares[i].leftTop.x == figure.squares[j].leftTop.x &&
        figure.squares[i].leftTop.y == figure.squares[j].leftTop.y)
        return FALSE;
  return TRUE;
}

int CreateFigure()
{
  MFIGURE figure;
  BOOLEAN correct;
  int currentSquareIndex = 2;
  enum { TOP, RIGHT, BOTTOM, LEFT } direction;
  COLORREF color = RGB(rand() % 256, rand() % 256, rand() % 256);
  int length = 1, height = 2;
  int isLength = -1;

  for (int i = 0; i < 4; i++)
    figure.squares[i].color = color;

  //инициализируем положение первых 2 квадратов всегда одинаково,чтобы ускорить и упростить процесс поиска центра

  figure.squares[0].leftTop.x = 4 * MBLOCK_SIZE + 4;
  figure.squares[0].leftTop.y = -3 * MBLOCK_SIZE - 3;

  figure.squares[1].leftTop = figure.squares[0].leftTop;
  figure.squares[1].leftTop.y += MBLOCK_SIZE + 1;

  while (currentSquareIndex < 4)
  {
    isLength = -1;
    correct = TRUE;
    direction = rand() % 4;

    if (currentSquareIndex == 2)
      figure.squares[currentSquareIndex].leftTop = figure.squares[1].leftTop;
    else
      figure.squares[currentSquareIndex].leftTop = figure.squares[1 + ((rand() % 4 > 2) ? 0 : 1)].leftTop;

    switch (direction)//смещаем блок относительно предыдущего в случайную сторону
    {
      case TOP:
        figure.squares[currentSquareIndex].leftTop.y -= MBLOCK_SIZE + 1;
        isLength = -1;
        break;
      case RIGHT:
        figure.squares[currentSquareIndex].leftTop.x += MBLOCK_SIZE + 1;
        length++;
        isLength = 1;
        break;
      case BOTTOM:
        figure.squares[currentSquareIndex].leftTop.y += MBLOCK_SIZE + 1;
        height++;
        isLength = 0;
        break;
      case LEFT:
        figure.squares[currentSquareIndex].leftTop.x -= MBLOCK_SIZE + 1;
        length++;
        isLength = 1;
    }

    //проверяем, что блоки в фигуре не пересеклись

    correct = CheckSquaresPlacement(figure,currentSquareIndex);

    if (correct)
      currentSquareIndex++;
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
  return 0;
}

void InitGame()
{
  scene.downWall = (MBLOCK_SIZE + 1) * 15 - 1;
  scene.rightWall = (MBLOCK_SIZE + 1) * 10 - 1;

  for (int i = 0; i < 20; i++)
    for (int j = 0; j < 10; j++)
      scene.sceneSquares[i][j].isEmpty = TRUE;

  scene.score = 0;
  CreateFigure();
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

  gameStarted = FALSE;
  isPause = FALSE;

  return 0;
}

void CloseGame()
{
  DeleteMyMenu(startMenu);
  DeleteMyMenu(inGameMenu);
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
  if (!gameStarted)
  {
    PaintMenu(startMenu, hdc);
    WCHAR str[11];
    _itow_s(record, str, 10, 10);
    TextOutW(hdc, 120, 0, str, lstrlenW(str));
  }
  else if (isPause)
    PaintMenu(inGameMenu, hdc);
  else
  {
    for (int i = 0; i < 20; i++)
      for (int j = 0; j < 10; j++)
        if (!scene.sceneSquares[i][j].isEmpty)
          PaintSquare(hdc, scene.sceneSquares[i][j].square);

    for (int i = 0; i < 4; i++)
      PaintSquare(hdc, scene.figure.squares[i]);
    //SetPixel(hdc, scene.figure.center.x, scene.figure.center.y, RGB(255, 0, 0));
    MoveToEx(hdc, scene.rightWall - 1, 0, NULL);
    LineTo(hdc, scene.rightWall - 1, scene.downWall - 1);
    LineTo(hdc, 0, scene.downWall - 1);
    WCHAR str[11];
    _itow_s(scene.score, str, 10, 10);
    TextOutW(hdc, scene.rightWall + 2, 0, str, lstrlenW(str));
  }
}

int MoveFigure(MKEY key)
{
  //если игра на паузе, то передаем все нажатия в меню

  if (key == ESCAPE)
  {
    isPause = TRUE;
  }

  if (isPause || !gameStarted)
  {
    LPSTR str;

    if (gameStarted)
      str = SetActiveElement(inGameMenu, key);
    else
      str = SetActiveElement(startMenu, key);
    if (!str)
      return 0;

    if (!(strcmp(str, "Start") && strcmp(str, "Continue")))
    {
      if (!gameStarted)
        InitGame();
      gameStarted = TRUE;
      isPause = FALSE;
      
    }

    if (!strcmp(str, "Exit"))
    {
      if (gameStarted)
      {
        gameStarted = FALSE;
        return 0;
      }
      else
      {
        CloseGame();
        return -1;
      }
    }
    return 0;
    //добавить выход из игры
  }
  //если игра не на паузе, то перемещаем фигуру, 

  MFIGURE figure = scene.figure;//создаем копию изначальной фигуры, чтобы откатить изменения было проще при возникновыении ошибок
  POINT offset = { 0,0 };
  POINT vector;
  int indexI, indexJ;

  switch (key)
  {
    case RIGHT:
      offset.x = MBLOCK_SIZE + 1;
      break;
    case LEFT:
      offset.x = -MBLOCK_SIZE - 1;
      break;
    case DOWN:
      offset.y = MBLOCK_SIZE + 1;
      break;
    case UP:
      offset.y = -MBLOCK_SIZE - 1;
      break;
    case ROTATE_RIGHT:
      for (int i = 0; i < 4; i++)
      {
        vector.x = figure.squares[i].leftTop.x - figure.center.x;
        vector.y = figure.squares[i].leftTop.y - figure.center.y;
        figure.squares[i].leftTop.x = figure.center.x - vector.y;
        figure.squares[i].leftTop.y = figure.center.y + vector.x;
        figure.squares[i].leftTop.x -= MBLOCK_SIZE - 1;
      }
      break;
    case ROTATE_LEFT:
      for (int i = 0; i < 4; i++)
      {
        vector.x = figure.squares[i].leftTop.x - figure.center.x;
        vector.y = figure.squares[i].leftTop.y - figure.center.y;
        figure.squares[i].leftTop.x = figure.center.x + vector.y;
        figure.squares[i].leftTop.y = figure.center.y - vector.x;
        figure.squares[i].leftTop.y -= MBLOCK_SIZE - 1;
      }
      break;
  }

  figure.center.x += offset.x;
  figure.center.y += offset.y;

  for (int i = 0; i < 4; i++)
  {
    figure.squares[i].leftTop.x += offset.x;
    figure.squares[i].leftTop.y += offset.y;
  }

  //Нужна проверка расположения фигуры
  for (int i = 0; i < 4; i++)
  {
    if (figure.squares[i].leftTop.x > scene.rightWall || figure.squares[i].leftTop.x < 0 ||
      figure.squares[i].leftTop.y > scene.downWall)
      return 1;

    indexI = figure.squares[i].leftTop.y / (MBLOCK_SIZE + 1);
    indexJ = figure.squares[i].leftTop.x / (MBLOCK_SIZE + 1);

    if (indexI >= 0 && indexJ >= 0 && !scene.sceneSquares[indexI][indexJ].isEmpty)//столкновение с другими блоками
      return 2;
  }

  scene.figure = figure;
  return 0;
}

int GameTick()
{
  int indexI, indexJ;
  int counter;
  if (!isPause && gameStarted)
    if (MoveFigure(DOWN))
    {
      for (int i = 0; i < 4; i++)
      {
        indexI = scene.figure.squares[i].leftTop.y / (MBLOCK_SIZE + 1);
        indexJ = scene.figure.squares[i].leftTop.x / (MBLOCK_SIZE + 1);
        if (indexI < 0)
        {
          gameStarted = FALSE;
          if (scene.score > record)
            record = scene.score;
        }
        scene.sceneSquares[indexI][indexJ].isEmpty = FALSE;
        scene.sceneSquares[indexI][indexJ].square = scene.figure.squares[i];
      }

      if (CreateFigure());//gameover

      for (int i = 0; i < 15; i++)
      {
        counter = 0;
        for (int j = 0; j < 10; j++)
          if (!scene.sceneSquares[i][j].isEmpty)
            counter++;

        if (counter == 10)
        {
          scene.score++;
          for (int j = 0; j < 10; j++)
            scene.sceneSquares[0][j].isEmpty = TRUE;

          for (int k = i; k > 0; k--)
            for (int j = 0; j < 10; j++)
            {
              scene.sceneSquares[k][j].isEmpty = scene.sceneSquares[k - 1][j].isEmpty;
              scene.sceneSquares[k][j].square.leftTop.x = scene.sceneSquares[k - 1][j].square.leftTop.x;
              scene.sceneSquares[k][j].square.leftTop.y = scene.sceneSquares[k - 1][j].square.leftTop.y + MBLOCK_SIZE + 1;
              scene.sceneSquares[k][j].square.color = scene.sceneSquares[k - 1][j].square.color;
            }
          i = 0;
        }
      }
    }
  return 0;
}