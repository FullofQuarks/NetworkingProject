#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

void createFiles(int, int);
void readFile();

struct bridge {
    int id;
    int numPorts;
    int *neighbors;
};

int main(int argc, char **argv) {
    if(argc < 3)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct bridge newBridge;
    newBridge.id = strtol(argv[1], NULL, 10);
    newBridge.numPorts = strtol(argv[2], NULL, 10);
    newBridge.neighbors = new int[argc-3];
    for(int ix = 0; ix < argc-3; ++ix)
    {
        newBridge.neighbors[ix] = strtol(argv[ix+3], NULL, 10);
    }

    //Create the files
    createFiles(newBridge.id, newBridge.numPorts);
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

void createFiles(int bid, int numPorts)
{
    for(int ix = 0; ix < numPorts; ++ix)
    {
        //Create to file
        ofstream toFile;
        string file = "toB";
        file = file + to_string(bid);
        file = file + "P" + to_string(ix+1) + ".txt";
        toFile.open(file, ios::app);
        toFile.close();

        //Create from file
        ofstream fromFile;
        file = "fromB";
        file = file + to_string(bid);
        file = file + "P" + to_string(ix+1) + ".txt";
        fromFile.open(file, ios::app);
        fromFile.close();
    }
}