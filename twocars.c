#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#define LINHAS 25
#define COLUNAS 18

struct termios orig_termios;
char mapa[LINHAS][COLUNAS];

void die(char* s){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void disableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
}

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void montaCarro(int colunaInicio){
	for(int i = LINHAS-3; i<LINHAS; i++){
		for(int j = colunaInicio; j<colunaInicio+3; j++){
			mapa[i][j] = 'a';
		}
	} 
}

int main(){
	enableRawMode();
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	for(int i = 0; i<LINHAS;i++){
		for(int j = 0 ; j<COLUNAS-2 ;j++){
			if(j == 3 || j == 7 || j == 8 || j == 12){ 
				mapa[i][j] = '.';
			} else {
				mapa[i][j] = ' ';
			}
		}
		mapa[i][COLUNAS-2] = '\r';
		mapa[i][COLUNAS-1] = '\n';
	}

	montaCarro(0);
	montaCarro(9);

	char buf[LINHAS*COLUNAS];
	int posicao = 0;
	for(int i = 0; i<LINHAS;i++){
		for(int j = 0 ; j<COLUNAS;j++){
			buf[posicao++] = mapa[i][j];
		}
	}
	write(STDOUT_FILENO,&buf,LINHAS*COLUNAS);
}










