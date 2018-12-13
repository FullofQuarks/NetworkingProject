/*
* Nicholas Smith
* nas150630@utdallas.edu
* Fall 2019 4390.501
*/
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
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
void ipRecvFromEth(string, struct host *);
void transRecvFromIP(string, int [], struct host *);
void ipRecvFromTrans(string, int [], struct host *);
void ethRecvFromIP(struct host *, int, string);
void transportTasks(struct host *);

struct host {
    int ip[2];
    int ethAddr;
    int gateway[2];
    int bridge;
    int bridgePort;
    int toIP[2];
    string message;
    int ARPtable[9][9] = {};
    string buffer;

};

struct transport {
    int sn[2] = {}; //transport sequence numbers, indexes is the concurrent logical channels
    int sent[2]; //has the frame been sent
    string data;
    int timeout[2] = {30, 30};
    int pos;
    string buffer[2];
} session;

int main(int argc, char **argv) {
    if(argc != 11 && argc != 8)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }
    struct host *newHost = new host;
    newHost->ip[0] = strtol(argv[1], NULL, 10);
    newHost->ip[1] = strtol(argv[2], NULL, 10);
    newHost->ethAddr = strtol(argv[3], NULL, 10);
    newHost->gateway[0] = strtol(argv[4], NULL, 10);
    newHost->gateway[1] = strtol(argv[5], NULL, 10);
    newHost->bridge = strtol(argv[6], NULL, 10);
    newHost->bridgePort = strtol(argv[7], NULL, 10);
    if(argc == 11)
    {
        newHost->toIP[0] = strtol(argv[8], NULL, 10);
        newHost->toIP[1] = strtol(argv[9], NULL, 10);
        newHost->message = argv[10];
    }
    else
        newHost->message = "";
    string fromFile = "fromB";
    fromFile = fromFile + argv[6] + "P" + argv[7] + ".txt";
    //printHost(newHost);

    //Create from file if not exist
    createFiles(newHost);

    //Process message in arguments, if any
    if(argc == 11)
    {
        ipRecvFromTrans(newHost->message, newHost->toIP, newHost);
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
            if(!newHost->message.empty())
                transportTasks(newHost);
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
    int sourceEthAddr=0;
    int destEthAddr=0;
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
                    newHost->ARPtable[srcIP[0]-1][srcIP[1]-1] = srcEth;
                    
                    //Send ARP Reply
                    //ARP REP target-IP-address target-Ethernet-address source-IP-address source-Ethernet-address
                    string reply = "ARP REP ";
                    reply = reply + to_string(tgtIP[0]) + " " + to_string(tgtIP[1]) + " " + to_string(newHost->ethAddr) + " " + to_string(srcIP[0]) + " " + to_string(srcIP[1]) + " " + to_string(sourceEthAddr);
                    reply = to_string(newHost->ethAddr) + " " + to_string(sourceEthAddr) + " " + reply;
                    ofstream toFile;
                    string toFilename = "toB";
                    toFilename = toFilename + to_string(newHost->bridge) + "P" + to_string(newHost->bridgePort) + ".txt";
                    toFile.open(toFilename, ios::app);
                    toFile << reply << '\n';
                    toFile.close();
                }
            }
        }
    }
    //This frame was meant for us!
    else if(destEthAddr == newHost->ethAddr)
    {
        string line;
        ss >> line;
        //Deal with ARP reply
        if(!line.compare(0,3,"ARP"))
        {
            ss >> line;
            if(!line.compare(0,3,"REP"))
            {
                int tgtIP[2];
                int tgtEthAddr;
                ss >> tgtIP[0];
                ss >> tgtIP[1];
                ss >> tgtEthAddr;
                newHost->ARPtable[tgtIP[0]-1][tgtIP[1]-1] = tgtEthAddr;
                ipRecvFromTrans(newHost->buffer, tgtIP, newHost);
            }
        }
        //Packet from another Host
        if(!line.compare(0,2, "IP"))
        {
            string ipPacket;
            getline(ss, ipPacket);
            ipRecvFromEth(ipPacket, newHost);
        }
    }
    else //Not destined for this host, drop packet
    {

    }
}

