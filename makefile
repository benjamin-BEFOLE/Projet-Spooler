
all : demon deposer lister retirer droits


# Exécutables
demon : security.o file.o demon.o
	gcc -o demon demon.o security.o file.o

deposer : security.o file.o deposer.o
	gcc -o deposer deposer.o security.o file.o

lister : lister.o file.o security.o
	gcc -o lister lister.o file.o security.o

retirer : retirer.o file.o security.o
	gcc -o retirer retirer.o file.o security.o


# Fichiers .O
security.o : security.c security.h file.h
	gcc -c security.c

file.o : file.c file.h security.h
	gcc -c file.c

demon.o : demon.c demon.h file.h security.h
	gcc -c demon.c

deposer.o : deposer.c deposer.h file.h security.h
	gcc -c deposer.c

lister.o : lister.c lister.h file.h
	gcc -c lister.c

retirer.o : retirer.c retirer.h file.h
	gcc -c retirer.c


# Elévation des droits
droits : demon deposer lister retirer
	chmod u+s demon
	chmod u+s deposer
	chmod u+s lister
	chmod u+s retirer


# Efface
clean :
	rm *.o *.gz demon deposer lister retirer spool/j-*  spool/d-* 

# Archive
archive :
	tar -zcvf BEFOLE_MASSART.tar.gz *.c *.h makefile script2.sh

