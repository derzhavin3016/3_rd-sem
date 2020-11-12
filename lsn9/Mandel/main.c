#include "mand.h"
#include "gui_funcs.h"

int main( int argc, char *argv[] )
{
  if (argc != 2)
  {
    printf("USAGE: ./mand [is_thread]\n0 - no threads, 1 - threads\n");
    return 1;
  }

  // Calculate mandel
  Draw(atoi(argv[1]));

  GlutWork(argc, argv);

  return 0;
}