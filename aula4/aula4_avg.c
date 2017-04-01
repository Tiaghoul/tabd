// AVG function
#include "Yap/YapInterface.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct aggr_prot {
	char *name;
	void (*init)(void **handle);
	void (*iter)(void *handle, char *value);
	void (*final)(void *handle, void *result);
	void *result;
} aggr_prot_type;

struct avg {
	int count;
	double sum;
};

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a);
void avg_init(void **handle);
void avg_iter(void *handle, char *value);
void avg_final(void *handle, void *result);

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a){
	MYSQL_ROW row;
	void *handle;
	(*a->init)(&handle);
	while ((row = mysql_fetch_row(res_set))!=NULL){
		(*a->iter)(handle,row[0]);
	}
	(*a->final)(handle,a->result);
}

void avg_init(void **handle){
	struct avg *local_handle = (struct avg *)
	malloc(sizeof(struct avg));
	local_handle->count=0;
	local_handle->sum=0;
	*handle = (void *) local_handle;
}


void avg_iter(void *handle, char *value){
	struct avg *local_handle = (struct avg *) handle;
	local_handle->count++;
	local_handle->sum+=atof(value);
}

void avg_final(void *handle,void *result){
	double *local_result;
	struct avg *local_handle = (struct avg *) handle;
	local_result = (double *) result;
	*local_result = local_handle->sum/local_handle->count;
	free(handle);
}

int main(){
	fflush(stdout);
	aggr_prot_type a;
	a.name = "AVG";
	a.init = &avg_init;
	a.iter = &avg_iter;
	a.final = &avg_final;
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
