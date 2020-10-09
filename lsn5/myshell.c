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

struct String
{
  char *str;
  size_t len;
};

$ ls -a | wc

for (cmd = strtok(line, |))
{
  for (a = strtok(cmd, " \t"))
}

// TODO: man strtok_r

int main( void )
{
  char buffer[BUFFER_SIZE];
  printf("\e[32;1m$ \e[m");
  scanf("%s", buffer);
  size_t


  CMD_ARGS *args = ParseStr()

  return 0;
}

// TODO: string parser, execute & create PIPE
//
//



