#include "utils.h"

int sem_id = 0;
int shm_id = 0;

char *buffer = 0;

int ProcStr( char *buf )
{
  int len = strlen(buf);

  if (buf[len - 1] != '\n')
  {
    buf[len] = '\n';
    buf[len + 1] = 0;
  }
}

void Work( void )
{
  // SEC = MEM, FST = CHAN
  V(CHAN, 1);
  //V(MEM, 1);
  while (1)
  {
    // grab memory
    Z(MEM);
    V(MEM, 1);
    //printf("Memory grabbed\n");
    //ProcStr(buffer);
    printf("%s", buffer);
    V(CHAN, 1);
  }
}

int main( void )
{
  char buf[BUFFER_SIZE];

  key_t key = ftok(SERV_NAME, SERV_ID);
  shm_id = shmget(key, BUFFER_SIZE, MAX_ACCESS | IPC_CREAT);
  sem_id = semget(key, 2, MAX_ACCESS | IPC_CREAT);
  buffer = shmat(shm_id, NULL, 0);
  setvbuf(stdout, buf, _IOLBF, BUFFER_SIZE);

  // initialize flag


  Work();

  shmctl(shm_id, IPC_RMID, NULL);
  shmctl(sem_id, 0, IPC_RMID);

  return 0;
}
