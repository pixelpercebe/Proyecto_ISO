#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // Para lseek y close

#include "s_my_sha256header.h"
#include "codes.h"





int extrae_fichero(char * f_mysha256_Repo, char * f_dat)
{
    int fd_repo;
    int fd_reader;
    struct c_sha256header my_read_sha256header;
    ssize_t n;
    printf("\n Entra en extraer fichero \n");
    if ((fd_repo = open(f_mysha256_Repo, O_RDWR,0600))==-1)
    {
        printf("\nError opening f_mysha256_Repo\n");
        return ERROR_OPEN_SHA_REPO_FILE;
    }
    //while (lseek(fd_repo)>0)

    if ((n = read(fd_repo, &my_read_sha256header, sizeof(my_read_sha256header)))==sizeof(my_read_sha256header)) {
        printf("\nNombre del archivo: %s\n", my_read_sha256header.fname);
        printf("\ntamaño del archivo: %s\n", my_read_sha256header.size);

    }
    else
    {
        printf("\nError reading f_mysha256_Repo header\n");
        return ERROR_READ_FILE;
    }
    printf("\n SALE DE EXTRAER FICHERO \n");

    return OK;
    
}

