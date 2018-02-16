#include <unistd.h> 
#include <sys/types.h>

// « set-user-id-on-exec »: SEUID
// « user-id-on-exec »: EUID





/**
* @fn void retirer(char *id_job, uid_t euid)
* @brief Retire un job du SPOOL connaissant son identifiant
* @param id_job Identifiant du job
* @param euid EUID de l'utilisateur
*/
void retirer(char *id_job, uid_t euid);