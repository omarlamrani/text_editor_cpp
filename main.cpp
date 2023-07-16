#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <stdio.h>
using namespace std;

struct termios og_termios;

void disableRawMode(){
    // TCSAFLUSH => ???
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
}

void enableRawMode(){
    // reads char by char instead of line+enter
    tcgetattr(STDIN_FILENO, &og_termios);
    atexit(disableRawMode);

    struct termios raw = og_termios;
    // ICANON = canonical flag 
    // ISIG = turns out CtrlC anf CtrlZ
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_oflag &= ~(OPOST);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK |  ISTRIP);
    raw.c_cflag |= (CS8);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}

int main() {
    enableRawMode();
    char a;
    // reads line and stores last char to a
    while (read(STDIN_FILENO, &a, 1) == 1 && (int)a != 96) {
        if (iscntrl(a)) {
            cout << a << "\r\n";
        } else {
            cout << a << " ("<< (int)a << ")\r\n";
        }
    }
    
    return 0;
}