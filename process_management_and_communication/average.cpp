#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
    double sum = stod(argv[1]);
    double count = stod(argv[2]);
    string lesson = argv[3];
    cout << lesson << " " << sum / count << endl;
}