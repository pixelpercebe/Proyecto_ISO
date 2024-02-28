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

// #define DATAFILE_BLOCK_SIZE  4096

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
    char FileDataBlock[READ_BLOCK_SIZE];
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

    printf("\n Total : %ld  bytes writen \n", NumWriteBytes); // Traze
    return NumWriteBytes;
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

    if (argc != 3)
    {
        fprintf(stderr, "Use: %s Filename  RepoSha256FileName\n", argv[0]);
        return 1;
    }
    strcpy(FileName, argv[1]);
    strcpy(RepoFileName, argv[2]);

    // ------------------------------------------------------------------------------
    // (1.0) Build my_sha256header structure with FileName info
    bzero(&my_sha256header, sizeof(my_sha256header));
    ret = Builsha256RepoHeader(FileName, &my_sha256header);
    if (ret != HEADER_OK)
    {
        fprintf(stderr, "The sha256header data was not generated correctly\n");
        return ERROR_GENERATE_SHA256_HEADER;
    }

    // ----------------------------------------------------------------
    // (1.2) open sha256 Repository File
    if ((fd_RepoFile = open(RepoFileName, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
    {
        fprintf(stderr, "Cannot open file RepoFileName %s\n", argv[2]);
        return ERROR_OPEN_SHA_REPO_FILE;
    }

    // ----------------------------------------------------------------
    // (1.3)Write  my_sha256 header (of FileName) to the Reository File (RepoFileName)
    RepoFileSize = 0;
    n = write(fd_RepoFile, &my_sha256header, sizeof(my_sha256header));
    RepoFileSize = RepoFileSize + n;                                                // Actualize RepoFileSize file size
    printf(" (my_sha256 header) written %ld bytes to file %s \n", n, RepoFileName); // Traze

    // ----------------------------------------------------------------
    // (1.4) write the data file (FileName) to RepoFile (it is already open)
    //       must be written in blocks of 4096 bytes(4KB)

    // open FileName only for read
    ///  To complete the code
    /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    if ((fd_DatFile = open(FileName, O_RDONLY)) == -1)
    {
        fprintf(stderr, "Cannot open file FileName %s\n", argv[1]);
        return ERROR_OPEN_DAT_FILE;
    }

    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Call to WriteFileDataBlocks function
    Tam = WriteFileDataBlocks(fd_DatFile, fd_RepoFile);
    RepoFileSize = RepoFileSize + Tam;

    // Close data file (FileName)
    ///  To complete the code
    /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    // close the file
    if ((close(fd_DatFile)) == -1)
    {
        fprintf(stderr, "Error closing FileName%s\n", argv[1]);
        return ERROR_CLOSE_FILE;
    }

    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // -------------------------------------------------------------
    // (1.5) Close RepoFile  file
    ///  To complete the code
    /// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    if ((close(fd_RepoFile)) == -1)
    {
        fprintf(stderr,"Error closing RepoFileName%s\n", argv[2]);
        return ERROR_CLOSE_FILE;
    }

    /// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // Final message to user
    printf("  %s  RepoFileName of %s  data file  has been generated (Repo Size: %ld bytes) \n",
           RepoFileName, FileName, RepoFileSize);

    return OK;
} // (OK=0)

