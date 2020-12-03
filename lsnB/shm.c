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
  int sz = size;

  while (sz > 0)
  {
    bytes_read = read(fd, ptr, sz);

    CHECK_DF(bytes_read, "Reader err")
    if (bytes_read == 0)
      return 0;

    ptr += bytes_read;
    sz -= bytes_read;
  }
  return 0;
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

int Reader( int fin )
{
  int length = GetFlength(fin);
  CHECK_DF(length, "length err")

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  CHECK_DF(shm_id, "Reader shared mem error")

  CHECK_DF(ftruncate(shm_id, length), "Truncate err")

  char *ptr = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0);
  CHECK(ptr != MAP_FAILED, "Map failed")

  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  CHECK_DF(ReadFileMq(fin, ptr, length), "Read error")

  char msg_bf[BUFFER_SIZE];

  CHECK_DF(munmap(ptr, length), QUEUE_NAME)

  CHECK_DF(mq_send(mqd, msg_bf, BUFFER_SIZE, 0), "Reader msg sending error")

  CHECK_DF(close(fin), "close failed")
  CHECK_DF(close(shm_id), QUEUE_NAME)
  return 0;
}

int Writer( int fout )
{
  mqd_t mqd = mq_open(QUEUE_NAME, O_RDWR);
  CHECK_DF(mqd, "Queue get err")

  int shm_id = shm_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  CHECK_DF(shm_id, "Writer")

  int length = GetFlength(shm_id);
  CHECK_DF(length, "length error")

  char msg_bf[BUFFER_SIZE];
  CHECK_DF(mq_receive(mqd, msg_bf, BUFFER_SIZE, NULL), "Writer msg recieving error")

  char *ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  CHECK(ptr != MAP_FAILED, "Map failed in writer")

  WriteFileMq(fout, ptr, length);

  CHECK_DF(close(shm_id), QUEUE_NAME)
  CHECK_DF(munmap(ptr, length), QUEUE_NAME)
  CHECK_DF(close(fout), "close failed")
  return 0;
}

int InitObjs( mqd_t *mqd, int *shm_id )
{
  struct mq_attr att = {0, 10, BUFFER_SIZE, 0};
  *mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, MAX_ACCESS, &att);
  CHECK_DF(*mqd, "Queue")
  *shm_id = shm_open(QUEUE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  CHECK_DF(*shm_id, "Shared memory")

  return 0;
}


int CloseObjs( mqd_t mqd, int shm )
{
  CHECK_DF(close(shm), "Close shared mem err")
  CHECK_DF(mq_close(mqd), "Close queue err")
  CHECK_DF(mq_unlink(QUEUE_NAME), "Unlink error")
  CHECK_DF(shm_unlink(QUEUE_NAME), "Unlink error")

  return 0;
}

int Forks( int fin, int fout )
{
  pid_t reader_pid = fork();
  if (reader_pid == 0)
    exit(Reader(fin));

  pid_t writer_pid = fork();
  if (writer_pid == 0)
    exit(Writer(fout));

  for (int i = 0; i < 2; ++i)
    wait(NULL);
  return 0;
}

int main( int argc, char *argv[] )
{
  if (argc != 3)
  {
    printf("USAGE: %s INPUT_FILE OUTPUT_FILE\n", argv[0]);
    return 0;
  }
  mqd_t mqd_id = 0;
  int shm_id = 0;
  CHECK(InitObjs(&mqd_id, &shm_id) == 0, "Init failed: ");

  int fin = open(argv[1], O_RDONLY, MAX_ACCESS);
  CHECK_DF(fin, argv[1])
  int fout = open(argv[2], O_CREAT | O_WRONLY, MAX_ACCESS);
  CHECK_DF(fout, argv[2])

  Forks(fin, fout);

  CHECK(CloseObjs(mqd_id, shm_id) == 0, "Close failed: ");
  return 0;
}
