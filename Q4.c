/*
 * Main_Threads.c
 *
 *  Created on: Apr 7, 2019
 *      Author: aviv
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define NUM_OF_TASKS 	27
#define N				7

typedef struct
{
	char	arr[SUDUKU_SIZE];
	char 	res[NUM_OF_TASKS];
}suduku_t;

typedef struct
{
	unsigned char index 		: 5;
	unsigned char sud_index 	: 7;
	unsigned char type			: 2;
}task_t;

typedef struct
{
	const char*	prog_name;
	int 		task_index;
	task_t* 	tasks[NUM_OF_TASKS];
	suduku_t 	data;
	int 		result;
	int			finished;
}mission_t;

mission_t 		mission;
suduku_t 		data;
pthread_mutex_t tasks_lock,result_lock;
pthread_cond_t 	cond;

int suduku_thread(task_t* args)
{
	int res;
	res = suduku_part_is_legal(data.arr + args->sud_index,args->type);
	free(args);
	return res;
}

void* thread_action(void* args)
{
	int res;
	task_t* task;
	while(1)
	{
		check_error(pthread_mutex_lock(&tasks_lock),mission.prog_name,MUTEX_LOCK_ERR);
		if(mission.task_index >= NUM_OF_TASKS)
		{
			check_error(pthread_mutex_unlock(&tasks_lock),mission.prog_name,MUTEX_UNLOCK_ERR);
			break;
		}
		task = mission.tasks[mission.task_index++];
		check_error(pthread_mutex_unlock(&tasks_lock),mission.prog_name,MUTEX_UNLOCK_ERR);
		res = suduku_thread(task);
		check_error(pthread_mutex_lock(&result_lock),mission.prog_name,MUTEX_LOCK_ERR);
		mission.result += res;
		mission.finished++;
		check_error(pthread_cond_signal(&cond),mission.prog_name,COND_SIG_ERR);
		check_error(pthread_mutex_unlock(&result_lock),mission.prog_name,MUTEX_UNLOCK_ERR);
	}
	return NULL;
}

int main(int argc, const char* argv[])
{
	char raw_sud[SUDUKU_SIZE*2];
	int reader,i;
	pthread_t threads[N];
	mission.data = data;
	mission.result = 0;
	mission.task_index = 0;
	mission.prog_name = argv[0];

	check_error((reader = argc > 1 ? open(argv[1],O_RDONLY) : STDIN_FILENO),argv[0],OPEN_ERR);

	check_error(read(reader,raw_sud,SUDUKU_SIZE*2),argv[0],READ_ERR);
	char_to_int_suduku(raw_sud,data.arr);

	for(i = 0; i < SUDUKU_RAW_SIZE ; i++)
	{
		task_t *a,*b,*c;
		a = malloc(sizeof(task_t)); b = malloc(sizeof(task_t)); c = malloc(sizeof(task_t));
		a->index = i; 				a->sud_index = i; 						a->type = COLS_CHECK;
		b->index = i+9;				b->sud_index = i*9;						b->type = ROWS_CHECK;
		c->index = i+18;			c->sud_index = ((i/3)*27)+((i%3)*3);	c->type = BLOCK_CHECK;
		mission.tasks[i*3 + 0] = a;
		mission.tasks[i*3 + 1] = b;
		mission.tasks[i*3 + 2] = c;
	}
	check_error(pthread_mutex_init(&tasks_lock,NULL),argv[0],MUTEX_INIT_ERR);
	check_error(pthread_mutex_init(&result_lock,NULL),argv[0],MUTEX_INIT_ERR);
	check_error(pthread_cond_init(&cond,NULL),argv[0],COND_INIT_ERR);
	for(i = 0 ; i < N ; i++)
		check_error(pthread_create(&threads[i],NULL,thread_action,NULL),argv[0],TH_CREATE_ERR);
	check_error(pthread_mutex_lock(&result_lock),argv[0],MUTEX_LOCK_ERR);
	while(mission.finished != NUM_OF_TASKS)
		check_error(pthread_cond_wait(&cond,&result_lock),argv[0],COND_WAIT_ERR);
	check_error(pthread_mutex_unlock(&result_lock),argv[0],MUTEX_UNLOCK_ERR);
	for(i = 0 ; i < N ; i++)
		check_error(pthread_join(threads[i],NULL),argv[0],TH_JOIN_ERR);
	printf(mission.result == NUM_OF_TASKS ? "solution is legal\n" : "solution is not legal\n");
}
