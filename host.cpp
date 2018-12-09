#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

using namespace std;

void readFile();
void printHost(struct host);

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
    cout << "Hello, World!" << endl;
    if(argc < 10 || argc > 11)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct host newHost;
    newHost.ip[0] = argv[1][0] - '0';
    newHost.ip[1] = argv[2][0] - '0';
    newHost.ethAddr = strtol(argv[3], NULL, 10);
    newHost.gateway[0] = argv[4][0] - '0';
    newHost.gateway[1] = argv[5][0] - '0';
    newHost.bridge = strtol(argv[6], NULL, 10);
    newHost.bridgePort = strtol(argv[7], NULL, 10);
    newHost.toIP[0] = argv[8][0] - '0';
    newHost.toIP[1] = argv[9][0] - '0';
    if(argc == 11)
        newHost.message = argv[10];
    else
        newHost.message = " ";
    printHost(newHost);
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

void printHost(struct host newHost)
{
    //Print host details
    int ix = 1;
    cout << "Printing out details for Host " << ix << endl;
    cout << "IP address: " << newHost.ip[0] << ", " << newHost.ip[1] << endl;
    cout << "Ethernet Address: " << newHost.ethAddr << endl;
    cout << "Gateway: " << newHost.gateway[0] << ", " << newHost.gateway[1] << endl;
    cout << "Bridge ID: " << newHost.bridge << endl;
    cout << "BridgePort: " << newHost.bridgePort << endl;
    cout << "Communicating with IP: " << newHost.toIP[0] << ", " << newHost.toIP[1] << endl;
    cout << "Message: " << newHost.message << endl;
}
