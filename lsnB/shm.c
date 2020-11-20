#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <mqueue.h>

#define MAX_ACCESS 0777
#define QUEUE_NAME "/shmque"
#define BUFFER_SIZE 8193

#define CHECK(cond, err_str) \
if (!(cond))                 \
  return MyErr(err_str);

#define CHECK_DF(expr, err_str) CHECK((expr) != -1, err_str)

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

struct Buf
{
  char *ptr;
  size_t size;
} shared_buff = {NULL, 0};


int Reader( char *file_in )
{
  int fin = open(file_in, O_RDONLY, MAX_ACCESS);
  CHECK_DF(fin, file_in)

  struct stat buf;
  CHECK_DF(fstat(fin, &buf), file_in)

  shared_buff.size = buf.st_size;
  shared_buff.ptr = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fin, 0);
  CHECK(shared_buff.ptr != MAP_FAILED, "Map failed")
  CHECK_DF(close(fin), file_in)

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  char dum_buff[BUFFER_SIZE] = "rend";
  CHECK_DF(mq_send(mqd, dum_buff, BUFFER_SIZE, 2), "Reader msg sending error")


  CHECK_DF(mq_receive(mqd, dum_buff, BUFFER_SIZE, NULL), "Reader msg recieving error")
  return 0;
}

int Writer( char *file_out )
{
  int fout = open(file_out, O_CREAT, MAX_ACCESS);
  CHECK_DF(fout, file_out)

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  char dum_buff[BUFFER_SIZE] = "wend";
  CHECK_DF(mq_receive(mqd, dum_buff, BUFFER_SIZE, NULL), "Writer msg recieving error")

  CHECK_DF(write(fout, shared_buff.ptr, shared_buff.size), file_out)

  CHECK_DF(mq_send(mqd, dum_buff, BUFFER_SIZE, 1), "Writer msg sending error")

  CHECK_DF(close(fout), file_out)
  return 0;
}

int main( int argc, char *argv[] )
{
  if (argc != 3)
  {
    printf("USAGE: ./mq INPUT_FILE OUTPUT_FILE\n");
    return 0;
  }
  mqd_t mqd_id = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, MAX_ACCESS, NULL);
  CHECK_DF(mqd_id, "Queue")
  int shm_id = shm_open(QUEUE_NAME, O_CREAT | O_RDWR, MAX_ACCESS);
  CHECK_DF(shm_id, "Shared memory")

  pid_t reader_pid = fork();
  if (reader_pid == 0)
    return Reader(argv[1]);

  pid_t writer_pid = fork();
  if (writer_pid == 0)
    return Writer(argv[2]);

  for (int i = 0; i < 2; ++i)
    wait(NULL);

  CHECK_DF(close(shm_id), "Close shared mem err")
  CHECK_DF(mq_close(mqd_id), "Close queue err")
  CHECK_DF(mq_unlink(QUEUE_NAME), "Unlink error")
  CHECK_DF(shm_unlink(QUEUE_NAME), "Unlink error")
  return 0;
}