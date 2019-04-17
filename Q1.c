/*
 * Main.c
 *
 *  Created on: Apr 3, 2019
 *      Author: aviv
 */

#include <sys/unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define READ 	0
#define WRITE 	1

const char* DIGITS[] = {"0","1","2"};
int main(int argv, const char* args[])
{
	char suduku[FILE_CHARS], results[3] = {0}, result = 0;
	int in_pipe[2], out_pipe[6];
	int pid,i,j,int_suduku[81];
	int sud_in= -1;
	if(argv <= 1)
	{
		argv = 2;
		sud_in = STDIN_FILENO;
	}
	for(i = 1 ; i < argv ; i++)
	{
		if(sud_in != STDIN_FILENO)
			check_error(sud_in = open(args[i],O_RDONLY));
		check_error(read(sud_in,suduku,FILE_CHARS));
		char_to_int_suduku(suduku,int_suduku);
		if(pipe(in_pipe) < 0 || pipe(out_pipe) < 0 || pipe(&out_pipe[2]) < 0 || pipe(&out_pipe[4]) < 0)
			check_error(-1);
		for(j = 0 ; j < 3 ; j++)
		{
			// Parent
			if((pid = fork()))
			{
				check_error(pid);
				close(out_pipe[j*2 + READ]);
				write(out_pipe[j*2 + WRITE],int_suduku,sizeof(int)*SUDUKU_SIZE);
				close(out_pipe[j*2 + WRITE]);
			}
			// Child
			else
				break;
		}
		// Child
		if(!pid)
		{
			check_error(dup2(out_pipe[j*2 + READ],STDIN_FILENO));
			check_error(dup2(in_pipe[WRITE],STDOUT_FILENO));
			execlp(CALCULATE_PATH,DIGITS[j],NULL);
			// If execlp faild...
			write(STDOUT_FILENO,"0",1);
			check_error(-1);
		}
		// Parent
		else
		{
			for(j = 0 ; j < 3 ; j++)
				wait(NULL);
			check_error(read(in_pipe[READ],results,3));
			check_error(close(in_pipe[READ]));
			check_error(close(in_pipe[WRITE]));
			result = results[0] && results[1] && results[2];
			printf(result ? "%s is legal\n" : "%s is not legal\n",
					(sud_in != STDIN_FILENO) ? args[i] : "STD_ID");
		}
	}
	if(sud_in > 0)
		check_error(close(sud_in));
	return 0;
}
