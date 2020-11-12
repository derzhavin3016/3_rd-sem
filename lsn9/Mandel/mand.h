#ifndef INC_3_RD_SEM_MAND_H
#define INC_3_RD_SEM_MAND_H

#include <stdio.h>
#include <complex.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char BYTE;

#define FRAME_W  800
#define FRAME_H  800
#define WND_H 800
#define WND_W 800


extern BYTE Frame[FRAME_H * FRAME_W * 3];

BYTE Clamp( BYTE val, BYTE min, BYTE max );

void FrameInit( BYTE r, BYTE g, BYTE b );

void Mandel( int x, int y, BYTE *frame );

void DrawLine( int line );

void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b, BYTE *frame );

void Draw( int is_thr, int n_threads );

void DrawSingle( void );

void *DrawThread( void *params );

#endif //INC_3_RD_SEM_MAND_H