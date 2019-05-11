#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define _ERROR_MSG "%s: %s Error! errno: %d\n"

#define PIPE_ERR			0
#define FORK_ERR			1
#define OPEN_ERR			2
#define CLOSE_ERR			3
#define READ_ERR			4
#define WRITE_ERR			5
#define DUP2_ERR			6
#define MMAP_ERR			7
#define MUNAP_ERR			8
#define FILE_N_F_ERR		9
#define TH_CREATE_ERR		10
#define TH_JOIN_ERR			11
#define MEM_ALLOC_ERR		12
#define MUTEX_INIT_ERR		13
#define MUTEX_LOCK_ERR		14
#define MUTEX_UNLOCK_ERR	15
#define COND_INIT_ERR		16
#define COND_WAIT_ERR		17
#define COND_SIG_ERR		18
#define QUEUE_OPEN_ERR		19
#define QUEUE_GETATTR_ERR	20
#define QUEUE_RECV_ERR		21
#define QUEUE_CLOSE_ERR		22
#define QUEUE_SEND_ERR		23
#define QUEUE_UNLINK_ERR	24

void check_error(int err, const char* name, char reason)
{
	const char* MSG[] = {
			"Pipe",
			"Fork",
			"Open",
			"Close",
			"Read",
			"Write",
			"Dup2",
			"Mmap",
			"Munmap",
			"File Not Found",
			"Create PThread",
			"Join Pthread",
			"Dynamic Allocation",
			"Mutex Init",
			"Mutex Lock",
			"Mutex Unlock",
			"Condition Init",
			"Condition Wait",
			"Condition Signal",
			"Queue Open",
			"Queue Get Attributes",
			"Queue Receive",
			"Queue Close",
			"Queue Send",
			"Queue Unlink"
	};
	if(err >= 0)
		return;
	fprintf(stderr,_ERROR_MSG, name, MSG[reason], errno);
	exit(err);
}

#endif /* ERROR_HANDLING_H_ */
