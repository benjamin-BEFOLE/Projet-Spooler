#include <unistd.h> 
#include <sys/types.h>
#include <string.h>


// « set-user-id-on-exec »: SEUID
// « user-id-on-exec »: EUID



/**
* @fn void testProprietaire(uid_t euid)
* @brief Teste si un utilisateur à le driot d'utiliser les options -l et -u
* @param euid EUID de l'utilisateur
*/
void testProprietaire(uid_t euid);

/**
* @fn void lister()
* @brief Liste les jobs du SPOOL (sans options -l et -u)
*/
void lister();

/**
* @fn void listerOptionLU(int optionL, int optionU, char *userName)
* @brief Liste les jobs du SPOOL (avec option -l ou option -u)
* @param optionL Vaut 1 avec option -l, et 0 sans option -l
* @param optionU Vaut 1 avec option -u, et 0 sans option -u
* @param userName Paramètre de l'option -u
*/
void listerOptionLU(int optionL, int optionU, char *userName);