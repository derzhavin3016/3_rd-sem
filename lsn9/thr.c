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
  int n_threads =  *(int *)param,
      exp_val = *((int *)param + 1);

  for (int i = 0; i < exp_val / n_threads; ++i)
    ++count;

  long long a = 0;
  for (int j = 0; j < 1000000; ++j)
    ++a;

  return 0;
}

void *crit_inc( void *param )
{
  int n_threads =  *(int *)param,
      exp_val = *((int *)param + 1);

  for (int i = 0; i < exp_val / n_threads; ++i)
  {
    pthread_mutex_lock(&pmut);
    ++count;
    pthread_mutex_unlock(&pmut);
  }

  return 0;
}

void *crit_cycle( void *param )
{
  int n_threads =  *(int *)param,
    exp_val = *((int *)param + 1);

  pthread_mutex_lock(&pmut);
  for (int i = 0; i < exp_val / n_threads; ++i)
    ++count;

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

  int argv_rout[2] = {n_threads, exp_val};

  // allocating memory for threads ids
  pthread_t tids[n_threads];
  routine rt = native_routine;
  swtch_rt(&rt, alg_num);

  pthread_mutex_unlock(&pmut);

  for (int i = 0; i < n_threads; ++i)
    pthread_create(tids + i, NULL, rt, argv_rout);

  for (int i = 0; i < n_threads; ++i)
    pthread_join(tids[i], NULL);

  printf("expected = %d\ncounted = %lld\n", exp_val, count);
  return 0;
}