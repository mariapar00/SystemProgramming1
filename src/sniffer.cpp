#include  <iostream>
#include  <cstring>
#include  <stdlib.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <dirent.h>
#include  <signal.h>
#include  <sys/wait.h>
#include  <sys/resource.h>
#include  "manageWorkers.h"

#define   BUFSIZE 100
#define   READ    0
#define   WRITE   1

using namespace std;


int listener;
DIR* dir;


void myhandler1(int) {
    int status;
    int pid = waitpid(-1, &status, WUNTRACED);
    dismiss_worker_pid(pid);   // put worker back in free workers
}


void myhandler2(int) {
    if ( closedir(dir) < 0 ) {  // close directory
        perror("closedir");
        exit(1);
    }
    if ( kill(listener, SIGKILL) < 0 ) {   // worker, if stopped, is continued
        perror("kill: SIGKILL");
        exit(1);
    }
    kill_workers();
}


int main(int argc, char *argv[]) {
    pid_t  pid;
    int fd[2];

    signal( SIGCHLD, myhandler1 );     // catch signal SIGCHLD and apply myhandler1
    signal( SIGINT, myhandler2 );     // catch signal SIGINT and apply myhandler2

    char* name = ".";
    if ( argc > 1 ) name = argv[2];
  
    if ( (dir = opendir(name)) == NULL ) {   // open directory
        perror("opendir");
        exit(1);
    }

    
    if (pipe(fd) == -1) {   // create pipe
        perror("pipe"); 
        exit(1); 
    }

    if ( (pid = fork()) == -1 ) {   // create child process listener 
        perror("fork"); 
        exit(1); 
    }

    // child
    if ( pid == 0 ) {               
        close(fd[READ]);
        dup2(fd[WRITE], 1);
        close(fd[WRITE]);

        execlp("inotifywait", "inotifywait", "-m", "-q", "-e", "create", "--format", "%w%f\n", name, NULL );
                    // -m : monitoring
                    // -r : quiet
                    // -e create : only notify at the event of file creation
                    // --format %w%f : output watched directory and file that caused event (dir/file)
        perror("listener: execlp");
    } 

    // parent
    close(fd[WRITE]);
    listener = pid;


    
    int dirsize = 1;
    while ( struct dirent* entry = readdir(dir) ) {    // create workers - one worker for each directory entry
        char* filename = entry->d_name;
        if( strcmp(filename, ".") == 0 ) continue;
        if( strcmp(filename, "..") == 0 ) continue;
      
        struct worker* wkr = create_worker(dirsize);

        int writefd = wkr->writefd;

        char path[strlen(filename)+strlen(name)+1];
        sprintf(path, "%s/%s", name, filename);
        
        if ( write(writefd, path, strlen(path)+1) < 0 )  {  // send filename to worker through fifo
            perror("manager: write");
            exit(1);
        }
        dirsize++;
    }

    
    int n;
    char* filename = new char[BUFSIZE];
    while( (n = read(fd[READ], filename, BUFSIZE)) > 0 ) {
        filename = strtok(filename, "\n");  // path may have trash or invisible characters after \n

        struct worker* wkr = hire_worker(&dirsize);
        
        int writefd = wkr->writefd;

        if ( write(writefd, filename, strlen(filename)+1) < 0 )  {  // send filename to worker through fifo
            perror("manager: write");
            exit(1);
        }
        if ( kill(wkr->pid, SIGCONT) < 0 ) {   // worker, if stopped, is continued
            perror("kill: SIGSTOP");
            exit(1);
        }  
        
    }
    if ( n < 0 ) { 
        perror("manager: read");
        exit(1);
    }

}




