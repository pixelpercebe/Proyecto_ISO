/* * @file my-sha256-repo.c
 * * @author G.A.
 * * @date 6/02/2024
 * * @brief First version of mysha256-repo
 * * @details  Create a tar file with only one "data file"
 * *
 * */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "s_my_sha256header.h"
#include "codes.h"

extern int sha256sum_file(char *path, char outputBuffer[]); // HEX_SHA256_HASH_SIZE Bytes
extern void sha256_hash_to_string(unsigned char *hash, unsigned char *outputBuffer);
extern int insertar_fichero(char RepoFileName[256]);
extern int insertar_fichero_lseek(char RepoFileName[256]);
extern int extrae_fichero(char * f_mysha256_Repo, char * f_dat);


// Build RepoHeader structure with FileName (fill a new RepoHeader)
int Builsha256RepoHeader(char *FileName, struct c_sha256header *psha256header)
{
    char calc_hash[HEX_SHA256_HASH_SIZE];
    int Return_Code;

    // Fill all struct psha256header members

    bzero(psha256header, sizeof(struct c_sha256header)); // Fill all struct data with zeros

    strcpy(psha256header->fname, FileName); // File name

    psha256header->size = 0; // At the moment it is an unknown value
                             // Later we can fill it in

    // Call to function (sha256_file) to generate sha256 sum of file data
    Return_Code = sha256sum_file(FileName, calc_hash);

    if (Return_Code != 0)
    {
        fprintf(stderr, "Error on hash function call (err=%d) \n", Return_Code);
        return SHA256_GEN_ERR;
    }

    strcpy(psha256header->hash, calc_hash); // fill  sha256 hash psha256header of argv[1]
                                            // in psha256header
    return HEADER_OK;
}

// ----------------------------------------------------------------
// (1.4) write the data file (FileName)  to RepoFile (blocks of 4096 bytes -4KB)
unsigned long WriteFileDataBlocks(int fd_DataFile, int fd_RepoFile)
{
    char FileDataBlock[DATAFILE_BLOCK_SIZE];
    unsigned long NumWriteBytes;
    int Tam, n;

    // write the data file (blocks of READ_BLOCK_SIZE  bytes) (4KB)
    NumWriteBytes = 0;
    bzero(FileDataBlock, sizeof(FileDataBlock));

    while ((Tam = read(fd_DataFile, FileDataBlock, sizeof(FileDataBlock))) > 0)
    {
        n = write(fd_RepoFile, FileDataBlock, Tam);
        if (n < 0)
        {
            // Handle write error when open returns -1
            fprintf(stderr, "Write error");
            exit(ERROR_WRITE_FILE);
        }
        NumWriteBytes += n;
        // Clear the buffer
        bzero(FileDataBlock, sizeof(FileDataBlock));
    }

    if (Tam < 0)
    {
        // Handle read error when open returns -1
        fprintf(stderr, "read error");
        exit(ERROR_READ_FILE);
    }
    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    unsigned long bytesFaltantes = DATAFILE_BLOCK_SIZE - (NumWriteBytes % DATAFILE_BLOCK_SIZE);
    if (bytesFaltantes > 0 && bytesFaltantes < DATAFILE_BLOCK_SIZE) 
    {
        char bufferCeros[DATAFILE_BLOCK_SIZE] = {0}; // Inicializa un buffer con ceros.
        write(fd_RepoFile, bufferCeros, bytesFaltantes); // Escribe los bytes faltantes como ceros.
    }

    printf("\n Total : %ld  bytes writen \n", NumWriteBytes); // Traze
    return (NumWriteBytes + bytesFaltantes);
}



