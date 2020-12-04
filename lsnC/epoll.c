#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ACCESS 0777
#define QUEUE_NAME "/shmque"
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

int GetFlength( int fd )
{
  struct stat buf;
  return fstat(fd, &buf) == -1 ? -1 : buf.st_size;
}


int forks_num = 0;

int main( int argc, char *argv[] )
{
  if (argc != 4)
  {
    printf("USAGE: %s INPUT OUTPUT N_OF_PROC\n", argv[0]);
    return 0;
  }
  forks_num = atoi(argv[3]);
  int pipe_fds[forks_num][2][2];
  int fd_in  = open(argv[1], O_RDONLY, MAX_ACCESS),
      fd_out = open(argv[2], O_WRONLY | O_CREAT, MAX_ACCESS);
      
  // Creating epoll
  int epoll_fd = epoll_create1(0);
        
  CHECK_DF( fd_in, argv[1]);
  CHECK_DF(fd_out, argv[2]);
  struct epoll_event events[forks_num - 1];
  // creating pipes 
  for (int i = 0; i < forks_num; ++i)
  {  
    CHECK_DF(pipe(pipe_fds[i][0]), "pipe create error");
    CHECK_DF(pipe(pipe_fds[i][1]), "pipe create error");
    if (i != forks_num - 1)
    {
      events[i].events = EPOLLIN;
      events[i].data.fd = pipe_fds[i + 1][1][1];
      CHECK_DF(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fds[i][0][0], events + i),
               "ctl error");
    }
    
  }
  
  // attach the INPUT and OUTPUT fds to pipe's array (for easier usage)
  CHECK_DF(dup2(fd_in, pipe_fds[0][1][0]), "dup error");
  CHECK_DF(dup2(fd_out, pipe_fds[forks_num - 1][0][1]), "dup error");
  
  for (int i = 0; i < forks_num; ++i)
  {
    pid_t cpid = fork();
    
    CHECK_DF(cpid, "fork error");
    
    if (cpid == 0)
    {
      int fd_src = pipe_fds[i][1][0],
          fd_dst = pipe_fds[i][0][1];
          
      CHECK(ReadWriteFile(fd_src, fd_dst) == 0, "RW err");
      CHECK_DF(close(fd_src), "error closing src");
      CHECK_DF(close(fd_dst), "error closing dst");
      return 0;
    }
  } 
  
  // Here parent resell data between pipes
  for (int i = 0; i < forsk_num; ++i)
  {
    struct epoll_event ev;      
    int n = epoll_wait(epoll_fd, &ev, forks_num, -1);
    ReadWriteFile(pipe_fds[i][][], )  
  }
  
  

  for (int i = 0; i < forks_num; ++i)
  {
    // waiting for child process
    wait(NULL);
    
    // closing parents's pipes
    CHECK_DF(close(pipe_fds[i][0][0]), "pipe close error");
    CHECK_DF(close(pipe_fds[i][1][1]), "pipe close error");
  }  

  CHECK_DF(close(epoll_fd), "epoll closing error");
	return 0;
}
