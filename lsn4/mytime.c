#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

typedef struct timespec timespc;

int main( int argc, char *argv[] )
{
  if (argc == 1)
    return 1;

  timespc t_beg = {}, t_end = {};

  clock_gettime(CLOCK_MONOTONIC, &t_beg);
  pid_t pid = fork();

  if (pid == 0)
  {
    execvp(argv[1], argv + 1);
    exit(0);
  }
  int status = 0;

  if (wait(&status) < 0)
    return MyErr("Proc err");

  clock_gettime(CLOCK_MONOTONIC, &t_end);

  int exec_sec =  t_end.tv_sec - t_beg.tv_sec;
  int exec_nano = t_end.tv_nsec - t_beg.tv_nsec;

  double exec_time = exec_sec + exec_nano * 1e-9;

  printf("Process %s executed.\nExecution time: %.03lf s.\n",
         argv[1], exec_time);

  return 0;
}

