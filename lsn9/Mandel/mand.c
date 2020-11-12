//
// Created by andrey on 12.11.2020.
//

#include "mand.h"

BYTE Frame[FRAME_H * FRAME_W * 3];

BYTE Clamp( BYTE val, BYTE min, BYTE max )
{
  return val < min ? min : val > max ? max : val;
}

void Mandel( int x, int y, BYTE *frame )
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

      PutPixel(x, y, (255 - i) * mulr, (255 - i) * mulg, (255 - i) * mulb, frame);
      return;
    }
    Z0 = Znext;
  }
  PutPixel(x, y, 0, 0, 0, frame);

#undef len2
}

void DrawSingle( void )
{
  for (int i = 0; i < FRAME_W; ++i)
    DrawLine(i);
}

pthread_mutex_t pmut = PTHREAD_MUTEX_INITIALIZER;

void Draw( int is_thr, int n_threads )
{
  FrameInit(0.3 * 255, 0.5 * 255, 0.7 * 255);

  is_thr = Clamp(is_thr, 0, 1);

  if (!is_thr)
  {
    DrawSingle();
    return;
  }
  pthread_t threads[n_threads];
  int step = (double)FRAME_H / n_threads;
  int bounds0[2] = {0, step + FRAME_H % n_threads};
  int bounds[2] = {bounds0[1], bounds0[1] + step};

  pthread_mutex_unlock(&pmut);

  pthread_create(threads, NULL, DrawThread, bounds0);

  for (int i = 1; i < n_threads; ++i)
  {
    pthread_create(threads + i, NULL, DrawThread, bounds);
    bounds[0] += step;
    bounds[1] += step;
  }


  for (int i = 0; i < n_threads; ++i)
    pthread_join(threads[i], NULL);
}

void *DrawThread( void *params )
{
  int start = ((int *)params)[0];
  int end = ((int *)params)[1];

  size_t size = (end - start) * FRAME_W * 3;
  BYTE loc_frame[size];

  for (int i = start; i < end; ++i)
    for (int j = 0; j < FRAME_W; ++j)
      Mandel(i, j, loc_frame);

  // Critical section
  pthread_mutex_lock(&pmut);
  //memcpy(Frame + start * FRAME_W * 3, loc_frame, size);
  pthread_mutex_unlock(&pmut);

  return 0;
}

void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b, BYTE *frame )
{
  if (y > FRAME_H || x > FRAME_W || x < 0 || y < 0)
    return;

  frame[y * FRAME_W  * 3 + x * 3] = Clamp(b, 0, 255);
  frame[y * FRAME_W  * 3 + x * 3 + 1] = Clamp(g, 0, 255);
  frame[y * FRAME_W  * 3 + x * 3 + 2] = Clamp(r, 0, 255);
}

void DrawLine( int line )
{
  for (int i = 0; i < FRAME_H; ++i)
    Mandel(line, i, Frame);
}

void FrameInit( BYTE r, BYTE g, BYTE b )
{
  for (int i = 0; i < FRAME_W; ++i)
    for (int j = 0; j < FRAME_H; ++j)
      PutPixel(i, j, r, g, b, Frame);
}