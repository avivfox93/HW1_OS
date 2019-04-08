
all: Q1 Q2 Q3 Q4 SudukuCalculatorProg

Q1:
	gcc -o Q1 Q1.c SudukuCalculator.c
Q2:
	gcc -o Q2 Q2.c SudukuCalculator.c
Q3:
	gcc -pthread -o Q3 Q3.c SudukuCalculator.c
Q4:
	gcc -pthread -o Q4 Q4.c SudukuCalculator.c

SudukuCalculatorProg: 
	gcc -o SudukuCalculatorProg SudukuCalculatorProg.c SudukuCalculator.c

clean:
	rm -f Q1 Q2 Q3 Q4 SudukuCalculatorProg