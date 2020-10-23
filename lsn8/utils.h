//
// Created by andrey on 23.10.2020.
//

#ifndef INC_3_RD_SEM_UTILS_H
#define INC_3_RD_SEM_UTILS_H

#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>


#define BUFFER_SIZE 4096
#define MAX_ACCESS 0700

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

extern int sem_id;

int P_FLG( int sem_num, int n, short flags )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, -n, flags}};

  return semop(sem_id, sem, 1);
}

int P( int sem_num, int n )
{
  return P_FLG(sem_num, n, 0);
}

int V_FLG( int sem_num, int n, short flags )
{
  assert(n > 0);

  struct sembuf sem[] = {{sem_num, n, flags}};

  return semop(sem_id, sem, 1);
}

int V( int sem_num, int n )
{
  return V_FLG(sem_num, n, 0);
}

int Z_FLG( int sem_num, short flags )
{
  struct sembuf sem[] = {{sem_num, 0, flags}};

  return semop(sem_id, sem, 1);
}

int Z( int sem_num )
{
  return Z_FLG(sem_num, 0);
}

#endif //INC_3_RD_SEM_UTILS_H
