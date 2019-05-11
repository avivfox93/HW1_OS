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
	char 	ready;
	char	die;
	char	arr[81];
	char 	res[3];
	char	finished[3];
}suduku_t;

int main(int argc, const char* argv[])
{
	char suduku[FILE_CHARS],res;
	suduku_t* data_mem;
	int i,j,pid[3] = {0},sud_in = -1;
	data_mem = mmap(NULL,sizeof(suduku_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(data_mem <= 0)
		check_error(-1,argv[0],MMAP_ERR);
	for(i = 0 ; i < NUM_OF_PROC ; i++)
	{
		// Child
		if(!(pid[i] = fork()))
			for(;;)
			{
				data_mem->finished[i] = 0;
				while(!data_mem->ready) // Waiting...
					if(data_mem->die) return 0;
				data_mem->res[i] = suduku_is_legal(data_mem->arr,i);
				data_mem->finished[i] = 1;
				while(data_mem->ready);
			}
		// Parent
		check_error(pid[i],argv[0],FORK_ERR);
	}
	if(argc <= 1)
	{
		argc = 2;
		sud_in = STDIN_FILENO;
	}
	for(i = 1 ; i < argc ; i++)
	{
		if(sud_in != STDIN_FILENO && (sud_in = open(argv[i],O_RDONLY)) < 0)
		{
			data_mem->die = 1;
			check_error(sud_in,argv[0],OPEN_ERR);
		}
		if(read(sud_in,suduku,FILE_CHARS) < 0)
		{
			data_mem->die = 1;
			check_error(-1,argv[0],READ_ERR);
		}
		char_to_int_suduku(suduku,data_mem->arr);
		data_mem->ready = 1;
		for(j = 0 ; j < NUM_OF_PROC ; j++)
			while(!data_mem->finished[j]);
		data_mem->ready = 0;
		res = data_mem->res[0] && data_mem->res[1] && data_mem->res[2];
		printf(res ? "%s is legal\n" : "%s is not legal\n",
				(sud_in != STDIN_FILENO) ? argv[i] : "STD_ID");
	}
	data_mem->die = 1;
	// Make sure there are no survivors...
	for(i = 0 ; i < NUM_OF_PROC ; i++)
		wait(NULL);
	check_error(munmap(data_mem,sizeof(suduku_t)),argv[0],MUNAP_ERR);
	exit(0);
}