void transportTasks(struct host *newHost)
{
    //Timeout is less than 30 seconds
    if(session.timeout[0] < 30)
    {

    }
    else
    {
        session.timeout[0] = 0;
        string data = "DA ";
        int size = newHost->message.length();
        if(size - session.pos > 5)
        {
            string truncated = newHost->message.substr(0,5);
            newHost->message = newHost->message.substr(5, newHost->message.length()-5);
            data += to_string(session.sn[0]) + " " + to_string(0) + " " + truncated;
            session.buffer[0] = truncated;
            session.pos += 5;
        }
        else
        {
            data += to_string(session.sn[0]) + " " + to_string(0) + " " + newHost->message.substr(session.pos, size-session.pos);
            session.buffer[0] = newHost->message.substr(session.pos, size-session.pos);
        }
        ipRecvFromTrans(data, newHost->ip, newHost);
    }
    if(session.timeout[1] < 30)
    {

    }
    else
    {
        session.timeout[1] = 0;
        string data = "DA ";
        int size = newHost->message.length();
        if(size - session.pos > 5)
        {
            string truncated = newHost->message.substr(0,5);
            newHost->message = newHost->message.substr(5, newHost->message.length()-5);
            data += to_string(session.sn[1]) + " " + to_string(1) + " " + truncated;
            session.buffer[1] = truncated;
            session.pos += 5;
        }
        else
        {
            data += to_string(session.sn[1]) + " " + to_string(1) + " " + newHost->message.substr(session.pos, size-session.pos);
            session.buffer[1] = newHost->message.substr(session.pos, size-session.pos);
        }
        ipRecvFromTrans(data, newHost->ip, newHost);
    }
}

void ipRecvFromEth(string packet, struct host *newHost)
{
    stringstream ss;
    ss << packet;
    string line;
    int ip[2];
    ss >> line;
    ss >> line;
    ss >> ip[0];
    ss >> ip[1];
    getline(ss, packet);
    transRecvFromIP(packet, ip, newHost);
    return;
}

void transRecvFromIP(string data, int ip[2], struct host *newHost)
{
    cout << data << " from " << ip[0] << " " << ip[1] << endl;
    stringstream ss;
    ss << data;
    string line, message;
    ss >> line;
    int sn, cn;
    ss >> sn;
    ss >> cn;
    ss >> message;
    if(!line.compare(0,2,"DA"))
    {
        string reply = "AK";
        reply = reply + " " + to_string(sn) + " " + to_string(cn);
        ipRecvFromTrans(reply, ip, newHost);
    }
    else if(!line.compare(0,2,"AK"))
    {
        session.sn[cn] = sn;
    }
    return;
}

void ipRecvFromTrans(string reply, int ip[2], struct host *newHost)
{
    if(newHost->ARPtable[ip[0]-1][ip[1]-1] == 0)
    {
        reply = "IP " + to_string(ip[0]) + " " + to_string(ip[1]) + " " + to_string(newHost->ip[0]) + " " + to_string(newHost->ip[1]) + " " + reply;
        newHost->buffer = reply;
        arp(newHost, ip);
    }
    else
    {
        ethRecvFromIP(newHost, newHost->ARPtable[ip[0]-1][ip[1]-1], newHost->buffer);
    }
}

void ethRecvFromIP(struct host *newHost, int destEthAddr, string packet)
{
    string frame;
    frame += to_string(newHost->ethAddr) + " " + to_string(destEthAddr) + " " + packet;
    ofstream toFile;
    string file = "toB";
    file = file + to_string(newHost->bridge) + "P" + to_string(newHost->bridgePort) + ".txt";
    toFile.open(file, ios::app);
    toFile << frame << '\n';
    toFile.close();
    return;
}

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
