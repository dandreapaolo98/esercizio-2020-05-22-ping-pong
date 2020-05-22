/*
 * esercizio-2020-5-22-ping-pong.c
 *
 *  Created on: May 21, 2020
 *      Author: marco
 */

/***************TESTO ESERCIZIO***************
Scrivere un programma che realizza un "ping-pong" tra due processi utilizzando una coppia di pipe, una per ogni direzione.

Il contatore (di tipo int) viene incrementato ad ogni ping ed ad ogni pong e viene trasmesso attraverso la pipe.

Quanto il contatore raggiunge il valore MAX_VALUE il programma termina.

proc_padre manda a proc_figlio il valore 0 attraverso pipeA.
proc_figlio riceve il valore 0, lo incrementa (=1) e lo manda a proc_padre attraverso pipeB.
proc_padre riceve il valore 1, lo incremente (=2) e lo manda a proc_figlio attraverso pipeA.
proc_figlio riceve il valore 2 .....

fino a MAX_VALUE, quando termina il programma.

#define MAX_VALUE 1000000

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>

#define MAX_VALUE 1000000

int main(int argc, char * argv[]) {

	int pipe1_fd[2], pipe2_fd[2];
	int res;
	int *counter = calloc(1, sizeof(int));
	if (counter == NULL) {
		perror("calloc()");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipe1_fd) == -1) {
		perror("pipe()");

		exit(EXIT_FAILURE);
	}

	if (pipe(pipe2_fd) == -1) {
		perror("pipe()");

		exit(EXIT_FAILURE);
	}

	switch (fork()) {
		case -1:
			perror("problema con fork");

			exit(EXIT_FAILURE);

		case 0: // processo FIGLIO

			close(pipe1_fd[1]);
			close(pipe2_fd[0]);

			while(*counter < MAX_VALUE){
				if(res = read(pipe1_fd[0], counter, sizeof(int)) == -1){
					perror("pipe read error");
					exit(EXIT_FAILURE);
				}
				//printf("[child]counter = %d \n", *counter);

				(*counter)++;

				if(res = write(pipe2_fd[1], counter, sizeof(int)) == -1){
					perror("pipe write error");
					exit(EXIT_FAILURE);
				}
			}

			printf("\n[child] bye\n");


			exit(EXIT_SUCCESS);

		default: // processo PADRE
			close(pipe1_fd[0]);
			close(pipe2_fd[1]);

			if(res = write(pipe1_fd[1], counter, sizeof(int)) == -1){	//faccio partire il primo ping
				perror("pipe write error");
				exit(EXIT_FAILURE);
			}
			while(*counter < MAX_VALUE){
				if(res = read(pipe2_fd[0], counter, sizeof(int)) == -1){
					perror("pipe read error");
					exit(EXIT_FAILURE);
				}
				//printf("[parent]counter = %d \n", *counter);

				(*counter)++;

				if(res = write(pipe1_fd[1], counter, sizeof(int)) == -1){
					perror("pipe write error");
					exit(EXIT_FAILURE);
				}
			}

			printf("\n[parent] bye\n");

			exit(EXIT_SUCCESS);
	}

	return 0;
}

