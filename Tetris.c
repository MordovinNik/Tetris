#include "Tetris.h"
#include "Menu.h"

#define MBLOCK_SIZE 21

typedef struct
{
	POINT leftTop;
	COLORREF color;
}MSQUARE;

typedef struct
{
	MSQUARE squares[4];
	POINT center;
}MFIGURE;

typedef struct
{
	MSQUARE squares[20][10];
	MFIGURE figure;
	BOOLEAN isEmpty[20][10];
  int score;
}MSCENE;