int main(int argc, char *argv[])
{
    char FileName[256];
    char RepoFileName[256]; // Sha256 repository file name

    struct c_sha256header my_sha256header;

    char FileDataBlock[READ_BLOCK_SIZE];

    int i, ret, Tam;
    unsigned long RepoFileSize, n, offset;

    int fd_DatFile, fd_RepoFile;
    char *buffer;

    char operation;
    off_t savedpos;

    if (argc != 4)
    {
        fprintf(stderr, "Use: %s Filename  RepoSha256FileName\n", argv[0]);
        return 1;
    }
    strcpy(FileName, argv[2]);
    strcpy(RepoFileName, argv[3]);
    operation = argv[1][0];




    // ----------------------------------------------------------------
    // open sha256 Repository File and Check flags for call insert
    switch (operation)
    {
        case 'I':
            printf("INSERTAR\n");

            // ------------------------------------------------------------------------------
            // Build my_sha256header structure with FileName info
            bzero(&my_sha256header, sizeof(my_sha256header));
            ret = Builsha256RepoHeader(FileName, &my_sha256header);
            if (ret != HEADER_OK)
            {
                fprintf(stderr, "The sha256header data was not generated correctly\n");
                return ERROR_GENERATE_SHA256_HEADER;
            }

            if ((fd_RepoFile = insertar_fichero_lseek(RepoFileName)) < 0) 
            {
                fprintf(stderr, " Error inserting(err=%d) \n", fd_RepoFile);
                return WRONG_INSERT_OR_EXTRACT_FLAG;
            }   

            //safe header pos
            if((savedpos = lseek(fd_RepoFile,0,SEEK_CUR))==(off_t)-1)
            {
                fprintf(stderr,"Error when scrolling through the content");
                return ERROR_WRITE_FILE;
            }

            // Write  my_sha256 header (of FileName) to the Reository File (RepoFileName)
            RepoFileSize = 0;
            n = write(fd_RepoFile, &my_sha256header, sizeof(my_sha256header));
            RepoFileSize = RepoFileSize + n;
            printf(" (my_sha256 header) written %ld bytes to file %s \n", n, RepoFileName); // Traze

            // ----------------------------------------------------------------
            // write the data file (FileName) to RepoFile (it is already open)
            // must be written in blocks of 4096 bytes(4KB)

            // open FileName only for read
            /// To complete the code
            /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

            if ((fd_DatFile = open(FileName, O_RDONLY)) == -1)
            {
                fprintf(stderr, "Cannot open file FileName %s\n", argv[2]);
                return ERROR_OPEN_DAT_FILE;
            }

            /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
            // Call to WriteFileDataBlocks function
            Tam = WriteFileDataBlocks(fd_DatFile, fd_RepoFile);
            RepoFileSize = RepoFileSize + Tam;
            my_sha256header.size = (off_t)(my_sha256header.size + RepoFileSize); // Actualize RepoFileSize file size

            if((lseek(fd_RepoFile,savedpos,SEEK_SET))==(off_t)-1)
            {
                fprintf(stderr,"Error when scrolling through the content");
                return ERROR_WRITE_FILE;
            }
            printf("\nmy_sha256header size  = %lld\n", my_sha256header.size);
            if (write(fd_RepoFile, &my_sha256header, sizeof(my_sha256header)) == -1)
            {
                fprintf(stderr,"Error defining size");
                return ERROR_WRITE_FILE;

            }

            // Close data file (FileName)
            ///  To complete the code
            /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

            // close the file
            if ((close(fd_DatFile)) == -1)
            {
                fprintf(stderr, "Error closing FileName%s\n", argv[2]);
                return ERROR_CLOSE_FILE;
            }
            break;


        case 'E':
            printf("EXTRAER\n");
            if ((fd_RepoFile = extrae_fichero(RepoFileName, FileName)) < 0) 
            {
                fprintf(stderr, " Error extracting(err=%d) \n", fd_RepoFile);
                return WRONG_INSERT_OR_EXTRACT_FLAG;
            }
            
            break;
        default: 
            fprintf(stderr,"Error with the first argument of the call, the argument must be I for insert or E for extract\n");
            return WRONG_INSERT_OR_EXTRACT_FLAG;
            break;
        }


    // ----------------------------------------------------------------


    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // -------------------------------------------------------------
    // Close RepoFile  file
    ///  To complete the code
    /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    if ((close(fd_RepoFile)) == -1)
    {
        fprintf(stderr,"Error closing RepoFileName%s\n", argv[3]);
        return ERROR_CLOSE_FILE;
    }

    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Final message to user
    printf("  %s  RepoFileName of %s  data file  has been generated (Repo Size: %ld bytes) \n",
           RepoFileName, FileName, RepoFileSize);

    return OK;
} // (OK=0)
