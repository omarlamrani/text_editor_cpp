#include <iostream>
#include <unistd.h>
#include <termios.h>
using namespace std;

void enableRawMode(){

}

int main() {
    char a;
    // reads line and stores last char to a
    while (read(STDIN_FILENO, &a, 1) == 1 && a != '0');
    cout << a << "\n";
    return 0;
}