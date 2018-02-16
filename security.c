#include "security.h"
#include "file.h"


void mySeteuid(uid_t euid)
{
	if(seteuid(euid) < 0){
		perror("seteuid");
		exit(EXIT_FAILURE);
	}
}


void verrouiller(uid_t euid, int *fd, char *name)
{
	// Elevation des privilèges
	mySeteuid(euid);

	// Ouverture du fichier "verrou"
	*fd = open(name, O_RDWR | O_CREAT, 0666);

	// Erreur d'ouverture
	if(*fd < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	
	while(lockf(*fd, F_TEST, 0) < 0)
    {
    	// printf("Fichier verrouillé\n");
		sleep(1); /* tant qu'il est lock on sleep */
    }

    // printf("Fichier non verrouillé\n"); //

    // Erreur de verrouillage
	if(lockf(*fd, F_LOCK, 0) < 0)
	{
		perror("lockf");
	}

	// Diminution des privilèges
	mySeteuid(getuid());
}

void deverrouiller(int fd)
{
	// Erreur de deverrouillage
	if(lockf(fd, F_ULOCK, 0) < 0)
	{
		perror("lockf");
	}

	myClose(fd);
}