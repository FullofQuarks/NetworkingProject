/*
* Nicholas Smith
* nas150630@utdallas.edu
* Fall 2019 4390.501
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

void readFile();
void arp(struct host *, int []);

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
    for(int ix = 0; ix < (argc-1)/5; ++ix)
    {
        networks[ix]->ip[0] = strtol(argv[ix+1], NULL, 10);
        networks[ix]->ip[1] = strtol(argv[ix+2], NULL, 10);
        networks[ix]->bid = strtol(argv[ix+3], NULL, 10);
        networks[ix]->port = strtol(argv[ix+4], NULL, 10);
        networks[ix]->ethAddr = strtol(argv[ix+5], NULL, 10);
    }

    for(int ix = 0; ix < (argc-1)/5; ++ix)
    {
        cout << "Network " << (ix+1) << "- \n";
        cout << "IP: " << networks[ix]->ip[0] << ", " << networks[ix]->ip[1] << endl;
        cout << "Bridge ID: " << networks[ix]->bid << endl;
        cout << "Bridge Port: " << networks[ix]->port << endl;
        cout << "Ethernet Address: " << networks[ix]->ethAddr << endl;
    }

    //readFile();
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
/*
void arp(struct host *newHost, int ip[2])
{
    // ARP request format:
    // ARP REQ target-IP-address source-IP-address source-Ethernet-address
    string arpRequest = to_string(newHost->ethAddr) + " " + to_string(99);
    arpRequest = arpRequest + " ARP REQ " + to_string(ip[0]) + " " + to_string(ip[1]) + " " + to_string(newHost->ip[0]) + " " + to_string(newHost->ip[1]) + " " + to_string(newHost->ethAddr);
    ofstream toFile;
    string file = "toB";
    file = file + to_string(newHost->bridge) + "P" + to_string(newHost->bridgePort) + ".txt";
    toFile.open(file, ios::app);
    toFile << arpRequest << '\n';
    toFile.close();
}*/
