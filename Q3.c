#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define NUM_OF_THREADS 	27

typedef struct
{
	char	arr[SUDUKU_SIZE];
	char 	res[NUM_OF_THREADS];
}suduku_t;

typedef struct
{
	unsigned char index 		: 5;
	unsigned char sud_index 	: 7;
	unsigned char type			: 2;
}task_t;

suduku_t data;

void *suduku_thread(void* arg)
{
	task_t* args = (task_t*)arg;
	data.res[args->index] = suduku_part_is_legal(data.arr + args->sud_index,args->type);
	free(args);
	return NULL;
}

int main(int argc, const char* argv[])
{
	char raw_sud[SUDUKU_SIZE*2],result;
	int reader,i;
	pthread_t threads[NUM_OF_THREADS];

	check_error((reader = argc > 1 ? open(argv[1],O_RDONLY) : STDIN_FILENO),argv[0],OPEN_ERR);
	check_error(read(reader,raw_sud,SUDUKU_SIZE*2),argv[0],READ_ERR);
	char_to_int_suduku(raw_sud,data.arr);

	for(i = 0; i < SUDUKU_RAW_SIZE ; i++)
	{
		task_t *a,*b,*c;
		a = malloc(sizeof(task_t)); b = malloc(sizeof(task_t)); c = malloc(sizeof(task_t));
		if(!a | !b | !c)
			check_error(-1,argv[0],MEM_ALLOC_ERR);
		a->index = i; 				a->sud_index = i; 						a->type = COLS_CHECK;
		b->index = i+9;				b->sud_index = i*9;						b->type = ROWS_CHECK;
		c->index = i+18;			c->sud_index = ((i/3)*27)+((i%3)*3);	c->type = BLOCK_CHECK;
		check_error(pthread_create(&threads[i],		NULL,suduku_thread,(void*)a),argv[0],TH_CREATE_ERR);
		check_error(pthread_create(&threads[i+9],	NULL,suduku_thread,(void*)b),argv[0],TH_CREATE_ERR);
		check_error(pthread_create(&threads[i+18],	NULL,suduku_thread,(void*)c),argv[0],TH_CREATE_ERR);
	}
	for(i = 0 ; i < NUM_OF_THREADS ; i++)
		check_error(pthread_join(threads[i],NULL),argv[0],TH_JOIN_ERR);
	for(i = 0, result = 1 ; i < NUM_OF_THREADS ; i++)
		result &= data.res[i];
	printf(result ? "solution is legal\n" : "solution is not legal\n");
}
