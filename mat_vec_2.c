/****************************************************************************
 *  Matrix vector multiplication using one thread
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

int N ;
int thread_count;
float** a ;
float* b ;
float* x ;
float *SUM;
pthread_mutex_t lock;

struct thread_arg {
	int seed;
	int threads;
	int size;
};

void *mult (void *);

main (int argc, char *argv[] )    {
/* the array size should be supplied as a command line argument */
  if(argc != 3) {printf("wrong number of arguments") ; exit(2) ;}
  N = atoi(argv[1]) ;
  thread_count = atoi(argv[2]);
  printf("Array size = %d \n ", N );
  int mid = (N+1)/2;
  int i, j;
  int ptret = 0;
  double time_start, time_end;
  struct timeval tv;
  struct timezone tz;

    /*pthread variables*/
  pthread_t *threads;
  pthread_attr_t attr;
  struct thread_arg *args;
  
  /*allocate space for */
  threads = malloc(thread_count * sizeof(pthread_t));
  
/* allocate arrays dynamically */
  a = malloc(sizeof(float*)*N);
  for (i = 0; i < N; i++) {
		a[i] = malloc(sizeof(float)*N);
  }
  b = malloc(sizeof(float)*N);
  x = malloc(sizeof(float)*N);
  SUM = malloc(sizeof(float)*N);
  args = malloc(thread_count * sizeof(struct thread_arg));
  
  /* Inititialize matrix A and vector B. */
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      if (j == i)                    { a[i][j] = 2.0; }
      else if (j == i-1 || j == i+1) { a[i][j] = 1.0; }
      else                           { a[i][j] = 0.01; }
    }
    b[i] = mid - abs(i-mid+1);
  }
  for (i=0; i<N; i++) x[i] = 0.0;

  gettimeofday (&tv ,   &tz);
  time_start = (double)tv.tv_sec +
            (double)tv.tv_usec / 1000000.0;
			
			
  for ( i = 0; i < thread_count; i++) {
	  args[i].seed = i;
	  ptret = pthread_create(&threads[i], &attr, mult, (void *) &args[i]); 
	  if (ptret != 0) printf("The thread failed\n");
  }
  
  for (i=0; i< thread_count; i++)	{
	  pthread_join (threads[i], NULL);

  }


  gettimeofday (&tv ,  &tz);
  time_end = (double)tv.tv_sec +
          (double)tv.tv_usec / 1000000.0;

/* this is for checking the results */
  for (i=0; i<N; i+=N/10) { printf(" %10.6f",x[i]); }
  printf("\n");
  printf ("time = %lf\n", time_end - time_start);

}

void *mult(void * arg) {
	struct thread_arg *local;
	local = arg;
	int i = 0, j=0;
	int start = local->seed * N/thread_count;
	int end = (local->seed + 1) * N/thread_count;
	if (local->seed == thread_count - 1) end = N;
	float *tempsum;
	tempsum = malloc(N * sizeof(int));
	
	for (i=0; i<N; i++) {
      for (j=start; j<end; j++) {
         tempsum[i] += a[i][j] * b[j];
      }
    }
	
	pthread_mutex_lock(&lock);
	for (i=0;i<N;i++) x[i] += tempsum[i];
	pthread_mutex_unlock(&lock);
}
