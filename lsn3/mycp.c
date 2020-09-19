#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

// Keys bits constants
#define F_KEY 0x1
#define I_KEY 0x2
#define V_KEY 0x4

// useful typedef
typedef struct option OPT;

enum
{
  INV_KEY = -2,
  END_OF_KEYS = -1
};

const OPT OPTIONS[] =
  {
    {"force", 0, NULL, 'f'},
    {"interactive", 0, NULL, 'i'},
    {"verbose", 0, NULL, 'v'},
  };
const char OPT_STR[] = "fiv";

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

int MyWrite( int fd, void *buffer, size_t buf_size )
{
  size_t bytes_written = 0;

  while (bytes_written < buf_size)
  {
    int write_ret = write(fd, buffer, buf_size - bytes_written);

    if (write_ret < 0)
      return MyErr("Error with writing to stdout");

    bytes_written += write_ret;
    buffer += write_ret;
  }

  return 0;
}

int CopyFile( int fd_src, int fd_dest )
{
  int bytes_read = 0;

  do
  {
    char buffer[BUFFER_SIZE];

    bytes_read = read(fd_src, buffer, BUFFER_SIZE);
    if (bytes_read < 0)
      return MyErr("Error with reading file");

    if (MyWrite(fd_dest, buffer, bytes_read))
      return 0;

  } while (bytes_read != 0);

  return 1;
}


int GetOptions( int argc, char *argv )
{
  int getopt_ret = 0;
  int flag = 0;

  while (1)
  {
    getopt_ret = getopt_long(argc, argv, OPT_STR, OPTIONS);

    switch (getopt_ret)
    {
    case -1:
      return flag;
    case '?':
      printf("Unrecognized option: \"%s\"\n", argv[optind])
      return END_OF_KEYS;
    case 'f':
      flag |= F_KEY;
      break;
    case 'i':
      flag |= I_KEY;
      break;
    case 'v':
      flag |= V_KEY;
      break;
    default:
      printf("Unrecognized return value: %d\n", getopt_ret);
      return INV_KEY;
    }
  }
}

int main( int argc, char *argv[] )
{
  int flags = GetOptions(argc, argv);

  if (flags == INV_KEY)
    return 1;




  return 0;
}