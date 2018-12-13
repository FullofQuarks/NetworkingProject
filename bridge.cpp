#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

struct bridge {
    int id;
    int numPorts;
    int *neighbors;
    int hostCache[100][1] = {};
};

//Functions
vector<string> createFiles(int, int, int [], int);
void readFile(vector<string>, struct bridge *);
void process(string, struct bridge *, int);
int lineToPort(string);

int main(int argc, char **argv) {
    if(argc < 3)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct bridge *newBridge = (struct bridge *)malloc(sizeof(struct bridge));
    newBridge->id = strtol(argv[1], NULL, 10);
    newBridge->numPorts = strtol(argv[2], NULL, 10);
    newBridge->neighbors = new int[argc-3];
    for(int ix = 0; ix < argc-3; ++ix)
    {
        newBridge->neighbors[ix] = strtol(argv[ix+3], NULL, 10);
    }

    //Create the files
    vector<string> files = createFiles(newBridge->id, newBridge->numPorts, newBridge->neighbors, (argc-3));
    readFile(files, newBridge);
    return 0;
}

void readFile(vector<string> files, struct bridge *b)
{
    //Multiple hosts, so we will need to keep track of multiple file positions
    streampos pos[files.size()];

    while(1)
    {
        for(size_t ix = 0; ix < files.size(); ++ix)
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
            line.clear();
        }
        sleep(1);
    }

}

void process(string newFrame, struct bridge *b, int port)
{
    //Strip packet of ethernet headers
    int sourceEthAddr, destEthAddr;
    stringstream ss;
    cout << "Bridge Processing command " << newFrame << endl;
    ss << newFrame;
    ss >> sourceEthAddr;
    ss >> destEthAddr;
    if(b->hostCache[sourceEthAddr][0] == 0)
    {
	//The port number is the index of the file vector, plus one
    b->hostCache[sourceEthAddr][0] = port+1;
    }

    //We've extracted the layer 2 headers, now get the remainder of the frame
    string restOfFrame;
    getline(ss, restOfFrame);

    // Broadcast to all nodes that is not the originating node
    if(destEthAddr == 99)
    {
        for(int ix = 0; ix < b->numPorts; ++ix)
        {
            if((ix+1) != b->hostCache[sourceEthAddr][0])
            {
                ofstream toFile;
                string toFilename = "fromB";
                toFilename = toFilename + to_string(b->id) + "P" + to_string(ix+1) + ".txt";
                toFile.open(toFilename, ios::app);
                toFile << newFrame << '\n';
                toFile.close();
            }
        }
    }
    else //Destination ethernet address known, forward frame to that port
    {
        ofstream toFile;
        string toFilename = "fromB";
        toFilename = toFilename + to_string(b->id) + "P" + to_string(b->hostCache[destEthAddr][0]) + ".txt";
        toFile.open(toFilename, ios::app);
        toFile << newFrame << '\n';
        toFile.close();
    }

}

vector<string> createFiles(int bid, int numPorts, int neighbors[], int numNeighbors)
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

    for(int ix = 0; ix < numNeighbors; ++ix)
    {
        //Create neighbor bridge files if not exist
        ofstream bridgeFile;
        string file = "B";
        file = file + to_string(bid) + "B" + to_string(neighbors[ix]) + ".txt";
        bridgeFile.open(file, ios::app);
        bridgeFile.close();

        ofstream tobridgeFile;
        file = "B";
        file = file + to_string(neighbors[ix]) + "B" + to_string(bid) + ".txt";
        tobridgeFile.open(file, ios::app);
        tobridgeFile.close();
    }

    return v;
}
