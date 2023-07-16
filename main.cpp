#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

using namespace std;

#define CTRL_KEY(k) ((k) & 0x1f)

// DATA //

struct editorState {
    int win_rows;
    int win_cols;
    struct termios og_termios;
};

struct editorState E;

// TERMINAL // 

void kill(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode(){
    // TCSAFLUSH => ???
   if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.og_termios) == -1){
    kill("tcsetattr");
   }
}

void enableRawMode(){
    // reads char by char instead of line+enter
    if (tcgetattr(STDIN_FILENO, &E.og_termios) == -1){
    kill("tcsetattr");
   }
    atexit(disableRawMode);

    struct termios raw = E.og_termios;
    // ICANON = canonical flag 
    // ISIG = turns out CtrlC anf CtrlZ
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_oflag &= ~(OPOST);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK |  ISTRIP);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){
    kill("tcsetattr");
   }
}

char readKey() {
    int err_read;
    char a;
    // err_read = -1 if error
    while ((err_read = read(STDIN_FILENO, &a, 1)) != 1){
        if (err_read == -1 && errno != EAGAIN){
            kill("read");
        }
    }
    return a;
}

int getWindowSize(int *rows, int *cols){
    struct winsize win;
    // succes places TIOCGW.. in winsize
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &win) == -1 || win.ws_col == 0) {
        return -1;
    } else {
        *cols = win.ws_col;
        *rows = win.ws_row;
        return 0;
    }
}

// OUTPUT //

void drawRows() {
    int y;
    for (y=0;y<E.win_rows;y++) {
        write(STDOUT_FILENO, ">\r\n", 3);
    }
}

void updateScreen(){
    // 4 bytes = esc(terminal format task) + [(always after esc) + 
    // 2(clear entire screen, 0/1 also available,) + J(clear screen func)
    write(STDOUT_FILENO, "\x1b[2J", 4);

    //H function takes care of cursor positioning (top-left here (1;1))
    write(STDOUT_FILENO, "\x1b[H", 3);

    drawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

// INPUT //

void keyPress() {
    char a = readKey();

    switch (a) 
    {
    case CTRL_KEY('x'):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;

    }
}

// INIT // 
void initEditor() {
    if (getWindowSize(&E.win_rows, &E.win_cols) == -1) {
        kill("getWindowSize");
    }
}

int main() {
    enableRawMode();
    initEditor();
    
    while (1) {
        updateScreen();
        keyPress();
    }
    
    return 0;
}