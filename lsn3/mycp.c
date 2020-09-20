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

#define MAX_ACCESS 0777

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

__inline int MyErr( const char *str_err ) __attribute__((always_inline));

int MyErr( const char *str_err )
{
  perror(str_err);
  return errno;
}

int MyWrite( int fd, const void *buffer, size_t buf_size )
{
  size_t bytes_written = 0;

  while (bytes_written < buf_size)
  {
    int write_ret = write(fd, buffer, buf_size - bytes_written);

    if (write_ret < 0)
      return MyErr("Error with writing in file");

    bytes_written += write_ret;
    buffer += write_ret;
  }

  return 0;
}

int ReadWriteFile( int fd_src, int fd_dst )
{
  int bytes_read = 0;

  do
  {
    char buffer[BUFFER_SIZE];

    bytes_read = read(fd_src, buffer, BUFFER_SIZE);
    if (bytes_read < 0)
      return MyErr("Error with reading file");

    if (MyWrite(fd_dst, buffer, bytes_read))
      return 1;

  } while (bytes_read != 0);

  return 0;
}


int GetOptions( int argc, char *argv[] )
{
  int getopt_ret = 0;
  int flag = 0;

  while (1)
  {
    getopt_ret = getopt_long(argc, argv, OPT_STR, OPTIONS, NULL);

    switch (getopt_ret)
    {
    case -1:
      return flag;
    case '?':
      return INV_KEY;
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

void PrintVrb( const char *src_name, const char *dst_name )
{
  printf("\'%s\' -> \'%s\'\n", src_name, dst_name);
}

void WritePrompt( const char* dst_name )
{
  const char fst[] = "cp: rewrite '";
  const char sec[] = "'? ";

  MyWrite(STDOUT_FILENO, fst, sizeof(fst));
  MyWrite(STDOUT_FILENO, dst_name, strlen(dst_name));
  MyWrite(STDOUT_FILENO, sec, sizeof(sec));
}

int GetPrompt( const char* dst_name )
{
  WritePrompt(dst_name);
  char buffer[BUFFER_SIZE];
  if (read(STDIN_FILENO, buffer, BUFFER_SIZE) <0)
    return MyErr("cp: Error with stdin: ");
  if (toupper(buffer[0]) == 'Y')
    return 0;
  return -1;
}

int DstProcess( int *fd_dst, const char *dst_name, int isF, int isI )
{
  if (*fd_dst < 0)
  {
    if (errno == EEXIST && (isI || isF))
    {
      int isCp = 1;
      if (isI)
        isCp = !GetPrompt(dst_name);
      if (!isCp)
        return 1;
      *fd_dst = open(dst_name, O_WRONLY, MAX_ACCESS);
      if (*fd_dst < 0)
        return MyErr(dst_name);
      return 0;
    }
    return MyErr(dst_name);
  }
  return 0;
}

int CopyFile( const char *src_name, const char* dst_name, int flags )
{
  int isF = (flags & F_KEY) && 1,
      isI = (flags & I_KEY) && 1,
      isV = (flags & V_KEY) && 1;

  int fd_src = open(src_name, O_RDONLY, MAX_ACCESS);

  if (fd_src < 0)
    return MyErr(src_name);

  int fd_dst = open(dst_name, O_WRONLY | O_CREAT | O_EXCL, MAX_ACCESS);
  if (DstProcess(&fd_dst, dst_name, isF, isI) || ReadWriteFile(fd_src, fd_dst))
    return -1;

  if (isV)
    PrintVrb(src_name, dst_name);

  if (close(fd_dst) < 0)
    return MyErr(dst_name);

  if (close(fd_src) < 0)
    return MyErr(src_name);
  return 0;
}

int CheckIfDir( const char *filename )
{
  int fd = open(filename, O_DIRECTORY);
  if (fd < 0)
  {
    if (errno == ENOTDIR)
      return 0;  // it is a file
    return MyErr(filename);
  }

  if (close(fd) < 0)
    return MyErr(filename);

  return -1; // it is a directory
}

int ArrProcess( int flags, char *names[], size_t size )
{
  char buffer[BUFFER_SIZE];
  strcpy(buffer, names[size - 1]);
  size_t path_size = strlen(names[size - 1]);
  buffer[path_size++] = '/';
  buffer[path_size] = '\0';

  for (size_t i = 0; i < size - 1; ++i)
  {
    strcat(buffer, names[i]);
    CopyFile(names[i], buffer, flags);
    buffer[path_size] = '\0';
  }

  return 0;
}

int main( int argc, char *argv[] )
{
  int flags = GetOptions(argc, argv);

  if (flags == INV_KEY)
    return 1;
  if (argc - optind < 2)
  {
    printf("Too few arguments: %d\n", argc - optind);
    return 1;
  }

  int isDir = CheckIfDir(argv[argc - 1]);
  if (isDir > 0)
    return 1;
  if (isDir < 0 && ArrProcess(flags, argv + optind, argc - optind))
    return 1;

  if (!isDir && CopyFile(argv[optind], argv[optind + 1], flags))
    return 1;

  return 0;
}