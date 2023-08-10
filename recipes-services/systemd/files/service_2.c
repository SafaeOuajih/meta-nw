#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <syslog.h>

#define ByteSize 5
#define BackingFile "/shMemE"
#define AccessPerms 0644
#define TempSize 5
#define StatIter 5

void p_exit(const char* msg) {
	syslog(LOG_ERR,"%s\n",msg);
	exit(-1);
}

int average(int a[], int n)
{
	int sum = 0;
	for (int i = 0; i < n; i++)
		sum += a[i];

	return sum / n;
}

int main() {
	char *temperature = malloc(sizeof(char)*TempSize);
	int it = 0, i = 0;
	int *saved_data = (int *)malloc(StatIter*sizeof(int));
	int fd = shm_open(BackingFile, O_RDWR, AccessPerms);

	if (fd < 0)
	{
		free(saved_data);
		free(temperature);
		p_exit("Can't get file descriptor...");
	}
	/* get a pointer to memory */
	caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
			      ByteSize,   /* how many bytes */
			      PROT_READ | PROT_WRITE, /* access protections */
			      MAP_SHARED, /* mapping visible to other processes */
			      fd,         /* file descriptor */
			      0);         /* offset: start at 1st byte */

	if ((caddr_t) -1 == memptr)
	{
		free(temperature);
		p_exit("Can't access segment");
	}

	sem_t* semptr_empty = sem_open("empty",       /* name */
				       O_CREAT,       /* create the semaphore */
				       AccessPerms,   /* protection perms */
				       0);            /* initial value */

	while(1)
	{
		if (sem_wait(semptr_empty) == -1)    /* Decrement the semaphore */
		{
			free(temperature);
			p_exit("sem_wait: fail");
		}

		strcpy(temperature, memptr);         /* Get the temperature value from the shared memory */

		if (sem_post(semptr_empty) < 0)      /* Increment the semaphore */
		{
			free(temperature);
			p_exit("sem_post: fail");
		}

		syslog(LOG_INFO, "Temperature %s\n", temperature);

		if (i == 2)
		{
			/* Get statistics */
			saved_data[it++] = atoi(temperature);
			i = 0;
		}
		if (it == StatIter)
		{
			syslog(LOG_INFO, "The average from %d cycles is : %d\n", StatIter, average(saved_data, StatIter));
			it = 0;
		}

		i++;
		sleep(1);
	}

	return 0;
}
