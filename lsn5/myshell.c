#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>

#define BUFFER_SIZE 4096

#define MAX_ACCESS 0777

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}


int main( int argc, char *argv[] )
{
  printf("\e[32;1m$ \e[m");
  return 0;
}


