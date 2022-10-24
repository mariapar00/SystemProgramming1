#include  <iostream>
#include  <cstring>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <vector>
#include  <regex>
#include  "common.h"

using namespace std;

struct token {
    char* location;
    int times;
};


void write_locations(int);
int find_location(char*);
void set_location(char*);
char* extract_location(char*);