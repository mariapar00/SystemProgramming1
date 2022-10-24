#include  "location.h"

vector<struct token*> locations;


// write locations on <filename>.out and empty vector
void write_locations(int fd) {
    for ( auto& it : locations ) {
        int size = strlen(it->location) + digits(it->times) + 2;
        char buffer[size];
        sprintf(buffer, "%s %d\n", it->location, it->times);

        if ( write(fd, buffer, size) < 0 )  {  // write locations found in this file in <filename>.out
            perror("manager: write");
            exit(1);
        }
    }
    cout << "wrote " << locations.size() << " urls" << endl;

    for ( auto& it : locations ) {
        delete it;
    }
    locations.clear();

}


// if location exists in file increase times, else return 1
int find_location(char* location) {
    if ( locations.size() > 0 ) {
        for ( auto& it : locations ) {
            if ( strcmp(it->location, location) == 0 ){
                it->times++;
                return 0;
            }
        }
    }
    return 1;
}


void set_location(char* location) {
    struct token* token = new struct token;
    token->times = 1;
    token->location = new char[strlen(location)];
    strcpy(token->location, location);
    locations.push_back(token);
}



char* extract_location(char* result) {
    string str((const char*)result);
    if ( regex_match(str, regex("http://www.")) ) {
        memmove(result, result+11, strlen(result+11) + 1);
        result = strtok(result, "/");
    } else {
        memmove(result, result+7, strlen(result+7) + 1);
        result = strtok(result, "/");
    }

    return result;
}