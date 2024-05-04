#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#define LINHAS 25
#define COLUNAS 18

struct termios orig_termios;
char mapa[LINHAS][COLUNAS];
int posicao1 = 0;
int posicao2 = 9;

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

void removeCarros(){
	for(int i = LINHAS-3; i<LINHAS; i++){
		for(int j = 0; j<COLUNAS; j++){
			if(mapa[i][j] == 'a') mapa[i][j] = ' ';
		}
	} 
}

void montaCarro(int colunaInicio){
	for(int i = LINHAS-3; i<LINHAS; i++){
		for(int j = colunaInicio; j<colunaInicio+3; j++){
			mapa[i][j] = 'a';
		}
	} 
}

int moveCarro(){
	char leitura;
	if(read(STDIN_FILENO, &leitura, 1)){
		if(leitura == 'f') {
			if(posicao1 == 4){posicao1 = 0;}
			else {posicao1 = 4;}
		}
		if(leitura == 'j'){
		       if(posicao2 == 13){posicao2 = 9;}
		       else{posicao2 = 13;}
		}
		if(leitura == 'q') return 1;
	}
	return 0;
}

void *dorme(void *arg){
	usleep(100000);
}

void iniciaMapa(){
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
}

void desceObjetosNaColuna(int coluna){
	for(int i = LINHAS-4; i >= 0 ; i--){
		if(mapa[i][coluna] != ' '){
			mapa[i+1][coluna] = mapa[i][coluna];
			mapa[i][coluna] = ' ';
		}
	}
}
void desceObjetos(){
	desceObjetosNaColuna(1);
	desceObjetosNaColuna(5);
	desceObjetosNaColuna(10);
	desceObjetosNaColuna(14);
}

void criaObjetoNaColuna(int coluna){
	int coisa = rand() % 2;
	if(coisa) {
		mapa[0][coluna] = 'x';
	} else {
		mapa[0][coluna] = 'o';
	}
}

void criaObjetoLado(int lado){
	int pista = rand() % 2;
	int col1 = 1;
	int col2 = 5;
	if(lado == 2) {
		col1 = 10;
		col2 = 14;
	}
	if(pista){
		criaObjetoNaColuna(col1);
	} else {
		criaObjetoNaColuna(col2);
	}
}

int main(){
	srand(time(NULL));

	enableRawMode();
	
	iniciaMapa();
	
	int loopConta = 0;
	while(1){
		pthread_t thread;
		pthread_create(&thread, NULL, dorme, NULL);

		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
	
		if(loopConta%2 == 0) desceObjetos();	
		if(loopConta%15 == 0) criaObjetoLado(1);
		if(loopConta%15 == 4) criaObjetoLado(2);	

		
		removeCarros();
		montaCarro(posicao1);
		montaCarro(posicao2);

		char buf[LINHAS*COLUNAS];
		int posicaobuf = 0;
		for(int i = 0; i<LINHAS;i++){
			for(int j = 0 ; j<COLUNAS;j++){
				buf[posicaobuf++] = mapa[i][j];
			}
		}
		write(STDOUT_FILENO,&buf,LINHAS*COLUNAS);
		int sair = moveCarro();
	
		loopConta++;

		pthread_join(thread, NULL);

		if(sair) break;
	}
}










