#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define SHORT_OPTS "fiv"

// Keys bits constants
#define F_KEY 0x1
#define I_KEY 0x2
#define V_KEY 0x4

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
    int write_ret = write(fd, buffer, buf_size);

    if (write_ret < 0)
      return MyErr("Error with writing to stdout");

    bytes_written += write_ret;
    buffer += write_ret;
  }

  return 0;
}

int GetOptions(  )

int main( int argc, char *argv[] )
{


}