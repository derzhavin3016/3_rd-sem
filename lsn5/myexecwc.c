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

// Keys bits constants
#define W_KEY 0x1

// useful typedef
typedef struct option OPT;

typedef struct TAGwc
{
  int strings;
  int words;
  int bytes;
} WC;

enum
{
  INV_KEY = -2,
  END_OF_KEYS = -1
};

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

const OPT OPTIONS[] =
  {
    {"wc", 0, NULL, 'w'},
    {0, 0, 0, 0},
  };

const char OPT_STR[] = "";

typedef struct timespec timespc;

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
    case 'w':
      flag |= W_KEY;
      break;
    default:
      printf("Unrecognized return value: %d\n", getopt_ret);
      return INV_KEY;
    }
  }
}

int StrCnt( char *buf, size_t size )
{
  if (buf == NULL)
    return -1;

  int cnt = 0;

  for (size_t i = 0; i < size; ++i)
  {
    if (buf[i] == '\n')
      ++cnt;
  }

  return cnt;
}

int WrdCnt( char *buf, size_t size, int *isword )
{
  if (buf == NULL)
    return -1;

  int cnt = 0;

  for (size_t i = 0; i < size; ++i)
  {
    if (!isspace(buf[i]) && !*isword)
    {
      ++cnt;
      *isword = 1;
    }
    else if (isspace(buf[i]))
      *isword = 0;
  }

  return cnt;
}

int WordCount( int fd_in, WC *w_cnt )
{
  if (w_cnt == NULL)
    return -1;
  int bytes_read = 0;
  w_cnt->bytes = w_cnt->strings = w_cnt->words = 0;
  int isword = 0;

  do
  {
    char buffer[BUFFER_SIZE];

    bytes_read = read(fd_in, buffer, BUFFER_SIZE);
    if (bytes_read < 0)
      return MyErr("Error with reading file");

    w_cnt->bytes   += bytes_read;
    w_cnt->strings += StrCnt(buffer, bytes_read);
    w_cnt->words   += WrdCnt(buffer, bytes_read, &isword);

  } while (bytes_read != 0);

  return 0;
}

int main( int argc, char *argv[] )
{
  int iswc = GetOptions(argc, argv);

  if (iswc == INV_KEY)
    return 1;

  int pip_fd[2] = {0};

  if (iswc && pipe(pip_fd) < 0)
    return MyErr("myexecwc:");

  timespc t_beg = {};

  clock_gettime(CLOCK_MONOTONIC, &t_beg);
  pid_t pid = fork();

  if (pid == 0)
  {
    if (iswc)
    {
      close(pip_fd[0]);
      pip_fd[1] = dup2(pip_fd[1], STDOUT_FILENO);
    }

    execvp(argv[optind], argv + optind);

    exit(0);
  }

  if (iswc)
  {
    close(pip_fd[1]);

    WC w_cnt = {0};
    WordCount(pip_fd[0], &w_cnt);
    close(pip_fd[0]);

    printf("%d %d %d\n", w_cnt.strings, w_cnt.words, w_cnt.bytes);
  }

  if (wait(NULL) < 0)
    return MyErr("Proc err");


  timespc t_end = {};
  clock_gettime(CLOCK_MONOTONIC, &t_end);

  int exec_sec =  t_end.tv_sec - t_beg.tv_sec;
  int exec_nano = t_end.tv_nsec - t_beg.tv_nsec;

  double exec_time = exec_sec + exec_nano * 1e-9;

  printf("Process %s executed.\nExecution time: %.03lf s.\n",
         argv[optind], exec_time);

  return 0;
}


