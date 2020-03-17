// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Menu.h"

typedef struct
{
  BOOLEAN isActive;
  POINT leftPos, rightPos;
  COLORREF Color, HiglightColor;
  LPSTR name;
  int TriggerWindowCode;//код окна которое будет открывать эта кнопка при наведении или нажатии (<0 если она не должна взаимодействовать с окнами)
}MYBUTTON;

typedef struct
{
  BOOLEAN isActive;
  MYBUTTON *buttons;
  UINT numberOfButtons;
  int WindowCode;//<0 если окно отображается сразу иначе будет отображено только если будет активна кнопка с кодом этого окна
  COLORREF Color;
  POINT leftPos, rightPos;
}MYWINDOW;

typedef struct
{
  MYWINDOW *windows;
  UINT numberOfWindows;
}MYMENU;

enum COMMANDS
{
  FAIL, MENU_BEGIN, WINDOW_BEGIN, BUTTON, WINDOW_END, MENU_END
};

enum COMMANDS GetCommand(LPCSTR str)
{//проверяем команду
  if (!strcmp(str, "MenuBegin"))
    return MENU_BEGIN;
  if (!strcmp(str, "WindowBegin"))
    return WINDOW_BEGIN;
  if (!strcmp(str, "Button"))
    return BUTTON;
  if (!strcmp(str, "WindowEnd"))
    return WINDOW_END;
  if (!strcmp(str, "MenuEnd"))
    return MENU_END;
  return FAIL;
}

int GetDigit(FILE* stream, int* c)
{
  int strIndex = 0;
  char str[100];
  while (!isdigit(*c = getc(stream)) && *c != '-');

  do
  {
    str[strIndex] = *c;
    strIndex++;

  } while (isdigit(*c = getc(stream)));

  str[strIndex] = 0;

  return atoi(str);
}

int GetButtonName(FILE* stream, char* str, int* currentChar)
{
  int i = 0;
  while (*currentChar != '\"')
    if ((*currentChar = getc(stream)) == EOF)
      return -1;

  while ((*currentChar = getc(stream)) != '\"' && i < 257)
  {
    str[i] = *currentChar;
    i++;
  }
  return 0;
}

int GetWindowProperties(FILE* stream, MYWINDOW* wnd, int currentChar)
{
  while (currentChar != '(')
  {
    currentChar = getc(stream);
    if (currentChar == EOF)
      return -1;
  }
  int c;

  wnd->leftPos.x = GetDigit(stream, &c);
  wnd->leftPos.y = GetDigit(stream, &c);
  wnd->rightPos.x = GetDigit(stream, &c);
  wnd->rightPos.y = GetDigit(stream, &c);
  wnd->Color = RGB(GetDigit(stream, &c), GetDigit(stream, &c), GetDigit(stream, &c));
  wnd->WindowCode = GetDigit(stream, &c);

  if (wnd->WindowCode != -1)
    wnd->isActive = FALSE;
  else
    wnd->isActive = TRUE;

  if (c == ')')
    return 0;
  else
    while ((c = getc(stream)) != ')')
      if (c == EOF)
        return -1;
  return 0;
}

int GetButtonProperties(FILE* stream, MYBUTTON* button, int currentChar)
{
  while (currentChar != '(')
    if ((currentChar = getc(stream)) != EOF)
      return -1;
  int c;

  button->leftPos.x = GetDigit(stream, &c);
  button->leftPos.y = GetDigit(stream, &c);
  button->rightPos.x = GetDigit(stream, &c);
  button->rightPos.y = GetDigit(stream, &c);
  button->Color = RGB(GetDigit(stream, &c), GetDigit(stream, &c), GetDigit(stream, &c));
  button->HiglightColor = RGB(GetDigit(stream, &c), GetDigit(stream, &c), GetDigit(stream, &c));
  GetButtonName(stream, button->name, &c);
  button->TriggerWindowCode = GetDigit(stream, &c);
  button->isActive = FALSE;

  if (c == ')')
    return 0;
  else
    while ((c = getc(stream)) != ')')
      if (c == EOF)
        return -1;
  return 0;
}

void DeleteMyMenu(void *menuVoid)
{
  MYMENU *menu = (MYMENU*)menuVoid;
  for (int i = 0; i < menu->numberOfWindows; i++)
  {
    for (int j = 0; j < menu->windows[i].numberOfButtons; j++)
    {
      free(menu->windows[i].buttons[j].name);
    }
    free(menu->windows[i].buttons);
  }
  free(menu);
}

