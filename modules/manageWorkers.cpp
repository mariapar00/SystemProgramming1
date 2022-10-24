#include  "manageWorkers.h"

vector<struct worker*> all_workers;
queue<struct worker*> free_workers;


struct worker* create_worker(int fifo_no) {
    int pid;
    int readfd, writefd;
    struct worker* wkr = new struct worker;

    char str[4+digits(fifo_no)];
    sprintf(str, "fifos/fifo.%d", fifo_no);

    if ( mkfifo(str, PERMS) < 0 ) {
        perror("mkfifo");
        exit(1);
    }

    if ( (pid = fork()) == -1 ) {   // create child process worker
        perror("fork"); 
        exit(1); 
    }
    if( pid != 0 ) {            // parent
        if ( (writefd = open(str, O_WRONLY)) < 0 )  {  
            perror("manager: open fifo");
            exit(1);
        }
    } else {                    // child
        if ( (readfd = open(str, O_RDONLY))  < 0)  {
            perror("worker: open fifo");
            exit(1);
        } 
        char arg[digits(readfd)];
        sprintf(arg, "%d", readfd);
        execl( "worker", "worker", arg, NULL );
        //exec doesn't exit; if it does, it's an error
        perror("worker: execl");
        exit(1);
    } 

    // parent
    wkr->pid = pid;
    wkr->writefd = writefd;
    all_workers.push_back(wkr);

    cout << "created worker " << pid << endl;

    return wkr;
}

struct worker* hire_worker(int* fifo_no) {
    struct worker* wkr;
    if ( free_workers.size() == 0 ) {
        ++*fifo_no;
        wkr = create_worker(*fifo_no);
    } else {
        wkr = free_workers.front();
        free_workers.pop();
    }
    cout << "hired worker " << wkr->pid << endl;

    return wkr;
}

void dismiss_worker(struct worker* wkr) {
    free_workers.push(wkr);       // worker is available
    cout << "dismissed worker " << wkr->pid << endl;
}

void dismiss_worker_pid(int pid) {
    for( auto &it : all_workers )    {
        if ( it->pid == pid ) dismiss_worker(it);
    }
}

void kill_workers() {
    for ( auto& it : all_workers ) {
        if ( kill(it->pid, SIGKILL) < 0 ) {   // worker, if stopped, is continued
            perror("kill: SIGKILL");
            exit(1);
        }  
    }
}