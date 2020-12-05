#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#define __USE_GNU
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ACCESS 0777
#define BUFFER_SIZE 4096

#define CHECK(cond, err_str) \
do {                         \
if (!(cond))                 \
  return MyErr(err_str);     \
  } while(0)

#define CHECK_DF(expr, err_str) CHECK((expr) != -1, err_str)

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
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
    {
      if (errno == EAGAIN)
        break;
      return MyErr("Error with writing in file");
    }

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
    {
      if (errno == EAGAIN)
        break;
      return MyErr("Error with reading file");
    }


    if (MyWrite(fd_dst, buffer, bytes_read))
      return 1;

  } while (bytes_read > 0);

  return 0;
}

///////////////////////////////////////
typedef struct tagPipePair
{
  int to_read[2];
  int to_write[2];
} PPair;
/////////////////////////////////////

int GetFlength( int fd )
{
  struct stat buf;
  return fstat(fd, &buf) == -1 ? -1 : (int)buf.st_size;
}


int forks_num = 0;

int ParentNPipes( int epoll_fd )
{
  // Here parent resell data between pipes
  struct epoll_event ev;
  int num_eps = forks_num - 1;
  while (num_eps)
  {
    int n = epoll_wait(epoll_fd, &ev, 1, 0);
    if (!n)
      continue;
    CHECK_DF(n, "wait error");
    if (ev.events & EPOLLERR || ev.events & EPOLLHUP)
      break;

    PPair *pev = (PPair *)ev.data.ptr;
    int fd_src = pev->to_read[0];
    int fd_dst = (pev + 1)->to_write[1];

    //close(pev->to_read[1]);
    //close((pev + 1)->to_write[0]);

    CHECK(ReadWriteFile(fd_src, fd_dst) == 0, "Parent RW error");
    //ReadWriteFile(fd_src, STDOUT_FILENO);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd_src, NULL);
    close(fd_src);
    --num_eps;
    //ReadWriteFile(fd_dst, STDOUT_FILENO);
    close(fd_dst);
  }

  return 0;
}
/*
int main( void )
{
  int epoll_fd = epoll_create1(0);
  int fnum = 2;
  PPair fds[fnum];
  int fd_in  = open("hell", O_RDONLY, MAX_ACCESS),
    fd_out = open("qqq", O_WRONLY | O_CREAT, MAX_ACCESS);

  CHECK_DF(fd_in, "hell");
  CHECK_DF(fd_out, "qq");

  struct epoll_event evs[fnum - 1];
  for (int i = 0; i < fnum; ++i)
  {
    pipe(fds[i].to_read);
    pipe(fds[i].to_write);
    if (i != fnum - 1)
    {
      evs[i].events = EPOLLIN;
      evs[i].data.ptr = fds + i;

      CHECK_DF(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fds[i].to_read[0], evs + i), "ctl err");
    }
  }

  dup2(fd_in, fds[0].to_write[0]);
  dup2(fd_out, fds[1].to_read[1]);

  ReadWriteFile(fds[0].to_write[0], fds[0].to_read[1]);

  struct epoll_event ev;
  int n = epoll_wait(epoll_fd, &ev, fnum, -1);
  if (ev.events & EPOLLERR)
    return printf("OHSHIT\n");

  char buffer[BUFFER_SIZE];
  close(fds[0].to_read[1]);
  for (int i = 0; i < 2; ++i)
  {
    int nr = read(fds[0].to_read[0], buffer, BUFFER_SIZE);
    printf("nr = %d\n", nr);
  }
  //ReadWriteFile(fds[0].to_read[0], fds[1].to_write[1]);

  close(fds[1].to_write[1]);
  ReadWriteFile(fds[1].to_write[0], fds[1].to_read[1]);
  for (int i = 0; i < fnum; ++i)
  {
    close(fds[i].to_read[1]);close(fds[i].to_read[0]);close(fds[i].to_write[1]);close(fds[i].to_write[0]);
  }
  close(epoll_fd);
  return 0;
}
*/
/**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *      IMPORTANT!!!!!!!
 ************************************
 *      pipe fds- ---- 3-dim array
 *      parent has an access to all of that pipes
 *      the num config is:
 *      pipe_fds[     ]                            [     ]                                     [     ]
 *                 ^                                  ^                                           ^
 *            num of child       0 - parent can read from this pipe                        0 - read fd
 *         (each has 2 pipes)    1 - parent can write to this pipe                         1 - write fd
 *  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
int main( int argc, char *argv[] )
{
  // argc argv processing
  if (argc != 4)
  {
    printf("USAGE: %s INPUT OUTPUT N_OF_PROC\n", argv[0]);
    return 0;
  }
  ///////////////////////////////////////////

  // getting arguments, open files
  forks_num = atoi(argv[3]);
  PPair pipe_fds[forks_num];
  int fd_in  = open(argv[1], O_RDONLY, MAX_ACCESS),
      fd_out = open(argv[2], O_WRONLY | O_CREAT, MAX_ACCESS);

  // check if files open was ok
  CHECK_DF( fd_in, argv[1]);
  CHECK_DF(fd_out, argv[2]);

  // Creating epoll & check valid
  int epoll_fd = epoll_create1(0);
  CHECK_DF(epoll_fd, "epoll_create1");

  // creating pipes 
  for (int i = 0; i < forks_num; ++i)
  {
    CHECK_DF(pipe2(pipe_fds[i].to_write, O_NONBLOCK), "pipe create error");
    CHECK_DF(pipe2(pipe_fds[i].to_read, O_NONBLOCK), "pipe create error");
    if (i != forks_num - 1)
    {
      struct epoll_event ev;
      ev.events = EPOLLIN;
      ev.data.ptr = pipe_fds + i;

      CHECK_DF(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fds[i].to_read[0], &ev),
               "ctl error");
    }
    
  }
  
  // attach the INPUT and OUTPUT fds to pipe's array (for easier usage)
  CHECK_DF(dup2(fd_in, pipe_fds[0].to_write[0]), "dup error");
  CHECK_DF(dup2(fd_out, pipe_fds[forks_num - 1].to_read[1]), "dup error");
  //close(fd_in);
  //close(fd_out);
  char buf[BUFFER_SIZE];
  //read(pipe_fds[0].to_write[0], buf, BUFFER_SIZE);
  //printf("%s", buf);

  //return 0;
  // creating processes
  for (int i = 0; i < forks_num; ++i)
  {
    pid_t cpid = fork();

    CHECK_DF(cpid, "fork error");

    if (cpid == 0)
    {
      int fd_src = pipe_fds[i].to_write[0],
          fd_dst = pipe_fds[i].to_read[1];

      //CHECK_DF(close(pipe_fds[i].to_write[1]), "close err");
      //CHECK_DF(close(pipe_fds[i].to_read[0]), "close err");

      CHECK(ReadWriteFile(fd_src, fd_dst) == 0, "RW err");
      CHECK_DF(close(fd_src), "close err");
      CHECK_DF(close(fd_dst), "close err");
      return 0;
    }
    if (forks_num > 1)
      ;//ParentNPipes(epoll_fd);
  }
  ///////////////////////////////////////////
  if (forks_num > 1)
    CHECK_DF(ParentNPipes(epoll_fd), "ParentNPipes");
  
  ///// NEAR THE END ///////////////////////////////////
  for (int i = 0; i < forks_num; ++i)
  {
    // waiting for child process
    wait(NULL);
    
    // closing parent's pipes
    //CHECK_DF(close(pipe_fds[i].to_read[0]), "pipe close error");
    //CHECK_DF(close(pipe_fds[i].to_write[1]), "pipe close error");
  }  

  CHECK_DF(close(epoll_fd), "epoll closing error");
	return 0;
}
