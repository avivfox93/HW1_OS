#include <sys/unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define READ 		0
#define WRITE 		1

const char* DIGITS[] = {"0","1","2"};
const char zero = 0;
int main(int argc, const char* argv[])
{
	char suduku[FILE_CHARS], final_suduku[81], results[3] = {0}, result = 0;
	int in_pipe[2], out_pipe[6];
	int pid[3] = {0},i,j;
	int sud_in= -1;

	check_error(pipe(in_pipe),argv[0],PIPE_ERR);

	for(i = 0 ; i < NUM_OF_PROC ; i++)
	{
		check_error(pipe(out_pipe + i*2),argv[0],PIPE_ERR);
		// Parent
		if((pid[i] = fork()))
		{
			check_error(pid[i],argv[0],FORK_ERR);
			check_error(close(out_pipe[i*2 + READ]),argv[0],CLOSE_ERR);
		}
		// Child
		else
		{
			check_error(close(in_pipe[READ]),argv[0],CLOSE_ERR);
			check_error(close(out_pipe[i*2 + WRITE]),argv[0],CLOSE_ERR);
			if(out_pipe[i*2 + READ] != STDIN_FILENO)
			{
				check_error(dup2(out_pipe[i*2 + READ],STDIN_FILENO),argv[0],DUP2_ERR);
				check_error(close(out_pipe[i*2 + READ]),argv[0],DUP2_ERR);
			}
			if(in_pipe[WRITE] != STDOUT_FILENO)
			{
				check_error(dup2(in_pipe[WRITE],STDOUT_FILENO),argv[0],DUP2_ERR);
				check_error(close(in_pipe[WRITE]),argv[0],CLOSE_ERR);
			}
			execlp(CALCULATE_PATH,argv[0],DIGITS[i],NULL);
			// If execlp failed...
			check_error(-1,argv[0],FILE_N_F_ERR);
		}
	}
	check_error(close(in_pipe[WRITE]),argv[0],CLOSE_ERR);
	if(argc <= 1)
	{
		argc = 2;
		sud_in = STDIN_FILENO;
	}
	for(i = 1 ; i < argc ; i++)
	{
		if(sud_in != STDIN_FILENO)
			check_error((sud_in = open(argv[i],O_RDONLY)),argv[0],OPEN_ERR);
		check_error(read(sud_in,suduku,FILE_CHARS),argv[0],READ_ERR);
		if(sud_in > 0)
			check_error(close(sud_in),argv[0],CLOSE_ERR);
		char_to_int_suduku(suduku,final_suduku);
		for(j = 0 ; j < NUM_OF_PROC ; j++)
			check_error(write(out_pipe[j*2 + WRITE],final_suduku, SUDUKU_SIZE),argv[0],WRITE_ERR);
		for(j = 0 ; j < NUM_OF_PROC ; j++)
			check_error(read(in_pipe[READ],results+j,1),argv[0],READ_ERR);
		result = results[0] && results[1] && results[2];
		printf(result ? "%s is legal\n" : "%s is not legal\n",
			(sud_in != STDIN_FILENO) ? argv[i] : "STD_ID");
	}
	for(i = 0 ; i < NUM_OF_PROC ; i++)
		check_error(close(out_pipe[i*2 + WRITE]),argv[0],CLOSE_ERR);
	check_error(close(in_pipe[READ]),argv[0],CLOSE_ERR);
	return 0;
}
