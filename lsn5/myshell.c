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

int WrdCnt( char *buf, size_t size )
{
  if (buf == NULL)
    return -1;

  int cnt = 0;

  for (size_t i = 0; i < size; ++i)
  {
    size_t j = i;
    for (; j < size && !isspace((int)buf[j]); ++j);
    cnt += i != j && j < size;
    i = j;
  }

  return cnt;
}

int WordCount( int fd_in, WC *w_cnt )
{
  if (w_cnt == NULL)
    return -1;
  int bytes_read = 0;
  w_cnt->bytes = w_cnt->strings = w_cnt->words = 0;

  do
  {
    char buffer[BUFFER_SIZE];

    bytes_read = read(fd_in, buffer, BUFFER_SIZE);
    if (bytes_read < 0)
      return MyErr("Error with reading file");

    w_cnt->bytes   += bytes_read;
    w_cnt->strings += StrCnt(buffer, bytes_read);
    w_cnt->words   += WrdCnt(buffer, bytes_read);

  } while (bytes_read != 0);

  return 0;
}

int main( int argc, char *argv[] )
{
  opterr = 0;
  int iswc = GetOptions(argc, argv);

  if (iswc == INV_KEY)
    return 1;

  int pip_fd[2] = {0};

  if (iswc && pipe(pip_fd) < 0)
    return MyErr("myexecwc: ");

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


