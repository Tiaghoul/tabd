PROGRAM=db_functions
CC=clang -Wall -g
default:
	$(CC) -shared -fPIC `mysql_config --cflags` -c $(PROGRAM).c
	$(CC) -shared `mysql_config --libs` -o $(PROGRAM).so $(PROGRAM).o
