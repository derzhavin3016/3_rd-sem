#include "mand.h"
#include "gui_funcs.h"

int main( int argc, char *argv[] )
{
  glutInit(&argc, argv);
  glutInitWindowSize(WND_W, WND_H);
  glutInitWindowPosition(0, 0);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  Draw(0);


  glutCreateWindow("HUI");
  glutKeyboardFunc(onKeyboard);
  glutDisplayFunc(onDisplay);
  glutReshapeFunc(onResize);

  glutMainLoop();

  return 0;
}