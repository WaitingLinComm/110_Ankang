// Ref: https://shengyu7697.github.io/cpp-read-text-file/
/////////////////////////////////////////////////////////
// Read number from .txt to string
/////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
using namespace std;

int main() {
    ifstream ifs;
    char buffer[256] = {0};
    int count = 0;

    ifs.open("hello.txt");
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        while (!ifs.eof()) {
            ifs.getline(buffer, sizeof(buffer));
            cout << buffer << "\n";
        }
        ifs.close();
    }
    return 0;
}