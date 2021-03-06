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
  BOAT_IN,
  BOAT_OUT,
  IS_LOAD,
  IS_UNLOAD,
  IS_JRN,
  NEXT_CYC,
  WT
};

int P_FLG( enum Semaphores sem_num, int n, short flags )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, -n, flags}};

  return semop(sem_id, sem, 1);
}

int P( enum Semaphores sem_num, int n )
{
  return P_FLG(sem_num, n, 0);
}

int V_FLG( enum Semaphores sem_num, int n, short flags )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, n, flags}};

  return semop(sem_id, sem, 1);
}

int V( enum Semaphores sem_num, int n )
{
  return V_FLG(sem_num, n, 0);
}

int Z_FLG( enum Semaphores sem_num, short flags )
{
  struct sembuf sem[] = {{sem_num, 0, flags}};

  return semop(sem_id, sem, 1);
}

int Z( enum Semaphores sem_num )
{
  return Z_FLG(sem_num, 0);
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

  // inital semaphores values
  V(IN,   trap_cap);
  V(OUT,  trap_cap);
  V(BOAT_IN, min_boat_pass);
  V(BOAT_OUT, min_boat_pass);
  V(IS_LOAD, 1);
  V(IS_UNLOAD, 1);
  V(NEXT_CYC, 1);
  V(WT, n_pass);

  printf("Capitan in bay\nLadder down\n");
  for (int i = 0; i < num_of_chill; ++i)
  {
    Z(WT);
    P(NEXT_CYC, 1);

    P(IS_LOAD, 1);
    // wating for passengers on boat
    Z(BOAT_IN);
    V(BOAT_IN, min_boat_pass);
    // ladder up
    V(IS_LOAD, 1);
    printf("Ladder is up\n");
    // starting journey
    printf("Journey start\n");

    // journeyy
    printf("CHILLIN......\n");

    // end journey
    printf("Journey end\n");
    printf("Ladder is down\n");
    P(IS_UNLOAD, 1);

    Z(BOAT_OUT);
    printf("Boat is unload\n");
    V(BOAT_OUT, min_boat_pass);
    V(IS_UNLOAD, 1);
    printf("Next cycle\n");
    V(NEXT_CYC, 1);
    printf("searching for pass\n");
    V(WT, n_pass);
  }
  printf("preparing for depart\n");
  Z(WT);
  printf("Capitan is out of bay\n");
  V(IS_JRN, 1);
  P(NEXT_CYC, 1);
  return 0;
}

int Pass( int i_pass )
{
  // Waiting
  Z(IS_LOAD);

  P(BOAT_IN, 1);
  printf("Pass #%d buy ticket to boat\n", i_pass);

  // step on ladder
  P(IN, 1);
  printf("Pass #%d step on a ladder\n", i_pass);

  // step out from boat
  printf("Pass #%d step out from a ladder\n", i_pass);
  V(IN, 1);

  // step on boat
  printf("Pass #%d is on the boat\n", i_pass);

  //printf("Pass #%d chilling\n", i_pass);
  Z(IS_UNLOAD);

  // step from boat
  printf("Pass #%d step out from boat\n", i_pass);

  // step on ladder
  P(OUT, 1);
  printf("Pass #%d step on a ladder (exit)\n", i_pass);

  // step out from boat
  printf("Pass #%d step out from a ladder (exit)\n", i_pass);
  V(OUT, 1);
  printf("Pass #%d throw his ticket out\n", i_pass);
  P(BOAT_OUT, 1);

  return 1;
}

int Pass_cycle( int i_pass )
{
  int cnt = 0;
  while (1)
  {
    // waits for next cycle (synhronize point)
    printf("Pass #%d stay on ground\n", i_pass);
    P(WT, 1);
    Z(NEXT_CYC);
    if (Z_FLG(IS_JRN, IPC_NOWAIT) == -1 && errno ==  EAGAIN)
    {
      printf("Pass #%i go home\n", i_pass);
      break;
    }
    cnt += Pass(i_pass);
  }


  return cnt;
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

  sem_id = semget(IPC_PRIVATE, 9, MAX_ACCESS);

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
      printf("!!!! Pass #%d chills %d times\n", i, Pass_cycle(i));
      return 0;
    }
  }

  for (int i = 0; i < n_pass + 1; ++i)
    if (wait(NULL) < 0)
      return MyErr("Wait error");

  semctl(sem_id, 0, IPC_RMID);
  return 0;
}

