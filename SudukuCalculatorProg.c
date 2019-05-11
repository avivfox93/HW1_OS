#include <sys/unistd.h>
#include "SudukuCalculator.h"
#include "error_handling.h"

#define SUDUKU_SIZE 81

#define RES_OK 0b1111111110

int main(int args, const char* argv[])
{
	char res,i;
	for(res = 0;;)
	{
		char suduku[SUDUKU_SIZE] = {0};
		check_error((i = read(STDIN_FILENO, suduku, SUDUKU_SIZE)),argv[0],READ_ERR);
		if(i == 0)
			return 0;
		res = args > 1 ? suduku_is_legal(suduku,*(argv[1])-'0') : 0;
		check_error(write(STDOUT_FILENO,&res,1),argv[0],WRITE_ERR);
	}
	return 0;
}
