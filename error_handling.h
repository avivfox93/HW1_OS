/*
 * error_handling.h
 *
 *  Created on: Apr 17, 2019
 *      Author: aviv
 */

#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#include <stdio.h>
#include <stdlib.h>

#define _ERROR_MSG "%s: %s Error!\n"

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
			"Condition Signal"
	};
	if(err >= 0)
		return;
	fprintf(stderr,_ERROR_MSG, name, MSG[reason]);
	exit(err);
}

#endif /* ERROR_HANDLING_H_ */
