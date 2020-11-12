//
// Created by andrey on 12.11.2020.
//

#include "mand.h"

BYTE Frame[FRAME_H][FRAME_W][3];

BYTE Clamp( BYTE val, BYTE min, BYTE max )
{
  return val < min ? min : val > max ? max : val;
}

void Mandel( int x, int y )
{
#define len2(Z) (creal(Z) * creal(Z) + cimag(Z) + cimag(Z))

  double newx = ((double)x / FRAME_W - 0.5) * 4;
  double newy = ((double)y / FRAME_H - 0.5) * 4;

  double complex Z0 = 0 + I * 0;
  double complex c = (double)newx + I * (double)newy;
  double complex Znext;

  for (int i = 0; i < 255; ++i)
  {
    Znext = (Z0 * Z0) + c;
    if (len2(Znext) > 4)
    {
      double mulr = i * sin(i), mulg = cos(i) / (i + 1), mulb = cosh(i);
      
      PutPixel(x, y, (255 - i) * mulr, (255 - i) * mulg, (255 - i) * mulb);
      return;
    }
    Z0 = Znext;
  }
  PutPixel(x, y, 0, 0, 0);

#undef len2
}

void DrawSingle( void )
{
  for (int i = 0; i < FRAME_W; ++i)
    DrawLine(i);
}

void Draw( int is_thr )
{
  FrameInit(0.3 * 255, 0.5 * 255, 0.7 * 255);

  is_thr = Clamp(is_thr, 0, 1);

  if (!is_thr)
    DrawSingle();
}


void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b )
{
  if (y > FRAME_H || x > FRAME_W || x < 0 || y < 0)
    return;

  Frame[y][x][0] = Clamp(b, 0, 255);
  Frame[y][x][1] = Clamp(g, 0, 255);
  Frame[y][x][2] = Clamp(r, 0, 255);
}

void DrawLine( int line )
{
  for (int i = 0; i < FRAME_H; ++i)
    Mandel(line, i);
}

void FrameInit( BYTE r, BYTE g, BYTE b )
{
  for (int i = 0; i < FRAME_W; ++i)
    for (int j = 0; j < FRAME_H; ++j)
      PutPixel(i, j, r, g, b);
}