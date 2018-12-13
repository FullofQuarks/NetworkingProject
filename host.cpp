#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

void createFiles(struct host *);
void readFile(string);
void printHost(struct host *);

struct host {
    int ip[2];
    int ethAddr;
    int gateway[2];
    int bridge;
    int bridgePort;
    int toIP[2];
    string message;
};

int main(int argc, char **argv) {
    if(argc < 10 || argc > 11)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct host *newHost;
    newHost->ip[0] = strtol(argv[1], NULL, 10);
    newHost->ip[1] = strtol(argv[2], NULL, 10);
    newHost->ethAddr = strtol(argv[3], NULL, 10);
    newHost->gateway[0] = strtol(argv[4], NULL, 10);
    newHost->gateway[1] = strtol(argv[5], NULL, 10);
    newHost->bridge = strtol(argv[6], NULL, 10);
    newHost->bridgePort = strtol(argv[7], NULL, 10);
    newHost->toIP[0] = strtol(argv[8], NULL, 10);
    newHost->toIP[1] = strtol(argv[9], NULL, 10);
    string fromFile = "fromB";
    fromFile = fromFile + argv[6] + "P" + argv[7] + ".txt";
    if(argc == 11)
        newHost->message = argv[10];
    else
        newHost->message = " ";
    printHost(newHost);

    //Create from file if not exist
    createFiles(newHost);
    readFile(fromFile);
    return 0;
}

void readFile(string fromFile)
{
    streampos b;
    string line;
    while(1)
    {
        ifstream fileOpen(fromFile);
        fileOpen.seekg(b);
        getline(fileOpen, line);
        cout << line << endl;
        if(fileOpen.tellg() != -1)
            b = fileOpen.tellg();
        sleep(1);
        fileOpen.close();
        line.clear();
    }

}

void printHost(struct host *newHost)
{
    //Print host details
    int ix = 1;
    cout << "Printing out details for Host " << ix << endl;
    cout << "IP address: " << newHost->ip[0] << ", " << newHost->ip[1] << endl;
    cout << "Ethernet Address: " << newHost->ethAddr << endl;
    cout << "Gateway: " << newHost->gateway[0] << ", " << newHost->gateway[1] << endl;
    cout << "Bridge ID: " << newHost->bridge << endl;
    cout << "BridgePort: " << newHost->bridgePort << endl;
    cout << "Communicating with IP: " << newHost->toIP[0] << ", " << newHost->toIP[1] << endl;
    cout << "Message: " << newHost->message << endl;
}

void createFiles(struct host *newHost)
{
    //Create "toBXPX.txt" file. We'll want to remember these files as we want to read from them
    ofstream toFile;
    string file = "toB";
    file = file + to_string(newHost->bridge);
    file = file + "P" + to_string(newHost->bridgePort) + ".txt";
    toFile.open(file, ios::app);
    toFile.close();

    //Create "fromBXPX.txt" file
    ofstream fromFile;
    file = "fromB";
    file = file + to_string(newHost->bridge);
    file = file + "P" + to_string(newHost->bridgePort) + ".txt";
    fromFile.open(file, ios::app);
    fromFile.close();
}