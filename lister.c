#include "lister.h"
#include "file.h"
#include "security.h"


void testProprietaire(uid_t euid)
{
	if(euid != getuid())
	{
		fprintf(stderr, "lister: Option -l or -u are restricted to spool owner\n");
		exit(EXIT_FAILURE);
	}
}


void lister()
{
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	DIR *dp;
	struct dirent *d;

	// Elevation des privilèges
	mySeteuid(geteuid());

	// Ouverture du SPOOL
	dp = opendir(spool);

	// Lecture de tout le dossier SPOOL
	while((d = readdir(dp)) != NULL)
	{
		if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0
			&& d->d_name[0] != '.')
		{
			if(d->d_name[0] == 'd')
			{
				char d_name[MAX_CHAR];
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
					char dayOfWeek[MAX_CHAR];
					char month[MAX_CHAR];
					char day[MAX_CHAR];
					char hour[MAX_CHAR];
					char year[MAX_CHAR];
					char trash[MAX_CHAR];

					// Lecture
					fscanf(fichier, "%s", trash);
					fscanf(fichier, "%s", trash);
					fscanf(fichier, "%s", trash);
					fscanf(fichier, "%s", dayOfWeek);
					fscanf(fichier, "%s", month);
					fscanf(fichier, "%s", day);
					fscanf(fichier, "%s", hour);
					fscanf(fichier, "%s", year);

					// Affichage des jobs du SPOOL
					printf("%s  ", d->d_name + 2);
					printf("%s\t\t", getlogin());
					printf("%s  ", dayOfWeek);
					printf("%s    ", month);
					printf("%s  ", day);
					printf("%s  ", hour);
					printf("%s\n", year);

					// Fermeture du fichier
					fclose(fichier);
				}
			}
		}
	}

	// Fermeture du spool
	monCloseDir(dp);

	// Restriction des privilèges
	mySeteuid(getuid());
}


void listerOptionLU(int optionL, int optionU, char *userName)
{
	char *env_projetse = getenv("PROJETSE");
	char *spool = (env_projetse == NULL) ? SPOOL : env_projetse;
	DIR *dp;
	struct dirent *d;

	// Elevation des privilèges
	mySeteuid(geteuid());

	// Ouverture du SPOOL
	dp = opendir(spool);

	// Lecture de tout le dossier SPOOL
	while((d = readdir(dp)) != NULL)
	{
		if(strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0
			&& d->d_name[0] != '.')
		{
			if(d->d_name[0] == 'd')
			{
				char d_name[MAX_CHAR];
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
					char fileName[MAX_CHAR];
					char dayOfWeek[MAX_CHAR];
					char month[MAX_CHAR];
					char day[MAX_CHAR];
					char hour[MAX_CHAR];
					char year[MAX_CHAR];
					char trash[MAX_CHAR];

					// Lecture
					fscanf(fichier, "%s", login);
					fscanf(fichier, "%s", fileName);
					fscanf(fichier, "%s", trash);
					fscanf(fichier, "%s", dayOfWeek);
					fscanf(fichier, "%s", month);
					fscanf(fichier, "%s", day);
					fscanf(fichier, "%s", hour);
					fscanf(fichier, "%s", year);

					// Affichage des jobs du SPOOL
						// Option -u activée
						if(optionU)
						{
							if(strcmp(userName, login) == 0)
							{
								// Si l'option -l est activée
								if(optionL)
									printf("%20s ", fileName);
								
								printf("%s  ", d->d_name + 2);
								printf("%s\t\t", login);
								printf("%s  ", dayOfWeek);
								printf("%s    ", month);
								printf("%s  ", day);
								printf("%s  ", hour);
								printf("%s\n", year);	
							}
						}

						// Option -u non activée
						else
						{
							// Si l'option -l est activée
							if(optionL)
								printf("%20s ", fileName);
							
							printf("%s  ", d->d_name + 2);
							printf("%s\t\t", login);
							printf("%s  ", dayOfWeek);
							printf("%s    ", month);
							printf("%s  ", day);
							printf("%s  ", hour);
							printf("%s\n", year);	
						}

					// Fermeture du fichier
					fclose(fichier);
				}
			}
		}
	}

	// Fermeture du spool
	monCloseDir(dp);

	// Restriction des privilèges
	mySeteuid(getuid());
}



int main(int argc, char *argv[])
{
	char optstring[] = "lu";  // options permises -l et -u
	char *userName; 
	char *env_projetse = getenv("PROJETSE");
	char lockFile[MAX_CHAR];
	int fd_lockFile;
	int option;
	int testOptionL = 0;
	int testOptionU = 0;
	uid_t euid = geteuid();

	lockFile[0] = '\0';
	strcat(lockFile, (env_projetse == NULL) ? SPOOL : env_projetse);
	strcat(lockFile, "/verrou");

	// Erreur: argc est supérieur au nombre max d'argument
	if(argc > 4)
	{
		fprintf(stderr, "USAGE:\tlister [-l] [-u utilisateur]\n");
		exit(EXIT_FAILURE);
	}

	while( (option = getopt(argc, argv, optstring)) != EOF ) 
	{
		switch(option)
		{
			case 'u':
				testOptionU = 1;

				// Pas d'utilisateur en paramètre
				if(optind+1 >= argc)
				{
					fprintf(stderr, "USAGE:\tlister [-l] [-u utilisateur]\n");
					exit(EXIT_FAILURE);
				}
				
				userName = argv[optind];
				break;

			case 'l':
				testOptionL = 1;
				break;

			default:
				fprintf(stderr, "USAGE:\tlister [-l] [-u utilisateur]\n");
				exit(EXIT_FAILURE);
		}	
	}

	// Verrouillage
	verrouiller(geteuid(), &fd_lockFile, lockFile);

	if(testOptionL || testOptionU)
	{
		testProprietaire(euid);

		if(testOptionU) 
		{
			// Pas d'utilisateur en paramètre
			if(userName == NULL)
			{
				fprintf(stderr, "USAGE:\tlister [-l] [-u utilisateur]\n");
				exit(EXIT_FAILURE);
			}
		}

		// utilisateur bien déféni
		listerOptionLU(testOptionL, testOptionU, userName);

		return 0;
	}

	// Affichage des jobs du SPOOL
	lister();

	// Deverrouillage
	deverrouiller(fd_lockFile);

	return 0;
}