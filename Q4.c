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
	int		arr[SUDUKU_SIZE];
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
		check_error(pthread_mutex_lock(&tasks_lock));
		if(mission.task_index >= NUM_OF_TASKS)
		{
			check_error(pthread_mutex_unlock(&tasks_lock));
			break;
		}
		task = mission.tasks[mission.task_index++];
		check_error(pthread_mutex_unlock(&tasks_lock));
		res = suduku_thread(task);
		check_error(pthread_mutex_lock(&result_lock));
		mission.result += res;
		mission.finished++;
		check_error(pthread_cond_signal(&cond));
		check_error(pthread_mutex_unlock(&result_lock));
	}
	return NULL;
}

int main(int argv, const char* args[])
{
	char raw_sud[SUDUKU_SIZE*2];
	int reader,i;
	pthread_t threads[N];
	mission.data = data;
	mission.result = 0;
	mission.task_index = 0;

	check_error(reader = argv > 1 ? open(args[1],O_RDONLY) : STDIN_FILENO);

	check_error(read(reader,raw_sud,SUDUKU_SIZE*2));
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
	check_error(pthread_mutex_init(&tasks_lock,NULL)); check_error(pthread_mutex_init(&result_lock,NULL));
	check_error(pthread_cond_init(&cond,NULL));
	for(i = 0 ; i < N ; i++)
		check_error(pthread_create(&threads[i],NULL,thread_action,NULL));
	check_error(pthread_mutex_lock(&result_lock));
	while(mission.finished != NUM_OF_TASKS)
		check_error(pthread_cond_wait(&cond,&result_lock));
	check_error(pthread_mutex_unlock(&result_lock));
	for(i = 0 ; i < N ; i++)
		check_error(pthread_join(threads[i],NULL));
	printf(mission.result == NUM_OF_TASKS ? "solution is legal\n" : "solution is not legal\n");
}
