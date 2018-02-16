#include "deposer.h"
#include "file.h"
#include "security.h"



char *getIdJob(char *str)
{
	char *tmp1 = strtok(str, "/"), *tmp2;

	while((tmp1 = strtok(NULL, "/")) != NULL)
	{
		tmp2 = tmp1; 
	}

	return tmp2 + 2;
}

char *getFileName(char *str)
{
	char *tmp1 = str, *tmp2 = str;

	while(*tmp1 != '\0')
	{
		if(*tmp1 == '/')
			tmp2 = tmp1 + 1; 
		tmp1++;
	}

	return tmp2;
}



int main(int argc, char *argv[])
{
	// ERROR : pas d'argument
	if(argc == 1)
	{
		errno = EINVAL;
		perror("deposer fichier ... fichier");
		exit(EXIT_FAILURE);
	}

	int fd_lockFile;
	uid_t euid = geteuid();
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	char lockFile[MAX_CHAR];


	lockFile[0] = '\0';
	strcat(lockFile, spool);
	strcat(lockFile, "/verrou");

	for (int i = 1; i < argc; ++i)
	{
		int fd_source;
		int fd_j_name;
		int fd_d_name;
		char *fileSource;
		FILE *fichier;
		char *id_job;
		char j_name[MAX_CHAR]; 
		char d_name[MAX_CHAR]; 	
		struct stat *buf;
		time_t temps;

		// Allocation mémoire
		buf = malloc(sizeof(struct stat));

		// Verrouillage
		verrouiller(euid, &fd_lockFile, lockFile);

		// Ouverture du fichier source 
		if(myOpen(&fd_source, argv[i]))
		{	
			j_name[0] = '\0';
			strcat(j_name, spool);
			strcat(j_name, "/j-XXXXXX");

			// Création du fichier temporaire j_name
			myMkstemp(euid, &fd_j_name, j_name);

			// Affichage de l'id du job
			id_job = getIdJob(j_name);
			printf("%s\n", id_job);

			// Copie du fichier argv[i] dans j_name
			copyFile(fd_source, fd_j_name);

			d_name[0] = '\0';
			strcat(d_name, (env_projetse == NULL) ? SPOOL : env_projetse);
			strcat(d_name, "/d-");
			strcat(d_name, id_job);

			// Ouverture du fichier d_name en écriture
			mySeteuid(euid);
			fichier = fopen(d_name, "w");
			mySeteuid(getuid());

			// On vérifie que le fichier est bien ouvert
			if(fichier != NULL)
			{
				stat(argv[i], buf); 
				fileSource = getFileName(argv[i]);
				temps = time(NULL);

				// Ecriture
				fprintf(fichier, "%s\n", getlogin());
				fprintf(fichier, "%s\n", (fileSource == NULL) ? argv[i] : fileSource);
				fprintf(fichier, "%lld\n", buf->st_size);
				fprintf(fichier, "%s\n", ctime(&temps));

				// Fermeture du fichier
				fclose(fichier);
			}

			// Fermeture des fichiers
			myClose(fd_source);
			myClose(fd_j_name);
		}

		// Free
		free(buf);

		// Deverrouillage
		deverrouiller(fd_lockFile);
	}

	return 0;
}