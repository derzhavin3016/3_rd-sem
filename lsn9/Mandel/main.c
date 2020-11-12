#include "mand.h"
#include "gui_funcs.h"

int main( int argc, char *argv[] )
{
  if (argc < 2)
  {
    printf("USAGE: ./mand is_thread [threads amount]\n0 - no threads, 1 - threads\n");
    return 1;
  }
  int is_threads = atoi(argv[1]);
  int n_threads = 0;
  if (is_threads)
  {
    if (argc != 3)
    {
      printf("No threads amount as input. Terminated.\n");
      return 0;
    }
    n_threads = atoi(argv[2]);
  }

  // Calculate mandel
  Draw(is_threads, n_threads);

  GlutWork(argc, argv);

  return 0;
}