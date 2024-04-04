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

// function that opens a RepoFile with O_append
int insertar_fichero(char RepoFileName[256])
{
    int fd_RepoFile = open(RepoFileName, O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd_RepoFile == -1)
    {
    fprintf(stderr, "Cannot open file RepoFileName %s\n", RepoFileName);
    return ERROR_OPEN_SHA_REPO_FILE;
    }
    return fd_RepoFile;
}


// function that opens a RepoFile with lseek and header.
int insertar_fichero_lseek(char RepoFileName[256]) {
    
    if (RepoFileName == NULL || RepoFileName[0] == '\0') {
        fprintf(stderr, "Invalid file RepoName.\n");
        return ERROR_OPEN_FILE;
    }

    int fd_RepoFile = open(RepoFileName, O_WRONLY | O_CREAT, 0600);

    
    if (fd_RepoFile == -1) 
    {
        fprintf(stderr, "Cannot open file RepoFileName %s\n", RepoFileName);
        return ERROR_OPEN_SHA_REPO_FILE;
    }
    // Move the pointer to the end of the 'RepoFile' file to ensure that the content is appended to the end.
    if (lseek(fd_RepoFile, 0, SEEK_END) == (off_t)-1) {
        fprintf(stderr, "ERROR WITH LSEEK %d\n", E_DESCO);
        return E_DESCO;
    }

    return fd_RepoFile; 
}


