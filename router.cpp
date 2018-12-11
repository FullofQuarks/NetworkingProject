#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

void readFile();

struct network {
    int ip[2];
    int bid;
    int port;
    int ethAddr;
};

int main(int argc, char **argv) {
    if(argc < 6 || (argc-1)%5 != 0)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }
    struct network **networks;
    networks = new network*[(argc-1)/5];
    for(int ix = 0; ix < (argc-1)/5; ++ix)
    {
        networks[ix] = new network;
    }
    for(int ix = 0; ix < argc-1;)
    {
        networks[ix%5]->ip[0] = strtol(argv[ix+1], NULL, 10);
        networks[ix%5]->ip[1] = strtol(argv[ix+2], NULL, 10);
        networks[ix%5]->bid = strtol(argv[ix+3], NULL, 10);
        networks[ix%5]->port = strtol(argv[ix+4], NULL, 10);
        networks[ix%5]->ethAddr = strtol(argv[ix+5], NULL, 10);
        ix += 5;
    }

    readFile();
    return 0;
}

void readFile()
{
    streampos b;
    while(1)
    {
        ifstream fileOpen("host.txt");
        fileOpen.seekg(b);
        string line;
        fileOpen >> line;
        cout << line << endl;
        if(fileOpen.tellg() != -1)
            b = fileOpen.tellg();
        sleep(1);
        fileOpen.close();
    }

}
