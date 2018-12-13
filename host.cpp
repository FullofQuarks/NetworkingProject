#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sstream>

using namespace std;

void createFiles(struct host *);
void readFile(string, struct host *);
void process(string, struct host *);
void printHost(struct host *);
void arp(struct host *, int []);

struct host {
    int ip[2];
    int ethAddr;
    int gateway[2];
    int bridge;
    int bridgePort;
    int toIP[2];
    string message;
    int ARPtable[9][9] = {};
};

int main(int argc, char **argv) {
    if(argc < 10 || argc > 11)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct host *newHost = (struct host *)malloc(sizeof(struct host));
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
    //printHost(newHost);

    //Create from file if not exist
    createFiles(newHost);

    //Process message in arguments, if any
    if(argc == 11)
    {
        arp(newHost, newHost->toIP);

    }
    readFile(fromFile, newHost);
    return 0;
}

void readFile(string fromFile, struct host *newHost)
{
    streampos b;
    string line;
    while(1)
    {
        ifstream fileOpen(fromFile);
        fileOpen.seekg(b);
        getline(fileOpen, line);
        if(!line.empty())
        {
            process(line, newHost);
        }
        if(fileOpen.tellg() != -1)
            b = fileOpen.tellg();
        sleep(1);
        fileOpen.close();
        line.clear();
    }

}

void process(string command, struct host *newHost)
{
    //ARP reply
    int sourceEthAddr, destEthAddr;
    stringstream ss;
    ss << command;
    ss >> sourceEthAddr;
    ss >> destEthAddr;
    if(destEthAddr == 99)
    {
        string arpCommand;
        ss >> arpCommand;
        if(arpCommand.compare(0,4," ARP"))
        {
            cout << "This is an ARP message.\n";
            string line;
            ss >> line;
            if(line == "REQ")
            {
                int tgtIP[2];
                int srcIP[2];
                int srcEth;
                ss >> tgtIP[0];
                ss >> tgtIP[1];
                ss >> srcIP[0];
                ss >> srcIP[1];
                ss >> srcEth;
                //Then it's my address!
                if(tgtIP[0] == newHost->ip[0] && tgtIP[1] == newHost->ip[1])
                {
                    newHost->ARPtable[srcIP[0]][srcIP[1]] = srcEth;
                    
                    //Send ARP Reply
                    //ARP REP target-IP-address target-Ethernet-address source-IP-address source-Ethernet-address
                    string reply = "ARP REP ";
                    reply = reply + to_string(tgtIP[0]) + " " + to_string(tgtIP[1]) + " " + to_string(newHost->ethAddr) + " " + to_string(srcIP[0]) + " " + to_string(srcIP[1]) + " " + to_string(sourceEthAddr);
                    cout << reply << endl;
                }
            }
            else if(line == "REP")
            {
                int tgtIP[2];
                int tgtEthAddr;
                ss >> tgtIP[0];
                ss >> tgtIP[1];
                ss >> tgtEthAddr;
                cout << "ARP replied with target address of: " << tgtEthAddr << endl;
            }
        }
    }
    else if(destEthAddr == newHost->ethAddr)
    {

    }
    else //Not destined for this host, drop packet
    {

    }
}

void arp(struct host *newHost, int ip[2])
{
    cout << "Ethernet address for IP: " << ip[0] << ", " << ip[1] << " is " << newHost->ARPtable[ip[0]][ip[1]] << endl;
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
