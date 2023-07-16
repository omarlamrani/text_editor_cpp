#include <iostream>
#include <unistd.h>
#include <termios.h>
using namespace std;

struct termios og_termios;

void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &og_termios);
}

void enableRawMode(){
    // reads char by char instead of line+enter
    tcgetattr(STDIN_FILENO, &og_termios);
    atexit(disableRawMode);

    struct termios raw = og_termios;
    raw.c_lflag &= ~(ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}

int main() {
    enableRawMode();
    char a;
    // reads line and stores last char to a
    while (read(STDIN_FILENO, &a, 1) == 1 && a != '0');
    cout << a << "\n";
    return 0;
}