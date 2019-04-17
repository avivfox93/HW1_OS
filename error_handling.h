/*
 * error_handling.h
 *
 *  Created on: Apr 17, 2019
 *      Author: aviv
 */

#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

#define _ERROR_MSG "Fatal Failed!\n"

void check_error(int err)
{
	if(err >= 0)
		return;
	printf(_ERROR_MSG);
	exit(err);
}

#endif /* ERROR_HANDLING_H_ */
