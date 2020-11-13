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

  double complex Z = 0 + I * 0;
  double complex c = (double)newx + I * (double)newy;

  for (int i = 0; i < 255; ++i)
  {
    Z = Z * Z + c;
    if (len2(Z) > 4)
    {
      double mulr = i * sin(i), mulg = cos(i) / (i + 1), mulb = sinh(i);

      PutPixel(x, y, (255 - i) * mulr, (255 - i) * mulg, (255 - i) * mulb);
      return;
    }
  }
  PutPixel(x, y, 0, 0, 0);

#undef len2
}

void DrawMand( int start, int end )
{
  for (int i = start; i < end; ++i)
    for (int j = 0; j < FRAME_W; ++j)
      Mandel(j, i);
}

//pthread_mutex_t pmut = PTHREAD_MUTEX_INITIALIZER;

void FillBounds( int *bounds, int step, int size )
{
  for (int i = 2; i < size; ++i)
    bounds[i] = bounds[i - 1] + step;
}

void Draw( int n_threads )
{
  //FrameInit(0.3 * 255, 0.5 * 255, 0.7 * 255);

  pthread_t threads[n_threads];
  int step = (double)FRAME_H / n_threads;
  int bounds[n_threads + 1];
  bounds[0] = 0;
  bounds[1] = step + FRAME_H % n_threads;
  FillBounds(bounds, step, n_threads + 1);

  for (int i = 0; i < n_threads; ++i)
    pthread_create(threads + i, NULL, DrawThread, bounds + i);

  for (int i = 0; i < n_threads; ++i)
    pthread_join(threads[i], NULL);
}

void *DrawThread( void *params )
{
  int start = ((int *)params)[0];
  int end = ((int *)params)[1];

  DrawMand(start, end);

  return 0;
}

void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b )
{
  if (y > FRAME_H || x > FRAME_W || x < 0 || y < 0)
    return;

  Frame[y][x][0] = Clamp(b, 0, 255);
  Frame[y][x][1] = Clamp(g, 0, 255);
  Frame[y][x][2] = Clamp(r, 0, 255);
}

void FrameInit( BYTE r, BYTE g, BYTE b )
{
  for (int i = 0; i < FRAME_W; ++i)
    for (int j = 0; j < FRAME_H; ++j)
      PutPixel(i, j, r, g, b);
}