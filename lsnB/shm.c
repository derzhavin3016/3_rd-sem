#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <mqueue.h>

#define MAX_ACCESS 0777
#define QUEUE_NAME "/shmque"
#define BUFFER_SIZE 4096

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

int ReadFileMq( int fd, char *ptr, size_t size )
{
  size_t bytes_read = 0;

  return read(fd, ptr, size);
/*
  while (sz > 0)
  {
    bytes_read = read(fd, ptr, sz);

    CHECK_DF(bytes_read, "Reader err")
    if (bytes_read == 0)
      return 0;

    ptr += bytes_read;
    sz -= bytes_read;
  }
  return 0;*/
}

int WriteFileMq( int fd, char *ptr, size_t size )
{
  size_t bytes_written = 0;

  while (size > 0)
  {
    bytes_written = write(fd, ptr, size);

    CHECK_DF(bytes_written, "Writer err")
    if (bytes_written == 0)
      return 0;

    ptr += bytes_written;
    size -= bytes_written;
  }
  return 0;
}

int GetFlength( int fd )
{
  struct stat buf;
  return fstat(fd, &buf) == -1 ? -1 : buf.st_size;
}

enum
{
  TO_READER = 1,
  TO_WRITER = 2
};


int Reader( char *file_in )
{
  int fin = open(file_in, O_RDONLY, MAX_ACCESS);
  CHECK_DF(fin, file_in)

  int length = GetFlength(fin);
  CHECK_DF(length, file_in)

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  CHECK_DF(shm_id, "Reader")

  CHECK_DF(ftruncate(shm_id, length), "Truncate err")

  char *ptr = mmap(NULL, length * sizeof(char), PROT_WRITE | PROT_READ,
                   MAP_SHARED, shm_id, 0);
  CHECK(ptr != MAP_FAILED, "Map failed")

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  char pptr[length];
  CHECK_DF(ReadFileMq(fin, ptr, length), "FNC ERR")

  char msg_bf[BUFFER_SIZE];

  CHECK_DF(munmap(ptr, length), QUEUE_NAME)

  CHECK_DF(mq_send(mqd, msg_bf, BUFFER_SIZE, TO_WRITER), "Reader msg sending error")
  unsigned prior = TO_READER;

  CHECK_DF(close(fin), file_in)
  CHECK_DF(close(shm_id), QUEUE_NAME)
  return 0;
}

int Writer( char *file_out )
{
  int fout = open(file_out, O_CREAT | O_WRONLY, MAX_ACCESS);
  CHECK_DF(fout, file_out)

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  CHECK_DF(shm_id, "Writer")

  int length = GetFlength(shm_id);
  CHECK_DF(length, file_out)

  char msg_bf[BUFFER_SIZE];

  unsigned prior = TO_WRITER;
  CHECK_DF(mq_receive(mqd, msg_bf, BUFFER_SIZE, &prior), "Writer msg recieving error")

  char *ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  CHECK(ptr != MAP_FAILED, "Map failed in writer")

  WriteFileMq(fout, ptr, length);

  CHECK_DF(close(shm_id), QUEUE_NAME)
  CHECK_DF(munmap(ptr, length), QUEUE_NAME)
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
  struct mq_attr att = {0, 10, BUFFER_SIZE, 0};
  mqd_t mqd_id = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, MAX_ACCESS, &att);
  CHECK_DF(mqd_id, "Queue")
  int shm_id = shm_open(QUEUE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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