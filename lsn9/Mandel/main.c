#include "mand.h"
#include "gui_funcs.h"

int main( int argc, char *argv[] )
{
  if (argc < 2)
  {
    printf("USAGE: ./mand THREADS_AMOUNT\n");
    return 1;
  }
  int n_threads = atoi(argv[1]);
  if (n_threads <= 0)
  {
    printf("Incorrect amount of threads: %d", n_threads);
    return 0;
  }


  // start measure
  timespc t_beg = {};
  clock_gettime(CLOCK_MONOTONIC, &t_beg);

  // Calculate mandel
  Draw(n_threads);

  timespc t_end = {};
  clock_gettime(CLOCK_MONOTONIC, &t_end);

  int exec_sec =  t_end.tv_sec - t_beg.tv_sec;
  int exec_nano = t_end.tv_nsec - t_beg.tv_nsec;

  double exec_time = exec_sec + exec_nano * 1e-9;

  printf("Mandel calculated.\nCalculation time: %.03lf s.\n", exec_time);
  GlutWork(argc, argv);

  return 0;
}