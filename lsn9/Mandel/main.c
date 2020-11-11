#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <complex.h>

#pragma STDC CX_LIMITED_RANGE on

typedef unsigned char BYTE;

#define FRAME_W  800
#define FRAME_H  800
#define WND_H 800
#define WND_W 800

double ZoomX = (double)WND_W / FRAME_W, ZoomY = (double)WND_H / FRAME_H;

BYTE Frame[FRAME_H][FRAME_W][3];

void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b );

void FrameInit( BYTE r, BYTE g, BYTE b )
{
  for (int i = 0; i < FRAME_W; ++i)
    for (int j = 0; j < FRAME_H; ++j)
      PutPixel(i, j, r, g, b);
}

#define len2(Z) (creal(Z) * creal(Z) + cimag(Z) + cimag(Z))

void Mandel( int x, int y )
{
  double newx = ((double)x / FRAME_W - 0.5) * 4;
  double newy = ((double)y / FRAME_H - 0.5) * 4;

  double complex Z0 = 0 + I * 0;
  double complex c = (double)newx + I * (double)newy;
  double complex Znext;

  for (int i = 0; i < 125; ++i)
  {
    Znext = (Z0 * Z0) + c;
    if (len2(Znext) > 4)
      return;
    Z0 = Znext;
  }
  PutPixel(x, y, 0, 0, 0);
}

void DrawLine( int line )
{
  for (int i = 0; i < FRAME_H; ++i)
    Mandel(line, i);
}

void Draw( void )
{
  FrameInit(0.3 * 255, 0.5 * 255, 0.7 * 255);

  for (int i = 0; i < FRAME_W; ++i)
    DrawLine(i);
}


void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b )
{
  if (y > FRAME_H || x > FRAME_W || x < 0 || y < 0)
    return;

  Frame[y][x][0] = b;
  Frame[y][x][1] = g;
  Frame[y][x][2] = r;
}



void onDisplay( void )
{
  glClearColor(0.3, 0.5, 0.7, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glRasterPos2d(-1, 1);

  Draw();
  glPixelZoom(ZoomX, -ZoomY);
  glDrawPixels(FRAME_W, FRAME_H, GL_BGR_EXT, GL_UNSIGNED_BYTE, Frame);

  glFinish();
  glutPostRedisplay();
  glutSwapBuffers();
}

void onKeyboard( BYTE key, int x, int y )
{
  if (key == 27)
    exit(0);
}

void onResize( int newW, int newH )
{
  //ZoomX = 1 / ((double)newW / WND_W);
  //ZoomY = 1 / ((double)newH / WND_H);

  glViewport(0, 0, newW, newH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
}

int main( int argc, char *argv[] )
{
  glutInit(&argc, argv);
  glutInitWindowSize(WND_W, WND_H);
  glutInitWindowPosition(0, 0);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);



  glutCreateWindow("HUI");
  glutKeyboardFunc(onKeyboard);
  glutDisplayFunc(onDisplay);
  glutReshapeFunc(onResize);

  glutMainLoop();

  return 0;
}