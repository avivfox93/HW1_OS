#ifndef SUDUKUCALCULATOR_H_
#define SUDUKUCALCULATOR_H_

#define SUDUKU_SIZE 	81
#define FILE_CHARS 		2*SUDUKU_SIZE
#define SUDUKU_RAW_SIZE	9

#define ROWS_CHECK 	0
#define COLS_CHECK 	1
#define BLOCK_CHECK	2

#define RES_OK 0b1111111110

#define NUM_OF_PROC	3

#define QUEUE_ANS 	"suduku_answers.txt"
#define QUEUE_OUT	"suduku_out.txt"
#define CALCULATE_PATH "./SudukuCalculatorProg"

char suduku_is_legal(const char* suduku, char type);
char suduku_part_is_legal(const char* part, char type);
void char_to_int_suduku(const char* in, char* out);

#endif /* SUDUKUCALCULATOR_H_ */
