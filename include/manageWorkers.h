#pragma once
#include  <iostream>
#include  <cstring>
#include  <stdlib.h>
#include  <unistd.h>
#include  <fcntl.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <signal.h>
#include  <queue>
#include  <vector>
#include  "common.h"

#define   PERMS   0666

using namespace std;

struct worker {
    int pid;
    int writefd;
};


struct worker* create_worker(int);
struct worker* hire_worker(int*);
void dismiss_worker(struct worker*);
void dismiss_worker_pid(int);
void kill_workers();