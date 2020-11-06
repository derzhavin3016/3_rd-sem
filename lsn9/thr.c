#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

__inline int MyErr( char *str_err ) __attribute__((always_inline));

int MyErr( char *str_err )
{
  perror(str_err);
  return errno;
}

unsigned long long count = 0;
pthread_mutex_t pmut = PTHREAD_MUTEX_INITIALIZER;

typedef void *(*routine)(void *);

void *native_routine( void *param )
{
  int cnt =  *(int *)param;

  for (int i = 0; i < cnt; ++i)
    ++count;

  return 0;
}

void *crit_inc( void *param )
{
  int cnt =  *(int *)param;

  for (int i = 0; i < cnt; ++i)
  {
    pthread_mutex_lock(&pmut);
    ++count;
    pthread_mutex_unlock(&pmut);
  }

  return 0;
}

void *crit_cycle( void *param )
{
  int cnt =  *(int *)param;

  pthread_mutex_lock(&pmut);
  for (int i = 0; i < cnt; ++i)
    ++count;

  pthread_mutex_unlock(&pmut);

  return 0;
}

void *cool_algos( void *param )
{
  int cnt =  *(int *)param;
  int loc_count = 0;

  for (int i = 0; i < cnt; ++i)
    ++loc_count;

  pthread_mutex_lock(&pmut);
  count += loc_count;
  pthread_mutex_unlock(&pmut);

  return 0;
}

void swtch_rt( routine *rt, int alg_num )
{
  switch (alg_num)
  {
  case 1:
    *rt = native_routine;
    break;
  case 2:
    *rt = crit_inc;
    break;
  case 3:
    *rt = crit_cycle;
    break;
  case 4:
    *rt = cool_algos;
    break;
  default:
    printf("Ты чево наделал....\n");
    exit(1);
  }
}

int main( int argc, char *argv[] )
{
  if (argc < 4)
  {
    printf("USAGE: ./th [expexted_value] [num_of_threads] [algorithm_num]\n");
    printf("algorithm_num = 1(native), 2(crit section in increment), 3(crit sec in cycle)\n");
    return 1;
  }

  int n_threads = atoi(argv[2]),
      exp_val   = atoi(argv[1]),
      alg_num   = atoi(argv[3]);

  int param = exp_val / n_threads;

  // allocating memory for threads ids
  pthread_t tids[n_threads];
  routine rt = native_routine;
  swtch_rt(&rt, alg_num);

  pthread_mutex_unlock(&pmut);

  int param0 = param + exp_val % n_threads;
  pthread_create(tids, NULL, rt, &param0);

  for (int i = 1; i < n_threads; ++i)
    pthread_create(tids + i, NULL, rt, &param);


  for (int i = 0; i < n_threads; ++i)
    pthread_join(tids[i], NULL);

  printf("expected = %d\ncounted = %lld\n", exp_val, count);
  return 0;
}