#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


#define BUFFER_SIZE 4096
#define MAX_ACCESS 0700

int n_pass = 0, trap_cap = 0, boat_cap = 0, sem_id = 0;

__inline int MyErr( char *str_err ) __attribute__((always_inline));

__inline int Min( int a, int b ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

int Min( int a, int b )
{
  return a < b ? a : b;
}
////////////////////////////////////////////////////////////////////////
///////////SEMAPHORES HANDLE
/////////////////////////////////////////////////////////////////

union semun
{
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

typedef struct tagSem_t
{
  unsigned short id;
  short sem_op;
  short sem_flg;
} Sem_t;

enum Semaphores
{
  IN = 0,
  OUT,
  BOAT,
  LOAD
};

void P( enum Semaphores sem_num, int n )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, -n, 0}};

  semop(sem_id, sem, 1);
}

void V( enum Semaphores sem_num, int n )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, n, 0}};

  semop(sem_id, sem, 1);
}

void Z( enum Semaphores sem_num )
{
  struct sembuf sem[] = {{sem_num, 0, 0}};

  semop(sem_id, sem, 1);
}

void SetSemVal( enum Semaphores sem_num, int val )
{
  struct semid_ds dummy;
  union semun VALUE = {val};

  semctl(sem_id, sem_num, SETVAL, &dummy, &VALUE);
}
///////////////////////////////////////////////////////

int Capitan( int num_of_chill )
{
  int min_boat_pass = Min(boat_cap, n_pass);


  printf("Capitan in bay\nLadder down\n");
  for (int i = 0; i < num_of_chill; ++i)
  {
    // wating for loading
    Z(BOAT);
    // ladder up
    printf("Let ladder up\n");
    V(LOAD, 1);
    printf("Ladder up\n");
    // set trap state for zero (put up trap)
    P(IN, trap_cap);
    printf("End of loading\n");
    printf("!!!START OF A JOURNEY #%d\n", i);
    // chilllllllll

    // wait for unloading
    V(BOAT, min_boat_pass);
    printf("!!!END OF JOURNEY #%d\n", i);
    Z(BOAT);
    printf("End of unloading\n");

    //set trap state fro non zero (get ready for new journey)
    V(IN, trap_cap);
    P(LOAD, 1);
  }

  return 0;
}

int Pass( int i_pass )
{
  // check if there is a boat
  Z(LOAD);

  // go on boat
  P(BOAT, 1);
  printf("Pass #%d buy a ticket to boat\n", i_pass);

  // go on trap
  P(IN, 1);
  printf("Pass #%d go to trap\n", i_pass);
  // go from trap
  V(IN, 1);
  printf("Pass #%d has gone from ladder\n", i_pass);

  // chill on boat
  printf("Pass #%d is on boat\n", i_pass);
  printf("Pass #%d chilling\n", i_pass);

  // go from boat
  P(OUT, 1);
  printf("Pass #%d go from boat\n", i_pass);
  V(OUT, 1);
  printf("Pass #%d is on the ground\n", i_pass);
  P(BOAT, 1);

  return 1;
}

int main( int argc, char *argv[] )
{
  if (argc < 5)
  {
    printf("Too few args\n");
    return 0;
  }
  char buffer[BUFFER_SIZE];

  n_pass   = atoi(argv[1]);
  trap_cap = atoi(argv[2]);
  boat_cap = atoi(argv[3]);

  int num_of_chill = atoi(argv[4]);

  setvbuf(stdout, buffer, _IOLBF, BUFFER_SIZE);
  //key_t sem_key = ftok(I, 0);

  sem_id = semget(IPC_PRIVATE, 4, MAX_ACCESS);

  SetSemVal(IN, trap_cap);
  SetSemVal(OUT, trap_cap);
  SetSemVal(BOAT, Min(boat_cap, n_pass));
  SetSemVal(LOAD, 0);

  pid_t cap_pid = fork();

  if (cap_pid == 0)
  {
    Capitan(num_of_chill);
    return 0;
  }
  if (cap_pid < 0)
    return MyErr("Error with creating process");

  for (int i = 1; i <= n_pass; ++i)
  {
    pid_t pas_pid = fork();

    if (pas_pid == -1)
      return MyErr("process error:");

    if (pas_pid == 0)
    {
      // pass process
      Pass(i);
      return 0;
    }
  }


  for (int i = 0; i < n_pass + 1; ++i)
    if (wait(NULL) < 0)
      return MyErr("Wait error");

  semctl(sem_id, 0, IPC_RMID);
  return 0;
}

