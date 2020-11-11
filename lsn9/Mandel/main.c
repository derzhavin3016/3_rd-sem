#include <GL/gl.h>
#include <GL/glut.h>

typedef unsigned char BYTE;

void onDisplay( void )
{
  glClearColor(0.3, 0.5, 0.7, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(0.0f, 0.0f, 1.0f);
  glLineWidth(1);

  glBegin(GL_POLYGON);
  glVertex2f(0, 0.5f);
  glVertex2f(0, -0.5f);
  glVertex2f(0.5f, 0);
  glEnd();

  glFinish();
  glutSwapBuffers();
  glutPostRedisplay();
}

void onKeyboard( BYTE key, int x, int y )
{
  if (key == 27)
    exit(0);
}

void onResize( int newW, int newH )
{
  glViewport(0, 0, newW, newH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
}

int main( int argc, char *argv[] )
{
  glutInit(&argc, argv);
  glutInitWindowSize(200, 200);
  glutInitWindowPosition(0, 0);

  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

  glutCreateWindow("HUI");
  glutKeyboardFunc(onKeyboard);
  glutDisplayFunc(onDisplay);
  glutReshapeFunc(onResize);

  glutMainLoop();

  return 0;
}