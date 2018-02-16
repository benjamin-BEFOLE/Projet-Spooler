#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h> 





// « set-user-id-on-exec »: SEUID
// « user-id-on-exec »: EUID



/**
* @fn void mySeteuid(uid_t euid)
* @brief Change l'EUID d'un utilisateur
* @param euid nouveau EUID
*/
void mySeteuid(uid_t euid);

/**
* @fn void verrouiller(uid_t euid, int *fd, char *name)
* @brief Verrouille un fichier (SEUID pris en compte)
* @param euid 
* @param fd Reçoit le descripteur du fichier verrouillé
* @param name Nom du fichier à verrouiller
*/
void verrouiller(uid_t euid, int *fd, char *name);

/**
* @fn void deverrouiller(int fd)
* @brief Deverrouille un fichier 
* @param fd Descripteur du fichier à déverrouiller
*/
void deverrouiller(int fd);