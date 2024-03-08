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
#include <stdbool.h>

#include "s_my_sha256header.h"
#include "codes.h"


int my_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}

int extrae_fichero(char * f_mysha256_Repo, char * f_dat)
{
    off_t header_pos;
    int fd_repo;
    int data_size;
    struct c_sha256header my_readed_sha256header;
    ssize_t n;
    bool is_not_header;

    header_pos = 0;
    is_not_header = true;

    if ((fd_repo = open(f_mysha256_Repo, O_RDWR,0600))==-1)
    {
        printf("\nError opening f_mysha256_Repo\n");
        return ERROR_OPEN_SHA_REPO_FILE;
    }

    while (is_not_header)
    {
        if ((n = read(fd_repo, &my_readed_sha256header, sizeof(my_readed_sha256header)))==sizeof(my_readed_sha256header)) 
        {
            if (my_strcmp(my_readed_sha256header.fname, f_dat)==0)
            {
                is_not_header = false;
            }
            else
            {
                //data_size = (my_readed_sha256header.size)-FILE_HEADER_SIZE; //saves the size of the data block to be skipped
                printf("\nData name: %s\n", my_readed_sha256header.fname);
                printf("\nData hash: %s\n", my_readed_sha256header.hash);
                header_pos = my_readed_sha256header.size-FILE_HEADER_SIZE;
                
                if (lseek(fd_repo, header_pos, SEEK_CUR) == (off_t)-1)
                {
                    write(1, "\nError when jumping to the next header\n",39);
                    return ERROR_OTHER_1;
                }
                memset(&my_readed_sha256header, 0, sizeof(my_readed_sha256header));
            }
            
        }
        else
        {
            fprintf(stderr,"\nFILE NOT FOUND\n");
            return OK;
        }
        
    }

    if (!is_not_header)
    {
        printf("\nFile name: %s\n", my_readed_sha256header.fname);
        printf("\nFile hash: %s\n", my_readed_sha256header.hash);
        write(1,"\nFile found\n",13);
    }
    return fd_repo;
}
