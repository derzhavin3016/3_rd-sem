#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

void Print( int x )
{
  printf("Hello\n");
}

void Resize( int x )
{
  // TODO: draw frame on screen by '*'
  struct winsize win;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
  printf("cols = %d\nrows = %d\n", win.ws_col, win.ws_row);
}

int main( void )
{
  signal(SIGINT, Print);
  signal(SIGWINCH, Resize);
  pause();
}