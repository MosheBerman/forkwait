/* forkwait.c
 *
 * This is the second program for CISC 3350 Workstation Programming, Spring 2013
 *
 * Shell by Dayton Clark
 *
 * Completed by: 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>	// Necessary for Solaris.


/* Min and max number of processes to create.
 */
#define MINFORKS	3
#define MAXFORKS	5

/* Min and max sleeping time for the sub processes.
 */
#define MINPAUSE	100000
#define MAXPAUSE	2000000

static long elapsedTime(int next);
static unsigned generateSeed(unsigned seed);

/* Here we go!
 */
int
main(int argc, char* argv[])
{	
	//	Wire up the timer
	long time = elapsedTime(0);
	printf("%li: Start\n", time);
	
	/*	Generate a random number between MINFORKS and MAXFORKS
	 */
	unsigned int seed = generateSeed(0);
	int n = rand_r(&seed) % MAXFORKS + MINFORKS;
	
	
	
	/*	Log next step
	 */
	time = elapsedTime(1);
	printf("%li: Number of forks = %i\n", time, n);
	
	/*	Hang on to the PIDs so we can wait for them after forking
	 */
	int *PIDs = (pid_t *)(malloc(sizeof(*PIDs)*n));
	
	int sleeptime;
	
	/*	Fork n times
	 */
	
	int i;
	for (i = 0; i < n ;i++)
	{

		/* Randomize the child sleep time
		 */
		seed = generateSeed(0);
		sleeptime = (rand_r(&seed) % MAXPAUSE) + MINPAUSE;
		
		/*	Call fork() and retain the returned identifier
		 */
		
		pid_t processIdentifier = fork();

		/*	Check for errors
		 */
		if (processIdentifier == -1)
		{
			//	Errorr
			continue;
		}

		if (processIdentifier == 0)
		{
			/*	We're in the child process,
			 *	sleep and then exit with
			 *	i as the error code.
			 */

			usleep(sleeptime);
			_exit(i);
		}
		
		else if(processIdentifier > 0)
		{
			/*	We're in the parent:
			 *	Store the PID and
			 *  print out the results.
			 */
			
			PIDs[i] = processIdentifier;
			
			time = elapsedTime(1);
			printf("%li: Child %i, pid = %i, forked, waits %i usec\n", time,  i, processIdentifier, sleeptime);
		}
	}
	
	/*
	 *  Loop through the processes and wait for them
	 *	to terminate. Then print the childid, and the
	 *	the pid.
	 */
	
	
	for (i = 0; i < n; i++)
    {
		
        /*  Wait for the child process
         *  and grab it's status info
         */
        int status;
		
        pid_t pid = waitpid(-1, &status, 0);
		
        int childid =  WEXITSTATUS(status);
		
        /*  Log the results
         */
        time = elapsedTime(1);
        printf("%li: Child %i, pid = %i, terminated\n", time, childid, pid);
    }
	
	/* Release our PID array
	 */
	
	free(PIDs);
	
	/*	All done!
	 */
	time = elapsedTime(1);
	printf("%li: End\n", time);
	
}

/* elapsedTime(int next)
 *
 * If next is 0, then the timer is reset and elapsedTime() returns the value 0.
 * Otherwise elapsedTime returns the time elapsed since the last time the time
 * was set.  The time is returned in microseconds.
 *
 * NB: ElapsedTime() returns an int, so the elapsed time is rather limited.
 */
long
elapsedTime(int next)
{
  static struct timeval baseTime;
  struct timeval 	currentTime;
  long	 	 	elapsedTime;

#define USECPERSEC (1000000)

  /* Get the current time.
   */
  if(gettimeofday(&currentTime, NULL) < 0)
    {
      perror("gettimeofday()");
      exit(-1);
    }
  
  if(next)
    {
      /* Calculate the elapsed time.
       */
      elapsedTime = ((currentTime.tv_sec - baseTime.tv_sec) * USECPERSEC
      		     + (currentTime.tv_usec - baseTime.tv_usec));
    }
  else
    {
      /* If we are to initialize, set the base time.
       * ElapsedTime is always 0 for this case.
       */
      baseTime.tv_sec = currentTime.tv_sec;
      baseTime.tv_usec = currentTime.tv_usec;
      elapsedTime = 0;
    }

  return elapsedTime;
}

/* generateSeed(unsigned value)
 *
 * Generates a seed for random number generation.  If value is non-zero then is
 * used as the seed, if the value is zero a seed based on the current time of
 * day is generated.
 *
 * NB: the returned seed is always forced to be an odd number by turning on bit 0.
 */
static unsigned
generateSeed(unsigned seed)
{
  struct timeval 	currentTime;

  if(seed == 0)
    {   
      /* Get the current time.
       */
      if(gettimeofday(&currentTime, NULL) < 0)
	{
	  perror("gettimeofday()");
	  exit(-1);
	}
      seed = (unsigned) currentTime.tv_usec;
    }

  return(seed | 0x01);
}
