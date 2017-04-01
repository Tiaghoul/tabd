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

struct max {
	int maximum;
};

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a);
void max_init(void **handle);
void max_iter(void *handle, char *value);
void max_final(void *handle, void *result);

void aggr_function(MYSQL_RES *res_set, aggr_prot_type *a){
	MYSQL_ROW row;
	void *handle;
	(*a->init)(&handle);
	while ((row = mysql_fetch_row(res_set))!=NULL){
		(*a->iter)(handle,row[0]);
	}
	(*a->final)(handle,a->result);
}

void max_init(void **handle){
	struct max *local_handle = (struct max *)
	malloc(sizeof(struct max));
	local_handle->maximum=INT_MIN;
	*handle = (void *) local_handle;
}


void max_iter(void *handle, char *value){
	struct max *local_handle = (struct max *) handle;
	int new_maximum = atof(value);
	if(new_maximum > local_handle->maximum){
		local_handle->maximum = new_maximum;
	}
}

void max_final(void *handle,void *result){
	double *local_result;
	struct max *local_handle = (struct max *) handle;
	local_result = (double *) result;
	*local_result = local_handle->maximum;
	free(handle);
}

int main(){
	aggr_prot_type a;
	a.name = "MAX";
	a.init = &max_init;
	a.iter = &max_iter;
	a.final = &max_final;
	a.result= malloc(sizeof(double));
	MYSQL mysql;
	MYSQL_RES *res_set;
	mysql_init(&mysql);
	mysql_real_connect(&mysql,"localhost","root","12345","dvds",0,NULL,0);
	mysql_query(&mysql,"SELECT length FROM film");
	res_set = mysql_store_result(&mysql);
	aggr_function(res_set,&a);
	printf("%s: %f\n",a.name,*((double *) a.result));
	free(a.result);
	mysql_free_result(res_set);
}
