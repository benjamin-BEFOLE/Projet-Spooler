#!/bin/bash
# Usage: script1.sh [<path to executable files>]
# written by Vincent Loechner, Oct. 2017.
# MacOS/Linux + bash compatible

# L'emplacement des fichiers exécutables (demon deposer lister retirer) doit se
# trouver dans la variable $PATH, ou peut être donné en argument de ce script.
#
# Ce script fait des sudo du propriétaire du fichier exécutable demon, pour créer
# des répertoires, tuer des processus, etc. Si le fichier demon appartient à
# l'utilisateur qui lance ce script, certains tests ne seront pas fait (ceux qui
# nécessitent l'existence de plusieurs utilisateurs).
#
# La sortie normale est donnée ci-dessous (avec l'utilisateur "nobody" qui lance
# le demon, les exécutables se trouvant dans le répertoire ../corrige/).
# Si ce script affiche d'autres choses, c'est que votre implémentation ne respecte
# pas ce qui est demandé dans le sujet ! Si vous effectuez des affichages
# supplémentaires, non demandés et parasites, supprimez-les (les messages d'erreur
# doivent être affichés sur la sortie d'erreur).
# Lorsque ce script s'arrête suite à une erreur, les fichiers temporaires ne sont
# pas détruits et le demon tourne toujours : vous pouvez lancer la commande ayant
# provoqué l'erreur à la main pour vérifier pourquoi il y a eu une erreur.
#
# exécution réussie du script :
# $ ./script1.sh ../corrige/
# Cleaning: ok
# Running daemon (as 'nobody'): success
# 1. dropping one file in spooler
#    waiting for job(s) to finish..... ok
# 1b.checking log... ok
# 1c.checking 'lister -l' usage... ok
# 2. dropping two files simultaneously in spooler
#    waiting for job(s) to finish..... ok
# 3. dropping two files simultaneously and removing one
#    waiting for job(s) to finish..... ok
# 4. protecting folders and trying to drop one file
#    waiting for job(s) to finish..... ok
# 5. checking daemon running options: ok
# Stopping daemon and cleaning files: ok


# préfixe du spooldir et autres fichiers du demon (répertoire créé avec mktemp et log)
SPOOL=/tmp/demon

# fonctions d'affichage et de fin
ROUGE=1
VERT=2

plouf ()
{
  tput setaf $ROUGE
  echo "$1"
  tput sgr0
  exit 1
}

ok ()
{
  tput setaf $VERT
  echo "$1"
  tput sgr0
}

# fonction d'attente de fin de jobs
finjob ()
{
  echo -n "   waiting for job(s) to finish"
  while [ `lister | wc -l` -ne 0 ]
  do
    echo -n "."
    sleep 0.2
  done
  ok " ok"
}

