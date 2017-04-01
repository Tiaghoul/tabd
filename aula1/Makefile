PROGRAM=tabd1ex1
CC=clang -Wall -g

default:
	$(CC) `mysql_config --cflags` -c $(PROGRAM).c
	$(CC) `mysql_config --libs` -o $(PROGRAM) $(PROGRAM).o
