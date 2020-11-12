#include "gui_funcs.h"

double ZoomX = (double)WND_W / FRAME_W, ZoomY = (double)WND_H / FRAME_H;

void onDisplay( void )
{
  glClearColor(0.3, 0.5, 0.7, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glRasterPos2d(-1, 1);
  glPixelZoom(ZoomX, -ZoomY);
  glDrawPixels(FRAME_W, FRAME_H, GL_BGR_EXT, GL_UNSIGNED_BYTE, Frame);

  glFinish();
  //glutPostRedisplay();
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