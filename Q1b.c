#include <sys/unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define MAX_SIZE 1024
#define QUEUE_FILE		"/suduku_queue.txt"
#define QUEUE_RES_FILE	"/suduku_results.txt"

int main(int argc, const char* argv[])
{
	int pid,i,j,length,sud_in;
	mqd_t mq_out_id, mq_in_id;
	struct mq_attr in_attr = {0}, out_attr = {0};
	char result, final_result, suduku[FILE_CHARS], buff[SUDUKU_SIZE];

	for(i = 0; i < NUM_OF_PROC ; i++)
	{
		//Child
		if(!(pid = fork()))
		{
			in_attr.mq_msgsize = SUDUKU_SIZE; out_attr.mq_msgsize = 1; in_attr.mq_maxmsg = 10; out_attr.mq_maxmsg = 10;
			check_error((mq_out_id = mq_open(QUEUE_FILE, O_RDONLY | O_CREAT,0666,&in_attr)),argv[0],QUEUE_OPEN_ERR);
			check_error((mq_in_id = mq_open(QUEUE_RES_FILE, O_WRONLY | O_CREAT,0666,&out_attr)),argv[0],QUEUE_OPEN_ERR);
			for(;;)
			{
				check_error((length = mq_receive(mq_out_id,buff,SUDUKU_SIZE,0)),argv[0],QUEUE_RECV_ERR);
				if(!(*buff))
					break;
				result = suduku_is_legal(buff, i);
				check_error(mq_send(mq_in_id,&result,1,0),argv[0],QUEUE_SEND_ERR);
			}
			check_error(mq_close(mq_out_id),argv[0],QUEUE_CLOSE_ERR);
			check_error(mq_close(mq_in_id),argv[0],QUEUE_CLOSE_ERR);
			exit(0);
		}
		check_error(pid,argv[0],FORK_ERR);
	}
	out_attr.mq_msgsize = SUDUKU_SIZE; in_attr.mq_msgsize = 1; in_attr.mq_maxmsg = 10; out_attr.mq_maxmsg = 10;
	check_error((mq_in_id = mq_open(QUEUE_RES_FILE, O_RDONLY | O_CREAT,0666,&in_attr)),argv[0],QUEUE_OPEN_ERR);
	check_error((mq_out_id = mq_open(QUEUE_FILE, O_WRONLY | O_CREAT,0666,&out_attr)),argv[0],QUEUE_OPEN_ERR);
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
		if(sud_in != STDIN_FILENO)
			check_error(close(sud_in),argv[0],CLOSE_ERR);
		char_to_int_suduku(suduku,buff);
		for(j = 0 ; j < NUM_OF_PROC ; j++)
			check_error(mq_send(mq_out_id,buff,SUDUKU_SIZE,0),argv[0],QUEUE_SEND_ERR);
		for(j = 0, final_result = 1 ; j < NUM_OF_PROC ; j++)
		{
			check_error(mq_receive(mq_in_id,&result,1,0),argv[0],QUEUE_RECV_ERR);
			final_result &= result;
		}
		printf(final_result ? "%s is legal\n" : "%s is not legal\n",
					(sud_in != STDIN_FILENO) ? argv[i] : "STD_ID");
	}
	for(i = 0, buff[0] = 0 ; i < NUM_OF_PROC ; i++)
		check_error(mq_send(mq_out_id,buff,1,0),argv[0],QUEUE_SEND_ERR);
	check_error(mq_unlink(QUEUE_RES_FILE),argv[0],QUEUE_UNLINK_ERR);
	check_error(mq_unlink(QUEUE_FILE),argv[0],QUEUE_UNLINK_ERR);
	check_error(mq_close(mq_out_id),argv[0],QUEUE_CLOSE_ERR);
	check_error(mq_close(mq_in_id),argv[0],QUEUE_CLOSE_ERR);
}
