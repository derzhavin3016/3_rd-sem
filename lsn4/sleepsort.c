#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}


int main( int argc, char *argv[] )
{
  for (int i = 1; i < argc; ++i)
  {
    pid_t pid = fork();

    if (pid == 0)
    {
      int num = atoi(argv[i]);

      usleep(num * 1000);
      printf("%d ", num);
      return 0;
    }
  }

  for (int i = 1; i < argc; ++i)
  {
    int status = 0;
    if (wait(&status) < 0)
      return MyErr("Process error: ");
  }
  printf("\n");

  return 0;
}

