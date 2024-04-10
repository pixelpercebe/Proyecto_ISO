#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
void printPermissions(mode_t permissions) {
    printf("User: ");
    printf(permissions & S_IRUSR ? "r" : "-");
    printf(permissions & S_IWUSR ? "w" : "-");
    printf(permissions & S_IXUSR ? "x" : "-");
    printf("\nGroup: ");
    printf(permissions & S_IRGRP ? "r" : "-");
    printf(permissions & S_IWGRP ? "w" : "-");
    printf(permissions & S_IXGRP ? "x" : "-");
    printf("\nOther: ");
    printf(permissions & S_IROTH ? "r" : "-");
    printf(permissions & S_IWOTH ? "w" : "-");
    printf(permissions & S_IXOTH ? "x" : "-");
    printf("\n");
}