##### vérification de l'existence des exécutables dans le PATH ou en argument
if [ $# -ge 1 ]
then
  export PATH=$PATH:$1
fi
which demon deposer lister retirer >/dev/null || \
    plouf "Cannot find spooler commands. Usage: $0 [<path of project>]"

# extraction du propriétaire du demon,
demon=`which demon`
SPOOLUSER=`ls -l $demon | tr -s " " | cut -d " " -f 3`
NORMALUSER=`id -un`
if [ $NORMALUSER != $SPOOLUSER ]
then
  # vérifie que le setuid bit est bien positionné
  [ -u $demon ] || plouf "daemon owned by '$SPOOLUSER' but sticky bit not set"
fi

##### nettoyage en cas d'échec d'une exécution précédente, puis lancement du démon
echo -n "Cleaning: "
killall demon 2>/dev/null
rm -rf $SPOOL.* 2>/dev/null
sudo -u $SPOOLUSER killall demon 2>/dev/null
sudo -u $SPOOLUSER rm -rf $SPOOL.* 2>/dev/null
SPOOLDIR=`sudo -u $SPOOLUSER mktemp -d $SPOOL.XXXXXX`
sudo -u $SPOOLUSER chmod 755 $SPOOLDIR
export PROJETSE="$SPOOLDIR"
ok "ok"

echo -n "Running daemon (as '$SPOOLUSER'): "
demon $SPOOLDIR.log || plouf "Failed"
ps eaux | grep "^$SPOOLUSER.* demon" >/dev/null || plouf "No demon process owned by '$SPOOLUSER'"
ok "success"

##### création des fichiers temporaires
rm -rf toto titi
seq 100 >toto
seq 200 >titi

##### début des tests

# vérifie que la liste est vide au début
[ `lister | wc -l` -eq 0 ] || plouf "'lister' prints unexistent jobs"
lister -z 2>/dev/null && plouf "'lister' accepts unknown option '-z'"

# ajoute un job, et vérifie qu'il est là
echo "1. dropping one file in spooler"
deposer toto >toto.sub || plouf "deposer failed"
lister >toto.lst || plouf "lister failed"

[ `cat toto.lst | wc -l` -eq 1 ] || plouf "'lister' does not print submitted job"
grep -f toto.sub toto.lst >/dev/null || plouf "Job id \"`cat toto.sub`\" not found in list"
finjob

# vérifie que la compression s'est bien passée:
# les tailles des fichiers original et compressé doivent être présents dans le log
echo -n "1b.checking log... "
osize=`cat toto | wc -c`
zsize=`cat toto | gzip | wc -c`
zsize2=`gzip -c toto | wc -c`
sudo -u $SPOOLUSER grep $osize $SPOOLDIR.log >/dev/null || plouf "original file size ($osize) not found in log"
sudo -u $SPOOLUSER grep -e $zsize -e $zsize2 $SPOOLDIR.log >/dev/null || plouf "compressed file size ($zsize) not found in log"
ok "ok"

echo -n "1c.checking 'lister -l' usage... "
# vérifie que lister -l ne fonctionne pas pour les autres utilisateurs
if [ $NORMALUSER != $SPOOLUSER ]
then
  lister -l 2>/dev/null && plouf "'lister -l' does not fail for non spool user"
fi
lister=`which lister`
sudo -u $SPOOLUSER PROJETSE="$PROJETSE" $lister -l >/dev/null || plouf  "'lister -l' not working for spool owner"
ok "ok"

# ajoute deux jobs, et vérifie qu'ils sont là
echo "2. dropping two files simultaneously in spooler"
deposer toto titi >titi.sub || plouf "deposer(2) failed"
lister >titi.lst || plouf "lister(2) failed"

[ `cat titi.lst | wc -l` -eq 2 ] || plouf "'lister' does not print two submitted jobs"
[ `grep -f titi.sub titi.lst | wc -l` -eq 2 ] || plouf "Two jobs not found in list"
#essaie de retirer un job inexistant
retirer 424242 2>/dev/null && plouf "removing an nonexisting job does NOT fail"
finjob

# ajoute deux jobs, en supprime un, et et vérifie que l'autre est là
echo "3. dropping two files simultaneously and removing one"
deposer toto titi >titi.sub || plouf "deposer(3) failed"
retirer `head -1 titi.sub` || plouf "retirer(3) failed"
lister >titi.lst || plouf "lister(3) failed"

[ `cat titi.lst | wc -l` -eq 1 ] || plouf "'lister' does not print the remaining submitted jobs"
REMAINING=`tail -1 titi.sub`
[ `grep $REMAINING titi.lst | wc -l` -eq 1 ] || plouf "Remaining job not found in list"
finjob

# protège les répertoires/fichiers
echo "4. protecting folders and trying to drop one file"
sudo -u $SPOOLUSER chmod go-r $SPOOLDIR
chmod 600 toto

deposer toto >toto.sub || plouf "deposer(4) failed"
lister >toto.lst || plouf "lister(4) failed"

[ `cat toto.lst | wc -l` -eq 1 ] || plouf "'lister' does not print submitted job"
grep -f toto.sub toto.lst >/dev/null || plouf "Job number `cat toto.sub` not found in list"
finjob

# vérifie les options de demon
echo -n "5. checking daemon running options: "
demon -z  $SPOOLDIR.log 2>/dev/null && plouf "'demon' accepts unknown option '-z'"
demon -di 5 $SPOOLDIR.log || plouf "Failed launching 'demon -di 5'"

ps eaux | grep "^$SPOOLUSER.* demon" | tr -s " " | cut -d " " -f 2 | \
  xargs sudo -u $SPOOLUSER kill 2>/dev/null -9

# vérifie que -f lance le demon en avant-plan
pid=`( demon -f $SPOOLDIR.log >/dev/null & echo $! )`
ps eaux | grep "^$SPOOLUSER[ ]*$pid.* demon" >/dev/null \
    || plouf "No demon process (run with option -f) owned by '$SPOOLUSER'"
sudo -u $SPOOLUSER kill -9 $pid
ps eaux | grep "^$SPOOLUSER.* demon" | grep -v "grep" >/dev/null \
    && plouf "'demon -f' process not killed by 'kill $pid'"
ok "ok"

# fin normale : nettoyage
echo -n "Stopping daemon and cleaning files: "
sudo -u $SPOOLUSER killall demon 2>/dev/null
sudo -u $SPOOLUSER rm -rf $SPOOLDIR*
rm -rf toto* titi*
ok "ok"
