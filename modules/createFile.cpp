#include  "createFile.h"

// creates .out file in results directory 
// returns file descriptor
int create_file(char* filename) {
    int fd;
    char* first = strtok( filename, "/" );
    while ( first != NULL ) {
        filename = first;
        first = strtok(NULL, "/");
    }
    char path[strlen(filename)+12];
    sprintf(path, "results/%s.out", filename);
  
    if( (fd = creat(path, PERMS)) < 0 ) {
        perror("worker: creat");
        exit(1);
    }
    cout << "created file " << path << endl;

    return fd;
}