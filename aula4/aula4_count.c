// AVG function
#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef struct aggr_prot {
	char *name;
	void (*init)(void **handle);
	void (*iter)(void *handle, char *value);
	void (*final)(void *handle, void *result);
	void *result;
} aggr_prot_type;

struct count {
	int counter;
};

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a);
void count_init(void **handle);
void count_iter(void *handle, char *value);
void count_final(void *handle, void *result);

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a){
	MYSQL_ROW row;
	void *handle;
	(*a->init)(&handle);
	while ((row = mysql_fetch_row(res_set))!=NULL){
		(*a->iter)(handle,row[0]);
	}
	(*a->final)(handle,a->result);
}

void count_init(void **handle){
	struct count *local_handle = (struct count *) malloc(sizeof(struct count));
	local_handle->counter=0;
	*handle = (void *) local_handle;
}


void count_iter(void *handle, char *value){
	struct count *local_handle = (struct count *) handle;
	local_handle->counter += 1 ;
}

void count_final(void *handle,void *result){
	double *local_result;
	struct count *local_handle = (struct count *) handle;
	local_result = (double *) result;
	*local_result = local_handle->counter;
	free(handle);
}

int main(){
	aggr_prot_type a;
	a.name = "COUNT";
	a.init = &count_init;
	a.iter = &count_iter;
	a.final = &count_final;
	a.result= malloc(sizeof(double));
	MYSQL mysql;
	MYSQL_RES *res_set;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0);
	mysql_query(&mysql,"SELECT * FROM film");
	res_set = mysql_store_result(&mysql);
	aggr_function(res_set,&a);
	printf("%s: %f\n",a.name,*((double *) a.result));
	free(a.result);
	mysql_free_result(res_set);
}
