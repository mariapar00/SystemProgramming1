#include  <iostream>
#include  <sys/errno.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <cstring>
#include  <string>
#include  <signal.h>
#include  <regex>
#include  <climits>
#include  "createFile.h"
#include  "location.h"

#define   BUFSIZE 1000

using namespace std;



int main(int argc, char *argv[]) {
    if ( argc < 2 ) {
        perror("arguments");
        exit(1);
    }
    int readfd = atoi(argv[1]);
    
    char* filename = new char[BUFSIZE];
    int n;
    while ( (n = read(readfd, filename, BUFSIZE)) >= 0 ) {
        cout << "processing file " << filename << endl;
        int fd;

        if ( (fd = open(filename, O_RDONLY)) < 0 ) {
            perror("worker: open");
            exit(1);
        }
        
        char file[1];
        string str = "";
        int k;

        smatch match_result;
        regex url("http://[^ ]+ ");
        while ( (k = read(fd, file, 1)) ) {
            string fil(file);
            str += fil;
            if ( strcmp(file, " ") == 0 ) {
                while ( regex_search(str, match_result, url) ) { // search for urls
                    for ( auto x : match_result ) {
                        string it = x.str();
                        const char* res = it.c_str();     // res is a url
    
                        char* location = extract_location((char*)res);     // extract location from url res
    
                        if ( find_location(location) ) set_location(location); // first time finding location
                    }
                    str = match_result.suffix().str();
                }
                str = "";
            }

            lseek(fd, 0, SEEK_CUR);
        }
        if( k < 0 ) {
            perror("worker file: read");
            exit(1);
        }

        if ( close(fd) < 0 ) {
            perror("worker: close");
            exit(1);
        }

        int newfd = create_file(filename);  // create new file <filename>.out

        delete filename;

        write_locations(newfd);  // write locations found in this file in <filename>.out

        if ( kill(getpid(), SIGSTOP) < 0 ) {   // worker is stopped
            perror("worker: SIGSTOP");
            exit(1);
        }    
    }
    if ( n == -1 ) {
        perror("worker filename: read");
        exit(1);
    }

}