void* GetMenuProperties(FILE* stream)
{
  //инициализируем структуру меню
  MYMENU *Menu = malloc(sizeof(MYMENU));

  if (!Menu)
    return NULL;

  Menu->numberOfWindows = 0;
  Menu->windows = malloc(sizeof(MYWINDOW));

  if (!Menu->windows)
  {
    free(Menu);
    return 0;
  }


  //начинаем читать файл, записываем попавшиеся слова и сравниваем их с командами 

  enum COMMANDS cmd;
  char str[30];
  int c;
  UINT strIndex = 0;
  BOOLEAN isWord = FALSE,
    MenuStarted = FALSE,
    isWindow = FALSE;

  while (1)
  {
    c = getc(stream);

    if (!isWord && isalpha(c))
      isWord = TRUE;

    if (isWord && !isalpha(c))
      isWord = FALSE;

    if (isWord)
    {
      str[strIndex] = (char)c;
      strIndex++;
    }
    else if (strIndex)//слово записалось
    {
      str[strIndex] = 0;
      strIndex = 0;
      cmd = GetCommand(str);

      //в зависимости от команды запускаем ее обработчик который получит параметры и заполнит структуру
      switch (cmd)
      {
        case FAIL:
          break;
        case MENU_BEGIN:
          MenuStarted = TRUE;
          break;
        case WINDOW_BEGIN:
          if (MenuStarted == TRUE)
          {
            //инициализация параметров окна (вт.ч. выделение памяти под кнопки и инициализация количества)

            isWindow = TRUE;
            Menu->numberOfWindows++;
            void* l = realloc(Menu->windows, Menu->numberOfWindows * sizeof(MYWINDOW));

            if (!l)
            {
              DeleteMyMenu(Menu);
              return NULL;
            }

            Menu->windows = l;

            int index = Menu->numberOfWindows - 1;

            Menu->windows[index].buttons = malloc(sizeof(MYBUTTON));
            Menu->windows[index].numberOfButtons = 0;

            if (!Menu->windows[index].buttons)
              return NULL;


            //вызвать обработчик окна
            if (GetWindowProperties(stream, &Menu->windows[Menu->numberOfWindows - 1], c))
              return NULL;
          }
          break;
        case BUTTON:
          if (MenuStarted == TRUE && isWindow)
          {
            //инициализация кнопки и увеличение массива кнопок

            int index = Menu->numberOfWindows - 1;
            Menu->windows[index].numberOfButtons++;

            void* l = realloc(Menu->windows[index].buttons, sizeof(MYBUTTON)*Menu->windows[index].numberOfButtons);

            if (!l)
              return NULL;

            Menu->windows[index].buttons = l;

            Menu->windows[index].buttons[Menu->windows[index].numberOfButtons - 1].name = calloc(257, 1);

            if (!Menu->windows[index].buttons[Menu->windows[index].numberOfButtons - 1].name)
              return NULL;

            //вызывать обработчик кнопки
            if (GetButtonProperties(stream, &Menu->windows[index].buttons[Menu->windows[index].numberOfButtons - 1], c))
              return NULL;
          }
          break;
        case WINDOW_END:
          if (MenuStarted == TRUE)
            isWindow = FALSE;
          break;
        case MENU_END:
          if (MenuStarted == TRUE)
          {
            return Menu;
          }
          //выход из функции
          break;
      }
    }

    if (c == EOF)
      break;
  }

  return Menu;
}

void SetWindowActive(MYMENU* menu, int code, BOOLEAN isActive)
{
  for (int i = 0; i < menu->numberOfWindows; i++)
    if (menu->windows[i].WindowCode == code)
      menu->windows[i].isActive = isActive;
}

