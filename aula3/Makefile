PROGRAM=db_functions
CC=gcc -Wall -g
default:
	$(CC) -shared -fPIC `mysql_config --cflags` -c $(PROGRAM).c
	$(CC) -shared `mysql_config --libs` -o $(PROGRAM).so $(PROGRAM).o
