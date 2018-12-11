#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>

using namespace std;

void readFile();

struct bridge {
    int id;
    int numPorts;
    vector<int> neighbors;
};

int main(int argc, char **argv) {
    cout << "Hello, World!" << endl;
    if(argc < 10 || argc > 11)
    {
        cout << "Wrong arguments used." << endl;
        exit(1);
    }

    struct bridge newHost;
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