LPSTR SetActiveElement(void* menuV, WPARAM key)
{
  MYMENU *menu = (MYMENU*)menuV;
  int indexI = -1, indexJ = -1;

  //находим текущий активный элемент
  for (int i = 0; i < menu->numberOfWindows; i++)
  {
    if (menu->windows[i].isActive)
      for (int j = 0; j < menu->windows[i].numberOfButtons; j++)
        if (menu->windows[i].buttons[j].isActive)
        {
          indexI = i;
          indexJ = j;
        }
  }

  if (indexI == -1)
  {
    for (int i = 0; i < menu->numberOfWindows; i++)
    {
      if (menu->windows[i].isActive && menu->windows[i].numberOfButtons)
      {
        menu->windows[i].buttons[0].isActive = TRUE;
        indexI = i; indexJ = 0;
        if (key == VK_LEFT || key == VK_RIGHT)
          break;
        else
          return NULL;
      }
    }
  }
  if (indexI != -1)
    switch (key)
    {
      case VK_LEFT:
        if (menu->windows[indexI].WindowCode != -1)
        {
          //закрываем текущее активное окно, если оно было вызвано одной из кнопок и делаем активной ту кнопку
          //menu->windows[indexI].isActive = FALSE;
          menu->windows[indexI].buttons[indexJ].isActive = FALSE;

          if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)//если кнопка делала активным окно, закрываем его
            SetWindowActive(menu, menu->windows[indexI].buttons[indexJ].TriggerWindowCode, FALSE);

          for (int i = 0; i < menu->numberOfWindows; i++)
          {
            for (int j = 0; j < menu->windows[i].numberOfButtons; j++)
              if (menu->windows[i].buttons[j].TriggerWindowCode == menu->windows[indexI].WindowCode)
                menu->windows[i].buttons[j].isActive = TRUE;
          }
        }
        break;
      case VK_RIGHT:
        //переходим к первой кнопке правого окна
        if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)
        {
          int code = menu->windows[indexI].buttons[indexJ].TriggerWindowCode;
          //найти окно с этим кодом и сделать его активынм

          for (int i = 0; i < menu->numberOfWindows; i++)
            if (menu->windows[i].WindowCode == code)
            {
              menu->windows[i].isActive = TRUE;
              menu->windows[i].buttons[0].isActive = TRUE;
              menu->windows[indexI].buttons[indexJ].isActive = FALSE;
              break;
            }
          break;
        }
        break;

      case VK_DOWN:

        //сделать активной кнопку ниже
        menu->windows[indexI].buttons[indexJ].isActive = FALSE;
        if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)//если кнопка делала активным окно, закрываем его
          SetWindowActive(menu, menu->windows[indexI].buttons[indexJ].TriggerWindowCode, FALSE);

        indexJ++;

        if (indexJ >= menu->windows[indexI].numberOfButtons)
          indexJ = 0;

        menu->windows[indexI].buttons[indexJ].isActive = TRUE;

        if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)
          SetWindowActive(menu, menu->windows[indexI].buttons[indexJ].TriggerWindowCode, TRUE);

        break;

      case VK_UP:

        menu->windows[indexI].buttons[indexJ].isActive = FALSE;
        if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)
          SetWindowActive(menu, menu->windows[indexI].buttons[indexJ].TriggerWindowCode, FALSE);
        indexJ--;

        if (indexJ < 0)
          indexJ = menu->windows[indexI].numberOfButtons - 1;

        menu->windows[indexI].buttons[indexJ].isActive = TRUE;
        if (menu->windows[indexI].buttons[indexJ].TriggerWindowCode != -1)
          SetWindowActive(menu, menu->windows[indexI].buttons[indexJ].TriggerWindowCode, TRUE);
        break;

      case VK_RETURN:

        return menu->windows[indexI].buttons[indexJ].name;
        break;
    }
  return NULL;
}

int PaintMenu(void* menuVoid, HDC hdc)
{
  MYMENU * menu = (MYMENU*)menuVoid;

  POINT left, right;

  for (int i = 0; i < menu->numberOfWindows; i++)
  {
    if (!menu->windows[i].isActive)
      continue;

    left = menu->windows[i].leftPos;
    right = menu->windows[i].rightPos;

    //нарисовать окно
    HBRUSH hWindowBrush = CreateSolidBrush(menu->windows[i].Color);
    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

    SelectObject(hdc, hBorderPen);
    Rectangle(hdc, left.x, left.y, right.x, right.y);
    SelectObject(hdc, hWindowBrush);
    Rectangle(hdc, left.x + 1, left.y - 1, right.x - 1, right.y + 1);

    for (int j = 0; j < menu->windows[i].numberOfButtons; j++)
    {
      //нарисовать кнопку
      MYBUTTON button = menu->windows[i].buttons[j];
      HBRUSH hButtonBrush;
      HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

      if (button.isActive)
      {
        hButtonBrush = CreateSolidBrush(button.HiglightColor);
        SetBkColor(hdc, button.HiglightColor);
      }
      else
      {
        hButtonBrush = CreateSolidBrush(button.Color);
        SetBkColor(hdc, button.Color);
      }

      SelectObject(hdc, hBorderPen);
      Rectangle(hdc, button.leftPos.x, button.leftPos.y, button.rightPos.x, button.rightPos.y);
      SelectObject(hdc, hButtonBrush);
      Rectangle(hdc, button.leftPos.x + 1, button.leftPos.y - 1, button.rightPos.x - 1, button.rightPos.y + 1);
      TextOutA(hdc, button.leftPos.x + 2, button.leftPos.y + 2, button.name, strlen(button.name));
    }
  }
  return 0;
}
