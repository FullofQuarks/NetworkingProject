#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <sstream>

using namespace std;

struct bridge {
    int id;
    int numPorts;
    int *neighbors;
    int hostCache[100][1] = {0};
};

//Functions
vector<string> createFiles(int, int);
void readFile(vector<string>, struct bridge *);
void process(string, struct bridge *, int);
int lineToPort(string);

int main(int argc, char **argv) {
    if(argc < 3)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct bridge *newBridge;
    newBridge->id = strtol(argv[1], NULL, 10);
    newBridge->numPorts = strtol(argv[2], NULL, 10);
    newBridge->neighbors = new int[argc-3];
    for(int ix = 0; ix < argc-3; ++ix)
    {
        newBridge->neighbors[ix] = strtol(argv[ix+3], NULL, 10);
    }

    //Create the files
    vector<string> files = createFiles(newBridge->id, newBridge->numPorts);
    readFile(files, newBridge);
    return 0;
}

void readFile(vector<string> files, struct bridge *b)
{
    //Multiple hosts, so we will need to keep track of multiple file positions
    streampos pos[files.size()];

    while(1)
    {
        for(int ix = 0; ix < files.size(); ++ix)
        {
            ifstream fileOpen(files.at(ix));
            fileOpen.seekg(pos[ix]);
            string line;
            getline(fileOpen, line);
            if(!line.empty())
            {
                process(line, b, ix);
            }
            if(fileOpen.tellg() != -1)
            {
                pos[ix] = fileOpen.tellg();
            }
            fileOpen.close();
        }
        sleep(1);
    }

}

void process(string newFrame, struct bridge *b, int port)
{
    //Strip packet of ethernet headers
    int sourceEthAddr, destEthAddr;
    stringstream ss;
    ss << newFrame;
    ss >> sourceEthAddr;
    ss >> destEthAddr;
    if(b->hostCache[sourceEthAddr][0] == 0)
    {
        b->hostCache[sourceEthAddr][0] = port;
    }
}

vector<string> createFiles(int bid, int numPorts)
{
    vector<string> v;
    for(int ix = 0; ix < numPorts; ++ix)
    {
        //Create "toBXPX.txt" file. We'll want to remember these files as we want to read from them
        ofstream toFile;
        string file = "toB";
        file = file + to_string(bid);
        file = file + "P" + to_string(ix+1) + ".txt";
        v.push_back(file);
        toFile.open(file, ios::app);
        toFile.close();

        //Create "fromBXPX.txt" file
        ofstream fromFile;
        file = "fromB";
        file = file + to_string(bid);
        file = file + "P" + to_string(ix+1) + ".txt";
        fromFile.open(file, ios::app);
        fromFile.close();
    }

    return v;
}