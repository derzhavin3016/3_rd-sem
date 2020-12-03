#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_ACCESS 0777
#define QUEUE_NAME "/shmque"
#define BUFFER_SIZE 4096

#define CHECK(cond, err_str) \
if (!(cond))                 \
  return MyErr(err_str);

#define CHECK_DF(expr, err_str) CHECK((expr) != -1, err_str)

int main( void )
{
  printf("Hello\n");

	return 0;
}
