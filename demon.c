#include "demon.h"
#include "file.h"
#include "security.h"


int isNumber(char *number)
{
	char *tmp = number;
	while(*tmp != '\0')
	{
		if(*tmp < 48 || *tmp > 57)
			return 0;
		tmp++;
	}
	return 1;
}


void testProprietaireSpool(uid_t euid)
{
	if(euid != getuid())
	{
		fprintf(stderr, "demon:\trestricted to spool owner\n");
		exit(EXIT_FAILURE);
	}
}

void daemonize()
{
    pid_t pid, sid;
 
 	// Création du processus fils
    pid = fork();
 
 	// Erreur duplication processus
    if(pid < 0)
        exit(EXIT_FAILURE);

    // Processus père
    if( pid > 0)
        exit(EXIT_SUCCESS);

    // Processus fils
    if(pid == 0)
    {
    	printf("PID:\t%d\n", getpid());
    }
}

void compresser(char *file, int *status)
{
	switch (fork())
	{
		// Erreur
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			break;

		//On est dans le fils
		case 0:
			execlp("gzip", "gzip", "-n", file, NULL);
			break;

		//On est dans le père
		default:
		wait(status);
		;
	}
}

void treatSpool(uid_t euid, char *fichier)
{
	int fd_fichier;
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	FILE *fileLog;
	DIR *dp;
	struct dirent *d;
	struct stat *buf;
	
	// Allocation mémoire
	buf = malloc(sizeof(struct stat));

	// Ouverture du fichier log en écriture
	fileLog = fopen(fichier, "a");

	// Elevation des privilèges
	mySeteuid(euid);

	// Ouverture du SPOOL
	dp = opendir(spool);

	// Lecture de tout le dossier SPOOL
	while((d = readdir(dp)) != NULL)
	{
		// On cherche les fichiers d-XXXXXX
		if(d->d_name[0] == 'd')
		{
			// Données du job
			char login[MAX_CHAR];
			char fileName[MAX_CHAR];
			char dayOfWeek[MAX_CHAR];
			char month[MAX_CHAR];
			char day[MAX_CHAR];
			char hour[MAX_CHAR];
			char year[MAX_CHAR];
			char orgSize[MAX_CHAR];


			char d_name[MAX_CHAR];
			char j_name[MAX_CHAR];
			char j_zip[MAX_CHAR];
			time_t temps;
			char currentTime[MAX_CHAR];
			int status;
			FILE *d_file;

			d_name[0] = '\0';
			strcat(d_name, spool);
			strcat(d_name, "/");
			strcat(d_name, d->d_name);

			// Ouverture du fichier d_name en lecture
			d_file = fopen(d_name, "r");

			// On vérifie que le fichier est bien ouvert
			if(d_file != NULL)
			{
				// Lecture
				fscanf(d_file, "%s", login);
				fscanf(d_file, "%s", fileName);
				fscanf(d_file, "%s", orgSize);
				fscanf(d_file, "%s", dayOfWeek);
				fscanf(d_file, "%s", month);
				fscanf(d_file, "%s", day);
				fscanf(d_file, "%s", hour);
				fscanf(d_file, "%s", year);

				// Ecriture dans le fichier log
				fprintf(fileLog, "id=%s ", d->d_name + 2);
				fprintf(fileLog, "orgdate=%s  ", dayOfWeek);
				fprintf(fileLog, "%s  ", month);
				fprintf(fileLog, "%s ", day);
				fprintf(fileLog, "%s ", hour);
				fprintf(fileLog, "%s ", year);
				fprintf(fileLog, "user=%s ", login);
				fprintf(fileLog, "file=%s ", fileName);
				fprintf(fileLog, "file=%s\n", orgSize);
				temps = time(NULL);
				strcpy(currentTime, ctime(&temps));
				currentTime[strlen(currentTime)-1] = '\0';
				fprintf(fileLog, "\tcurdate=%s ", currentTime);


				j_name[0] = '\0';
				strcat(j_name, spool);
				strcat(j_name, "/");
				d->d_name[0] = 'j';
				strcat(j_name, d->d_name);

				j_zip[0] = '\0';
				strcat(j_zip, j_name);
				strcat(j_zip, ".gz");

				// compression du job
				compresser(j_name, &status);

				// Lecture et écriture de la taille du fichier zip
				stat(j_zip, buf);  
				fprintf(fileLog, "\tgzipsize=%lld ", buf->st_size);
				fprintf(fileLog, "\texit=%d\n", status);

				// Fermeture de fichier
				fclose(d_file);	
				unlink(d_name);	
			}
		}
	}

	// Free
	free(buf);

	// Fermeture du spool
	monCloseDir(dp);

	// Restriction des privilèges
	mySeteuid(getuid());

	// Fermeture du fichier log 
	fclose(fileLog);
}



int main(int argc, char *argv[])
{
	// ERROR : pas d'argument
	if(argc == 1)
	{
		errno = EINVAL;
		perror("demon [-d] [-f] [-i délai] fichier");
		exit(EXIT_FAILURE);
	}

	// On teste que l'utilisateur est bien le propriétaire du SPOOL
	uid_t euid = geteuid();
	testProprietaireSpool(euid);

	int delai = DELAI;
	char optstring[] = "dfi";  // options permises -d, -f et -i
	char *fichier = argv[argc-1];
	int option;
	int testOptionD = 0;
	int testOptionF = 0;
	int testOptionI = 0;

	while( (option = getopt(argc, argv, optstring)) != EOF ) 
	{
		switch(option)
		{
			case 'd':
				testOptionD = 1;
				break;

			case 'f':
				testOptionF = 1;
				break;

			case 'i':
				testOptionI = 1;
				if(isNumber(argv[optind]))
					delai = strtol(argv[optind], NULL, 10);
				else
				{
					fprintf(stderr, "delai:\tdoit être un entier\n");
					exit(EXIT_FAILURE);
				}
				break;

			// Options non supportés -> Erreur
			default:
				fprintf(stderr, "USAGE:\tlister [-l] [-u utilisateur]\n");
				exit(EXIT_FAILURE);
		}	
	}

	if(!testOptionD)
	{
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
	    close(STDERR_FILENO);
	}

	if(!testOptionF)
		daemonize();

	// On met à zéro le fichier gérant l'historique des traitements de jobs
	cleanFile(fichier);

	char lockFile[MAX_CHAR];
	char *env_projetse = getenv("PROJETSE");
	int fd_lockFile;

	lockFile[0] = '\0';
	strcat(lockFile, (env_projetse == NULL) ? SPOOL : env_projetse);
	strcat(lockFile, "/verrou");

	while(1)
	{
		// Verrouillage
		verrouiller(euid, &fd_lockFile, lockFile);

		// On traite les jobs du SPOOL
		treatSpool(euid, fichier);

		// Deverrouillage
		deverrouiller(fd_lockFile);

		printf("sleep!\n");
		sleep(delai);
	}

	return 0;
}