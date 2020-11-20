#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <mqueue.h>

#define BUFFER_SIZE 4096

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

int main( int argc, char *argv[] )
{
  if (argc != 3)
  {
    printf("USAGE: ./mq INPUT_FILE OUTPUT_FILE\n");
    return 0;
  }

  return 0;
}