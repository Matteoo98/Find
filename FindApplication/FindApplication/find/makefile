CC = gcc
CFLAGS = -c -g -ggdb
LDFLAGS= -g -ggdb

find: main.o find.o kmp.o report.o executor.o list.o record.o verbose.o
	$(CC) $(LDFLAGS) -o find main.o find.o kmp.o report.o executor.o list.o record.o verbose.o
main.o : main.c find.h report.h kmp.h executor.h verbose.h
	$(CC) $(CFLAGS) main.c
find.o : find.c find.h report.h kmp.h list.h record.h verbose.h
	$(CC) $(CFLAGS) find.c
record.o : record.c record.h list.h
	$(CC) $(CFLAGS) record.c
kmp.o : kmp.c kmp.h record.h
	$(CC) $(CFLAGS) kmp.c
report.o: report.c report.h list.h record.h
	$(CC) $(CFLAGS) report.c
executor.o : executor.c executor.h report.h kmp.h find.h list.h record.h
	$(CC) $(CFLAGS) executor.c
list.o : list.c list.h
	$(CC) $(CFLAGS) list.c
verbose.o : verbose.c verbose.h
	$(CC) $(CFLAGS) verbose.c