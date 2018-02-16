#include <sys/stat.h>
#include <string.h>
#include <time.h>

#ifndef DEMON_H

	#define DEMON_H 
	#define DELAI 5   // delai par défaut
	
#endif

// « set-user-id-on-exec »: SEUID
// « user-id-on-exec »: EUID



/**
* @fn int isNumber(char *number)
* @brief Teste si une chaîne de caractères est un nombre
* @param number Chaîne de caractères
* @return Un booléen
*/
int isNumber(char *number);

/**
* @fn void testProprietaireSpool(uid_t euid)
* @brief Teste si un utilisateur est le propriétaire du SPOOL
* @param euid EUID de l'utilisateur
*/
void testProprietaireSpool(uid_t euid);

/**
* @fn void compresser(char *file, int *status)
* @brief Permet de compresser un fichier
* @param file Nom du fichier à compresser
* @param status Reçoit le signal d'arrêt de "execlp()"
*/
void compresser(char *file, int *status);

/**
* @fn void daemonize()
* @brief Fait passé un processus en arrière plan
*/
void daemonize();

/**
* @fn void treatSpool(uid_t euid, char *fichier)
* @brief Permet de traiter les jobs du SPOOL
* @param euid EUID de l'utilisateur
* @param fichier Historique de traitement de jobs
*/
void treatSpool(uid_t euid, char *fichier);