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

void p_exit(const char* msg) {
	syslog(LOG_ERR, "%s\n", msg);
	exit(-1);
}

int main() {
	int temp = 0; /* Variable to hold the generated temperature value */
	char *temperature = malloc(sizeof(char)*TempSize); /* Variable to hold the temperature value */

	/* create/open shared memory */
	int fd = shm_open(BackingFile,      /* Name */
			  O_RDWR | O_CREAT, /* read/write, create if needed */
			  AccessPerms);     /* access permissions */
	if (fd < 0)
	{
		free(temperature);
		p_exit("Can't open shared mem segment...");
	}

	ftruncate(fd, ByteSize); /* truncate to ByteSize */

	/* memory map the shared memory object */
	caddr_t memptr = mmap(NULL,       /* let system pick where to put segment */
			      ByteSize,   /* how many bytes */
			      PROT_READ | PROT_WRITE, /* access protections */
			      MAP_SHARED, /* mapping visible to other processes */
			      fd,         /* file descriptor */
			      0);         /* offset: start at 1st byte */

	if ((caddr_t) -1  == memptr)
	{
		free(temperature);
		p_exit("Can't get segment");
	}

	/* Create/Open a semaphore */
	sem_t* semptr_empty = sem_open("empty",       /* semaphore name */
				       O_CREAT,       /* create the semaphore */
				       AccessPerms,   /* protection perms */
				       1);            /* initial value to 1 */

	while (1)
	{
		temp = rand() % 201 - 100; /* Generate a rand temperature value -100 to 100 C */
		sprintf(temperature,"%d",temp);

		/* Decrement the semaphore to write to shm */
		if (sem_wait(semptr_empty) == -1)
		{
			free(temperature);
			p_exit("sem_wait: fail");
		}

		strcpy(memptr, temperature);


		/* Increment the semaphore so that readers can read */
		if (sem_post(semptr_empty) < 0)
		{
			free(temperature);
			p_exit("sem_post: fail");
		}

		syslog(LOG_INFO,"Posted\n");
		sleep(2); /* Sleep to let readers get the value */
	}

	return 0;
}
