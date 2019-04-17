/*
 * Main.c
 *
 *  Created on: Apr 5, 2019
 *      Author: aviv
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

typedef struct
{
	int		arr[81];
	char 	res[3];
}suduku_t;

int main(int argv, const char* args[])
{
	char suduku[FILE_CHARS],res;
	suduku_t* data_mem;
	int i,j,pid,sud_in = -1;
	data_mem = mmap(NULL,sizeof(suduku_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(!data_mem)
		check_error(-1);
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
		char_to_int_suduku(suduku,data_mem->arr);
		for(j = 0 ; j < 3 ; j++)
			// Child
			if(!(pid = fork()))
			{
				data_mem->res[j] = suduku_is_legal(data_mem->arr,j);
				exit(0);
			}
			// Parent
			else
				check_error(pid);
		wait(NULL);
		res = data_mem->res[0] && data_mem->res[1] && data_mem->res[2];
		printf(res ? "%s is legal\n" : "%s is not legal\n",
				(sud_in != STDIN_FILENO) ? args[i] : "STD_ID");
	}
	check_error(munmap(data_mem,sizeof(suduku_t)));
}
