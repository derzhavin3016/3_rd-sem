#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void Print( int x )
{
  printf("Hello\n");
}

int main( void )
{
  signal(SIGINT, Print);

  while (1)
    pause();
}