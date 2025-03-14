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

extern void printPermissions(mode_t permissions);

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
                printf("\nName dont match: %s\n", my_readed_sha256header.fname);
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
            write(1,"\nFILE NOT FOUND\n",18);
            return OK;
        }
        
    }

    if (!is_not_header)
    {
        write(1,"\nFILE FOUND\n",14);
        printf("\nFile hash: %s\n", my_readed_sha256header.hash);
        printf("\nFile name: %s\n", my_readed_sha256header.fname);
        if (create_file(fd_repo,&my_readed_sha256header) < 0)
        {
            write(1,"\nError creating destination file for the extracted file\n",58);
            return E_CREATDEST;
        }
        
    }
    return fd_repo;
}

int create_file(int fd_repo, struct c_sha256header *my_readed_sha256header)
{
    int blocks_to_read= 0;
    int fd_newfile;
    int i,n;
    char FileDataBlock[DATAFILE_BLOCK_SIZE];

    mode_t saved_permissions;

    saved_permissions = my_readed_sha256header ->permissions;
    printPermissions(saved_permissions);
    if((fd_newfile = open(my_readed_sha256header->fname, O_WRONLY | O_CREAT | O_TRUNC, saved_permissions))<0)
    {
        fprintf(stderr,"Error opening new destination file in extract: %s\n",ERROR_OPEN_DAT_FILE);
        return ERROR_OPEN_DAT_FILE;
    }

    if (chmod(my_readed_sha256header->fname, saved_permissions) < 0) {
        fprintf(stderr, "Error setting permissions on destination file: %d\n", ERROR_CHANGING_PERMISSIONS);
    }

    printf("\nheader size = %ld\n",my_readed_sha256header->size);
    blocks_to_read = my_readed_sha256header->size / DATAFILE_BLOCK_SIZE;
    printf("\nblocks = %ld\n",blocks_to_read);    

    for (i = 0; i < blocks_to_read; i++){
        if((n = read(fd_repo,FileDataBlock,DATAFILE_BLOCK_SIZE))==-1)
        {
            return ERROR_READ_FILE;
        }
        if (write(fd_newfile,FileDataBlock,n)==-1)
        {
            return ERROR_WRITE_FILE;
        }
    }
    
    if ((close(fd_newfile)) == -1)
    {
        fprintf(stderr,"Error closing new destination file in extract: %s\n",ERROR_CLOSE_FILE );
        return ERROR_CLOSE_FILE;
    }
    return OK;
}
