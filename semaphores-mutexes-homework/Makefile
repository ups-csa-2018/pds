CC=gcc
CCFLAGS=-lpthread -lm -Wall -Wextra

all: exercise2v1 exercise2v2 exercise2v3 report.pdf

exercise2v1: rbuf.h common.c exercise2v1.c
	$(CC) $(CCFLAGS) common.c exercise2v1.c -o exercise2v1

exercise2v2: rbuf.h common.c exercise2v2.c
	$(CC) $(CCFLAGS) common.c exercise2v2.c -o exercise2v2

exercise2v3: rbuf.h common.c exercise2v3.c
	$(CC) $(CCFLAGS) common.c exercise2v3.c -o exercise2v3

report.pdf: report.tex exercise2v1.c exercise2v2.c exercise2v3.c exercise2v1.pseudoc exercise2v2.pseudoc exercise2v3.pseudoc
	pdflatex report.tex

clean:
	rm -rf exercise2v1 exercise2v2 exercise2v3 report.pdf report.log report.aux report.out
