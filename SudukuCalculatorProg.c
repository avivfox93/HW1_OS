/*
 * Main.c
 *
 *  Created on: Apr 3, 2019
 *      Author: aviv
 */

#include <sys/unistd.h>
#include "SudukuCalculator.h"

#define SUDUKU_SIZE 81

#define RES_OK 0b1111111110

int main(int args, const char* argv[])
{
	char res = 0;
	int suduku[SUDUKU_SIZE] = {0};
	read(STDIN_FILENO, suduku, sizeof(int)*SUDUKU_SIZE);
	res = args > 0 ? suduku_is_legal(suduku,*argv[0]-'0') : 0;
	write(STDOUT_FILENO,&res,1);
	return res;
}
