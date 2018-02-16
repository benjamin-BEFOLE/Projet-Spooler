#include "file.h"
#include "security.h"



int myOpen(int *fd, char *name)
{
	*fd = open(name, O_RDWR);

	// Erreurd'ouverture
	if(*fd < 0)
	{
		fprintf(stderr, "%s\t\t", name);
		perror("open");
		// exit(EXIT_FAILURE);
		return 0;
	}
	return 1;
}


void myClose(int fd)
{
	// Erreur de fermeture
	if(close(fd) < 0)
	{
		perror("close");
		// exit(EXIT_FAILURE);
	}
}


void isFile(char *fileName)
{
	uid_t euid = geteuid();

	// Restriction des privilèges
	mySeteuid(getuid());
	
	int fd = open(fileName, O_RDWR);

	// Erreur
	if(fd < 0)
	{
		fprintf(stderr, "isFile:\t%s:\tCannot find or permission denied\n", fileName);
		exit(EXIT_FAILURE);
	}
	myClose(fd);

	// Elévation des privilèges
	mySeteuid(euid);
}


void cleanFile(char *fileName)
{
	FILE *fichier;

	// Ouverture du fichier fileName en écriture
	fichier = fopen(fileName, "w");

	// Pas d'erreur d'ouverture
	if(fichier != NULL)
	{
		// Ecriture
		fprintf(fichier, "%s", "\0");

		// Fermeture du fichier
		fclose(fichier);	
	}
}


// Permet d'ouvrir un dossier
DIR* monOpenDir(char *directoryName)
{
	DIR *dp = opendir (directoryName);	
	return dp;
}


// Permet de fermer un dossier
void monCloseDir(DIR *dp)
{
	int ret = closedir(dp);

	// Erreur de fermeture
	if(ret == -1){
		perror("closedir");
	}
}


void myMkstemp(uid_t euid, int *fd, char *motif)
{
	// Elevation des privilèges
	mySeteuid(euid);

	// Création du fichier temporaire
	*fd = mkstemp(motif);

	// Erreur 
	if(*fd < 0)
	{
		perror("mkstemp");
		exit(EXIT_FAILURE);
	}

	// Restriction des privilèges
	mySeteuid(getuid());
}


void copyFile(int fd_source, int fd_cible)
{
    int nb_car_lu; 			// Nombre de caractères lus
    char buf[TAILLE_BUF];

	// On copie  
    while (( nb_car_lu = read(fd_source, buf, TAILLE_BUF)) > 0) {
        if ( write(fd_cible, buf, nb_car_lu) != nb_car_lu)
			perror("Erreur Ecriture en cour de copie");
    }
}
