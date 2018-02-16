#include <string.h>
#include "retirer.h"
#include "file.h"
#include "security.h"


void retirer(char *id_job, uid_t euid)
{
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	DIR *dp;
	struct dirent *d;
	int jobIsPresent = 0;

	// Elevation des privilèges
	mySeteuid(euid);

	// Ouverture du SPOOL
	dp = opendir(spool);

	// Lecture de tout le dossier SPOOL
	while((d = readdir(dp)) != NULL)
	{
		// On ne travaille qu'avec les fichiers d-XXXXXX
		if(d->d_name[0] == 'd')
		{
			if(strcmp(d->d_name + 2, id_job) == 0)
			{
				jobIsPresent = 1;
				char d_name[MAX_CHAR];
				char j_name[MAX_CHAR];
				// char tmp[MAX_CHAR];
				FILE *fichier;

				d_name[0] = '\0';
				strcat(d_name, spool);
				strcat(d_name, "/");
				strcat(d_name, d->d_name);

				// Ouverture du fichier d_name en lecture
				fichier = fopen(d_name, "r");

				// On vérifie que le fichier est bien ouvert
				if(fichier != NULL)
				{
					char login[MAX_CHAR];

					// Lecture du login
					fscanf(fichier, "%s", login);

					// L'utilisateur a les droits de suppression
					if(euid == getuid() || strcmp(login, getlogin()) == 0)
					{
						j_name[0] = '\0';
						strcat(j_name, spool);
						strcat(j_name, "/");
						d->d_name[0] = 'j';
						strcat(j_name, d->d_name);
						
						unlink(d_name);
						unlink(j_name);
						printf("job %s retiré\n", id_job);
					}
					
					// L'utilisateur n'a pas les droits de suppression
					else
						fprintf(stderr, "job %s:\tpermission denied\n", id_job);

					// Fermeture du fichier
					fclose(fichier);
				}
			}
		}
	}

	// Si l'id id_job n'existe pas dans le SPOOL
	if(!jobIsPresent)
	{
		fprintf(stderr, "Cannot find id %s\n", id_job);
		exit(EXIT_FAILURE);
	}

	// Fermeture du spool
	monCloseDir(dp);

	// Restriction des privilèges
	mySeteuid(getuid());
}


int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stderr, "USAGE:\tretirer id ... id\n");
		exit(EXIT_FAILURE);
	}

	int fd_lockFile;
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	char lockFile[MAX_CHAR];
	uid_t euid = geteuid();


	lockFile[0] = '\0';
	strcat(lockFile, spool);
	strcat(lockFile, "/verrou");

	// Verrouillage
	verrouiller(euid, &fd_lockFile, lockFile);

	for (int i = 1; i < argc; ++i)
	{
		retirer(argv[i], euid);
	}

	// Deverrouillage
	deverrouiller(fd_lockFile);

	return 0;
}