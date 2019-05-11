#include <sys/unistd.h>
#include "SudukuCalculator.h"

char suduku_is_legal(const char* suduku, char type)
{
	int i,j,k,l;
	unsigned int counter;
	switch(type)
	{
	case ROWS_CHECK:
		for(i = 0 ; i < 9 ; i++)
		{
			for(j = 0, counter = 0 ; j < 9 ; j++)
				counter |= 1 << suduku[i*9 + j];
			if(counter != RES_OK)
				return 0;
		}
		break;
	case COLS_CHECK:
		for(i = 0 ; i < 9 ; i++)
		{
			for(j = 0, counter = 0 ; j < 9 ; j++)
				counter |= 1 << suduku[j*9 + i];
			if(counter != RES_OK)
				return 0;
		}
		break;
	case BLOCK_CHECK:
		for(i = 0 ; i < 3 ; i++)
			for(l = 0 ; l < 3 ; l++)
			{
				for(j = 0 , counter = 0 ; j < 3 ; j++)
					for(k = 0 ; k < 3 ; k++)
						counter |= 1 << suduku[i*27 + l*3 + j*9 + k];
				if(counter != RES_OK)
					return 0;
			}
		break;
	}
	return 1;
}

char suduku_part_is_legal(const char* part, char type)
{
	int i,j;
	unsigned int counter;
	counter = 0;
	switch(type)
	{
	case ROWS_CHECK:
		for(i = 0 ; i < SUDUKU_RAW_SIZE ; i++)
			counter |= 1 << part[i];
		break;
	case COLS_CHECK:
		for(i = 0 ; i < SUDUKU_RAW_SIZE ; i++)
			counter |= 1 << part[i*SUDUKU_RAW_SIZE];
		break;
	case BLOCK_CHECK:
		for(i = 0 ; i < 3 ; i++)
			for(j = 0 ; j < 3 ; j++)
				counter |= 1 << part[i*9 + j];
		break;
	}
	return counter == RES_OK;
}

void char_to_int_suduku(const char* in, char* out)
{
	int i,j;
	for(i = 0, j = 0 ; i < SUDUKU_SIZE ; i++,j+=2)
		out[i] = in[j] - '0';
}
