#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#ifndef DEPOSER_H

#define DEPOSER_H 
#define TAILLE_BUF 1024 
#define MAX_CHAR 500 
#define SPOOL "/Users/BenTen/Public/spool"  // Chemin du SPOOL


#endif


// « set-user-id-on-exec »: SEUID

/**
* @fn int myOpen(int *fd, char *name)
* @brief Permet d'ouvrir un fichier
* @param fd Descripteur de fichier
* @param name Nom du fichier à ouvrir
*/
int myOpen(int *fd, char *name);

/**
* @fn void myClose(int fd)
* @brief Ferme un fichier
* @param fd Descipteur du fichier à fermer
*/
void myClose(int fd);

/**
* @fn isFile(char *fileName)
* @brief Permet de savoir si un fichier existe (SEUID pris en compte)
* @param fileName Nom du fichier
*/
void isFile(char *fileName);

/**
* @fn void cleanFile(char *fileName)
* @brief Remet à zéro un fichier
* @param fileName Nom du fichier
*/
void cleanFile(char *fileName);

/**
* @fn DIR* monOpenDir(char *directoryName)
* @brief Permet d'ouvrir un dossier
* @param directoryName Nom du dossier à ouvrir
*/
DIR* monOpenDir(char *directoryName);

/**
* @fn void monCloseDir(DIR *dp)
* @brief Permet de fermer un dossier
* @param dp De type DIR *
*/
void monCloseDir(DIR *dp);

/**
* @fn myMkstemp(uid_t euid, int *fd, char *motif)
* @brief Crée un fichier temporaire (SEUID pris en compte)
* @param euid Identificateur effectif de l'utilisateur
* @param fd Reçoit le descripteur du fichier créer
* @param motif Nom du fichier
*/
void myMkstemp(uid_t euid, int *fd, char *motif);

/**
* @fn void copyFile(int fd_source, int fd_cible)
* @brief Copie un fichier
* @param fd_source Descripteur du fichier à copier
* @param fd_cible Descripteur de fichier de la copie
*/
void copyFile(int fd_source, int fd_cible);