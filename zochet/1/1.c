#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/types.h>
//#include <sys/stat.h>

#include <fcntl.h>
#include <poll.h>

#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>

//2. Напишите программу, считающую число байт в потоках stdout и stderr дочернего процесса,
// заданной ей аргументами командной строки.
// много и в stdout и stdin

#define MAX_ACCESS 0777
#define BUFFER_SIZE 4096

#define CHECK(cond, err_str) \
do {                         \
if (!(cond))                 \
  {                           \
  printf("ASSERTION FAILED: %s\n", #cond);                           \
  return MyErr(err_str);     \
  }                           \
  } while(0)

#define CHECK_DF(expr, err_str) CHECK((expr) >= 0, err_str)

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

typedef struct TAGwc
{
  size_t strings;
  size_t words;
  size_t bytes;
} WC;

int StrCnt( const char *buf, size_t size )
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

int WrdCnt( const char *buf, size_t size, int *isword )
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
  ssize_t bytes_read = 0;
  w_cnt->bytes = w_cnt->strings = w_cnt->words = 0;

  do
  {
    char buffer[BUFFER_SIZE];

    bytes_read = read(fd_in, buffer, BUFFER_SIZE);

    if (bytes_read < 0)
    {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;
      return MyErr("Error with reading file");
    }

    w_cnt->bytes   += bytes_read;
    //w_cnt->strings += StrCnt(buffer, bytes_read);
    //w_cnt->words   += WrdCnt(buffer, bytes_read, &isword);
  } while (0 && bytes_read != 0);

  return 0;
}

int main( int argc, char *argv[] )
{
  if (argc < 2)
  {
    printf("2. Напишите программу, считающую число байт в потоках stdout и stderr дочернего процесса,\n"
           "заданной ей аргументами командной строки.\n");
    printf("USAGE: %s EXEC_CMD ARGS*\n", argv[0]);
    return 0;
  }

  // initializing pipe
  int pip_sout[2] = {0};
  int pip_serr[2] = {0};

  CHECK_DF(pipe2(pip_sout, O_NONBLOCK) < 0, "stdout pipe");
  CHECK_DF(pipe2(pip_serr, O_NONBLOCK) < 0, "stderr pipe");

  pid_t cpid = fork();

  CHECK_DF(cpid, "fork error");

  if (cpid == 0)
  {
    // Here goes a child
    CHECK_DF(close(pip_sout[0]), "Error closing out pipe");
    CHECK_DF(dup2(pip_sout[1], STDOUT_FILENO), "duplicating error stdout");

    CHECK_DF(close(pip_serr[0]), "Error closing out pipe");
    CHECK_DF(dup2(pip_serr[1], STDERR_FILENO), "duplicating error stderr");

    CHECK_DF(execvp(argv[1], argv + 1), "Execution error");

    exit(0);
  }

  WC wc[2] = {{.bytes = 0}, {.bytes = 0}}; //0 - out, 1 -err
  struct pollfd fds[] = {
    {.fd = pip_sout[0], .events = POLLIN | POLLERR | POLLNVAL},
    {.fd = pip_serr[0], .events = POLLIN | POLLERR | POLLNVAL},
  };

  do
  {
    int nfds = poll(fds, 2, 200);
    if (nfds < 0)
      return 0;
    if (nfds == 0)
      break;

    for (int i = 0; i < 2; ++i)
    {
      if (!(fds[i].events & POLLIN))
        continue;

      WC tmp = {.bytes = 0};
      CHECK_DF(WordCount(fds[i].fd, &tmp), "Word count from stdout");
      wc[i].bytes += tmp.bytes;
    }
  } while(1);
  CHECK_DF(close(pip_sout[0]), "Error closing stdout read pipe");
  CHECK_DF(close(pip_serr[0]), "Error closing stderr read pipe");
  CHECK_DF(close(pip_sout[1]), "Error closing stdout write pipe");
  CHECK_DF(close(pip_serr[1]), "Error closing stderr write pipe");


  printf("STDOUT bytes: %zd\n", wc[0].bytes);
  printf("STDERR bytes: %zd\n", wc[1].bytes);
  printf("I am waiting \n");

  CHECK_DF(wait(NULL), "Error waiting for process");

  printf("I am no wating \n");

  return 0;
}